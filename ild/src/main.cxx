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

#include "pch.h"

#include "csharp/module.h"
#include "shared/backend.h"
#include "shared/version.h"

enum ModuleKind
{
    CSharp
};

llvm::cl::OptionCategory Category("LILAC Backend Options");

llvm::cl::opt<ModuleKind> Module(
    "module",
    llvm::cl::Required,
    llvm::cl::cat(Category),
    llvm::cl::desc("Load the named module"),
    values(
        clEnumValN(CSharp, "csharp", "C# backend module")
    ));

llvm::cl::opt<std::string> XMLPathOpt(
    "i",
    llvm::cl::Required,
    llvm::cl::cat(Category),
    llvm::cl::value_desc("file"),
    llvm::cl::desc("Load interface information from <file>"));
llvm::cl::opt<std::string> LibPathOpt(
    "l",
    llvm::cl::Required,
    llvm::cl::cat(Category),
    llvm::cl::value_desc("name"),
    llvm::cl::desc("Use <name> as interface bridge library (*.dll, *.so etc.)"));
llvm::cl::opt<std::string> OutPathOpt(
    "o",
    llvm::cl::Required,
    llvm::cl::cat(Category),
    llvm::cl::value_desc("file"),
    llvm::cl::desc("Place the output into <file>"));

int main(int argc, const char* argv[])
{
    llvm::cl::SetVersionPrinter([](llvm::raw_ostream& ost)
    {
        ost << "ild (ILD) " << LILAC_DATE << '\n' << LILAC_SHORT_LICENSE;
    });
    llvm::cl::HideUnrelatedOptions(Category);
    llvm::cl::ParseCommandLineOptions(argc, argv);

    std::function<int(const std::string&, const std::string&, const std::string&)> caller;
    switch (Module.getValue())
    {
        case CSharp: caller = MODULE_REF(csharp);
            break;
    }

    return caller(XMLPathOpt.getValue(), LibPathOpt.getValue(), OutPathOpt.getValue());
}
