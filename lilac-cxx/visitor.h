#pragma once

#include "pch.h"
#include "lilac-core/hierarchy.h"

namespace lilac::cxx
{
    bool IsExported(const clang::NamedDecl* decl);

    bool IsExported(const clang::FunctionDecl* decl);

    const clang::Type* DesugarType(const clang::Type* t);

    std::optional<std::string> GetTypeString(const clang::Type* type, const clang::ASTContext& context);

    std::string GetActualName(const clang::CXXRecordDecl* decl);

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
        std::string m_Out;

    public:
        explicit ASTConsumer(std::string out);

        void HandleTranslationUnit(clang::ASTContext& ctx) override;
    };

    class ASTFrontendAction final : public clang::ASTFrontendAction
    {
        std::string m_Out;

    protected:
        std::unique_ptr<clang::ASTConsumer> CreateASTConsumer(clang::CompilerInstance&, llvm::StringRef) override;

    public:
        explicit ASTFrontendAction(std::string out);
    };

    class FrontendActionFactory final : public clang::tooling::FrontendActionFactory
    {
        std::string m_Out;

        std::unique_ptr<clang::FrontendAction> create() override;

    public:
        explicit FrontendActionFactory(std::string out);
    };
}
