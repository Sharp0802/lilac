#pragma once

#include "pch.h"

namespace lilac::cxx
{
    class ExportAttrInfo final : public clang::ParsedAttrInfo
    {
    public:
        static constexpr std::string AttrMangling = "__lilac_export";

        /************************************************************************
         * derived from clang::ParsedAttrInfo                                   *
         ************************************************************************/
        ExportAttrInfo();

        bool diagAppertainsToDecl(
            clang::Sema&             sema,
            const clang::ParsedAttr& attr,
            const clang::Decl*       decl) const override;

        AttrHandling handleDeclAttribute(
            clang::Sema&             sema,
            clang::Decl*             decl,
            const clang::ParsedAttr& attr) const override;

        [[nodiscard]]
        bool acceptsLangOpts(
            const clang::LangOptions& LO) const override;
    };
}
