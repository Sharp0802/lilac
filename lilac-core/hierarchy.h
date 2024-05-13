#pragma once

#include <cstdint>
#include <set>
#include <string>

namespace lilac::core
{
    enum HierarchyKind
    {
        HOK_Root,
        HOK_Namespace,

        HOK_Type,
        HOK_Constant,

        HOK_Function,
        HOK_Method,

        HOK_Property,
        HOK_Getter,
        HOK_Setter
    };

    struct ConstantData
    {
        bool Signed;
        uint8_t Size;
        union
        {
            uint64_t ConstantUnsigned;
            int64_t ConstantSigned;
        };
    };

    struct Hierarchy
    {
    private:
        void ToString(std::stringstream& ss, std::size_t indent) const;

    public:
        const std::string ActualName;
        const std::string Name;

        const HierarchyKind Kind;
        std::set<Hierarchy> Members;

        union
        {
            ConstantData Constant;
        };

        Hierarchy(HierarchyKind kind, std::string  actualName, std::string name);

        [[nodiscard]]
        std::string ToString() const;

        bool operator==(const Hierarchy&) const;
        bool operator!=(const Hierarchy&) const;

        bool operator<(const Hierarchy&) const;

        static Hierarchy CreateFromFile(const std::string& path);
    };
}
