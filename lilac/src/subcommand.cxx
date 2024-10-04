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

#include "subcommand.h"

llvm::cl::OptionCategory Category("LILAC Options");

lilac::SubCommand::SubCommand(const char* command, const char* desc)
    : m_SubCommand(command, desc)
{
}

lilac::SubCommand::operator llvm::cl::SubCommand&()
{
    return m_SubCommand;
}

lilac::SubCommand::operator bool() const
{
    return m_SubCommand.operator bool();
}

llvm::cl::OptionCategory& lilac::SubCommand::GetCategory()
{
    return Category;
}
