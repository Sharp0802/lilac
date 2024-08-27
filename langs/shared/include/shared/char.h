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

#include <ostream>
#include <array>

namespace lilac::shared
{
    constexpr int MaxIntrinIndent = 255;

    template<int... Ts>
    struct __IndentationVector
    {
        template<int N>
        struct __Indentation
        {
            static constexpr std::array<char, N> Get()
            {
                std::array<char, N> v{};
                v.fill('\t');
                return v;
            }

            static constexpr std::array<char, N> V = Get();
        };

        static constexpr std::array<const char*, sizeof...(Ts)> Vs = { __Indentation<Ts>::V.data()... };
    };

    template<int... Ts>
    constexpr const char* __GetIndentation(int c, std::integer_sequence<int, Ts...>)
    {
        return __IndentationVector<Ts...>::Vs[c];
    }

    /**
     * @brief Gets an indentation string with specified length
     *
     * An indentation strings shorter than lilac::shared::MaxIntrinIndent are generated during compile-time.
     * When you specified compile-time evaluable expression for @c c,
     * Calling expression for lilac::shared::GetIndent will be converted at compile-time.
     *
     * If you specify larger length than lilac::shared::MaxIntrinIndent,
     * An indentation strings will be generated on run-time and not be cached.
     * You should avoid use of very large indentation.
     *
     * @param c The number of indentation character
     * @return Indentation string with specified length
     */
    constexpr const char* GetIndent(int c = 1)
    {
        return __GetIndentation(c, std::make_integer_sequence<int, MaxIntrinIndent>{});
    }

    /**
     * @brief Write a newline on stream
     */
    inline std::ostream& endl(std::ostream& __os)
    {
        return __os.put('\n');
    }
}
