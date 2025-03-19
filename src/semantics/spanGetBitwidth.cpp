/// @file spanGetBitwidth.cpp
/// @brief
/// @copyright (c) 2024-2025 Electronic Systems Design (ESD) Lab @ UniVR This
/// file is distributed under the BSD 2-Clause License. See LICENSE.md for
/// details.

#include "hif/semantics/spanGetBitwidth.hpp"

#include "hif/application_utils/Log.hpp"
#include "hif/hif_utils/hif_utils.hpp"
#include "hif/manipulation/manipulation.hpp"
#include "hif/semantics/semantics.hpp"

namespace hif
{
namespace semantics
{

namespace /*anon*/
{

} // namespace

unsigned long long
spanGetBitwidth(Range *r, ILanguageSemantics *sem, const bool simplify, const manipulation::SimplifyOptions &opts)
{
    if (r == nullptr)
        return 0;
    Value *res = spanGetSize(r, sem, simplify, opts);
    if (res == nullptr)
        return 0;
    ConstValue *cvRes = dynamic_cast<ConstValue *>(res);
    if (cvRes == nullptr) {
        delete res;
        return 0;
    }
    IntValue *ivoRes = dynamic_cast<IntValue *>(cvRes);
    if (ivoRes != nullptr) {
        long long resInt = ivoRes->getValue();
        if (resInt < 0)
            resInt = 0;
        delete ivoRes;
        return static_cast<unsigned long long>(resInt);
    }

    Int it1;
    it1.setSigned(true);
    it1.setSpan(new Range(63, 0));
    it1.setConstexpr(true);

    ivoRes = dynamic_cast<IntValue *>(hif::manipulation::transformConstant(cvRes, &it1, sem));
    if (ivoRes != nullptr) {
        long long resInt = ivoRes->getValue();
        if (resInt < 0)
            resInt = 0;
        delete ivoRes;
        return static_cast<unsigned long long>(resInt);
    }

    delete cvRes;
    return 0;
}

unsigned long long typeGetSpanBitwidth(
    Type *type,
    ILanguageSemantics *sem,
    const bool simplify,
    const hif::manipulation::SimplifyOptions &opts)
{
    Range *span = hif::typeGetSpan(type, sem);
    return spanGetBitwidth(span, sem, simplify, opts);
}

} // namespace semantics
} // namespace hif
