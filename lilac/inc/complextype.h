#ifndef COMPLEXTYPE_H
#define COMPLEXTYPE_H

#include <optional>
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

    struct Property
    {
        std::string Name;
        Type Type;
        std::optional<Method> Getter;
        std::optional<Method> Setter;
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
            std::initializer_list<Property> properties,
            std::initializer_list<Method> methods);

        [[nodiscard]]
        std::vector<Property> getProperties() const;

        [[nodiscard]]
        std::vector<Method> getMethods() const;
    };
}

#endif //COMPLEXTYPE_H
