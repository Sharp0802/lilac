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
        std::shared_ptr<Type> Type;
    };

    class Method
    {
        std::string m_Name;
        std::shared_ptr<Type> m_Return;
        std::vector<Parameter> m_Parameters;

    protected:
        Method(std::string name, std::shared_ptr<Type> ret, std::vector<Parameter> params);

    public:
        [[nodiscard]]
        std::string getName() const;

        [[nodiscard]]
        std::shared_ptr<Type> getReturn() const;

        [[nodiscard]]
        std::vector<Parameter> getParameters() const;
    };

    struct Property
    {
        std::string Name;
        std::shared_ptr<Type> Type;
        std::shared_ptr<Method> Getter;
        std::shared_ptr<Method> Setter;
    };
}

#endif //METHOD_H
