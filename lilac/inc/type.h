#ifndef TYPE_H
#define TYPE_H

#include <cstdint>
#include <memory>

namespace lilac
{
    enum TypeKind
    {
        LLType_Invalid,

#pragma region built-in
        LLType_B,

        LLType_S8,
        LLType_S16,
        LLType_S32,
        LLType_S64,

        LLType_U8,
        LLType_U16,
        LLType_U32,
        LLType_U64,

        LLType_SIPtr,
        LLType_UIPtr,

        LLType_FP16,
        LLType_FP32,
        LLType_FP64,

        LLType_Pointer,

        // sugar for strings such as 'std::string'(C++), 'System.String'(C#), not character pointer.
        LLType_String,
#pragma endregion

        LLType_Enum,
        LLType_Complex
    };

    struct TypeConstraint
    {
        uint8_t Const: 1;
        uint8_t Volatile: 1;
    };

    class Type
    {
        TypeKind m_Kind;
        TypeConstraint m_Constraint;
        std::shared_ptr<Type> m_Next;

    public:
        Type(TypeKind kind, TypeConstraint constraint, const Type& next);

        Type(TypeKind kind, TypeConstraint constraint, std::shared_ptr<Type> next);

        [[nodiscard]]
        TypeKind getKind() const;

        [[nodiscard]]
        TypeConstraint getConstraint() const;

        [[nodiscard]]
        Type getNext() const;
    };
}

#endif //TYPE_H
