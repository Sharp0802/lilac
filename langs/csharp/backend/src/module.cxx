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

#include <iostream>

#include "pch.h"
#include "shared/backend.h"
#include "module.h"

#include "interfacevisitor.h"
#include "shared/exception.h"

std::map<int, std::string> errorMap = {
    { frxml::S_OK, "Operation completed successfully" },
    { frxml::E_NONAME, "Identifier expected" },
    { frxml::E_NOEQ, "Equal sign(=) expected" },
    { frxml::E_NOQUOTE, "Quote(\", \') expected" },
    { frxml::E_QUOTENOTCLOSED, "Quote not closed" },
    { frxml::E_INVCHAR, "Invalid character detected" },
    { frxml::E_NOTAG, "Tag expected" },
    { frxml::E_TAGNOTCLOSED, "Tag not closed" },
    { frxml::E_DUPATTR, "Duplicated attributes detected" },
    { frxml::E_ELEMNOTCLOSED, "Element not closed" },
    { frxml::E_INVETAG, "Invalid ETag" }
};

lilac::csharp::CSharpBackendAction::CSharpBackendAction(): BackendAction(shared::CSharp, "csharp", "C# backend module")
{
}

int lilac::csharp::CSharpBackendAction::Run(std::string confPath, std::string libPath, std::string outPath) const
{
    std::stringstream input;
    try
    {
        std::ifstream ifs(confPath);
        input << ifs.rdbuf();
    }
    catch (const std::ifstream::failure& e)
    {
        std::cerr << "Error occurred while opening file: " << e.what() << std::endl;
        return 1;
    }

    const std::string& source = input.str();

    frxml::doc doc(source);
    if (!doc)
    {
        const char* begin  = doc.error().source;
        size_t      length = 1;

        for (auto i = 0; i < 5 && begin > &source[0]; ++i)
            begin--;
        for (auto i = 0; i < 5 && begin + length < &source[source.size() - 1]; ++i)
            length++;

        std::cerr
            << "Error occurred while parsing XML: "
            << errorMap[doc.error().code]
            << " (" << std::string_view(begin, length) << ")\n";
    }

    try
    {
        std::ofstream ofs(outPath);

        VisitContext ctx{ "", libPath, ofs };
        CSharpAssemblyInterfaceVisitor visitor;
        visitor.Begin(ctx, doc.root() /* there is neither parent nor null */, doc.root(), 0);
    }
    catch (const shared::exception& e)
    {
        e.print();
    }
    catch (const std::ofstream::failure& e)
    {
        std::cerr << "Error occurred while opening file: " << e.what() << std::endl;
        return 1;
    }

    return 0;
}

[[maybe_unused]]
static lilac::csharp::CSharpBackendAction Action;
