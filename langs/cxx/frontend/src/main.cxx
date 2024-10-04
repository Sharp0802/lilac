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
#include "shared/frontend.h"
#include "shared/version.h"

#if __unix__
#include <glob.h>
#endif

class CXXFrontendAction final : public lilac::shared::FrontendAction
{
public:
    CXXFrontendAction()
        : FrontendAction(lilac::shared::F_CXX, "c++", "C++ frontend module")
    {
    }

    [[nodiscard]]
    int Run(std::string output, std::vector<std::string> argv) const override
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
        std::vector<const char*> buffer(argv.size() + 1);
        buffer[0] = "lilac-cxx";
        for (int i = 0; i < argv.size(); ++i)
            buffer[i + 1] = argv[i].c_str();


        int argc = buffer.size();

        auto expectedParser = clang::tooling::CommonOptionsParser::create(argc, buffer.data(), llvm::cl::getGeneralCategory());
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

        std::ofstream ofs(output);
        ofs << static_cast<std::string>(frxml::doc{ root });

        return 0;
    }
};

[[maybe_unused]]
static CXXFrontendAction _;
