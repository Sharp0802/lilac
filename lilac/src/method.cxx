#include <utility>

#include "method.h"

namespace lilac
{
    Method::Method(std::string name, std::shared_ptr<Type> ret, std::vector<Parameter> params)
        : m_Name(std::move(name)),
          m_Return(std::move(ret)),
          m_Parameters(std::move(params))
    {
    }

    std::string Method::getName() const
    {
        return m_Name;
    }

    std::shared_ptr<Type> Method::getReturn() const
    {
        return m_Return;
    }

    std::vector<Parameter> Method::getParameters() const
    {
        return m_Parameters;
    }
}
