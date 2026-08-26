/// @file for_generate_step_expansion_test.cpp
/// @brief
/// Copyright (c) 2026, Electronic Systems Design (ESD) Group,
/// University of Verona.
/// This file is distributed under the BSD 2-Clause License.
/// See LICENSE.md for details.
///
/// Regression for hif-core#24.
///
/// Expanding a `for generate` substituted the *iteration ordinal* for the
/// genvar instead of the value the index actually takes, because
/// `_resolveForLoopBound_calculateIterations` used the step only to count the
/// iterations and then recorded the range as `max = min + loops - 1`. A loop
/// over 0, 2, 4 was elaborated as 0, 1, 2 - a different design, produced with
/// exit status 0 and no diagnostic.
///
/// The same arithmetic was wrong in two further ways, which the cases below
/// pin separately:
///
///  - the iteration *count* was `span / step`, which truncates. A loop whose
///    span is not a multiple of its step lost its last iteration, and the
///    division failing to fold made the whole bound resolution fail.
///  - for `op_gt` the range started at the condition bound rather than at the
///    lowest value the index reaches, which is off by one even at step 1:
///    `for (g = 5; g > 0; g = g - 1)` elaborated as 0..4 instead of 1..5.
///
/// Every expectation below was validated against `iverilog -g2005` on the
/// equivalent Verilog: the leaf drives `y[g]` from `a[g]`, so the driven bits
/// read back as the substituted values and the undriven ones read as 'z'.

#include <hif/hif.hpp>
#include <algorithm>
#include <iostream>
#include <set>
#include <sstream>
#include <string>
#include <vector>

using namespace hif;

namespace
{

bool failed = false;

void check(bool condition, const std::string &what)
{
    if (!condition) {
        std::cerr << "FAIL: " << what << "\n";
        failed = true;
    }
}

std::string toString(const std::vector<long long> &values)
{
    std::stringstream ss;
    ss << "{";
    for (std::size_t i = 0; i < values.size(); ++i) {
        ss << (i == 0 ? "" : ", ") << values[i];
    }
    ss << "}";
    return ss.str();
}

/// @brief The 32-bit signed logic vector Verilog gives a genvar and an integer
///        literal. Building the header out of plain Int would not type under
///        Verilog semantics, and is not the shape the frontend produces.
Bitvector *genvarType(HifFactory &f) { return f.bitvector(f.range(31, 0), true, true, false, true); }

Bitvector *literalType(HifFactory &f) { return f.bitvector(f.range(31, 0), true, true, true, true); }

/// @brief One `for generate` loop header, written the way Verilog writes it.
struct LoopCase {
    std::string name;
    long long init;
    Operator condition;
    long long bound;
    long long step; ///< Magnitude; the condition operator carries the direction.
    std::vector<long long> expected;
};

/// @brief Builds `system -> du -> view -> contents` holding two 8-bit signals
///        and a `for generate` whose body continuously assigns `y[g] = a[g]`
///        and which declares one generate-local variable.
///
/// The generate-local declaration is there to pin the renaming contract: the
/// suffix follows the substituted value, not the iteration ordinal.
System *buildDesign(hif::semantics::ILanguageSemantics *sem, const LoopCase &c)
{
    HifFactory f(sem);

    auto *fg = new ForGenerate();
    fg->setName("blk");

    auto *genvar = f.variable(genvarType(f), "g", f.intval(c.init, literalType(f)));
    fg->initDeclarations.push_back(genvar);

    fg->setCondition(f.expression(f.identifier("g"), c.condition, f.intval(c.bound, literalType(f))));

    const Operator stepOperator =
        (c.condition == op_lt || c.condition == op_le) ? op_plus : op_minus;
    fg->stepActions.push_back(
        f.assignment(f.identifier("g"), f.expression(f.identifier("g"), stepOperator, f.intval(c.step, literalType(f)))));

    fg->declarations.push_back(f.variable(genvarType(f), "loc", f.intval(0, literalType(f))));

    auto *ga = new GlobalAction();
    ga->actions.push_back(f.assignment(f.member(f.identifier("y"), f.identifier("g")), f.member(f.identifier("a"), f.identifier("g"))));
    fg->setGlobalAction(ga);

    Contents *contents = f.contents(
        nullptr,
        (f.noDeclarations(), f.signal(f.bitvector(f.range(7, 0), true, true, false, false), "a"),
         f.signal(f.bitvector(f.range(7, 0), true, true, false, false), "y")),
        f.noGenerates(), f.noInstances(), f.noStateTables(), f.noLibraries());
    contents->generates.push_back(fg);

    View *view = f.view(
        "behav", contents, new Entity(), hif::rtl, f.noDeclarations(), f.noLibraries(), f.noTemplates());

    auto *du = new DesignUnit();
    du->setName("leaf");
    du->views.push_back(view);

    auto *system = new System();
    system->setName("system");
    system->setLanguageID(hif::rtl);
    system->designUnits.push_back(du);
    return system;
}

/// @brief Reads a constant back as an integer. The substituted value carries
///        the genvar's own type, so it arrives as a BitvectorValue rather than
///        as the IntValue the expansion started from.
bool constantToInt(hif::semantics::ILanguageSemantics *sem, Value *value, long long &result)
{
    HifFactory f(sem);
    if (auto *asInt = dynamic_cast<IntValue *>(value)) {
        result = asInt->getValue();
        return true;
    }
    auto *asConst = dynamic_cast<ConstValue *>(value);
    if (asConst == nullptr)
        return false;

    Int *intType         = f.integer(f.range(63, 0), true, true);
    ConstValue *asNumber = hif::manipulation::transformConstant(asConst, intType, sem);
    delete intType;
    auto *asInt = dynamic_cast<IntValue *>(asNumber);
    if (asInt != nullptr)
        result = asInt->getValue();
    delete asNumber;
    return asInt != nullptr;
}

/// @brief The index values the expansion actually substituted, read back from
///        the `a[<index>]` members that survive in the expanded design.
std::vector<long long> substitutedIndices(hif::semantics::ILanguageSemantics *sem, Object *root)
{
    hif::HifTypedQuery<Member> query;
    hif::HifTypedQuery<Object>::Results results;
    hif::search(results, root, query);

    std::set<long long> found;
    for (auto *object : results) {
        auto *member = dynamic_cast<Member *>(object);
        auto *prefix = dynamic_cast<Identifier *>(member->getPrefix());
        long long index = 0;
        if (prefix == nullptr || prefix->getName() != "a")
            continue;
        if (!constantToInt(sem, member->getIndex(), index))
            continue;
        found.insert(index);
    }
    return std::vector<long long>(found.begin(), found.end());
}

/// @brief The names the generate-local declaration was replicated under.
std::vector<std::string> localDeclarationNames(Object *root)
{
    hif::HifTypedQuery<Variable> query;
    hif::HifTypedQuery<Object>::Results results;
    hif::search(results, root, query);

    std::vector<std::string> names;
    for (auto *object : results) {
        const std::string name = dynamic_cast<Variable *>(object)->getName();
        if (name.rfind("loc_", 0) == 0)
            names.push_back(name);
    }
    std::sort(names.begin(), names.end());
    return names;
}

void runCase(hif::semantics::ILanguageSemantics *sem, const LoopCase &c)
{
    System *system = buildDesign(sem, c);

    hif::manipulation::SimplifyOptions opt;
    opt.simplify_generates = true;
    hif::manipulation::simplify(system, sem, opt);

    const std::vector<long long> got = substitutedIndices(sem, system);
    check(
        got == c.expected,
        c.name + ": expected indices " + toString(c.expected) + ", got " + toString(got));

    // The suffix has always followed the substituted value rather than the
    // ordinal - `for (g = 2; g < 6; g = g + 1)` yields loc_2..loc_5 today - so
    // the fix must keep doing that or it renames every loop not based at zero.
    std::vector<std::string> expectedNames;
    for (long long value : c.expected)
        expectedNames.push_back("loc_" + std::to_string(value));
    std::sort(expectedNames.begin(), expectedNames.end());
    check(
        localDeclarationNames(system) == expectedNames,
        c.name + ": generate-local declarations should be suffixed with the substituted values");

    delete system;
}

} // namespace

