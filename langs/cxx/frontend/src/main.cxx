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
#include "pluginaction.h"
#include "shared/version.h"

static llvm::cl::OptionCategory Category("LILAC Frontend Options");

static llvm::cl::opt<std::string> Output(
    "o",
    llvm::cl::Required,
    llvm::cl::ValueRequired,
    llvm::cl::value_desc("file"),
    llvm::cl::desc("Place output into <file>."),
    llvm::cl::cat(Category));

int main(int argc, const char* argv[])
{
    llvm::cl::SetVersionPrinter([](llvm::raw_ostream& ost)
    {
        ost << "lilac-cxx (LILAC C++) " << LILAC_DATE << '\n' << LILAC_SHORT_LICENSE;
    });

    auto expectedParser = clang::tooling::CommonOptionsParser::create(argc, argv, Category);
    if (!expectedParser)
    {
        llvm::errs() << expectedParser.takeError();
        return 1;
    }

    auto& parser = expectedParser.get();

    clang::tooling::ClangTool tool(parser.getCompilations(), parser.getSourcePathList());
    if (const auto ret = tool.run(clang::tooling::newFrontendActionFactory<lilac::cxx::LilacAction>().get()))
        return ret;

    auto& root = lilac::cxx::GetDOMRoot();

    std::ofstream ofs(Output.getValue());
    ofs << static_cast<std::string>(frxml::doc{ root });
}
