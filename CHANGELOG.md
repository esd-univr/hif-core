# Changelog

All notable changes to this project are documented in this file.

## [Unreleased]

- `SimplifyMap` now folds a reduction (`op_andrd`, `op_orrd`, `op_xorrd`) applied to a constant. `map(BitvectorValue *)` carried a bare `// TODO` for the three, and `map(IntValue *)` had no reduction branch at all, so `|32'd1` stayed an `Expression` however aggressively it was simplified. Verilog semantics lower `if (<cond>)` to `or_reduce(<cond>)` and the Verilog frontend builds an `if generate`'s else branch as the negation of that, so `_simplifyIfGenerate` was handed an `Expression` where it needs a `ConstValue` and aborted with "Cannot resolve if generate condition" — an `if generate` with an else branch could not be elaborated at all once anything requested generate expansion, which for `verilog2hif` means any design that also contains a module instance ([hif-frontend#32](https://github.com/hif-project/hif-frontend/issues/32)). The dominance order follows IEEE 1164's tables rather than a simplification of them: an absorbing value settles the result whatever it is combined with, so `U or 1` is `'1'` and `U and 0` is `'0'`, and only in their absence does `'U'` outrank `'X'`. `'Z'`, `'W'` and `'-'` carry no logic level and reduce as `'X'` does, which is the reading `op_bnot` already takes of them. The `op_not` `// TODO` in the same function is deliberately left alone: it is a separate gap and nothing measured here reaches it. ([hif-frontend#32](https://github.com/hif-project/hif-frontend/issues/32))

## [1.1.0] - 2026-08-13

- Implemented `VerilogAnalysis::map(Int*, Int*)`, closing a typing-rule gap that made any binary expression combining two `Int`-typed operands fail to type under Verilog semantics.
- Registered `$clog2` in Verilog's standard library (`VerilogSemantics::getStandardPackage()` had only a placeholder comment for it, unlike `$time`/`$stime`/`$random`).
- Migrated the project to the `hif-project` GitHub organization; updated internal references accordingly.
- Replaced the README's ecosystem-navigation list with a link to the organization profile.

## [1.0.0] - 2026-08-12

Initial coordinated release of the HIF toolchain baseline (hif-core, hif-frontend, hif-backend, hif-muffin, all tagged v1.0.0).

- Fixed `XmlParser::_addCodeInfoPropertiesComments()` overwriting every read object's line number with its column number on XML read.
- Consolidated CI to a single Linux workflow (gcc + clang), replacing an unreliable 12-way OS/compiler matrix.
- Added a proper README describing the toolchain and how to build and run tests.
