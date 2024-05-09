#include "typedecl.h"

namespace lilac
{
    bool TypeDecl::operator==(const TypeDecl& other) const
    {
        if (Name != other.Name)
            return false;
        return Owner == other.Owner;
    }

    bool NamespaceDecl::operator==(const NamespaceDecl& other) const
    {
        const auto* lhs = this;
        const auto* rhs = &other;

        while (true)
        {
            if (lhs->Name != rhs->Name)
                return false;
            if (lhs->Owner == nullptr || rhs->Owner == nullptr)
                return lhs->Owner == rhs->Owner;

            lhs = lhs->Owner;
            rhs = rhs->Owner;
        }
    }
}

bool std::less<lilac::FunctionDecl>::operator()(
    const lilac::FunctionDecl& lhs,
    const lilac::FunctionDecl& rhs) const
{
    return less<lilac::DeclRef>{}(lhs.Ref, rhs.Ref);
}

bool std::less<lilac::TypeDecl>::operator()(
    const lilac::TypeDecl& lhs,
    const lilac::TypeDecl& rhs) const
{
    if (lhs.Name != rhs.Name)
        return false;
    return less<lilac::OwnerRef>{}(lhs.Owner, rhs.Owner);
}

bool std::less<lilac::NamespaceDecl>::operator()(
    const lilac::NamespaceDecl& lhs,
    const lilac::NamespaceDecl& rhs) const
{
    for (const lilac::NamespaceDecl* plhs = &lhs, *prhs = &rhs;;)
    {
        if (plhs->Name != prhs->Name)
            return false;
        if (plhs->Owner == nullptr || prhs->Owner == nullptr)
            return plhs->Owner == prhs->Owner;
        plhs = plhs->Owner;
        prhs = prhs->Owner;
    }
}

bool std::less<lilac::OwnerRef>::operator()(const lilac::OwnerRef& lhs, const lilac::OwnerRef& rhs) const
{
    if (lhs.OwnerKind < rhs.OwnerKind)
        return true;
    if (lhs.OwnerKind == lilac::OwnerKind::Type)
        return less<lilac::TypeDecl>{}(*lhs.Type, *rhs.Type);
    return less<lilac::NamespaceDecl>{}(*lhs.Namespace, *rhs.Namespace);
}
