#pragma once

#include <clang/AST/Decl.h>
#include <clang/AST/RecursiveASTVisitor.h>

#include "visitor.h"
#include "lilac-core/hierarchy.h"

namespace lilac::cxx
{
    std::string GetActualName(const clang::FunctionDecl* decl);

    template<>
    class Visitor<clang::FunctionDecl> : public clang::RecursiveASTVisitor<Visitor<clang::FunctionDecl>>
    {
        clang::FunctionDecl* m_Decl;
        core::Hierarchy m_Hierarchy;

    public:
        explicit Visitor(clang::FunctionDecl* decl);

        [[nodiscard]]
        core::Hierarchy GetHierarchy() const;
    };
}
