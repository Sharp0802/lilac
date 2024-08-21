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

#define __IS_MONTH(month) \
    ((__DATE__[0]==#month[0]) && \
    (__DATE__[1]==#month[1]) && \
    (__DATE__[2]==#month[2]))

#define __DATE_MONTH \
    (__IS_MONTH(Jan) ? 0x1 : \
    __IS_MONTH(Feb) ? 0x2 : \
    __IS_MONTH(Mar) ? 0x3 : \
    __IS_MONTH(Apr) ? 0x4 : \
    __IS_MONTH(May) ? 0x5 : \
    __IS_MONTH(Jun) ? 0x6 : \
    __IS_MONTH(Jul) ? 0x7 : \
    __IS_MONTH(Aug) ? 0x8 : \
    __IS_MONTH(Sep) ? 0x9 : \
    __IS_MONTH(Oct) ? 0xA : \
    __IS_MONTH(Nov) ? 0xB : \
    __IS_MONTH(Dec) ? 0xC : 0)

#define __DATE_YEAR \
    ((__DATE__[7]-'0')*1000 + \
    (__DATE__[8]-'0')*100 + \
    (__DATE__[9]-'0')*10 + \
    (__DATE__[10]-'0'))

#define __DATE_DAY \
    ((__DATE__[4]-'0')*10 +\
    (__DATE__[5]-'0'))

#define LILAC_DATE (__DATE_YEAR * 10000 + __DATE_MONTH * 100 + __DATE_DAY)

#define LILAC_SHORT_LICENSE \
    "Copyright (C) 2024 Yeong-Won Seo                                           \n" \
    "This is free software; see the source for copying conditions.  There is NO \n" \
    "warranty; not even for MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE."
