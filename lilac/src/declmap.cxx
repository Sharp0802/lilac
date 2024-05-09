#include "declmap.h"

namespace lilac
{
    DeclMap::~DeclMap()
    {
        for (const auto target : m_Namespaces)
            delete target;
        for (const auto target : m_Types)
            delete target;
        for (const auto target : m_Functions)
            delete target;
    }

    std::set<NamespaceDecl*>& DeclMap::getNamespaces()
    {
        return m_Namespaces;
    }

    std::set<TypeDecl*>& DeclMap::getTypes()
    {
        return m_Types;
    }

    std::set<FunctionDecl*>& DeclMap::getFunctions()
    {
        return m_Functions;
    }
}
