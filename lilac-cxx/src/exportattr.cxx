#include "pch.h"

namespace
{
    class ExportAttrInfo final : public clang::ParsedAttrInfo
    {
        inline static clang::Visibility m_DefaultTypeVisibility = clang::DefaultVisibility;
        inline static clang::Visibility m_DefaultValueVisibility = clang::DefaultVisibility;

    public:
        static bool ShouldExport(
            clang::Sema&             sema,
            const clang::ParsedAttr& attr,
            const clang::NamedDecl*  decl);

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

        bool acceptsLangOpts(
            const clang::LangOptions& LO) const override;
    };

#define ATTR_NAME "apiexport"

    bool ExportAttrInfo::ShouldExport(clang::Sema& sema, const clang::ParsedAttr& attr, const clang::NamedDecl* decl)
    {
        /************************************************************************
         * Initialize diagnostics                                               *
         ************************************************************************/
        auto&      diag              = sema.getDiagnostics();
        const auto invalidVisibility = diag.getCustomDiagID(
            clang::DiagnosticsEngine::Warning,
            "declaration cannot be exported because of its visibility; exporting skipped!");
        const auto invalidAccess = diag.getCustomDiagID(
            clang::DiagnosticsEngine::Warning,
            "non-public declaration cannot be exported; exporting skipped!");

        /************************************************************************
         * Ensure visibility of declaration to default                          *
         ************************************************************************/
        if (const auto vis = decl->getExplicitVisibility(clang::isa<clang::RecordDecl>(decl)
                ? clang::TypeDecl::VisibilityForType
                : clang::TypeDecl::VisibilityForValue);
            (vis.has_value() && vis != clang::DefaultVisibility) ||
            (clang::isa<clang::RecordDecl>(decl)
                ? m_DefaultTypeVisibility
                : m_DefaultValueVisibility) != clang::DefaultVisibility)
        {
            sema.Diag(attr.getLoc(), invalidVisibility);
            return false;
        }

        /************************************************************************
         * Check access modifiers                                               *
         ************************************************************************/
        auto record = clang::cast_or_null<clang::CXXRecordDecl>(decl->getDeclContext());
        if (!record)
            return true;

        auto access = clang::AS_none;
        for (auto* member: record->decls())
        {
            if (clang::isa<clang::AccessSpecDecl>(member))
                access = clang::cast<clang::AccessSpecDecl>(member)->getAccess();
            if (clang::declaresSameEntity(member, decl))
                break;
        }

        if (access == clang::AS_public ||
            (record->isStruct() && access == clang::AS_none))
            return true;

        sema.Diag(attr.getLoc(), invalidAccess);
        return false;
    }

    ExportAttrInfo::ExportAttrInfo()
    {
        OptArgs = 0;

        static constexpr Spelling S[] = {
            { clang::ParsedAttr::AS_GNU, ATTR_NAME },
            { clang::ParsedAttr::AS_Declspec, ATTR_NAME },
            { clang::ParsedAttr::AS_C23, ATTR_NAME },
            { clang::ParsedAttr::AS_CXX11, ATTR_NAME },
            { clang::ParsedAttr::AS_CXX11, "lilac::" ATTR_NAME },
        };

        Spellings = S;
    }

    bool ExportAttrInfo::diagAppertainsToDecl(
        clang::Sema&             sema,
        const clang::ParsedAttr& attr,
        const clang::Decl*       decl) const
    {
        /************************************************************************
         * Ensure whether the declaration can be exported                       *
         ************************************************************************/
        if (!clang::isa<clang::FunctionDecl>(decl) &&
            !clang::isa<clang::MSPropertyDecl>(decl) &&
            !clang::isa<clang::RecordDecl>(decl))
        {
            const auto invalidDeclType = sema.getDiagnostics().getCustomDiagID(
                clang::DiagnosticsEngine::Warning,
                "'%0' attribute can be applied to only functions, records and properties");

            sema.Diag(attr.getLoc(), invalidDeclType) << attr;
            return false;
        }
        if (!ShouldExport(sema, attr, clang::cast<clang::NamedDecl>(decl)))
            return false;

        return true;
    }

    clang::ParsedAttrInfo::AttrHandling ExportAttrInfo::handleDeclAttribute(
        clang::Sema&             sema,
        clang::Decl*             decl,
        const clang::ParsedAttr& attr) const
    {
        if (attr.getNumArgs())
        {
            const auto anyArgs = sema.getDiagnostics().getCustomDiagID(
                clang::DiagnosticsEngine::Warning,
                "'" ATTR_NAME "' attribute accepts no argument");
            sema.Diag(attr.getLoc(), anyArgs);
            return AttributeNotApplied;
        }

        // TODO

        return AttributeApplied;
    }

    bool ExportAttrInfo::acceptsLangOpts(const clang::LangOptions& LO) const
    {
        m_DefaultTypeVisibility  = LO.getTypeVisibilityMode();
        m_DefaultValueVisibility = LO.getValueVisibilityMode();
        return true;
    }
}

[[maybe_unused]]
static clang::ParsedAttrInfoRegistry::Add<ExportAttrInfo> X("apiexport", "");
