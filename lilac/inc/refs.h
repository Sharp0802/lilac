#ifndef REFS_H
#define REFS_H

#include <string>

#include "kinds.h"

namespace lilac
{
    struct DeclRef
    {
        std::string Name;
        std::string ActualName;
        std::string Assembly;
    };

    struct TypeDecl;
    struct NamespaceDecl;

    struct OwnerRef
    {
        OwnerKind OwnerKind;
        union
        {
            TypeDecl* Type;
            NamespaceDecl* Namespace;
        };

        bool operator==(const OwnerRef& other) const;
    };
}

template<>
struct std::less<lilac::DeclRef>
{
    bool operator() (const lilac::DeclRef& lhs, const lilac::DeclRef& rhs) const;
};

#endif //REFS_H
