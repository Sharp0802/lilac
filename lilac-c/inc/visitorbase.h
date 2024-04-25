#ifndef VISITORBASE_H
#define VISITORBASE_H

#include <clang/AST/RecursiveASTVisitor.h>

namespace lilac
{
    template<typename TTarget, typename TResult>
    class VisitorBase
    {
        TTarget* m_Target;

    protected:
        static bool IsVisibilityAllowed(const clang::NamedDecl* decl)
        {
            return decl->getVisibility() == clang::Visibility::DefaultVisibility;
        }

        explicit VisitorBase(TTarget* target) : m_Target(target)
        {
        }

    public:
        virtual ~VisitorBase() = default;

        virtual TResult GetInfo() const = 0;
    };
}

#endif //VISITORBASE_H
