#include <utility>

#include "frontendactionfactory.h"

#include "astfrontendaction.h"
#include "lilac/lilac.h"

namespace lilac::cxx
{
    using TypeMap = std::map<clang::QualType, std::shared_ptr<Type> >;

    FrontendActionFactory::FrontendActionFactory() = default;

    TypeConstraint GetConsrtaint(const clang::QualType& type)
    {
        return {
            type.isConstQualified(),
            type.isVolatileQualified()
        };
    }

    const clang::Type* RelaxType(const clang::Type* type)
    {
        if (const auto decltypeT = clang::cast_or_null<clang::DecltypeType>(type))
            return decltypeT->getUnderlyingType().getTypePtr();
        if (const auto usingT = clang::cast_or_null<clang::UsingType>(type))
            return usingT->getUnderlyingType().getTypePtr();
        return type;
    }

    std::optional<Type> GetBuiltinType(const clang::QualType type)
    {
        using clang::BuiltinType::Kind;

        static std::map<clang::BuiltinType::Kind, TypeKind> s_Map{
            { Kind::Bool, LLType_B },

            { Kind::SChar, LLType_S8 },
            { Kind::Short, LLType_S16 },
            { Kind::Int, LLType_S32 },
            { Kind::Long, LLType_SIPtr },
            { Kind::LongLong, LLType_S64 },

            { Kind::UChar, LLType_U8 },
            { Kind::UShort, LLType_U16 },
            { Kind::UInt, LLType_U32 },
            { Kind::ULong, LLType_UIPtr },
            { Kind::ULongLong, LLType_U64 },

            { Kind::Float16, LLType_FP16 },
            { Kind::Float, LLType_FP32 },
            { Kind::Double, LLType_FP64 }
        };

        if (!type->isBuiltinType())
            return std::nullopt;

        const auto builtin = clang::cast<clang::BuiltinType>(type.getTypePtr());
        if (!s_Map.contains(builtin->getKind()))
            return std::nullopt;

        return Type{
            s_Map[builtin->getKind()],
            GetConsrtaint(type),
            nullptr
        };
    }

    ComplexType GetComplexType(const clang::QualType type)
    {
        const auto decl = type->getAsRecordDecl();
        const auto& ast = decl->getASTContext();

        return {
            decl->getNameAsString(),
            ast.getTypeSize(type),
            ast.getTypeAlign(type)
        };
    }

    std::shared_ptr<Type> GetType(
        const clang::QualType type,
        TypeMap map)
    {
        if (map.contains(type))
            return map[type];

        const auto* ptr = type.getTypePtr();

        if (type->isPointerType() || type->isReferenceType())
        {
            return std::make_shared<Type>(
                LLType_Pointer,
                GetConsrtaint(type),
                GetType(ptr->getPointeeType(), map)
            );
        }

        if (type->isBuiltinType())
        {
            if (auto builtin = GetBuiltinType(type); builtin.has_value())
                return std::make_shared<Type>(builtin.value());
        }

        return std::make_shared<ComplexType>(GetComplexType(type));
    }

    [[nodiscard]]
    std::shared_ptr<Method> SingleMethod(
        const std::vector<Method>& methods,
        const clang::IdentifierInfo* id,
        const std::function<void(const Method& old, const Method& found)>& dup)
    {
        bool failed = false;

        std::optional<Method> target;
        for (const auto& method: methods)
        {
            if (method.getName() != id->getName())
                continue;
            if (target.has_value())
            {
                dup(target.value(), method);
                failed = true;
                continue;
            }
            target = method;
        }

        return failed ? nullptr : target;
    }

    Method BakeFunction(clang::FunctionDecl* decl, const TypeMap& map)
    {
        std::vector<Parameter> parameters;
        parameters.reserve(decl->param_size());
        for (const auto parameter: decl->parameters())
            parameters.emplace_back(
                parameter->getNameAsString(),
                GetType(parameter->getType(), map));

        return {
            decl->getNameAsString(),
            GetType(decl->getReturnType(), map),
            parameters
        };
    }

