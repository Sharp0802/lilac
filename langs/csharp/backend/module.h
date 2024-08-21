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

//
// Created by sharp0802 on 8/21/24.
//

#pragma once

#include "shared/backend.h"

namespace lilac::csharp
{
    class CSharpBackendAction final : public shared::BackendAction
    {
    public:
        CSharpBackendAction();

        [[nodiscard]]
        int Run(std::string confPath, std::string libPath, std::string outPath) const override;
    };
}
