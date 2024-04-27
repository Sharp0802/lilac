#include <utility>

#include "enumtype.h"

namespace lilac
{
    EnumConstant::EnumConstant(std::string name, uint64_t u)
        : Name(std::move(name)), U(u)
    {
    }

    EnumConstant::EnumConstant(std::string name, int64_t s)
        : Name(std::move(name)), S(s)
    {
    }

    EnumType::EnumType(
        std::string name,
        std::shared_ptr<Type> type,
        std::vector<EnumConstant> constants)
        : Type(LLType_Enum, {}, std::move(type)),
          m_Name(std::move(name)),
          m_Constants(std::move(constants))
    {
    }

    const std::string& EnumType::getName() const
    {
        return m_Name;
    }

    const EnumType::ConstantVector& EnumType::getConstants() const
    {
        return m_Constants;
    }
}
