# Welcome to LILAC

> [!IMPORTANT]
> ***WORKING IN PROGRESS***

*LILAC* is a **L**LVM based **I**nteroperability **LA**yer **C**ompiler -
with a goal that producing bindings across languages.

Interoperability across languages always sucks developers...
(Especially, For managed languages such as Java, C#, Python)

For example, between C++ and C#:

- C++/CLI couldn't support modern C++ well
- P/Invoke couldn't support C++ well because of name mangling etc.

But, with power of LLVM, we hope that LILAC will generate bindings between C++ and C# ...or what else!

**Let's break down language barrier!**

## Infrastructure

```mermaid
flowchart LR
    Input --> Frontend(Frontend)

    Frontend --> Runtime[Marshal Runtime]
    Frontend --> Interface[Interface Model]

    Interface --> Backend(Backend)
    Backend --> Bridge[Bridge Library]

    subgraph Output
        Runtime
        Bridge
    end

    subgraph LILAC
        Frontend
        Output
        Interface
        Backend
    end

    Runtime --> Host[Host Program]
    Bridge --> Host
```

For example:

### Building

```mermaid
flowchart LR
    Input[C++ Library] -->  Runtime["Marshal Runtime (Native)"]
    Input --> Interface[Interface Model]

    Interface --> Bridge["Bridge Library (MSIL)"]

    subgraph Output
        Runtime
        Bridge
    end

    subgraph LILAC
        Output
        Interface
    end

    Runtime --> Host[C# Program]
    Bridge --> Host
```

### Runtime

```mermaid
flowchart LR
    Host[C# Program] --> Bridge["Bridge Library (MSIL)"]
    Bridge --> Marshal["Marshal Runtime (Native)"]
    Bridge --> Library["C++ Library"]
    Marshal --> Library

    subgraph LILAC
        Bridge
        Marshal
    end
```

In this case,
Frontend will be output thin marshalling library
to convert built-in type to type used in C++.
This is why bridge library can be generated
from only abstract interface model.

When A function in bridge library invoked,
Bridge library automatically handles function-call
with a marshal runtime generated with the frontend.

But, when C++ is host:

```mermaid
flowchart LR
    Input[C# Library] -->  Runtime["Marshal Runtime (MSIL)"]
    Input --> Interface[Interface Model]

    Interface --> Bridge["Bridge Library/Headers (Native)"]

    subgraph Output
        Runtime
        Bridge
    end

    subgraph LILAC
        Output
        Interface
    end

    Runtime --> Host[C++ Program]
    Bridge --> Host
```

In this case,
`hostfxr` with other marshalling libraries will be output
because .NET should be hosted by C++.

## Interface Model

![](docs/InterfaceModel.svg)
