# Infrastructure Spec

## Basic Linkage

![](img/process.svg)

- *Black line represents compile path*
- *Blue line represents runtime call-path*

### Compile Path

1. Compile IH (Intermediate Hierarchy) from source (*.jar for Java, *.dll for C#, etc...)
2. Generate bridge from IH

### Call Path

1. Host calls bridge
2. Bridge calls wrapper stub
3. Wrapper calls language-specific runtime
4. Runtime calls source

### Exempli gratia

For example (Use C# on C++):

![](img/wrap_dotnet_with_cxx.svg)

However, Native languages (such as C++) doesn't have language-specific runtime:

![](img/wrap_cxx_with_dotnet.svg)

## Bidirectional Linkage

![](img/bi-process.svg)

- *Black line represents compile path*
- *Blue/Green line represents runtime call-path*
- *Red line represents runtime load-path(such as `dlopen` or `LoadLibrary`)*

With this model, each language can be linked on one process(`Init Process` in this diagram).
