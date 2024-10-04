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

#include "subcommand.h"
#include "shared/frontend.h"

namespace lilac
{
    class CompileSymbolSubCommand final : public SubCommand
    {
        llvm::cl::opt<shared::FrontendKind> Language;
        llvm::cl::opt<std::string>          OutputFile;
        llvm::cl::list<std::string>         Argv;

    public:
        CompileSymbolSubCommand();

        int Run() override;
    };
}
