#ifndef METHOD_H
#define METHOD_H

#include <vector>

#include "type.h"

namespace lilac
{
    class ComplexType;

    struct Parameter
    {
        std::string Name;
        Type Type;
    };

    class Function
    {
        std::string m_Name;
        Type m_Return;
        std::vector<Parameter> m_Parameters;

    protected:
        Function(std::string name, Type ret, std::initializer_list<Parameter> params);

    public:
        [[nodiscard]]
        std::string getName() const;

        [[nodiscard]]
        Type getReturn() const;

        [[nodiscard]]
        std::vector<Parameter> getParameters() const;
    };

    class Method : public Function
    {
        std::shared_ptr<ComplexType> m_Owner;

    protected:
        Method(std::shared_ptr<ComplexType> owner, std::string name, Type ret, std::initializer_list<Parameter> params);

    public:
        [[nodiscard]]
        std::shared_ptr<ComplexType> getOwner() const;
    };

    struct Property
    {
        std::string Name;
        Type Type;
        std::shared_ptr<Method> Getter;
        std::shared_ptr<Method> Setter;
    };
}

#endif //METHOD_H
