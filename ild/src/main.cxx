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

#include "shared/backend.h"
#include "shared/version.h"

llvm::cl::OptionCategory Category("LILAC Backend Options");

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

template<typename T>
std::initializer_list<T> GetInitializerList(std::vector<T> vector)
{
    union
    {
        std::initializer_list<T> list = {};
        struct
        {
            T* begin;
            size_t size;
        };
    };

    begin = &vector[0];
    size  = vector.size();

    return list;
}

int main(int argc, const char* argv[])
{
    std::vector<llvm::cl::OptionEnumValue> values;
    for (const auto registered: lilac::shared::BackendAction::GetRegistered())
        values.emplace_back(registered->Name(), registered->Kind(), registered->Desc());

    llvm::cl::opt<lilac::shared::ModuleKind> module(
        "module",
        llvm::cl::Required,
        llvm::cl::cat(Category),
        llvm::cl::desc("Load the named module"),
        llvm::cl::ValuesClass(GetInitializerList(values)));

    llvm::cl::SetVersionPrinter([](llvm::raw_ostream& ost)
    {
        ost << "ild (ILD) " << LILAC_DATE << '\n' << LILAC_SHORT_LICENSE;
    });
    llvm::cl::HideUnrelatedOptions(Category);
    llvm::cl::ParseCommandLineOptions(argc, argv);

    for (const auto registered: lilac::shared::BackendAction::GetRegistered())
    {
        if (registered->Kind() == module.getValue())
            return registered->Run(XMLPathOpt.getValue(), LibPathOpt.getValue(), OutPathOpt.getValue());
    }

    llvm::errs() << "No such module '" << module.ValueStr << "'\n";
    return 1;
}
