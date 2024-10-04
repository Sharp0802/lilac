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

#include "generatesubcommand.h"

#include "utils.h"

static std::vector<llvm::cl::OptionEnumValue> GetBackends()
{
    std::vector<llvm::cl::OptionEnumValue> values;
    for (const auto registered: lilac::shared::BackendAction::GetRegistered())
        values.emplace_back(registered->Name(), registered->Kind(), registered->Desc());
    return values;
}

lilac::GenerateSubCommand::GenerateSubCommand()
    : SubCommand("bind", "Generate binding-assembly from symbols"),
      OutputPath{
          "o",
          llvm::cl::ValueRequired,
          llvm::cl::init("."),
          llvm::cl::cat(GetCategory()),
          llvm::cl::sub(*this),
          llvm::cl::value_desc("dir"),
          llvm::cl::desc("Place the output into <dir>")
      },
      Language{
          "to",
          llvm::cl::Required,
          llvm::cl::desc("Specify the language ID of the binding-assembly"),
          llvm::cl::cat(GetCategory()),
          llvm::cl::sub(*this),
          llvm::cl::ValuesClass(GetInitializerList(GetBackends()))
      },
      SymbolFile{
          "sym",
          llvm::cl::Required,
          llvm::cl::value_desc("file"),
          llvm::cl::desc("Read symbols from <file>"),
          llvm::cl::cat(GetCategory()),
          llvm::cl::sub(*this)
      },
      ReferenceName{
          "name",
          llvm::cl::Required,
          llvm::cl::desc("<library-name>"),
          llvm::cl::cat(GetCategory()),
          llvm::cl::sub(*this)
      },
      Argv{
          llvm::cl::Sink,
          llvm::cl::desc("<language-specific-args>..."),
          llvm::cl::cat(GetCategory()),
          llvm::cl::sub(*this)
      }
{
}

int lilac::GenerateSubCommand::Run()
{
    for (const auto registered: shared::BackendAction::GetRegistered())
    {
        if (registered->Kind() == Language.getValue())
            return registered->Run(
                SymbolFile.getValue(),
                ReferenceName.getValue(),
                OutputPath.getValue(),
                Argv);
    }

    llvm::errs() << "No such language-id '" << Language.ValueStr << "'\n";
    return -1;
}
