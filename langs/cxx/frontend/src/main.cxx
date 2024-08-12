#include "pch.h"
#include "pluginaction.h"

static llvm::cl::OptionCategory Category("LILAC Frontend Options");

static llvm::cl::opt<std::string> Output(
    "o",
    llvm::cl::Required,
    llvm::cl::ValueRequired,
    llvm::cl::value_desc("file"),
    llvm::cl::desc("Place output into <file>."),
    llvm::cl::cat(Category));

int main(int argc, const char* argv[])
{
    auto expectedParser = clang::tooling::CommonOptionsParser::create(argc, argv, Category);
    if (!expectedParser)
    {
        llvm::errs() << expectedParser.takeError();
        return 1;
    }

    auto& parser = expectedParser.get();

    clang::tooling::ClangTool tool(parser.getCompilations(), parser.getSourcePathList());
    if (const auto ret = tool.run(clang::tooling::newFrontendActionFactory<lilac::cxx::LilacAction>().get()))
        return ret;

    auto& root = lilac::cxx::GetDOMRoot();

    std::ofstream ofs(Output.getValue());
    ofs << static_cast<std::string>(frxml::doc{ root });
}
