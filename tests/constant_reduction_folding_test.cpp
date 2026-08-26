/// @file constant_reduction_folding_test.cpp
/// @brief
/// Copyright (c) 2026, Electronic Systems Design (ESD) Group,
/// University of Verona.
/// This file is distributed under the BSD 2-Clause License.
/// See LICENSE.md for details.
///
/// Regression for hif-frontend#32.
///
/// `SimplifyMap` never folded a reduction over a constant. `map(BitvectorValue *)`
/// carried a bare `// TODO` for `op_andrd`/`op_orrd`/`op_xorrd`, and
/// `map(IntValue *)` had no reduction branch at all - so `|32'd1` stayed an
/// Expression no matter how aggressively it was simplified.
///
/// That is not an abstract gap. Verilog semantics lower `if (<cond>)` to
/// `or_reduce(<cond>)`, and for an `if generate` with an else branch the frontend
/// builds the else condition as the negation of that. `_simplifyIfGenerate` needs
/// a ConstValue to decide which branch survives, got an Expression, and aborted
/// with "Cannot resolve if generate condition" - so an `if generate` with an else
/// could not be elaborated at all once anything asked for generate expansion.
///
/// The last case below is that exact shape, driven through the public
/// `simplify()` seam.

#include <hif/hif.hpp>
#include <iostream>
#include <string>

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

/// @brief Builds the 32-bit signed logic vector type Verilog gives an integer
///        literal, which is the type the operand carries on the #32 path.
Bitvector *verilogIntType(HifFactory &f)
{
    return f.bitvector(f.range(31, 0), true, true, true, true);
}

/// @brief Simplifies `<oper> <value>` and reports the folded bit, or '?' when
///        the expression did not fold to a single-bit constant.
char reduceToBit(hif::semantics::ILanguageSemantics *sem, const Operator oper, Value *value)
{
    HifFactory f(sem);
    Expression *expr = f.expression(oper, value);
    auto *simplified = dynamic_cast<Value *>(hif::manipulation::simplify(static_cast<Object *>(expr), sem));
    auto *bit        = dynamic_cast<BitValue *>(simplified);
    char result      = '?';
    if (bit != nullptr) {
        result = bitConstantToString(bit->getValue())[0];
    }
    delete simplified;
    return result;
}

void checkReduce(
    hif::semantics::ILanguageSemantics *sem,
    const Operator oper,
    const std::string &bits,
    const char expected,
    const std::string &what)
{
    HifFactory f(sem);
    const char got = reduceToBit(sem, oper, f.bitvectorval(bits, f.bitvector(f.range(static_cast<long long>(bits.size()) - 1, 0), true, true, true, false)));
    check(got == expected, what + ": expected '" + std::string(1, expected) + "', got '" + std::string(1, got) + "'");
}

} // namespace

