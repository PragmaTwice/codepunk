![logo](assets/logo.png)

[![GitHub](https://img.shields.io/github/license/PragmaTwice/codepunk)](https://www.gnu.org/licenses/gpl-3.0.html)
[![Project Status: WIP – Initial development is in progress, but there has not yet been a stable, usable release suitable for the public.](https://www.repostatus.org/badges/latest/wip.svg)](https://www.repostatus.org/#wip)
[![Github Actions](https://github.com/PragmaTwice/codepunk/workflows/Build/badge.svg)](https://github.com/PragmaTwice/codepunk/actions)

*a little interval analysis tool on LLVM IR*

## Dependencies

- LLVM ([releases/10.x](https://github.com/llvm/llvm-project/tree/release/10.x))
- GoogleTest ([master](https://github.com/google/googletest/tree/master))

## Algorithm

- interval analysis via abstract interpretation
- dataflow iterating in regard for path conditions

## Worklist

- more arthmetic/terminator instruction support
- interprocedural analysis via context sensitive
- more pattern support for interval solving
- modeling for array/heap memory
- unsigned integer support
- floating point support
