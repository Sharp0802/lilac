#pragma once
#include "exportattr.h"

namespace lilac::cxx
{
    [[nodiscard]]
    std::string GetName(const clang::NamedDecl* decl);

    [[nodiscard]]
    std::string GetDisplayName(const clang::NamedDecl* decl);

    [[nodiscard]]
    std::string GetTypeString(clang::QualType decl);

    template<typename T>
    concept NamedDeclContext =
        std::is_base_of<clang::DeclContext, T>::value &&
        std::is_base_of<clang::NamedDecl, T>::value;

    template<NamedDeclContext T>
    [[nodiscard]]
    std::string GetDisplayPath(T* decl)
    {
        std::stack<clang::DeclContext*> stk;
        for (
            auto* ctx = clang::cast<clang::DeclContext>(decl);
            ctx && ctx->getDeclKind() != clang::Decl::TranslationUnit;
            ctx = ctx->getParent())
        {
            stk.push(ctx);
        }

        std::stringstream ss;
        for (; !stk.empty(); stk.pop())
            ss << '/' << clang::cast<clang::NamedDecl>(stk.top())->getNameAsString();
        return ss.str();
    }

    [[nodiscard]]
    std::string GetFunctionTypeReg(const clang::FunctionDecl* decl);

    [[nodiscard]]
    clang::FunctionDecl* FindMethod(const clang::CXXRecordDecl* decl, const std::string& name);
}
