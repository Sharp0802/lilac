# Welcome to LILAC

![](https://img.shields.io/github/license/Sharp0802/lilac)
![](https://img.shields.io/github/repo-size/Sharp0802/lilac)
![](https://img.shields.io/github/commit-activity/m/Sharp0802/lilac)

> [!IMPORTANT]
> ***WORKING IN PROGRESS***

*LILAC* is a **L**anguage to language **I**nteroperability **LA**yer **C**ompiler -
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

| Language |      Backend       |      Frontend      |
|:--------:|:------------------:|:------------------:|
|   C++    | :grey_exclamation: |   :interrobang:    |
|    C#    |   :interrobang:    | :grey_exclamation: |

- :o: : Implemented.
- :grey_exclamation: : Not implemented, but planned.
- :interrobang: : Work in Progress

## Docs

- [INFRASTRUCTURE SPEC](docs/INFRASTRUCTURE.md)

## Sample

Below is a sample of that how C++ source codes are converted into intermediate interface representation:

- Source code (implements are omitted)

```c++
#include <string>

struct [[lilac::export]] SizeTestStruct
{
    char b;
    int a;
};

struct [[lilac::export]] DtorTestStruct
{
    char __v[101];
    std::string __s;

public:
    char* GetV();
};

struct [[lilac::export]] MethodTestStruct
{
    [[lilac::export]]
    DtorTestStruct TestFn(
        void* vp,
        int iv[8],
        int* ip,
        DtorTestStruct podv,
        DtorTestStruct* podp);
};
```

- XML interface representation

```xml
<assembly>
    <record align="4" name="SizeTestStruct" size="8"/>
    <record align="8" name="DtorTestStruct" size="136">
        <dtor mangle="_ZN14DtorTestStructD1Ev"/>
    </record>
    <record align="1" name="MethodTestStruct" size="1">
        <method mangle="_ZN16MethodTestStruct6TestFnEPvPiS1_14DtorTestStructPS2_" name="TestFn" return="DtorTestStruct">
            <param name="vp" type="void*"/>
            <param name="iv" type="s32*"/>
            <param name="ip" type="s32*"/>
            <param name="podv" type="DtorTestStruct"/>
            <param name="podp" type="DtorTestStruct*"/>
        </method>
    </record>
</assembly>
```
