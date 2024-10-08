# Getting Started (C++ to C#)

> [!IMPORTANT]
> This document assumes that you know basic grammar of C++ and C#

This sample describes how to create C# binding of C++ source...

See files used by this sample [here](https://github.com/Sharp0802/lilac/tree/master/samples/getting-started).

## Prepare C++ project

```
cxx
 ↳ test.cxx
MyConsole
 ↳ MyConsole.csproj
 ↳ Program.cs
CMakeLists.txt
MyConsole.sln
```

### CMakeLists.txt

We use `CMakeLists.txt` to compile C++ sources(`cxx/test.cxx`) to shared library:

```cmake
cmake_minimum_required(VERSION 3.5)
project(test)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

add_library(test SHARED cxx/test.cxx)
```

> [!NOTE]
> `CMAKE_EXPORT_COMPILE_COMMANDS=ON` make CMake to export compilation information to compile sources into
> `compile_commands.json` required by `libTooling`(`clang`) to analyze C/C++ source.

- `CMAKE_EXPORT_COMPILE_COMMANDS` must be `ON` to use lilac. (CMake 3.5 or later is required)
- A library should be always `SHARED`.

### test.cxx

```cpp
#include <iostream>

namespace cxx
{
    struct [[lilac::export]] CXXModule
    {
        [[lilac::export]]
        void Hello();
    };
}

void cxx::CXXModule::Hello()
{
    std::cout << "Hello from C++!\n";
}
```

- `cxx::CXXModule` and `cxx::CXXModule::Hello()` are export target (can be specified by `[[lilac::export]]`)
- `cxx::CXXModule::Hello()` prints `Hello from C++!` to console.

## Compile interface symbol

```shell
$ cd <sample-directory>
$ cmake -B build
```

- `cmake -B build` configure CMake project with folder `build` as build directory.
- During configuration job, `compile_commands.json` will be generated at build directory.

```shell
$ lilac sym --from c++ -o lilac.xml -p build cxx/test.cxx
```

- `sym --from c++` make `lilac` extract symbols from C++ sources
- `-o lilac.xml` make `lilac` place output to `lilac.xml`
- `-p build` make `lilac` use `build` as build directory
- if arguments provided without option (such as `cxx/test.cxx`), given arguments will be considered as analyzing target

## Prepare C# project

```
MyConsole
 ↳ MyConsole.csproj
 ↳ Program.cs
```

### MyConsole.csproj

```xml
<Project Sdk="Microsoft.NET.Sdk">

    <PropertyGroup>
        <OutputType>Exe</OutputType>
        <TargetFramework>net8.0</TargetFramework>
        <AllowUnsafeBlocks>true</AllowUnsafeBlocks>
    </PropertyGroup>
    
    <ItemGroup>
        <None Include="../build/libtest.so" CopyToOutputDirectory="PreserveNewest" />
    </ItemGroup>

</Project>
```

- Use `<AllowUnsafeBlocks>true</AllowUnsafeBlocks>` to allow unsafe (such as pointers) codes
- Use `<None Include="../build/libtest.so" CopyToOutputDirectory="PreserveNewest" />` to include build result of C++ project.

> [!NOTE]
> `../build/libtest.so` can be replaced by your build environment

### Program.cs

```csharp
new cxx.CXXModule().Hello();
```

Just use `cxx::CXXModule().Hello()` (C++) as `cxx.CXXModule().Hello()` (C#)!

## Compile C# binding

> [!NOTE]
> If you don't hear about P/Invoke,
> See [this](https://learn.microsoft.com/en-us/dotnet/standard/native-interop/pinvoke)

```shell
$ lilac bind --to c# --sym lilac.xml --name Test -o MyConsole
```

- `bind --to c#` specifies target language is C#
- `--sym lilac.xml` makes `lilac` to use `lilac.xml` as interface symbol
- `--name test` assumes name of native library is `libtest.so` or `test.so` or `libtest.dll` or `test.dll` etc...
- `-o MyConsole` let `lilac` place C# binding to `MyConsole/Test.cs`

Generated C# binding for given C++ source:

```csharp
// <auto-generated/>

namespace cxx
{
	[global::System.Runtime.InteropServices.StructLayout(global::System.Runtime.InteropServices.LayoutKind.Explicit, Size=1, Pack=1)]
	public struct CXXModule
	{
		[global::System.Runtime.InteropServices.FieldOffset(0)]
		private byte __ref;
	
		private IntPtr This
		{
			get
			{
				unsafe
				{
					return (IntPtr)global::System.Runtime.CompilerServices.Unsafe.AsPointer(ref __ref);
				}
			}
		}

		public void Hello()
		{
			__PInvoke(This);

			[System.Runtime.InteropServices.DllImport(
				"test",
				EntryPoint = "_ZN3cxx9CXXModule5HelloEv",
				CallingConvention = System.Runtime.InteropServices.CallingConvention.Cdecl,
				ExactSpelling = true)]
			static extern void __PInvoke(IntPtr @this);
		}
	}
}
```

## Execute!

Finally, We can run `MyConsole`!

Run result is as:

```
Hello from C++!
```

If a message not shown or broken message printed,
You may found a bug of lilac system.
Please contact to developer.
