/// @file aggregateUtils.cpp
/// @brief
/// @copyright (c) 2024-2025 Electronic Systems Design (ESD) Lab @ UniVR This
/// file is distributed under the BSD 2-Clause License. See LICENSE.md for
/// details.

#include "hif/manipulation/aggregateUtils.hpp"

#include "hif/application_utils/Log.hpp"
#include "hif/hif_utils/hif_utils.hpp"
#include "hif/manipulation/manipulation.hpp"
#include "hif/semantics/semantics.hpp"

namespace hif
{
namespace manipulation
{

namespace /*anon*/
{

bool _getIndexOfAggregate(
    Value *ind,
    Value *min,
    Int *itype,
    unsigned long long &vind,
    hif::semantics::ILanguageSemantics *sem)
{
    HifFactory _factory(sem);
    ind            = hif::manipulation::assureSyntacticType(ind, sem);
    Value *v       = _factory.expression(ind, op_minus, hif::copy(min));
    v              = simplify(v, sem);
    ConstValue *cv = dynamic_cast<ConstValue *>(v);
    if (cv == nullptr) {
        // cannot simplify
        delete v;
        return false;
    }

    IntValue *ival = dynamic_cast<IntValue *>(hif::manipulation::transformConstant(cv, itype, sem));
    if (ival == nullptr) {
        // cannot simplify
        delete cv;
        return false;
    }

    vind = static_cast<unsigned long long>(ival->getValue());
    delete ival;
    delete cv;
    return true;
}

} // namespace
bool transformAggregateRollingAlts(Aggregate *obj, const bool atLeastOne, hif::semantics::ILanguageSemantics * /*sem*/)
{
    if (obj->alts.empty())
        return false;
    const bool hasOthers = (obj->getOthers() != nullptr);

    Value *refAltValue = obj->alts.front()->getValue();
    bool allAltsEquals = !hasOthers; // Note: works only without others.
    std::set<AggregateAlt *> altsAsOthers;
    for (BList<AggregateAlt>::iterator i = obj->alts.begin(); i != obj->alts.end(); ++i) {
        AggregateAlt *alt = *i;

        // check if the alt value is equal to the others value.
        // in this case alt is candidate to be removed.
        if (hif::equals(obj->getOthers(), alt->getValue())) {
            altsAsOthers.insert(alt);
        }

        if (!allAltsEquals || alt == obj->alts.front()) {
            // Skip following checks.
            // since:
            // - already unmatched values.
            // - aggregate has others.
            // - first iteration is useless
            continue;
        } else {
            // update alts equals value
            allAltsEquals = hif::equals(refAltValue, alt->getValue());
        }
    }

    // TODO: What is the policy to don't fall into unrolling case?
    // at the moment simplify only if all alts can be removed.
    bool removesAltsAsOthers = hasOthers;
    if (atLeastOne)
        removesAltsAsOthers &= (altsAsOthers.size() > 0ULL);
    else
        removesAltsAsOthers &= (altsAsOthers.size() == obj->alts.size());

    if (!removesAltsAsOthers && !allAltsEquals)
        return false;

    if (removesAltsAsOthers) {
        for (std::set<AggregateAlt *>::iterator i = altsAsOthers.begin(); i != altsAsOthers.end(); ++i) {
            AggregateAlt *alt = *i;
            alt->replace(nullptr);
            delete alt;
        }
    } else if (allAltsEquals) {
        obj->setOthers(hif::copy(refAltValue));
        obj->alts.clear();
    }

    return true;
}

bool transformAggregateUnrollingAlts(
    Aggregate *obj,
    unsigned long long threshold,
    hif::semantics::ILanguageSemantics *sem,
    const bool force)
{
    if (obj->getOthers() == nullptr)
        return false;
    Type *t    = hif::semantics::getSemanticType(obj, sem);
    Type *bt   = hif::semantics::getBaseType(t, false, sem, false);
    Array *arr = dynamic_cast<Array *>(bt);
    if (arr == nullptr)
        return false;

    unsigned long long ss = 0ULL;
    if (force) {
        Range *arrSpan = arr->getSpan();
        Range *r       = hif::copy(arrSpan);
        arr->setSpan(r);
        hif::manipulation::SimplifyOptions sopt;
        sopt.simplify_constants = true;
        hif::manipulation::simplify(r, sem, sopt);
        ss = hif::semantics::spanGetBitwidth(r, sem);
        arr->setSpan(arrSpan);
        delete r;
    } else {
        ss = hif::semantics::spanGetBitwidth(arr->getSpan(), sem);
    }

    if (ss == 0ULL)
        return false;

    HifFactory _factory(sem);
    Value *min = hif::rangeGetMinBound(arr->getSpan());
    min        = hif::manipulation::assureSyntacticType(hif::copy(min), sem);

    typedef std::map<unsigned long long, Value *> IndexMap;
    IndexMap indexMap;
    Int *itype = _factory.integer();

    for (BList<AggregateAlt>::iterator i = obj->alts.begin(); i != obj->alts.end(); ++i) {
        AggregateAlt *a = *i;
        for (BList<Value>::iterator j = a->indices.begin(); j != a->indices.end(); ++j) {
            Value *ind = *j;
            if (dynamic_cast<Range *>(ind) != nullptr) {
                Range *r              = static_cast<Range *>(ind);
                unsigned long long rr = hif::semantics::spanGetBitwidth(r, sem);
                if (rr == 0) {
                    delete min;
                    delete itype;
                    return false;
                }
                Value *minRangeBound    = hif::rangeGetMinBound(r);
                Value *maxRangeBound    = hif::rangeGetMaxBound(r);
                unsigned long long vmin = 0;
                unsigned long long vmax = 0;
                if (!_getIndexOfAggregate(hif::copy(minRangeBound), min, itype, vmin, sem) ||
                    !_getIndexOfAggregate(hif::copy(maxRangeBound), min, itype, vmax, sem)) {
                    delete min;
                    delete itype;
                    return false;
                }

                for (unsigned long long k = vmin; k <= vmax; ++k) {
                    indexMap[k] = a->getValue();
                }
            } else {
                unsigned long long vind = 0;
                if (!_getIndexOfAggregate(hif::copy(ind), min, itype, vind, sem)) {
                    delete min;
                    delete itype;
                    return false;
                }
                indexMap[vind] = a->getValue();
            }
        }
    }

    delete itype;

    if (ss > threshold) {
        delete min;
        return false;
    }

    BList<AggregateAlt> unrolled;
    for (unsigned long long i = 0; i < ss; ++i) {
        Expression *ind = _factory.expression(hif::copy(min), op_plus, _factory.intval(static_cast<long long>(i)));

        Value *v = nullptr;
        if (indexMap.find(i) == indexMap.end()) {
            // add others
            v = hif::copy(obj->getOthers());
        } else {
            v = hif::copy(indexMap[i]);
        }

        AggregateAlt *alt = new AggregateAlt();
        alt->indices.push_back(ind);
        alt->setValue(v);
        unrolled.push_back(alt);
    }

    obj->alts.clear();
    delete obj->setOthers(nullptr);
    obj->alts.merge(unrolled);

    delete min;

    return true;
}
} // namespace manipulation
} // namespace hif
