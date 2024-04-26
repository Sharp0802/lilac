#ifndef ENUM_H
#define ENUM_H

#include <cstdint>
#include <string>
#include <vector>

#include "type.h"

namespace lilac
{
    struct EnumConstant
    {
        std::string Name;

        union
        {
            uint64_t U;
            int64_t S;
        };

        EnumConstant(std::string name, uint64_t u);

        EnumConstant(std::string name, int64_t s);
    };

    class EnumType : public Type
    {
    public:
        using ConstantVector = std::vector<EnumConstant>;

    private:
        std::string m_Name;
        ConstantVector m_Constants;

    public:
        EnumType(std::string name, const Type& type, std::initializer_list<EnumConstant> constants);

        [[nodiscard]]
        const std::string& getName() const;

        [[nodiscard]]
        const ConstantVector& getConstants() const;
    };
}

#endif //ENUM_H
