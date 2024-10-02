/*
 * Copyright (C) 2024  Yeong-won Seo
 *
 * This file is part of LILAC.
 *
 * LILAC is free software: you can redistribute it and/or modify it under
 * the terms of the GNU General Public License as published by the Free
 * Software Foundation, either version 3, or (at your option) any later
 * version.
 *
 * LILAC is distributed in the hope that it will be useful, but WITHOUT ANY
 * WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License
 * for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <https://www.gnu.org/licenses/>.
 */

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

        if (const auto parent = ctx;
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
            auto cur = ctx;
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

        if (clang::isa<clang::TagDecl>(decl))
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
            const auto name = clang::isa<clang::FunctionDecl>(decl)
                ? clang::ASTNameGenerator(decl->getASTContext()).getName(decl)
                : decl->getNameAsString();

            if (child.tag().view() != tag ||
                child.attr()["name"].view() != name)
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
        if (IsDuplicated(decl, "enum"))
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

        auto kind = clang::BuiltinType::Kind::Int;

        if (const auto underlyingT = decl->getIntegerType(); !underlyingT.isNull())
        {
            const auto type = underlyingT->getUnqualifiedDesugaredType();
            if (!type->isBuiltinType())
            {
                static auto err = m_Diag.getCustomDiagID(
                    Level::Error,
                    "Couldn't use non-builtin type as enumeration type");
                m_Sema.Diag(decl->getLocation(), err);

                return true;
            }

            kind = clang::cast<clang::BuiltinType>(type)->getKind();
        }

        const auto ns = GetNamespaceDOM(decl);
        if (!ns) return true;
        ns->children().push_back(frxml::dom::element(
            "enum",
            {
                { "name", decl->getNameAsString() },
                { "type", GetBuiltinTypeName(kind) }
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

    std::optional<frxml::dom> RecordFunction(
        clang::Sema&         sema,
        clang::FunctionDecl* fn)
    {
        static std::map<clang::Decl::Kind, std::string> tagMap = {
            { clang::Decl::CXXMethod, "method" },
            { clang::Decl::CXXConstructor, "ctor" },
            { clang::Decl::CXXDestructor, "dtor" }
        };
        std::string tag = "function";
        if (!fn->isStatic() && tagMap.contains(fn->getKind()))
            tag = tagMap[fn->getKind()];


        std::string virtuality = "none";
        if (const auto method = clang::dyn_cast<clang::CXXMethodDecl>(fn); method && method->isVirtual())
        {
            virtuality = "virtual";
            for (const auto attr: method->attrs())
                if (attr->getKind() == clang::attr::Final)
                    virtuality = "final";
        }


        const auto proto    = fn->getType()->getAs<clang::FunctionProtoType>();
        const auto callconv = clang::FunctionType::getNameForCallConv(proto->getCallConv());

        clang::ASTNameGenerator ang(fn->getASTContext());

        auto dom = frxml::dom::element(
            tag,
            {
                { "mangle", ang.getName(fn) },
                { "callconv", callconv.str() },
                { "virtual", virtuality }
            });

        if (fn->getKind() != clang::Decl::CXXConstructor &&
            fn->getKind() != clang::Decl::CXXDestructor)
        {
            std::string ret;
            if (!GetTypeName(sema, fn->getLocation(), fn->getReturnType().getTypePtr(), ret))
                return std::nullopt;

            dom.attr().emplace("name", fn->getNameAsString());
            dom.attr().emplace("return", ret);
        }

        if (!RecordParameters(sema, dom, fn))
            return std::nullopt;

        return dom;
    }

    // ReSharper disable once CppDFAConstantFunctionResult
    bool LilacASTVisitor::TraverseCXXRecordDecl(clang::CXXRecordDecl* decl)
    {
        if (!ShouldBeExported(decl))
            return true;
        if (IsDuplicated(decl, "record"))
            return true;

        std::vector<frxml::dom> children;

        static auto recordErr = m_Sema.getDiagnostics().getCustomDiagID(
            Level::Error,
            "CXXMethod cannot be exported");

        // Record member functions including constructors/destructor
        CXXRecordVisitor visitor{
            [&](clang::CXXMethodDecl* method)
            {
                if (!ShouldBeExported(method))
                    return;

                const auto dom = RecordFunction(m_Sema, method);
                if (!dom)
                {
                    m_Sema.Diag(method->getLocation(), recordErr);
                    return;
                }

                children.push_back(dom.value());
            }
        };
        visitor.TraverseDecl(decl);

        // Record implicit destructor
        if (const auto dtor = decl->getDestructor())
        {
            const auto dom = RecordFunction(m_Sema, dtor);
            if (!dom)
            {
                m_Sema.Diag(dtor->getLocation(), recordErr);
                return false;
            }

            children.push_back(dom.value());
        }

        const auto ns = GetNamespaceDOM(decl);
        if (!ns) return true;

        const auto typeInfo = decl->getASTContext().getTypeInfo(decl->getTypeForDecl());

        const auto size  = typeInfo.Width;
        const auto align = typeInfo.Align;
        if (size % 8 || align % 8)
        {
            static auto err = m_Diag.getCustomDiagID(
                Level::Error,
                "A size and alignment of record must be multiple of bytes");
            m_Sema.Diag(decl->getLocation(), err);
        }

        ns->children().push_back(frxml::dom::element(
            "record",
            {
                { "name", decl->getNameAsString() },
                { "size", std::to_string(size / 8) },
                { "align", std::to_string(align / 8) }
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

        const auto dom = RecordFunction(m_Sema, decl);
        if (!dom)
        {
            static auto err = m_Sema.getDiagnostics().getCustomDiagID(
                Level::Error,
                "FunctionDecl cannot be exported");
            m_Sema.Diag(decl->getLocation(), err);
            return false;
        }

        const auto ns = GetNamespaceDOM(decl);
        if (!ns) return true;

        ns->children().push_back(dom.value());
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
