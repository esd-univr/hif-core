# Changelog

All notable changes to this project are documented in this file.

## [Unreleased]

- Expanding a `for generate` now substitutes the values the index actually takes instead of consecutive integers. `_resolveForLoopBound_calculateIterations()` used the step only to count the iterations and then recorded the range as `max = min + loops - 1`, so a loop over `0, 2, 4` was elaborated as `0, 1, 2` — a different design, produced with exit status 0 and no diagnostic, and reachable from `verilog2hif` through `partialFlattening` -> `flattenDesign` -> `expandGenerates` with no patched code. The same arithmetic was wrong in two further ways that the same correction covers: the iteration count was `span / step`, which truncates, so a loop whose span is not a multiple of its step lost its last iteration — and, because the division then failed to fold to an `IntValue`, bound resolution failed outright rather than merely miscounting; and for `op_gt` the range started at the condition bound rather than at the lowest value the index reaches, which is off by one even at step 1, so `for (g = 5; g > 0; g = g - 1)` elaborated as `0..4` instead of `1..5`. The bound arithmetic is now done on plain integers rather than on a HIF expression tree carrying that division. The renaming of generate-local declarations is deliberately unchanged: the suffix follows the substituted value, as it always has, so no step-1 loop is renamed. `_simplifyConstantLoopFor()` shares the resolver and inherits the corrected count. ([hif-core#24](https://github.com/hif-project/hif-core/issues/24))

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
