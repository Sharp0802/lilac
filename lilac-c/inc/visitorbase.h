#ifndef VISITORBASE_H
#define VISITORBASE_H

#include <clang/AST/RecursiveASTVisitor.h>

namespace lilac
{
    template<typename TResult>
    class VisitorBase
    {
    protected:
        static bool IsVisibilityAllowed(const clang::NamedDecl* decl)
        {
            return decl->getVisibility() == clang::Visibility::DefaultVisibility;
        }

    public:
        virtual ~VisitorBase() = default;

        virtual TResult GetInfo() const = 0;
    };
}

#endif //VISITORBASE_H