int main()
{
    hif::application_utils::initializeLogHeader("TEST", "for_generate_step_expansion");
    auto *sem = hif::semantics::VerilogSemantics::getInstance();

    const std::vector<LoopCase> cases = {
        // ----------------------------------------------------------------
        // Non-unit steps. The reported case first: three iterations, and the
        // pre-fix expansion substituted 0, 1, 2 for them.
        // ----------------------------------------------------------------
        {"lt_step2", 0, op_lt, 6, 2, {0, 2, 4}},
        {"lt_step3", 1, op_lt, 8, 3, {1, 4, 7}},
        {"le_step2", 0, op_le, 6, 2, {0, 2, 4, 6}},

        // A span that is not a multiple of the step. `<` must round the count
        // up and `<=` must not let the bound add an iteration of its own.
        {"lt_step2_partial", 0, op_lt, 7, 2, {0, 2, 4, 6}},
        {"le_step2_partial", 0, op_le, 7, 2, {0, 2, 4, 6}},

        // Descending, where the lowest value reached is not the bound.
        {"gt_step2", 6, op_gt, 0, 2, {2, 4, 6}},
        {"ge_step2", 7, op_ge, 1, 2, {1, 3, 5, 7}},

        // ----------------------------------------------------------------
        // Step 1. `op_gt` was off by one here too; the other three forms were
        // already right and must stay byte-for-byte as they were.
        // ----------------------------------------------------------------
        {"gt_step1", 5, op_gt, 0, 1, {1, 2, 3, 4, 5}},
        {"ge_step1", 5, op_ge, 0, 1, {0, 1, 2, 3, 4, 5}},
        {"lt_step1_from_zero", 0, op_lt, 4, 1, {0, 1, 2, 3}},
        {"lt_step1_offset", 2, op_lt, 6, 1, {2, 3, 4, 5}},
        {"le_step1", 0, op_le, 3, 1, {0, 1, 2, 3}},

        // ----------------------------------------------------------------
        // Degenerate headers.
        // ----------------------------------------------------------------
        {"single_iteration", 3, op_le, 3, 1, {3}},
        {"step_overshoots_bound", 0, op_lt, 5, 10, {0}},
        {"never_runs", 0, op_lt, 0, 1, {}},
        {"never_runs_descending", 0, op_gt, 0, 1, {}},
    };

    for (const LoopCase &c : cases)
        runCase(sem, c);

    hif::application_utils::restoreLogHeader();
    if (failed) {
        std::cerr << "for_generate_step_expansion: FAILED\n";
        return 1;
    }
    std::cout << "for_generate_step_expansion: OK\n";
    return 0;
}
