#include "pluginaction.h"

#include "exportattr.h"
#include "typemap.h"

namespace lilac::cxx
{
    frxml::dom& GetDOMRoot()
    {
        static frxml::dom dom = frxml::dom::element("assembly");
        return dom;
    }

    std::unique_ptr<clang::ASTConsumer> LilacAction::CreateASTConsumer(
        clang::CompilerInstance& ci,
        llvm::StringRef)
    {
        return std::make_unique<LilacASTConsumer>(GetDOMRoot());
    }

    LilacASTConsumer::LilacASTConsumer(frxml::dom& dom) : m_Root(dom)
    {
    }

    void LilacASTConsumer::InitializeSema(clang::Sema& sema)
    {
        m_Sema = &sema;
    }

    void LilacASTConsumer::HandleTranslationUnit(clang::ASTContext& context)
    {
        LilacASTVisitor visitor(*m_Sema, m_Root);
        visitor.TraverseDecl(context.getTranslationUnitDecl());
    }

    frxml::dom* LilacASTVisitor::GetNamespaceDOM(clang::NamedDecl* decl) const
    {
        const auto ctx = decl->getDeclContext();

        if (const auto parent = ctx->getParent();
            parent &&
            parent->getDeclKind() != clang::Decl::Namespace &&
            parent->getDeclKind() != clang::Decl::TranslationUnit)
        {
            static auto err = m_Diag.getCustomDiagID(
                Level::Error,
                "Couldn't handle a nested declaration");

            m_Sema.Diag(decl->getLocation(), err);

            return nullptr;
        }

        std::stack<clang::NamespaceDecl*> stk;
        for (
            auto cur = ctx->getParent();
            cur != nullptr && cur->getDeclKind() == clang::Decl::Namespace;
            cur = cur->getParent())
        {
            stk.emplace(clang::cast<clang::NamespaceDecl>(cur));
        }

        frxml::dom* cur = &m_Root;
        for (; !stk.empty(); stk.pop())
        {
            frxml::dom* node = nullptr;

            auto ns = stk.top()->getNameAsString();
            for (auto& child: cur->children())
            {
                if (child.tag().view() != "namespace")
                    continue;
                if (child.attr()["name"].view() == ns)
                    node = &child;
            }
            if (!node)
            {
                node = &cur->children().emplace_back(frxml::dom::element(
                    "namespace",
                    { { "name", ns } }));
            }

            cur = node;
        }

        return cur;
    }

    LilacASTVisitor::LilacASTVisitor(clang::Sema& sema, frxml::dom& dom)
        : m_Sema(sema),
          m_Diag(m_Sema.getDiagnostics()),
          m_Root(dom)
    {
    }

    bool ShouldBeExported(clang::NamedDecl* decl)
    {
        auto exported = false;

        if (clang::isa<clang::RecordDecl>(decl))
        {
            for (const auto attr: decl->attrs())
            {
                if (const auto anot = clang::dyn_cast<clang::AnnotateTypeAttr>(attr);
                    !anot || anot->getAnnotation() != ExportAttrInfo::AttrMangling)
                    continue;

                exported = true;
                break;
            }
        }
        else
        {
            for (const auto attr: decl->attrs())
            {
                if (const auto anot = clang::dyn_cast<clang::AnnotateAttr>(attr);
                    !anot || anot->getAnnotation() != ExportAttrInfo::AttrMangling)
                    continue;

                exported = true;
                break;
            }
        }

        return exported;
    }

    bool LilacASTVisitor::IsDuplicated(clang::NamedDecl* decl, const std::string& tag)
    {
        const auto ns = GetNamespaceDOM(decl);
        if (!ns) return true;

        auto skip = false;
        for (auto& child: ns->children())
        {
            if (child.tag().view() != tag ||
                child.attr()["name"].view() != decl->getNameAsString())
                continue;
            skip = true;
            break;
        }

        return skip;
    }

