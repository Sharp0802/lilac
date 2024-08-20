#include "pch.h"

#include "csharp/module.h"
#include "shared/backend.h"
#include "shared/version.h"

enum ModuleKind
{
    CSharp
};

llvm::cl::OptionCategory Category("LILAC Backend Options");

llvm::cl::opt<ModuleKind> Module(
    "module",
    llvm::cl::Required,
    llvm::cl::cat(Category),
    llvm::cl::desc("Load the named module"),
    values(
        clEnumValN(CSharp, "csharp", "C# backend module")
    ));

llvm::cl::opt<std::string> XMLPathOpt(
    "i",
    llvm::cl::Required,
    llvm::cl::cat(Category),
    llvm::cl::value_desc("file"),
    llvm::cl::desc("Load interface information from <file>"));
llvm::cl::opt<std::string> LibPathOpt(
    "l",
    llvm::cl::Required,
    llvm::cl::cat(Category),
    llvm::cl::value_desc("name"),
    llvm::cl::desc("Use <name> as interface bridge library (*.dll, *.so etc.)"));
llvm::cl::opt<std::string> OutPathOpt(
    "o",
    llvm::cl::Required,
    llvm::cl::cat(Category),
    llvm::cl::value_desc("file"),
    llvm::cl::desc("Place the output into <file>"));

int main(int argc, const char* argv[])
{
    llvm::cl::SetVersionPrinter([] (llvm::raw_ostream& ost)
    {
        ost << "ild (ILD) " << LILAC_DATE << '\n' << LILAC_SHORT_LICENSE;
    });
    llvm::cl::HideUnrelatedOptions(Category);
    llvm::cl::ParseCommandLineOptions(argc, argv);

    std::function<int(const std::string&, const std::string&, const std::string&)> caller;
    switch (Module.getValue())
    {
        case CSharp: caller = MODULE_REF(csharp); break;
    }

    return caller(XMLPathOpt.getValue(), LibPathOpt.getValue(), OutPathOpt.getValue());
}
