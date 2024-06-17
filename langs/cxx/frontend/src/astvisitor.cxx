#include "astvisitor.h"
#include "exportattr.h"

namespace
{
    bool IsExported(const clang::Decl* decl)
    {
        const auto annotate = decl->getAttr<clang::AnnotateAttr>();
        return
            annotate &&
            annotate->getAnnotation().str() ==
            lilac::cxx::ExportAttrInfo::AttrMangling;
    }
}

namespace lilac::cxx
{
    LilacASTVisitor::LilacASTVisitor(LilacContext& context)
        : m_Context(&context)
    {
    }

    void LilacASTVisitor::SetSema(clang::Sema& sema)
    {
        m_Sema = &sema;
    }

    // ReSharper disable once CppMemberFunctionMayBeConst
    // ReSharper disable once CppDFAConstantFunctionResult
    bool LilacASTVisitor::VisitFunctionDecl(clang::FunctionDecl* decl)
    {
        if (!IsExported(decl))
            return true;

        m_Context->Functions.push_back(decl);

        return true;
    }

    // ReSharper disable once CppMemberFunctionMayBeConst
    // ReSharper disable once CppDFAConstantFunctionResult
    bool LilacASTVisitor::VisitEnumDecl(clang::EnumDecl* decl)
    {
        if (!IsExported(decl))
            return true;

        m_Context->Types.push_back(decl);
        return true;
    }
}
