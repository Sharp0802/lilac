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

namespace lilac
{
    /**
     * @brief An abstraction class of `llvm::cl::SubCommand`
     */
    class SubCommand
    {
        llvm::cl::SubCommand m_SubCommand;

    public:
        /**
         * @brief Creates a sub-command with given name and description
         * @param command A name of the sub-command
         * @param desc A description of the sub-command
         */
        SubCommand(const char* command, const char* desc);

        /**
         * @brief Runs a subcommand
         * @return Zero will be returned when completed successfully; otherwise, non-zero will be returned
         */
        virtual int Run() = 0;

        virtual ~SubCommand() = default;

        operator llvm::cl::SubCommand&();

        operator bool() const;

        /**
         * @brief Gets default category of all sub-commands registered by LILAC
         * @return Returns default category of all sub-commands registered by LILAC
         */
        static llvm::cl::OptionCategory& GetCategory();
    };
}
