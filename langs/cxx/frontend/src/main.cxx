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

        std::vector<const char*> buffer(argv.size() + 1);
        buffer[0] = "lilac-cxx";
        for (int i = 0; i < argv.size(); ++i)
            buffer[i + 1] = argv[i].c_str();

#if __unix__
        // NOTE: clang couldn't find `stddef.h` on UNIXen

        // Find candidates of clang with glob
        glob_t glob{};
        if (auto ret = ::glob("/usr/lib/clang/*/include", GLOB_TILDE, nullptr, &glob))
        {
            globfree(&glob);
            llvm::errs()
                << "glob() failed with return value: " << ret << '\n'
                << "is Clang installed with proper directory?";
            return 1;
        }

        std::vector<std::string> candidates(glob.gl_pathc);
        for (auto i = 0; i < glob.gl_pathc; ++i)
        {
            llvm::outs() << "Found candidate Clang installation: " << glob.gl_pathv[i] << '\n';
            candidates[i] = glob.gl_pathv[i];
        }

        // Use latest version of clang
        struct
        {
            static constexpr long int GetIndex(const std::string& t)
            {
                return strtol(t.data() + 15, nullptr, 10);
            }

            constexpr bool operator()(const std::string& lhs, const std::string& rhs) const
            {
                return GetIndex(lhs) > GetIndex(rhs);
            }
        } comp;
        std::ranges::sort(candidates, comp);

        llvm::outs() << "Selected Clang installation: " << candidates[0] << '\n';

        // Add include path
        buffer.push_back("--");
        buffer.push_back("-I");
        buffer.push_back(candidates[0].data());

        globfree(&glob);
#endif

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
