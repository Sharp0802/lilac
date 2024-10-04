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

#include "compilesymbolsubcommand.h"
#include "generatesubcommand.h"
#include "pch.h"
#include "subcommand.h"
#include "shared/version.h"

static lilac::GenerateSubCommand      GenerateSubCommand{};
static lilac::CompileSymbolSubCommand CompileSymbolSubCommand{};

int main(int argc, const char* argv[])
{
    llvm::cl::SetVersionPrinter([](llvm::raw_ostream& ost)
    {
        ost << "lilac (LIALC) " << LILAC_DATE << '\n' << LILAC_SHORT_LICENSE;
    });
    llvm::cl::HideUnrelatedOptions(lilac::SubCommand::GetCategory());
    llvm::cl::ParseCommandLineOptions(argc, argv);

    if (GenerateSubCommand)
        return GenerateSubCommand.Run();
    if (CompileSymbolSubCommand)
        return CompileSymbolSubCommand.Run();

    llvm::errs() << "Subcommand is required\n";
    return -1;
}
