#ifndef KINDS_H
#define KINDS_H

#include <cstdint>

namespace lilac
{
    enum class OwnerKind : uint8_t
    {
        Type,
        Namespace
    };

    enum class TypeKind : uint8_t
    {
        Builtin = 0,
        Pointer = 1,
        Function = 2,
        Elaborated = 3
    };

    enum class BuiltinKind : uint16_t
    {
        None = 0x0000,
        Void = 0x0001,
        Bool = 0x0002,
        S8 = 0x0010,
        S16 = 0x0011,
        S32 = 0x0012,
        S64 = 0x0013,
        SIPtr = 0x0014,
        U8 = 0x0020,
        U16 = 0x0021,
        U32 = 0x0022,
        U64 = 0x0023,
        UIPtr = 0x0024,
        FP16 = 0x0031,
        FP32 = 0x0032,
        FP64 = 0x0033,
        String = 0x000F,
        Array = 0x100F
    };
}

#endif //KINDS_H
