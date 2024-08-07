#include "pluginaction.h"

namespace lilac::cxx
{
    std::unique_ptr<clang::ASTConsumer> LilacAction::CreateASTConsumer(
        clang::CompilerInstance& ci,
        llvm::StringRef)
    {
        return std::make_unique<LilacASTConsumer>();
    }

    clang::PluginASTAction::ActionType LilacAction::getActionType()
    {
        return AddAfterMainAction;
    }

    bool LilacAction::ParseArgs(const clang::CompilerInstance& CI, const std::vector<std::string>& arg)
    {
        return true;
    }

    void LilacASTConsumer::HandleTranslationUnit(clang::ASTContext& context)
    {
        LilacASTVisitor visitor;
        visitor.TraverseDecl(context.getTranslationUnitDecl());
    }

    LilacASTVisitor::LilacASTVisitor()
    {
        const auto opt = llvm::cast<llvm::cl::opt<std::string>>(llvm::cl::getRegisteredOptions().lookup("o"));
        m_OutputFilename = opt->getValue();
    }

    LilacASTVisitor::~LilacASTVisitor()
    {

    }

    bool LilacASTVisitor::TraverseEnumDecl(clang::EnumDecl* decl)
    {
        std::vector<frxml::dom> children;
        EnumVisitor visitor = [&](clang::EnumConstantDecl* constant)
        {
            children.push_back()
        };
    }

    bool LilacASTVisitor::TraverseCXXRecordDecl(clang::CXXRecordDecl* decl)
    {
    }

    bool LilacASTVisitor::TraverseFunctionDecl(clang::FunctionDecl* decl)
    {
    }

    bool LilacASTVisitor::EnumVisitor::TraverseEnumConstantDecl(clang::EnumConstantDecl* decl)
    {
        m_Delegate(decl);
        return true;
    }
}

[[maybe_unused]]
static clang::FrontendPluginRegistry::Add<lilac::cxx::LilacAction> X(
    "lilac-cxx-frontend",
    "A plugin of clang-frontend for LILAC system"
);
