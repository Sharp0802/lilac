#pragma once

#include <clang/AST/Decl.h>
#include <clang/AST/RecursiveASTVisitor.h>

#include "visitor.h"
#include "lilac-core/hierarchy.h"

namespace lilac::cxx
{
    template<>
    class Visitor<clang::EnumDecl> : public clang::RecursiveASTVisitor<Visitor<clang::EnumDecl>>
    {
        clang::EnumDecl* m_Decl;
        core::Hierarchy m_Hierarchy;

    public:
        explicit Visitor(clang::EnumDecl* decl);

        bool VisitEnumConstantDecl(clang::EnumConstantDecl* decl);

        [[nodiscard]]
        core::Hierarchy GetHierarchy() const;
    };
}
