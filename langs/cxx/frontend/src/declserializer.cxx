#include "declserializer.h"

namespace lilac::cxx
{
    [[nodiscard]]
    std::string GetTypeString(const clang::TagDecl* decl);

    std::string GetName(const clang::NamedDecl* decl)
    {
        if (auto* tag = clang::dyn_cast<clang::TagDecl>(decl))
            return GetTypeString(tag);

        clang::ASTNameGenerator gen(decl->getASTContext());
        return gen.getName(decl);
    }

    std::string GetDisplayName(const clang::NamedDecl* decl)
    {
        return decl->getNameAsString();
    }

    std::string GetTypeString(const clang::TagDecl* decl)
    {
        static std::map<clang::TagTypeKind, std::string> map = {
            { clang::TagTypeKind::Struct, "struct" },
            { clang::TagTypeKind::Interface, "interface" },
            { clang::TagTypeKind::Union, "union" },
            { clang::TagTypeKind::Class, "class" },
            { clang::TagTypeKind::Enum, "enum" }
        };

        const clang::DeclContext* ctx = decl;
        std::stack<std::string>   stk;
        do
        {
            stk.emplace(decl->getName());
            ctx = ctx->getParent();
        } while (!ctx->isTranslationUnit());

        std::stringstream ss;
        ss << map[decl->getTagKind()];
        for (; !stk.empty(); stk.pop())
            ss << '.' << stk.top();

        return ss.str();
    }

    std::string GetTypeString(clang::QualType type)
    {
        type = type.getCanonicalType();

        std::size_t ref = 0;
        for (; type->isPointerType(); ref++, type = type->getPointeeType().getCanonicalType())
        {
        }

        std::stringstream ss;
        if (auto builtin = clang::dyn_cast<clang::BuiltinType>(type.getTypePtr()))
            ss << "builtin" << builtin->getKind();
        else
            ss << GetTypeString(type->getAsTagDecl());
        ss << std::string(ref, '*');
        return ss.str();
    }

    std::string GetFunctionTypeReg(const clang::FunctionDecl* decl)
    {
        std::stringstream ss;
        ss << '!' << GetTypeString(decl->getReturnType());
        if (const auto method = clang::dyn_cast<clang::CXXMethodDecl>(decl))
            ss << "/this$" << GetTypeString(method->getParent());
        for (const auto parameter : decl->parameters())
            ss << '/' << GetTypeString(parameter->getType());
        return ss.str();
    }

    clang::FunctionDecl* FindMethod(const clang::CXXRecordDecl* decl, const std::string& name)
    {
        for (const auto method: decl->methods())
            if (method->getName() == name)
                return method;
        return nullptr;
    }
}
