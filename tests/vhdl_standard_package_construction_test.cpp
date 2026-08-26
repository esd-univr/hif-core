/// @file vhdl_standard_package_construction_test.cpp
/// @brief
/// Copyright (c) 2026, Electronic Systems Design (ESD) Group,
/// University of Verona.
/// This file is distributed under the BSD 2-Clause License.
/// See LICENSE.md for details.
///
/// Regression for hif-core#20: three VHDL standard packages could not be
/// constructed at all. `HifFactory::library`'s third parameter is the
/// referenced LibraryDef's file name, declared `const std::string &`, and nine
/// call sites passed a literal `nullptr` for it. Constructing a std::string
/// from a null char* is undefined behaviour that libstdc++ diagnoses by
/// throwing, so asking for any of these packages ended the process:
///
///     terminate called after throwing an instance of 'std::logic_error'
///       what():  basic_string: construction from null is not valid
///
/// The throw is inside the package *constructor*, so it fired on the first
/// call and the package could never be produced. `std.textio` is the one that
/// matters most - it is in essentially every VHDL testbench - and
/// `ieee.std_logic_misc` is the standard pre-2008 way to write a reduction.
///
/// This test drives the construction directly rather than through a design,
/// because the defect is in hif-core and the tools that hit it live in other
/// repositories. Both name forms are checked: the plain VHDL one and the
/// `hif_vhdl_` one that HIFSemantics::getStandardLibrary resolves, since the
/// crashing call sites are guarded by `hifFormat` and only the second form was
/// exercised when this was found.
///
/// Note the packages are cached in function-local statics, so an aborting
/// build is not merely a first-call problem - there is no second chance.

#include <hif/hif.hpp>
#include <iostream>
#include <string>
#include <vector>

using namespace hif;
using namespace hif::semantics;

namespace
{

auto checkVhdl(const std::string &name) -> bool
{
    LibraryDef *ld = VHDLSemantics::getInstance()->getStandardLibrary(name);
    if (ld == nullptr) {
        std::cerr << "FAIL: VHDLSemantics could not provide '" << name << "'\n";
        return false;
    }
    if (ld->declarations.empty()) {
        std::cerr << "FAIL: '" << name << "' was built empty, so it carries none of its declarations\n";
        return false;
    }
    std::cout << "PASS: VHDLSemantics '" << name << "' (" << ld->declarations.size() << " declarations)\n";
    return true;
}

auto checkHif(const std::string &name) -> bool
{
    LibraryDef *ld = HIFSemantics::getInstance()->getStandardLibrary(name);
    if (ld == nullptr) {
        std::cerr << "FAIL: HIFSemantics could not provide '" << name << "'\n";
        return false;
    }
    if (ld->declarations.empty()) {
        std::cerr << "FAIL: '" << name << "' was built empty, so it carries none of its declarations\n";
        return false;
    }
    std::cout << "PASS: HIFSemantics '" << name << "' (" << ld->declarations.size() << " declarations)\n";
    return true;
}

} // namespace

auto main() -> int
{
    bool ok = true;

    // The three packages whose constructors passed a null file name. Reaching
    // any of these used to terminate the process rather than return.
    for (const auto &name : std::vector<std::string>{"std_textio", "ieee_std_logic_textio", "ieee_std_logic_misc"}) {
        ok &= checkVhdl(name);
    }
    for (const auto &name :
         std::vector<std::string>{
             "hif_vhdl_std_textio", "hif_vhdl_ieee_std_logic_textio", "hif_vhdl_ieee_std_logic_misc"}) {
        ok &= checkHif(name);
    }

    // A package that was never affected, as a control: if these started failing
    // too, something broader than the null file name is wrong.
    ok &= checkVhdl("ieee_std_logic_1164");

    // The reduction functions hif2vhdl lowers onto (hif-backend#92) have to be
    // in the package that is now constructible, not merely somewhere. Checked
    // by name so that a package built from the wrong constructor is caught.
    LibraryDef *misc = HIFSemantics::getInstance()->getStandardLibrary("hif_vhdl_ieee_std_logic_misc");
    if (misc != nullptr) {
        for (const auto &fn : std::vector<std::string>{"hif_vhdl_and_reduce", "hif_vhdl_or_reduce", "hif_vhdl_xor_reduce"}) {
            bool found = false;
            for (BList<Declaration>::iterator it(misc->declarations.begin()); it != misc->declarations.end(); ++it) {
                auto *sub = dynamic_cast<SubProgram *>(*it);
                if (sub != nullptr && sub->getName() == fn) {
                    found = true;
                    break;
                }
            }
            if (!found) {
                std::cerr << "FAIL: '" << fn << "' is not declared in hif_vhdl_ieee_std_logic_misc\n";
                ok = false;
            } else {
                std::cout << "PASS: '" << fn << "' is declared\n";
            }
        }
    }

    if (!ok) {
        std::cerr << "FAILED\n";
        return 1;
    }
    std::cout << "ALL CHECKS PASSED\n";
    return 0;
}
