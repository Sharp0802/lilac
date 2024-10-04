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

#include "shared/frontend.h"

static std::vector<lilac::shared::FrontendAction*> backends;


lilac::shared::FrontendAction::FrontendAction(FrontendKind kind, std::string name, std::string desc)
    : m_Name(std::move(name)), m_Desc(std::move(desc)), m_Kind(kind)
{
    backends.push_back(this);
}

const std::string& lilac::shared::FrontendAction::Name() const
{
    return m_Name;
}

const std::string& lilac::shared::FrontendAction::Desc() const
{
    return m_Desc;
}

lilac::shared::FrontendKind lilac::shared::FrontendAction::Kind() const
{
    return m_Kind;
}

const std::vector<lilac::shared::FrontendAction*>& lilac::shared::FrontendAction::GetRegistered()
{
    return backends;
}
