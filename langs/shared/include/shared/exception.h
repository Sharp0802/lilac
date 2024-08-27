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

#include <string>

#include "sub/frxml/frxml/frxml.h"

namespace lilac::shared
{
    class exception final : public std::exception
    {
        const frxml::dom& m_DOM;
        std::string       m_Message;

    public:
        exception(std::string msg, const frxml::dom& dom) noexcept;

        [[nodiscard]]
        const char* what() const noexcept override;

        void print() const;

        void print(const frxml::dom& location) const;
    };
}
