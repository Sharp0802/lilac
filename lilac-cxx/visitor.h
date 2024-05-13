#pragma once

#include <clang/AST/ASTConsumer.h>
#include <clang/AST/RecursiveASTVisitor.h>
#include <clang/Frontend/FrontendAction.h>
#include <clang/Tooling/Tooling.h>

#include "lilac-core/hierarchy.h"


namespace lilac::cxx
{
    bool IsExported(const clang::NamedDecl* decl);

    bool IsExported(const clang::FunctionDecl* decl);

    template<typename T>
    class Visitor;

    class ASTVisitor final : public clang::RecursiveASTVisitor<ASTVisitor>
    {
        core::Hierarchy m_Hierarchy;

    public:
        ASTVisitor();

        bool VisitCXXRecordDecl(clang::CXXRecordDecl* decl);

        bool VisitEnumDecl(clang::EnumDecl* decl);

        bool VisitFunctionDecl(clang::FunctionDecl* decl);

        [[nodiscard]]
        core::Hierarchy GetHierarchy() const;
    };

    class ASTConsumer final : public clang::ASTConsumer
    {
    public:
        void HandleTranslationUnit(clang::ASTContext& ctx) override;
    };

    class ASTFrontendAction final : public clang::ASTFrontendAction
    {
    protected:
        std::unique_ptr<clang::ASTConsumer> CreateASTConsumer(clang::CompilerInstance&, llvm::StringRef) override;
    };

    class FrontendActionFactory final : public clang::tooling::FrontendActionFactory
    {
        std::unique_ptr<clang::FrontendAction> create() override;
    };
}