    // ReSharper disable once CppDFAConstantFunctionResult
    bool LilacASTVisitor::TraverseEnumDecl(clang::EnumDecl* decl)
    {
        if (!ShouldBeExported(decl))
            return true;
        if (IsDuplicated(decl, "record"))
            return true;

        std::vector<frxml::dom> children;

        EnumVisitor visitor{
            [&](const clang::EnumConstantDecl* constant)
            {
                const auto value    = constant->getValue();
                const auto valueStr = decl->getIntegerType()->isSignedIntegerType()
                    ? std::to_string(value.getSExtValue())
                    : std::to_string(value.getZExtValue());

                children.push_back(frxml::dom::element(
                    "constant",
                    {
                        { "name", constant->getNameAsString() },
                        { "value", valueStr }
                    }));
            }
        };
        visitor.TraverseDecl(decl);

        const auto underlyingT = decl->getIntegerType();
        const auto typeName    = GetBuiltinTypeName(underlyingT.isNull()
            ? clang::BuiltinType::Kind::Int
            : clang::cast<clang::BuiltinType>(GetUnderlyingType(
                    m_Sema,
                    decl->getLocation(),
                    underlyingT.getTypePtr())
            )->getKind());

        const auto ns = GetNamespaceDOM(decl);
        if (!ns) return true;
        ns->children().push_back(frxml::dom::element(
            "enum",
            {
                { "name", decl->getNameAsString() },
                { "type", typeName }
            },
            children
        ));
        return true;
    }

    bool GetTypeName(
        clang::Sema&                sema,
        const clang::SourceLocation loc,
        const clang::Type*          type,
        std::string&                name)
    {
        type = type->getUnqualifiedDesugaredType();
        int nRef;
        for (nRef = 0; type->isPointerOrReferenceType(); ++nRef)
            type  = type->getPointeeType()->getUnqualifiedDesugaredType();

        if (type->isBuiltinType())
        {
            name = GetBuiltinTypeName(clang::cast<clang::BuiltinType>(type)->getKind()) + std::string(nRef, '*');
            return true;
        }

        if (type->isFunctionType())
        {
            static auto err = sema.getDiagnostics().getCustomDiagID(
                clang::DiagnosticsEngine::Error,
                "Function types cannot be exported");
            sema.Diag(loc, err);

            return false;
        }

        if (!type->getAsRecordDecl())
        {
            static auto err = sema.getDiagnostics().getCustomDiagID(
                clang::DiagnosticsEngine::Error,
                "Exported types must be built-in or convertible to record declaration");
            sema.Diag(loc, err);

            return false;
        }

        if (const auto parent = type->getAsRecordDecl()->getParent();
            parent &&
            parent->getDeclKind() != clang::Decl::Namespace &&
            parent->getDeclKind() != clang::Decl::TranslationUnit)
        {
            static auto err = sema.getDiagnostics().getCustomDiagID(
                clang::DiagnosticsEngine::Error,
                "Exported types must be in namespace or global");
            sema.Diag(loc, err);

            return false;
        }

        std::stack<std::string> stk;
        for (
            auto decl = type->getAsRecordDecl()->getParent();
            decl != nullptr && decl->getDeclKind() == clang::Decl::Namespace;
            decl = decl->getParent())
        {
            stk.push(clang::cast<clang::NamespaceDecl>(decl)->getNameAsString());
        }

        std::stringstream ss;
        for (; !stk.empty(); stk.pop())
        {
            ss << stk.top() << '/';
        }
        ss << type->getAsRecordDecl()->getNameAsString();
        ss << std::string(nRef, '*');

        name = ss.str();
        return true;
    }

    bool RecordParameters(clang::Sema& sema, frxml::dom& dom, clang::FunctionDecl* fn)
    {
        for (const auto parameter: fn->parameters())
        {
            std::string type;
            if (!GetTypeName(
                sema,
                parameter->getLocation(),
                parameter->getType()->getUnqualifiedDesugaredType(),
                type))
            {
                return false;
            }

            dom.children().push_back(frxml::dom::element(
                "param",
                {
                    { "name", parameter->getNameAsString() },
                    { "type", type }
                }
            ));
        }

        return true;
    }

