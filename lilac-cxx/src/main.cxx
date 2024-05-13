#include "pch.h"
#include "visitor.h"
#include "version.h"

namespace
{
    llvm::cl::OptionCategory s_IHCategory{
        "IH Compile Options"
    };

    llvm::cl::SubCommand s_IH{
        "ih",
        "Compile IH (Intermediate Hierarchy)"
    };

    llvm::cl::opt<std::string> s_IHOut{
        llvm::cl::Positional,
        llvm::cl::desc("<output>"),
        llvm::cl::sub(s_IH),
        llvm::cl::cat(s_IHCategory),
        llvm::cl::Required
    };

    llvm::cl::list<std::string> s_IHSources{
        llvm::cl::ConsumeAfter,
        llvm::cl::desc("<source>..."),
        llvm::cl::sub(s_IH),
        llvm::cl::cat(s_IHCategory)
    };

    llvm::cl::opt<std::string> s_IHCompileCommands{
        "c",
        llvm::cl::desc("Specify json compile-commands"),
        llvm::cl::sub(s_IH),
        llvm::cl::cat(s_IHCategory),
        llvm::cl::init("compile_commands.json"),
        llvm::cl::ValueRequired
    };
}

int main(int argc, const char* argv[])
{
    llvm::cl::SetVersionPrinter([](llvm::raw_ostream& os)
    {
        os << "lilac-cxx " VERSION " " TIMESTAMP "\n";
    });
    llvm::cl::HideUnrelatedOptions(s_IHCategory);
    llvm::cl::ParseCommandLineOptions(argc, argv);

    if (s_IH)
    {
        std::string error;
        const std::shared_ptr cd = clang::tooling::JSONCompilationDatabase::loadFromFile(
            s_IHCompileCommands,
            error,
            clang::tooling::JSONCommandLineSyntax::AutoDetect);
        if (!cd)
        {
            llvm::errs() << error << '\n';
            return 1;
        }

        clang::tooling::ClangTool tool(*cd, s_IHSources);

        const auto factory = std::make_unique<lilac::cxx::FrontendActionFactory>(s_IHOut);
        return tool.run(factory.get());
    }
}
