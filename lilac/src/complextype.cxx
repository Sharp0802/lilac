#include <utility>

#include "complextype.h"

namespace lilac
{
    ComplexType::ComplexType(
        std::string name,
        size_t size,
        size_t alignment)
        : Type(LLType_Complex, {}, nullptr),
          m_Name(std::move(name)),
          m_Size(size),
          m_Alignment(alignment)
    {
    }

    std::string ComplexType::getName() const
    {
        return m_Name;
    }

    size_t ComplexType::getSize() const
    {
        return m_Size;
    }

    size_t ComplexType::getAlignment() const
    {
        return m_Alignment;
    }

    ResolvedType::ResolvedType(
        std::string name,
        size_t size,
        size_t alignment,
        std::vector<Property> properties,
        std::vector<Method> methods)
        : ComplexType(std::move(name), size, alignment),
          m_Properties(std::move(properties)),
          m_Methods(std::move(methods))
    {
    }

    std::vector<Property>& ResolvedType::getProperties()
    {
        return m_Properties;
    }

    std::vector<Method>& ResolvedType::getMethods()
    {
        return m_Methods;
    }
}