    void FrontendActionFactory::ReportASTInfo(const llvm::StringRef file, const ASTInfo& info)
    {
#pragma region prebake types
        std::map<clang::QualType, std::shared_ptr<Type>> types;
        for (const auto& [
                 handle,
                 astProperties,
                 astMethods
             ]: info.Records)
        {
            const auto& ast = handle->getASTContext();
            const auto type = ast.getRecordType(handle);

            types[type] = std::make_shared<ResolvedType>(
                handle->getNameAsString(),
                ast.getTypeSize(type),
                ast.getTypeAlign(type),
                std::vector<auto>(),
                std::vector<auto>()
            );
        }
#pragma endregion

#pragma region functions
        std::vector<Method> functions;
        functions.reserve(info.Functions.size());
        for (const auto function: info.Functions)
            functions.push_back(BakeFunction(function, types));
#pragma endregion

#pragma region resolved types
        for (const auto& [
                 handle,
                 astProperties,
                 astMethods
             ]: info.Records)
        {
#pragma region methods
            std::vector<Method> methods;
            methods.reserve(astMethods.size());
            for (const auto method: astMethods)
                methods.push_back(BakeFunction(method, types));
#pragma endregion

#pragma region properties
            std::vector<Property> properties;
            properties.reserve(astProperties.size());
            for (auto property: astProperties)
            {
                std::shared_ptr<Method> getter = nullptr;
                std::shared_ptr<Method> setter = nullptr;
                if (property->hasGetter())
                {
                    getter = SingleMethod(methods, property->getGetterId(), [&](auto old, auto found)
                    {
                        llvm::errs()
                                << file << ": A property '"
                                << property->getNameAsString() << "' has ambigious getter: '"
                                << old->getName() << "', '"
                                << found->getName() << "'.\n";
                    });
                    if (getter == nullptr)
                    {
                        llvm::errs()
                                << file << ": A getter method '"
                                << property->getGetterId() << "' not found used in ' property "
                                << property->getNameAsString() << "'\n";
                    }
                }
                if (property->hasSetter())
                {
                    setter = SingleMethod(methods, property->getSetterId(), [&](auto old, auto found)
                    {
                        llvm::errs()
                                << file << ": A property '"
                                << property->getNameAsString() << "' has ambigious setter: '"
                                << old->getName() << "', '"
                                << found->getName() << "'.\n";
                    });
                    if (setter == nullptr)
                    {
                        llvm::errs()
                                << file << ": A setter method '"
                                << property->getGetterId() << "' not found used in ' property "
                                << property->getNameAsString() << "'\n";
                    }
                }

                auto type = GetType(property->getType(), types);
                if (type->getKind() == LLType_Complex)
                {
                    llvm::errs()
                            << file << ": A property '"
                            << property->getNameAsString() << "' shouldn't use complex type directly";
                }

                properties.emplace_back(
                    property->getNameAsString(),
                    type,
                    getter,
                    setter
                );
            }
#pragma endregion


            auto& ast = handle->getASTContext();
            auto type = ast.getTypeDeclType(handle);

            const auto ptr = static_cast<ResolvedType*>(types[type].get()); // NOLINT(*-pro-type-static-cast-downcast)

            ptr->getMethods()    = methods;
            ptr->getProperties() = properties;
        }
#pragma endregion

#pragma region enums
        std::vector<EnumType> enums;
        enums.reserve(info.Enums.size());
        for (const auto& [handle, constantDecls]: info.Enums)
        {
            const auto type     = handle->getIntegerType();
            const bool isSigned = handle->getIntegerType()->isSignedIntegerType();

            std::vector<EnumConstant> constants;
            constants.reserve(constantDecls.size());
            for (const auto constant: constantDecls)
            {
                auto value = constant->getValue();
                if (value.getBitWidth() > 64)
                {
                    llvm::errs()
                        << "A constant '" << constant->getName()
                        << "' in '" << handle->getName()
                        << "' has too big to process";
                    break;
                }

                if (isSigned)
                {
                    constants.emplace_back(
                        constant->getNameAsString(),
                        value.getSExtValue()
                    );
                }
                else
                {
                    constants.emplace_back(
                        constant->getNameAsString(),
                        value.getZExtValue()
                    );
                }
            }

            enums.emplace_back(
                handle->getNameAsString(),
                GetType(type, types),
                constants);
        }
#pragma endregion

        // TODO: Serialize Records
    }

    std::unique_ptr<clang::FrontendAction> FrontendActionFactory::create()
    {
        return std::make_unique<ASTFrontendAction>(this);
    }
}
