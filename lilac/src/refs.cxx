#include "refs.h"
#include "typedecl.h"

namespace lilac
{
    bool OwnerRef::operator==(const OwnerRef& other) const
    {
        if (OwnerKind != other.OwnerKind)
            return false;
        return OwnerKind == OwnerKind::Type
            ? *Type == *other.Type
            : *Namespace == *other.Namespace;
    }
}

bool std::less<lilac::DeclRef>::operator()(const lilac::DeclRef& lhs, const lilac::DeclRef& rhs) const
{
    if (lhs.Assembly == rhs.Assembly)
        return less<std::string>{}(lhs.ActualName, rhs.ActualName);
    return less<std::string>{}(lhs.Assembly, rhs.Assembly);
}
