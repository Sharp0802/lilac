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

#include "shared/exception.h"

#include <llvm/Support/raw_ostream.h>

lilac::shared::exception::exception(std::string msg, const frxml::dom& dom) noexcept
    : m_DOM(dom), m_Message(std::move(msg))
{
}

const char* lilac::shared::exception::what() const noexcept
{
    return m_Message.c_str();
}

void lilac::shared::exception::print() const
{
    llvm::errs() << m_Message << '\n';
}

void lilac::shared::exception::print(const frxml::dom& location) const
{
    llvm::errs()
        << m_Message
        << "\n=== XML DUMP ===\n"
        << frxml::doc{ location }
        << "=== END DUMP ===\n";
}
