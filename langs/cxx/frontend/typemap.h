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
