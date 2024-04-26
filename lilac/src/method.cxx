#include <utility>

#include "method.h"

namespace lilac
{
    Function::Function(std::string name, Type ret, std::initializer_list<Parameter> params)
        : m_Name(std::move(name)),
          m_Return(std::move(ret)),
          m_Parameters(params)
    {
    }

    std::string Function::getName() const
    {
        return m_Name;
    }

    Type Function::getReturn() const
    {
        return m_Return;
    }

    std::vector<Parameter> Function::getParameters() const
    {
        return m_Parameters;
    }

    Method::Method(
        std::shared_ptr<ComplexType> owner,
        std::string name,
        Type ret,
        std::initializer_list<Parameter> params)
        : Function(std::move(name), std::move(ret), params),
          m_Owner(std::move(owner))
    {
    }

    std::shared_ptr<ComplexType> Method::getOwner() const
    {
        return m_Owner;
    }
}
