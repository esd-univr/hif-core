/// @file terminal_prefix_options_defaults_test.cpp
/// @brief
/// Copyright (c) 2026, Electronic Systems Design (ESD) Group,
/// University of Verona.
/// This file is distributed under the BSD 2-Clause License.
/// See LICENSE.md for details.
///
/// Regression (hif-core#22): TerminalPrefixOptions was an aggregate with no
/// constructor and no default member initializers, which produced two distinct
/// defects from one declaration.
///
/// The first is that the defaults the header documents did not exist.
/// getTerminalPrefix's default argument value-initializes the temporary, which
/// for such an aggregate zero-initializes every member, so the effective
/// default was false for all four flags while the header promised true for
/// three of them. Callers that took the default therefore stopped at a Member
/// or a Slice instead of recursing to the Identifier underneath it - which is
/// what made hif2vhdl abort on every bit-select and part-select assignment
/// target (hif-backend#103).
///
/// The second is that a plain `TerminalPrefixOptions opts;` local is
/// default-initialized, not value-initialized, so every member the call site
/// did not assign held an indeterminate value and reading it was undefined
/// behaviour. Measured on this header before the fix, a dirtied stack yielded
/// recurseIntoFieldRefs=253 and recurseIntoMembers=127 - not merely wrong
/// values but invalid bool representations.
///
/// The two halves need separate checks, because fixing only the second (by
/// initializing everything to false) would leave the first in place. So this
/// covers both construction forms, and asserts behaviour through
/// getTerminalPrefix rather than only reading the flags back.

#include <hif/hif.hpp>

#include <iostream>
#include <type_traits>

using namespace hif;

namespace
{

int failures = 0;

void check(bool condition, const std::string &what)
{
    if (!condition) {
        std::cerr << "FAIL: " << what << "\n";
        ++failures;
    }
}

void checkFlags(const TerminalPrefixOptions &opt, const std::string &form)
{
    check(opt.recurseIntoFieldRefs, form + ": recurseIntoFieldRefs should default to true");
    check(opt.recurseIntoMembers, form + ": recurseIntoMembers should default to true");
    check(opt.recurseIntoSlices, form + ": recurseIntoSlices should default to true");
    check(!opt.recurseIntoDerefExpressions, form + ": recurseIntoDerefExpressions should default to false");
}

/// @brief Leaves a non-zero pattern on the stack.
/// Without it, a default-initialized local in a fresh frame usually reads zero
/// and the pre-fix defect hides: the check below would pass by luck rather than
/// because the members are initialized.
__attribute__((noinline)) void dirtyStack()
{
    volatile unsigned char buf[512];
    for (std::size_t i = 0; i < sizeof(buf); ++i) {
        buf[i] = static_cast<unsigned char>((i * 7U + 13U) & 0xFFU);
    }
    (void)buf[0];
}

/// @brief Asserts that getTerminalPrefix(@p value), called with no options,
/// returns @p expected, then destroys the tree.
/// Each caller owns a freshly built node, so destroying it here keeps the
/// assertion and the cleanup from being restated at every case.
void checkTerminalPrefix(Value *value, const Value *expected, const std::string &what)
{
    check(getTerminalPrefix(value) == expected, what);
    delete value;
}

/// @brief Builds y[3 downto 0] and returns the Slice, with @p inner set to the
/// Identifier it is built on.
auto makeSlice(Identifier *&inner) -> Slice *
{
    inner       = new Identifier("y");
    auto *slice = new Slice();
    slice->setPrefix(inner);
    slice->setSpan(new Range(3, 0));
    return slice;
}

} // namespace

auto main() -> int
{
    // ------------------------------------------------------------------
    // 1. The type must no longer be constructible into an indeterminate
    //    state. A trivially default-constructible aggregate is exactly the
    //    property that made `TerminalPrefixOptions opts;` undefined behaviour;
    //    a default member initializer on every member removes it.
    // ------------------------------------------------------------------
    check(
        !std::is_trivially_default_constructible<TerminalPrefixOptions>::value,
        "TerminalPrefixOptions is still trivially default-constructible, so a default-initialized "
        "local still leaves its members indeterminate");

    // ------------------------------------------------------------------
    // 2. Value initialization - the form getTerminalPrefix's default argument
    //    uses - must produce the documented defaults.
    // ------------------------------------------------------------------
    checkFlags(TerminalPrefixOptions(), "value-initialized");

    // ------------------------------------------------------------------
    // 3. Default initialization - the form eleven call sites use before
    //    overriding a subset - must produce the same deterministic values.
    // ------------------------------------------------------------------
    dirtyStack();
    {
        TerminalPrefixOptions opts;
        checkFlags(opts, "default-initialized local");
    }

    // ------------------------------------------------------------------
    // 4. The defaults must be observable through getTerminalPrefix itself,
    //    called with no options: a part-select and a bit-select both resolve
    //    to the Identifier they are built on.
    // ------------------------------------------------------------------
    {
        Identifier *inner = nullptr;
        Slice *slice      = makeSlice(inner);
        checkTerminalPrefix(slice, inner, "a Slice should resolve to the Identifier it is built on");
    }
    {
        auto *inner  = new Identifier("y");
        auto *member = new Member();
        member->setPrefix(inner);
        member->setIndex(new IntValue(3));
        checkTerminalPrefix(member, inner, "a Member should resolve to the Identifier it is built on");
    }
    {
        auto *inner = new Identifier("s");
        auto *field = new FieldReference();
        field->setPrefix(inner);
        field->setName("f");
        checkTerminalPrefix(field, inner, "a FieldReference should resolve to the Identifier it is built on");
    }

    // ------------------------------------------------------------------
    // 5. The one documented false must stay false: a deref expression is
    //    returned as itself unless a caller asks to recurse through it.
    // ------------------------------------------------------------------
    {
        auto *expr = new Expression();
        expr->setOperator(op_deref);
        expr->setValue1(new Identifier("p"));
        checkTerminalPrefix(
            expr, expr, "a deref Expression should be returned as itself, since recurseIntoDerefExpressions is false");
    }

    // ------------------------------------------------------------------
    // 6. The same behaviour must hold for a default-initialized local that a
    //    caller passes without setting anything - the shape that was undefined
    //    behaviour before the fix.
    // ------------------------------------------------------------------
    dirtyStack();
    {
        Identifier *inner = nullptr;
        Slice *slice      = makeSlice(inner);
        TerminalPrefixOptions opts;
        check(
            getTerminalPrefix(slice, opts) == inner,
            "getTerminalPrefix(Slice, default-initialized options) should recurse to the Identifier prefix");
        delete slice;
    }

    if (failures != 0) {
        std::cerr << failures << " check(s) failed\n";
        return 1;
    }
    std::cout << "PASS\n";
    return 0;
}
