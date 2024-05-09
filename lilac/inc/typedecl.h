#ifndef TYPEDECL_H
#define TYPEDECL_H

#include <functional>
#include <functional>
#include <string>
#include <vector>

#include "refs.h"
#include "type.h"

namespace lilac
{
    struct FunctionDecl
    {
        DeclRef Ref;
        Type* Type;
        OwnerRef Owner;
    };

    struct PropertyDecl
    {
        std::string Name;
        FunctionDecl* Getter;
        FunctionDecl* Setter;
    };

    struct TypeDecl
    {
        std::string Name;
        Type* Type;
        std::vector<FunctionDecl*> Methods;
        std::vector<PropertyDecl*> Properties;
        OwnerRef Owner;

        bool operator==(const TypeDecl& other) const;
    };

    struct NamespaceDecl
    {
        std::string Name;
        std::vector<TypeDecl*> Types;
        std::vector<FunctionDecl*> Functions;
        std::vector<NamespaceDecl*> Namespaces;
        NamespaceDecl* Owner;

        bool operator==(const NamespaceDecl& other) const;
    };
}

template<>
struct std::less<lilac::FunctionDecl>
{
    bool operator() (const lilac::FunctionDecl& lhs, const lilac::FunctionDecl& rhs) const;
};

template<>
struct std::less<lilac::TypeDecl>
{
    bool operator() (const lilac::TypeDecl& lhs, const lilac::TypeDecl& rhs) const;
};

template<>
struct std::less<lilac::NamespaceDecl>
{
    bool operator() (const lilac::NamespaceDecl& lhs, const lilac::NamespaceDecl& rhs) const;
};

template<>
struct std::less<lilac::OwnerRef>
{
    bool operator() (const lilac::OwnerRef& lhs, const lilac::OwnerRef& rhs) const;
};

#endif //TYPEDECL_H
