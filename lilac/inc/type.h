#ifndef TYPE_H
#define TYPE_H

#include <string>
#include <vector>
#include <sys/types.h>

#include "kinds.h"

namespace lilac
{
    struct Constraint
    {
        bool Constant;
        bool Volatile;
    };

    struct Type;

    struct Literal
    {
        BuiltinKind Kind;

        union
        {
            bool Bool;
            int8_t S8;
            int16_t S16;
            int32_t S32;
            int64_t S64;
            ssize_t SIPtr;
            uint8_t U8;
            uint16_t U16;
            uint32_t U32;
            uint64_t U64;
            size_t UIPtr;
            _Float16 FP16;
            float FP32;
            double FP64;
            std::string String;
        } Data;
    };

    struct ParameterDecl
    {
        std::string Name;
        Type* Type;
        Literal Default;
    };

    template<TypeKind>
    struct TypeData;

    template<>
    struct TypeData<TypeKind::Builtin>
    {
        BuiltinKind Kind;
    };

    template<>
    struct TypeData<TypeKind::Pointer>
    {
        Type* Next;
    };

    template<>
    struct TypeData<TypeKind::Function>
    {
        Type* Return;
        std::vector<ParameterDecl> Parameters;
    };

    template<>
    struct TypeData<TypeKind::Elaborated>
    {
        uint64_t Size;
        uint64_t Align;
    };

    struct Type
    {
        Constraint Constraint;
        TypeKind Kind;

        union
        {
            TypeData<TypeKind::Builtin> Builtin;
            TypeData<TypeKind::Pointer> Pointer;
            TypeData<TypeKind::Function> Function;
            TypeData<TypeKind::Elaborated> Elaborated;
        } Data;
    };
}

#endif //TYPE_H
