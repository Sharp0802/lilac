# :sparkles: Welcome to LILAC

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

## :globe_with_meridians: Supported Languages

| Language |      Backend       |      Frontend      |
|:--------:|:------------------:|:------------------:|
|   C++    | :grey_exclamation: |        :o:         |
|    C#    |        :o:         | :grey_exclamation: |

- :o: : Implemented.
- :grey_exclamation: : Not implemented, but planned.
- :interrobang: : Work in Progress

## :tada: Getting Started

This sample describes how to create C# binding of C++ source...

### Compile interface representation

Your C++ source code (`test.cxx`, implementation omitted) is here:

```c++
#include <iostream>

struct [[lilac::export]] Module
{
    int m_Value;

    [[lilac::export]]
    Module(int value);

    [[lilac::export]]
    int EntryPoint();
};
```

To extract interface representation from C++ source,
You should enable `CMAKE_EXPORT_COMPILE_COMMANDS` from your `CMakeLists.txt`.
`compile_commands.json` will be generated on build directory:

```cmake
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
```

Then, C++ source can be processed by `lilac-cxx`:

```
lilac-cxx -p <build-directory> -o interface.xml test.cxx
```

> [!NOTE]
> `compile_commands.json` must be in `<build-directory>`.
> Otherwise, processing will be failed.

`interface.xml` will be generated on working directory by `lilac-cxx`:

```xml
<assembly>
    <record align="4" name="Module" size="4">
        <method callconv="cdecl" mangle="_ZN6Module10EntryPointEv" name="EntryPoint" return="__s32"/>
    </record>
</assembly>
```

### Create C# Binding

Using `interface.xml` and `ild` (*interface link editor*),
You can create C# binding:

```
ild --module=csharp -i interface.xml -l test -o test.cs
```

Using the interface representation file specified by `-i` option,
C# binding will be placed on `test.cs` by `ild`:

```c#
public struct Module
{
	private byte __data[4];
	public int EntryPoint();
	{
		fixed (void* p = __data)
			__PInvoke(p);
		[System.Runtime.InteropServices.DllImport("test", EntryPoint="_ZN6Module10EntryPointEv", ExactSpelling=true)]
		static extern int __PInvoke(void* @this);
	}
}
```

#### `-l` option:

In C#, native interoperability comes with *P/Invoke* that requires library name to load native functions
(Not only C#, but other languages often requires library name to load native functions).
That library name can be specified by `-l` option.

So, build result of your C++ project should be `libtest.dll` or `test.dll` or `libtest.so` or `test.so` etc...

## :memo: Docs

- [INFRASTRUCTURE SPEC](docs/INFRASTRUCTURE.md)
