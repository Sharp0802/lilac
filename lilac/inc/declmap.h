#ifndef DECLMAP_H
#define DECLMAP_H

#include <set>

#include "typedecl.h"

namespace lilac
{
    class DeclMap
    {
        std::set<NamespaceDecl*> m_Namespaces;
        std::set<TypeDecl*> m_Types;
        std::set<FunctionDecl*> m_Functions;

    public:
        ~DeclMap();

        [[nodiscard]]
        std::set<NamespaceDecl*>& getNamespaces();

        [[nodiscard]]
        std::set<TypeDecl*>& getTypes();

        [[nodiscard]]
        std::set<FunctionDecl*>& getFunctions();

        template<typename T>
        std::set<T*>& getDatabase();

        template<typename T>
        size_t getIndex(const T* decl) const;

        template<typename T, typename... Args>
        T* emplace(Args&&... args);
    };

    namespace details_Map
    {
        template<typename T>
        [[nodiscard]]
        std::set<T*>& getDatabase(DeclMap* map) = delete;

        template<>
        inline std::set<NamespaceDecl*>& getDatabase<NamespaceDecl>(DeclMap* map)
        {
            return map->getNamespaces();
        }

        template<>
        inline std::set<TypeDecl*>& getDatabase<TypeDecl>(DeclMap* map)
        {
            return map->getTypes();
        }

        template<>
        inline std::set<FunctionDecl*>& getDatabase<FunctionDecl>(DeclMap* map)
        {
            return map->getFunctions();
        }
    }

    template<typename T>
    std::set<T*>& DeclMap::getDatabase()
    {
        return details_Map::getDatabase<T>(this);
    }

    template<typename T>
    size_t DeclMap::getIndex(const T* decl) const
    {
        size_t i = 0;
        for (const T* target : getDatabase<T>())
        {
            if (&target == decl)
                return i;
            i++;
        }
        return i;
    }

    template<typename T, typename ... Args>
    T* DeclMap::emplace(Args&&... args)
    {
        auto [ptr, success] = getDatabase<T>().emplace(args...);
        return success ? ptr : nullptr;
    }
}

#endif //DECLMAP_H
