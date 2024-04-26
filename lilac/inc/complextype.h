#ifndef COMPLEXTYPE_H
#define COMPLEXTYPE_H

#include <vector>

#include "method.h"
#include "type.h"

namespace lilac
{
    class ComplexType : public Type
    {
        std::string m_Name;
        size_t m_Size;
        size_t m_Alignment;

    public:
        ComplexType(std::string name, size_t size, size_t alignment);

        [[nodiscard]]
        std::string getName() const;

        [[nodiscard]]
        size_t getSize() const;

        [[nodiscard]]
        size_t getAlignment() const;
    };

    class ResolvedType : public ComplexType
    {
        std::vector<Property> m_Properties;
        std::vector<Method> m_Methods;

    public:
        ResolvedType(
            std::string name,
            size_t size,
            size_t alignment,
            std::vector<Property> properties,
            std::vector<Method> methods);

        [[nodiscard]]
        std::vector<Property>& getProperties();

        [[nodiscard]]
        std::vector<Method>& getMethods();
    };
}

#endif //COMPLEXTYPE_H
