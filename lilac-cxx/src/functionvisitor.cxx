#include "functionvisitor.h"

namespace lilac::cxx
{
    std::string GetActualName(const clang::FunctionDecl* decl)
    {
        clang::ASTNameGenerator gen{ decl->getASTContext() };
        return gen.getName(decl);
    }

    void ResolveFunctionIntoHierarchy(core::Hierarchy& origin, clang::FunctionDecl* decl)
    {
        auto ret = GetTypeString(decl->getReturnType().getTypePtr(), decl->getASTContext());
        if (!ret)
        {
            llvm::errs()
                << "Couldn't resolve return type of function '" << decl->getQualifiedNameAsString()
                << "'; may lead to generate invalid IH!\n";
            return;
        }
        origin.GetFunctionData().Type = ret.value();

        size_t i = 0;
        for (auto parameter: decl->parameters())
        {
            core::Hierarchy h{
                core::HOK_Parameter,
                parameter->getNameAsString(),
                parameter->getNameAsString()
            };

            auto type = parameter->getType().getTypePtr();
            auto str  = GetTypeString(type, parameter->getASTContext());

            if (!str)
            {
                llvm::errs()
                    << "Couldn't resolve type for parameter '" << parameter->getNameAsString()
                    << "' in function '" << decl->getQualifiedNameAsString()
                    << "'; may lead to generate invalid IH!\n";
                return;
            }

            h.GetParameterData().Index = i++;
            h.GetParameterData().Type  = str.value();

            origin.Members.emplace(h);
        }
    }

    Visitor<clang::FunctionDecl>::Visitor(clang::FunctionDecl* decl)
        : m_Decl(decl),
          m_Hierarchy(core::HOK_Function, GetActualName(decl), decl->getNameAsString())
    {
        ResolveFunctionIntoHierarchy(m_Hierarchy, decl);
    }

    core::Hierarchy Visitor<clang::FunctionDecl>::GetHierarchy() const
    {
        return m_Hierarchy;
    }
}
