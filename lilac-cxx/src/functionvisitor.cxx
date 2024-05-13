#include <clang/AST/Mangle.h>

#include "functionvisitor.h"

namespace lilac::cxx
{
    std::string GetActualName(const clang::FunctionDecl* decl)
    {
        clang::ASTNameGenerator gen{ decl->getASTContext() };
        return gen.getName(decl);
    }

    Visitor<clang::FunctionDecl>::Visitor(clang::FunctionDecl* decl)
        : m_Decl(decl),
          m_Hierarchy(core::HOK_Function, GetActualName(decl), decl->getNameAsString())
    {
    }

    core::Hierarchy Visitor<clang::FunctionDecl>::GetHierarchy() const
    {
        return m_Hierarchy;
    }
}
