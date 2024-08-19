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

### :tada: First interface

```c++
#include <iostream>

int main()
{
    std::cout << "Hello from C++" << std::endl;
    return 0;
}
```

- XML interface representation

```xml
<assembly>
	<function callconv="cdecl" mangle="main" name="main" return="s32"/>
</assembly>
```

### :bubbles: Deep dive...

```c++
#include <string>

#define export [[lilac::export]]
#define STRUCT(name, type, count, ...) struct export name { type __dummy[count]; __VA_ARGS__ };

STRUCT(teststruct__align_1, char, 64)
STRUCT(teststruct__align_2, short, 32)

namespace ns__struct
{
    STRUCT(teststruct__align_4, int, 16)
    STRUCT(teststruct__align_8, long long, 8)
}

class export testclass
{
    std::string __dummy;

public:
    testclass(char* p) : __dummy(p)
    {
    }

    export teststruct__align_1 testfn(
        teststruct__align_2 val,
        ns__struct::teststruct__align_4& ref,
        ns__struct::teststruct__align_8* ptr);

    export teststruct__align_1 __vectorcall vectorcall__testfn();
};

teststruct__align_1 testclass::testfn(
    teststruct__align_2 val,
    ns__struct::teststruct__align_4& ref,
    ns__struct::teststruct__align_8* ptr)
{
    return {};
}

teststruct__align_1 __vectorcall testclass::vectorcall__testfn()
{
    return {};
}
```

- XML interface representation

```xml
<assembly>
    <record align="1" name="teststruct__align_1" size="64"/>
    <record align="2" name="teststruct__align_2" size="64"/>
    <namespace name="ns__struct">
        <record align="4" name="teststruct__align_4" size="64"/>
        <record align="8" name="teststruct__align_8" size="64"/>
    </namespace>
    <record align="8" name="testclass" size="32">
        <method callconv="cdecl" mangle="_ZN9testclass6testfnE19teststruct__align_2RN10ns__struct19teststruct__align_4EPNS1_19teststruct__align_8E" name="testfn" return="teststruct__align_1">
            <param name="val" type="teststruct__align_2"/>
            <param name="ref" type="ns__struct/teststruct__align_4*"/>
            <param name="ptr" type="ns__struct/teststruct__align_8*"/>
        </method>
        <method callconv="vectorcall" mangle="_ZN9testclass18vectorcall__testfnEv" name="vectorcall__testfn" return="teststruct__align_1"/>
        <dtor callconv="cdecl" mangle="_ZN9testclassD1Ev"/>
    </record>
</assembly>

```
