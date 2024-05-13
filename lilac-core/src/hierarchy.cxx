#include <utility>
#include <sstream>

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
}
