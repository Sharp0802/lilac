#include "pch.h"

static bool ShouldExport(
    clang::Sema&             sema,
    const clang::ParsedAttr& attr,
    const clang::NamedDecl*  decl)
{
    auto&      diag              = sema.getDiagnostics();
    const auto invalidVisibility = diag.getCustomDiagID(
        clang::DiagnosticsEngine::Warning,
        "declaration cannot be exported because of its visibility; exporting skipped!");
    const auto invalidAccess = diag.getCustomDiagID(
        clang::DiagnosticsEngine::Warning,
        "non-public declaration cannot be exported; exporting skipped!");

    auto vis = decl->getExplicitVisibility(clang::isa<clang::RecordDecl>(decl)
        ? clang::TypeDecl::VisibilityForType
        : clang::TypeDecl::VisibilityForValue);

    if (vis.has_value() && vis != clang::DefaultVisibility)
    {
        sema.Diag(attr.getLoc(), invalidVisibility);
        return false;
    }
    // TODO: How to get default visibility (-fvisibility=?)

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

namespace
{
    class ExportAttrInfo final : public clang::ParsedAttrInfo
    {
    public:
        ExportAttrInfo();

        bool diagAppertainsToDecl(
            clang::Sema&             sema,
            const clang::ParsedAttr& attr,
            const clang::Decl*       decl) const override;

        AttrHandling handleDeclAttribute(
            clang::Sema&             sema,
            clang::Decl*             decl,
            const clang::ParsedAttr& attr) const override;


    };

#define ATTR_NAME "apiexport"

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
}

[[maybe_unused]]
static clang::ParsedAttrInfoRegistry::Add<ExportAttrInfo> X("apiexport", "");
