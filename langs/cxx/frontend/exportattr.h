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
