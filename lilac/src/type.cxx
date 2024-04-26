#include <utility>

#include "type.h"

namespace lilac
{
    Type::Type(TypeKind kind, TypeConstraint constraint, const Type& next)
        : m_Kind(kind),
          m_Constraint(constraint),
          m_Next(std::make_shared<Type>(next))
    {
    }

    Type::Type(TypeKind kind, TypeConstraint constraint, std::shared_ptr<Type> next)
        : m_Kind(kind),
          m_Constraint(constraint),
          m_Next(std::move(next))
    {
    }

    TypeKind Type::getKind() const
    {
        return m_Kind;
    }

    TypeConstraint Type::getConstraint() const
    {
        return m_Constraint;
    }

    Type Type::getNext() const
    {
        return *m_Next;
    }
}