    // ReSharper disable once CppDFAConstantFunctionResult
    bool LilacASTVisitor::TraverseCXXRecordDecl(clang::CXXRecordDecl* decl)
    {
        if (!ShouldBeExported(decl))
            return true;
        if (IsDuplicated(decl, "record"))
            return true;

        std::vector<frxml::dom> children;

        CXXRecordVisitor visitor{
            [&](clang::CXXMethodDecl* method)
            {
                if (!ShouldBeExported(method))
                    return;

                std::string ret;
                if (!GetTypeName(m_Sema, method->getLocation(), method->getReturnType().getTypePtr(), ret))
                    return;

                clang::ASTNameGenerator ang(method->getASTContext());

                auto dom = frxml::dom::element("", {
                                                   { "name", method->getNameAsString() },
                                                   { "return", ret },
                                                   { "mangle", ang.getName(method) }
                                               });
                if (clang::isa<clang::CXXConstructorDecl>(method))
                    dom.tag() = "ctor";
                else if (clang::isa<clang::CXXDestructorDecl>(method))
                    dom.tag() = "dtor";
                else
                    dom.tag() = static_cast<std::string>(method->isStatic() ? "function" : "method");
                RecordParameters(m_Sema, dom, method);

                children.push_back(dom);
            }
        };
        visitor.TraverseDecl(decl);

        const auto ns = GetNamespaceDOM(decl);
        if (!ns) return true;

        const auto typeInfo = decl->getASTContext().getTypeInfo(decl->getTypeForDecl());
        ns->children().push_back(frxml::dom::element(
            "record",
            {
                { "name", decl->getNameAsString() },
                { "size", std::to_string(typeInfo.Width / 8) },
                { "align", std::to_string(typeInfo.Align) }
            },
            children
        ));
        return true;
    }

    // ReSharper disable once CppDFAConstantFunctionResult
    bool LilacASTVisitor::TraverseFunctionDecl(clang::FunctionDecl* decl)
    {
        if (!ShouldBeExported(decl))
            return true;
        if (clang::isa<clang::CXXMethodDecl>(decl))
            return true;

        if (IsDuplicated(decl, "function"))
            return true;

        std::string ret;
        if (!GetTypeName(m_Sema, decl->getLocation(), decl->getReturnType().getTypePtr(), ret))
            return true;

        clang::ASTNameGenerator ang(decl->getASTContext());

        auto dom = frxml::dom::element(
            "function",
            {
                { "name", decl->getNameAsString() },
                { "return", ret },
                { "mangle", ang.getName(decl) }
            });
        if (!RecordParameters(m_Sema, dom, decl))
            return true;

        const auto ns = GetNamespaceDOM(decl);
        if (!ns) return true;

        ns->children().push_back(dom);
        return true;
    }

    LilacASTVisitor::EnumVisitor::EnumVisitor(std::function<void(clang::EnumConstantDecl* constant)> delegate)
        : m_Delegate(std::move(delegate))
    {
    }

    // ReSharper disable once CppMemberFunctionMayBeConst
    bool LilacASTVisitor::EnumVisitor::TraverseEnumConstantDecl(clang::EnumConstantDecl* decl)
    {
        m_Delegate(decl);
        return true;
    }

    LilacASTVisitor::CXXRecordVisitor::CXXRecordVisitor(std::function<void(clang::CXXMethodDecl*)> delegate)
        : m_Delegate(std::move(delegate))
    {
    }

    // ReSharper disable once CppMemberFunctionMayBeConst
    bool LilacASTVisitor::CXXRecordVisitor::TraverseCXXMethodDecl(clang::CXXMethodDecl* decl)
    {
        m_Delegate(decl);
        return true;
    }
}
