#include <utility>
#include <sstream>
#include <fstream>
#include <memory>
#include <stack>
#include <vector>
#include <ranges>
#include <algorithm>

#include "hierarchy.h"

namespace lilac::core
{
    std::string Replace(std::string str, const std::string& from, const std::string& to)
    {
        size_t start_pos;
        while ((start_pos = str.find(from)) != std::string::npos)
            str.replace(start_pos, from.length(), to);
        return str;
    }

    void Hierarchy::ToString(std::stringstream& ss, const std::size_t indent) const
    {
        const std::string indents(indent, '\t');

        ss << indents << Kind << ',' << Name << ',' << ActualName;
        if (Kind == HOK_Constant)
        {
            ss << ',' << Constant.Signed << ',' << static_cast<int>(Constant.Size) << ',';
            if (Constant.Signed)
                ss << Constant.ConstantSigned;
            else
                ss << Constant.ConstantUnsigned;
        }

        ss << '\n';

        for (const auto& member: Members)
            member.ToString(ss, indent + 1);
    }

    Hierarchy::Hierarchy(const HierarchyKind kind, std::string actualName, std::string name)
        : ActualName(std::move(actualName)),
          Name(std::move(name)),
          Kind(kind),
          Constant()
    {
    }

    std::string Hierarchy::ToString() const
    {
        std::stringstream ss;
        ToString(ss, 0);
        return ss.str();
    }

    bool Hierarchy::operator==(const Hierarchy& obj) const
    {
        return ActualName == obj.ActualName;
    }

    bool Hierarchy::operator!=(const Hierarchy& obj) const
    {
        return ActualName != obj.ActualName;
    }

    bool Hierarchy::operator<(const Hierarchy& obj) const
    {
        return ActualName < obj.ActualName;
    }

    Hierarchy Hierarchy::CreateFromFile(const std::string& path)
    {
        std::ifstream ifs(path);

        auto root = std::make_shared<Hierarchy>(HOK_Root, "%root", "%root");

        std::stack<Hierarchy*> stack;
        stack.push(root.get());

        Hierarchy* previous = nullptr;
        size_t previousIndent = 0;

        for (std::string token; std::getline(ifs, token);)
        {
            size_t indent = 0;
            for (; token[indent] == '\t'; indent++) {}

            if (indent > previousIndent)
            {
                stack.push(previous ? previous : root.get());
                previousIndent = indent;
            }
            else if (indent < previousIndent)
            {
                stack.pop();
                previousIndent = indent;
            }


            std::vector<std::string> tokens;
            tokens.reserve(std::ranges::count(token, ',') + 1);
            std::stringstream ss(token.erase(0, indent));
            while (std::getline(ss, token, ','))
                tokens.push_back(token);

            auto kind = static_cast<HierarchyKind>(std::stoi(tokens[0]));
            auto name = tokens[1];
            auto actual = tokens[2];

            if (kind == HOK_Root)
                continue;

            Hierarchy h{ kind, actual, name };

            constexpr size_t DataOffset = 3;
            switch (kind)
            {
                case HOK_Constant:
                {
                    h.Constant.Signed = std::stoi(tokens[DataOffset]);
                    h.Constant.Size = std::stoi(tokens[DataOffset + 1]);
                    if (h.Constant.Signed)
                        h.Constant.ConstantSigned = std::stoll(tokens[DataOffset + 2]);
                    else
                        h.Constant.ConstantUnsigned = std::stoull(tokens[DataOffset + 2]);
                    break;
                }

                default:
                    break;
            }

            auto [iter, _] = stack.top()->Members.emplace(h);
            previous = const_cast<Hierarchy*>(&*iter);
        }

        return *root;
    }
}
