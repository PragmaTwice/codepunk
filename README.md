![logo](assets/logo.png)

[![Project Status: WIP – Initial development is in progress, but there has not yet been a stable, usable release suitable for the public.](https://www.repostatus.org/badges/latest/wip.svg)](https://www.repostatus.org/#wip)
![Github Actions](https://github.com/PragmaTwice/codepunk/workflows/Build/badge.svg)

*a little interval analysis tool on LLVM IR*

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
