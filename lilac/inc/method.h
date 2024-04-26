#ifndef METHOD_H
#define METHOD_H

#include <vector>

#include "complextype.h"
#include "type.h"

namespace lilac
{
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
        virtual ~Function() = 0;

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
        ~Method() override = 0;

        [[nodiscard]]
        std::shared_ptr<ComplexType> getOwner() const;
    };
}

#endif //METHOD_H
