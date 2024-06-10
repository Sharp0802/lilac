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

        return GetTypeString(type->getAsTagDecl()) + std::string(ref, '*');
    }

    clang::FunctionDecl* FindFunction(const clang::CXXRecordDecl* decl, const std::string& name)
    {
        for (const auto method: decl->methods())
            if (method->getName() == name)
                return method;
        return nullptr;
    }
}
