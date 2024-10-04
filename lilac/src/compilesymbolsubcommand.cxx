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

#include "utils.h"
#include "shared/frontend.h"

static std::vector<llvm::cl::OptionEnumValue> GetFrontends()
{
    std::vector<llvm::cl::OptionEnumValue> values;
    for (const auto registered: lilac::shared::FrontendAction::GetRegistered())
        values.emplace_back(registered->Name(), registered->Kind(), registered->Desc());
    return values;
}

lilac::CompileSymbolSubCommand::CompileSymbolSubCommand()
    : SubCommand("sym", "Compile target-assembly into symbols"),
      Language{
          "from",
          llvm::cl::Required,
          llvm::cl::ValuesClass(GetInitializerList(GetFrontends())),
          llvm::cl::desc("<langauge id>"),
          llvm::cl::cat(GetCategory()),
          llvm::cl::sub(*this),
      },
      OutputFile{
          "o",
          llvm::cl::Required,
          llvm::cl::desc("<output file>"),
          llvm::cl::cat(GetCategory()),
          llvm::cl::sub(*this),
      },
      Argv{
          llvm::cl::Sink,
          llvm::cl::desc("<language-specific args>..."),
          llvm::cl::cat(GetCategory()),
          llvm::cl::sub(*this)
      }
{
}

int lilac::CompileSymbolSubCommand::Run()
{
    for (const auto registered: shared::FrontendAction::GetRegistered())
    {
        if (registered->Kind() == Language.getValue())
            return registered->Run(OutputFile.getValue(), Argv);
    }

    llvm::errs() << "No such language-id '" << Language.ValueStr << "'\n";
    return -1;
}