int main()
{
    hif::application_utils::initializeLogHeader("TEST", "constant_reduction_folding");
    auto *sem = hif::semantics::VerilogSemantics::getInstance();
    HifFactory f(sem);

    // --------------------------------------------------------------------
    // Two-state vectors. The plain arithmetic, so a mistake in the dominance
    // rules below cannot hide behind an unknown.
    // --------------------------------------------------------------------
    checkReduce(sem, op_orrd, "0000", '0', "or-reduce of all zeroes");
    checkReduce(sem, op_orrd, "0010", '1', "or-reduce with one bit set");
    checkReduce(sem, op_andrd, "1111", '1', "and-reduce of all ones");
    checkReduce(sem, op_andrd, "1101", '0', "and-reduce with one bit clear");
    checkReduce(sem, op_xorrd, "1011", '1', "xor-reduce of an odd number of ones");
    checkReduce(sem, op_xorrd, "1010", '0', "xor-reduce of an even number of ones");

    // 'L' and 'H' are forcing levels and reduce as '0' and '1' do.
    checkReduce(sem, op_orrd, "LLLL", '0', "or-reduce of weak zeroes");
    checkReduce(sem, op_andrd, "HHHH", '1', "and-reduce of weak ones");

    // --------------------------------------------------------------------
    // IEEE 1164 dominance. An absorbing value settles the result whatever it
    // is combined with - `U or 1` is '1' and `U and 0` is '0' - and only when
    // there is no absorbing value does 'U' win over 'X'.
    // --------------------------------------------------------------------
    checkReduce(sem, op_orrd, "001X", '1', "or-reduce: a one absorbs an unknown");
    checkReduce(sem, op_orrd, "001U", '1', "or-reduce: a one absorbs an undefined");
    checkReduce(sem, op_orrd, "000X", 'X', "or-reduce: no one, an unknown decides");
    checkReduce(sem, op_orrd, "000U", 'U', "or-reduce: no one, an undefined decides");
    checkReduce(sem, op_orrd, "00XU", 'U', "or-reduce: undefined outranks unknown");

    checkReduce(sem, op_andrd, "110X", '0', "and-reduce: a zero absorbs an unknown");
    checkReduce(sem, op_andrd, "110U", '0', "and-reduce: a zero absorbs an undefined");
    checkReduce(sem, op_andrd, "111X", 'X', "and-reduce: no zero, an unknown decides");
    checkReduce(sem, op_andrd, "111U", 'U', "and-reduce: no zero, an undefined decides");
    checkReduce(sem, op_andrd, "11XU", 'U', "and-reduce: undefined outranks unknown");

    // Exclusive-or has no absorbing element, so any bit without a level wins.
    checkReduce(sem, op_xorrd, "111X", 'X', "xor-reduce: an unknown decides");
    checkReduce(sem, op_xorrd, "111U", 'U', "xor-reduce: an undefined decides");

    // 'Z', 'W' and '-' carry no logic level, exactly as op_bnot reads them.
    checkReduce(sem, op_orrd, "000Z", 'X', "or-reduce: high impedance has no level");
    checkReduce(sem, op_orrd, "000W", 'X', "or-reduce: weak unknown has no level");
    checkReduce(sem, op_orrd, "000-", 'X', "or-reduce: don't care has no level");

    // --------------------------------------------------------------------
    // The #32 operand shape: an IntValue carrying a Bitvector type, which is
    // what a constant Verilog condition is by the time it reaches here. This
    // is the case map(IntValue *) had no branch for at all.
    // --------------------------------------------------------------------
    check(reduceToBit(sem, op_orrd, f.intval(1, verilogIntType(f))) == '1', "or-reduce of integer 1");
    check(reduceToBit(sem, op_orrd, f.intval(0, verilogIntType(f))) == '0', "or-reduce of integer 0");
    check(reduceToBit(sem, op_orrd, f.intval(2, verilogIntType(f))) == '1', "or-reduce of integer 2");
    check(reduceToBit(sem, op_andrd, f.intval(-1, verilogIntType(f))) == '1', "and-reduce of all-ones integer");
    check(reduceToBit(sem, op_andrd, f.intval(1, verilogIntType(f))) == '0', "and-reduce of integer 1");

    // --------------------------------------------------------------------
    // The whole #32 seam: the else branch of `if (1) ... else ...`, which the
    // frontend builds as `bool(~(|1))`. _simplifyIfGenerate needs this to
    // reach a ConstValue; before this fix it stayed an Expression and the
    // generate could not be elaborated.
    // --------------------------------------------------------------------
    {
        Value *elseCondition =
            f.cast(f.boolean(), f.expression(op_not, f.expression(op_orrd, f.intval(1, verilogIntType(f)))));
        auto *simplified = dynamic_cast<Value *>(hif::manipulation::simplify(static_cast<Object *>(elseCondition), sem));
        auto *constant   = dynamic_cast<ConstValue *>(simplified);
        check(constant != nullptr, "the else-branch condition of an if generate folds to a constant");

        Bool boolType;
        auto *asBool = dynamic_cast<BoolValue *>(
            (constant != nullptr) ? hif::manipulation::transformConstant(constant, &boolType, sem) : nullptr);
        check(asBool != nullptr, "the folded else-branch condition is convertible to a boolean");
        check(asBool != nullptr && !asBool->getValue(), "the else branch of `if (1)` is false");
        delete asBool;
        delete simplified;
    }

    hif::application_utils::restoreLogHeader();
    if (failed) {
        std::cerr << "constant_reduction_folding_test FAILED\n";
        return 1;
    }
    std::cout << "constant_reduction_folding_test passed.\n";
    return 0;
}
