
#include <llvm/Support/CommandLine.h>

#include <clang/Tooling/CommonOptionsParser.h>
#include <clang/Tooling/Tooling.h>
#include <clang/AST/RecursiveASTVisitor.h>

#include "frontendactionfactory.h"


static llvm::cl::OptionCategory s_Category{
    "lilac-c",
    "C/C++ compatibility layer compiler"
};

int main(int argc, const char* argv[])
{
    auto parser = clang::tooling::CommonOptionsParser::create(argc, argv, s_Category);
    if (auto error = parser.takeError())
    {
        llvm::errs() << toString(std::move(error)) << '\n';
        return -1;
    }
    clang::tooling::ClangTool tool(parser->getCompilations(), parser->getSourcePathList());

    auto factory = std::make_unique<lilac::FrontendActionFactory>();
    return tool.run(factory.get());
}
