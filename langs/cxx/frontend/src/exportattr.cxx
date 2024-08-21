/*
 * Copyright (C) 2024  Yeong-won Seo
 *
 * This file is part of LILAC.
 *
 * LILAC is free software: you can redistribute it and/or modify it under
 * the terms of the GNU General Public License as published by the Free
 * Software Foundation, either version 3, or (at your option) any later
 * version.
 *
 * LILAC is distributed in the hope that it will be useful, but WITHOUT ANY
 * WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License
 * for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <https://www.gnu.org/licenses/>.
 */

#include "exportattr.h"
#include "pch.h"

#define WARNMSG "; exporting skipped!"

namespace
{
    clang::Visibility g_DefaultTypeVisibility  = clang::DefaultVisibility;
    clang::Visibility g_DefaultValueVisibility = clang::DefaultVisibility;

    bool ShouldExport(clang::Sema& sema, const clang::NamedDecl* decl)
    {
        /************************************************************************
         * Initialize diagnostics                                               *
         ************************************************************************/
        auto&      diag              = sema.getDiagnostics();
        const auto invalidVisibility = diag.getCustomDiagID(
            clang::DiagnosticsEngine::Warning,
            "declaration cannot be exported because of its visibility" WARNMSG);
        const auto invalidAccess = diag.getCustomDiagID(
            clang::DiagnosticsEngine::Warning,
            "non-public declaration cannot be exported" WARNMSG);
        const auto invalidSignature = sema.getDiagnostics().getCustomDiagID(
            clang::DiagnosticsEngine::Warning,
            "%0 function '%1' cannot be exported" WARNMSG);

        /************************************************************************
         * Ensure visibility of declaration to default                          *
         ************************************************************************/
        if (const auto vis = decl->getExplicitVisibility(clang::isa<clang::RecordDecl>(decl)
                ? clang::TypeDecl::VisibilityForType
                : clang::TypeDecl::VisibilityForValue);
            (vis.has_value() && vis != clang::DefaultVisibility) ||
            (clang::isa<clang::RecordDecl>(decl)
                ? g_DefaultTypeVisibility
                : g_DefaultValueVisibility) != clang::DefaultVisibility)
        {
            sema.Diag(decl->getLocation(), invalidVisibility);
            return false;
        }

        /************************************************************************
         * Check function constraints                                           *
         ************************************************************************/
        if (const auto fn = clang::dyn_cast<clang::FunctionDecl>(decl))
        {
            if (fn->isConsteval() || fn->isConstexpr())
            {
                sema.Diag(fn->getDefaultLoc(), invalidSignature) << "consteval/constexpr" << decl;
                return false;
            }
            if (fn->isVariadic())
            {
                sema.Diag(fn->getDefaultLoc(), invalidSignature) << "variadic" << decl;
                return false;
            }
            if (fn->isDeleted())
            {
                sema.Diag(fn->getDefaultLoc(), invalidSignature) << "deleted" << decl;
                return false;
            }
            if (fn->isInlined())
            {
                sema.Diag(fn->getDefaultLoc(), invalidSignature) << "inlined" << decl;
                return false;
            }
            if (fn->isPureVirtual())
            {
                sema.Diag(fn->getDefaultLoc(), invalidSignature) << "pure-virtual" << decl;
                return false;
            }
        }

        /************************************************************************
         * Check access modifiers                                               *
         ************************************************************************/
        auto record = clang::dyn_cast<clang::CXXRecordDecl>(decl->getDeclContext());
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

        sema.Diag(decl->getLocation(), invalidAccess);
        return false;
    }
}

namespace lilac::cxx
{
    ExportAttrInfo::ExportAttrInfo()
    {
        OptArgs = 0;

        static constexpr Spelling S[] = {
            { clang::ParsedAttr::AS_GNU, "__lilac_export" },
            { clang::ParsedAttr::AS_Declspec, "__lilac_export" },
            { clang::ParsedAttr::AS_C23, "__lilac_export" },
            { clang::ParsedAttr::AS_C23, "lilac::export" },
            { clang::ParsedAttr::AS_CXX11, "__lilac_export" },
            { clang::ParsedAttr::AS_CXX11, "lilac::export" },
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
            !clang::isa<clang::RecordDecl>(decl) &&
            !clang::isa<clang::EnumDecl>(decl))
        {
            const auto invalidDeclType = sema.getDiagnostics().getCustomDiagID(
                clang::DiagnosticsEngine::Warning,
                "'%0' attribute can be applied to only functions, enumerations and records" WARNMSG);

            sema.Diag(attr.getLoc(), invalidDeclType) << attr;
            return false;
        }
        if (!ShouldExport(sema, clang::cast<clang::NamedDecl>(decl)))
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
                "'%0' attribute accepts no argument" WARNMSG);
            sema.Diag(attr.getLoc(), anyArgs) << attr;
            return AttributeNotApplied;
        }

        if (auto* const tag = clang::dyn_cast<clang::TagDecl>(decl))
        {
            tag->addAttr(clang::AnnotateTypeAttr::Create(sema.Context, AttrMangling, nullptr, 0));
        }
        else if (auto* const function = clang::dyn_cast<clang::FunctionDecl>(decl))
        {
            function->addAttr(clang::AnnotateAttr::Create(sema.Context, AttrMangling, nullptr, 0));
        }
        else
        {
            assert(false && "invalid decl type");
        }

        return AttributeApplied;
    }

    bool ExportAttrInfo::acceptsLangOpts(const clang::LangOptions& LO) const
    {
        g_DefaultTypeVisibility  = LO.getTypeVisibilityMode();
        g_DefaultValueVisibility = LO.getValueVisibilityMode();
        return true;
    }
}

[[maybe_unused]]
static clang::ParsedAttrInfoRegistry::Add<lilac::cxx::ExportAttrInfo> X("apiexport", "");
