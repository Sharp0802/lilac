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

#pragma once

#include "pch.h"

namespace lilac::cxx
{
    template<clang::BuiltinType::Kind Kind>
    struct TypeKindMap;
}

#define TYPEKINDMAP(kind, type) \
    template<> struct lilac::cxx::TypeKindMap<clang::BuiltinType::Kind::kind> \
    { \
        using Type = type; \
    }

TYPEKINDMAP(Void, void);

// UNSIGNED INTEGER
TYPEKINDMAP(Bool, bool);
TYPEKINDMAP(Char_U, uint8_t);
TYPEKINDMAP(UChar, uint8_t);
TYPEKINDMAP(WChar_U, uint16_t);
TYPEKINDMAP(Char8, uint8_t);
TYPEKINDMAP(Char16, uint16_t);
TYPEKINDMAP(Char32, uint32_t);
TYPEKINDMAP(UShort, uint16_t);
TYPEKINDMAP(UInt, uint32_t);
TYPEKINDMAP(ULong, uintptr_t);
TYPEKINDMAP(ULongLong, uint64_t);
TYPEKINDMAP(UInt128, __uint128_t);

// SIGNED INTEGER
TYPEKINDMAP(Char_S, int8_t);
TYPEKINDMAP(SChar, int8_t);
TYPEKINDMAP(WChar_S, int16_t);
TYPEKINDMAP(Short, int16_t);
TYPEKINDMAP(Int, int32_t);
TYPEKINDMAP(Long, intptr_t);
TYPEKINDMAP(LongLong, int64_t);
TYPEKINDMAP(Int128, __int128_t);

// FLOATING POINT
TYPEKINDMAP(Half, _Float16);
TYPEKINDMAP(Float, float);
TYPEKINDMAP(Double, double);
TYPEKINDMAP(LongDouble, long double);
TYPEKINDMAP(Float16, _Float16);
TYPEKINDMAP(BFloat16, __bf16);
TYPEKINDMAP(Float128, __float128);
#ifdef __powerpc
TYPEKINDMAP(Ibm128, __ibm128);
#endif

namespace lilac::cxx
{
    template<template<clang::BuiltinType::Kind> typename F, typename Value>
    std::map<clang::BuiltinType::Kind, Value> CreateTypeMap()
    {
#define QUERY(type) { clang::BuiltinType::Kind::type, F<clang::BuiltinType::Kind::type>() }
        return std::map<clang::BuiltinType::Kind, Value>{
            QUERY(Void),
            QUERY(Bool),
            QUERY(Char_U),
            QUERY(UChar),
            QUERY(WChar_U),
            QUERY(Char8),
            QUERY(Char16),
            QUERY(Char32),
            QUERY(UShort),
            QUERY(UInt),
            QUERY(ULong),
            QUERY(ULongLong),
            QUERY(UInt128),
            QUERY(Char_S),
            QUERY(SChar),
            QUERY(WChar_S),
            QUERY(Short),
            QUERY(Int),
            QUERY(Long),
            QUERY(LongLong),
            QUERY(Int128),
            QUERY(Half),
            QUERY(Float),
            QUERY(Double),
            QUERY(LongDouble),
            QUERY(Float16),
            QUERY(BFloat16),
            QUERY(Float128),
            QUERY(Ibm128)
        };
#undef QUERY
    }

    [[nodiscard]]
    constexpr std::string GetBuiltinTypeName(clang::BuiltinType::Kind kind)
    {
        switch (kind)
        {
#define CASE___(a, ...) case clang::BuiltinType::Kind::a: __VA_OPT__(CASE__(__VA_ARGS__))
#define CASE__(a, ...) case clang::BuiltinType::Kind::a: __VA_OPT__(CASE___(__VA_ARGS__))
#define CASE_(name, ...) CASE__(__VA_ARGS__) return name
            CASE_("__void", Void);
            CASE_("__bool", Bool);
            CASE_("__u8", Char_U);
            CASE_("__u8", UChar);
            CASE_("__u16", WChar_U);
            CASE_("__u8", Char8);
            CASE_("__u16", Char16);
            CASE_("__u32", Char32);
            CASE_("__u16", UShort);
            CASE_("__u32", UInt);
            CASE_("__uptr", ULong);
            CASE_("__u64", ULongLong);
            CASE_("__u128", UInt128);
            CASE_("__s8", Char_S, SChar);
            CASE_("__s16", WChar_S);
            CASE_("__s16", Short);
            CASE_("__s32", Int);
            CASE_("__sptr", Long);
            CASE_("__s64", LongLong);
            CASE_("__s128", Int128);
            CASE_("__fp16", Half);
            CASE_("__fp32", Float);
            CASE_("__fp64", Double);
            CASE_("__fp16", Float16);
            CASE_("__fp128", Float128);
#undef CASE_
#undef CASE__
#undef CASE___

            default:
                throw std::runtime_error(std::format("builtin type '{}' is not supported", static_cast<int>(kind)));
        }
    }
}

#undef TYPEKINDMAP
