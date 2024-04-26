#include "recordvisitor.h"

namespace lilac::cxx
{
    RecordVisitor::RecordVisitor(clang::RecordDecl* record): m_Record(record)
    {
    }

    bool RecordVisitor::VisitMSPropertyDecl(clang::MSPropertyDecl* property)
    {
        if (!IsVisibilityAllowed(property)) return true;

        if (!property->hasGetter() && !property->hasSetter())
        {
            llvm::errs()
                    << "A property '"
                    << m_Record->getName() << "::" << property->getNameAsString()
                    << "' with neither setter and getter is not allowed\n";
            return false;
        }

        m_Properties.push_back(property);
        llvm::outs() << m_Record->getName() << "::" << property->getName() << '\n';
        return true;
    }

    bool RecordVisitor::VisitCXXMethodDecl(clang::CXXMethodDecl* method)
    {
        if (IsVisibilityAllowed(method))
        {
            m_Methods.push_back(method);
            llvm::outs() << m_Record->getName() << "::" << method->getName() << '\n';
        }
        return true;
    }

    RecordInfo RecordVisitor::GetInfo() const
    {
        return {
            m_Record,
            m_Properties,
            m_Methods
        };
    }
}
