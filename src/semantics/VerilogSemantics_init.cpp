/// @file VerilogSemantics_init.cpp
/// @brief
/// @copyright (c) 2024-2025 Electronic Systems Design (ESD) Lab @ UniVR This
/// file is distributed under the BSD 2-Clause License. See LICENSE.md for
/// details.

#include "hif/semantics/VerilogSemantics.hpp"
namespace hif
{
namespace semantics
{
void VerilogSemantics::_initForbiddenNames()
{
    // Forbidden tab (i.e. reserved words) are cabled
}
void VerilogSemantics::_initStandardFilenames() {}
void VerilogSemantics::_initStandardSymbols()
{
    // TODO map the following libraries
    // * VHDL standard, ieee__std_logic_1164, ieee__std_logic_arith
    // * SC sc_core, tlm, hdtlib
}
} // namespace semantics
} // namespace hif
