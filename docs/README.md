# Welcome to LILAC

![](https://img.shields.io/github/license/Sharp0802/lilac)
![](https://img.shields.io/github/repo-size/Sharp0802/lilac)
![](https://img.shields.io/github/commit-activity/m/Sharp0802/lilac)
[![Deploy with Doxygen](https://github.com/Sharp0802/lilac/actions/workflows/doxygen.yml/badge.svg)](https://github.com/Sharp0802/lilac/actions/workflows/doxygen.yml)

*LILAC* is a <b>L</b>anguage to language <b>I</b>nteroperability <b>LA</b>yer <b>C</b>ompiler -
with a goal that producing bindings across languages.

Interoperability across languages always sucks developers...
(Especially, For managed languages such as Java, C#, Python)

For example, between C++ and C#:

- C++/CLI couldn't support modern C++ well
- In addition, C++/CLI doesn't support GNU/Linux
- P/Invoke couldn't support C++ well because of name mangling etc.

But, with power of LLVM, we hope that LILAC will generate bindings between C++ and C# ...or what else!
(Actually, For the languages not supporting LLVM, We need to implement frontend for each languages)

**Let's break down language barrier!**

## Supported Languages

| Language | Backend | Frontend |
|:--------:|:-------:|:--------:|
|   C++    |    ?    |    O     |
|    C#    |    O    |    ?     |

- O : Implemented.
- ? : Not implemented, but planned.
- ! : Work in Progress

## Getting Started

- [Getting Started with C++ and C#](https://lilac.sharp0802.com/md_docs_2GettingStartedCppCsharp.html)
