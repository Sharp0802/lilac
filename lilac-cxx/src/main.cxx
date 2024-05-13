
#include <llvm/Support/CommandLine.h>
#include <clang/Tooling/CommonOptionsParser.h>
#include <clang/Tooling/Tooling.h>
#include <clang/AST/RecursiveASTVisitor.h>

#include "visitor.h"

static llvm::cl::OptionCategory s_Category{
    "lilac-cxx",
    "C++ interoperability layer compiler"
};

int main(int argc, const char* argv[])
{
    if (argc < 2)
    {
        llvm::errs() << "Insufficient arguments\n";
        return -1;
    }

    if (strcmp(argv[1], "ihr") == 0)
    {
        argc--;
        auto parser = clang::tooling::CommonOptionsParser::create(argc, argv + 1, s_Category);
        if (auto error = parser.takeError())
        {
            llvm::errs() << toString(std::move(error)) << '\n';
            return -1;
        }

        clang::tooling::ClangTool tool(parser->getCompilations(), parser->getSourcePathList());

        const auto factory = std::make_unique<lilac::cxx::FrontendActionFactory>();
        return tool.run(factory.get());
    }
    if (strcmp(argv[1], "br") == 0)
    {
        // TODO : C++ Backend
        llvm::errs() << "Backend compiling for C++ is not supported yet.\n";
        return -1;
    }

    llvm::errs() << "Unrecognized subcommand '" << argv[1] << "'.\n";
    return -1;
}
