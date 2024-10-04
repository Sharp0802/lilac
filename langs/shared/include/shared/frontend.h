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

#include <string>
#include <vector>

namespace lilac::shared
{
    enum FrontendKind
    {
        F_CXX
    };

    /**
     * @brief An action that generates interface representation from codes
     */
    class FrontendAction
    {
        const std::string m_Name;
        const std::string m_Desc;
        FrontendKind m_Kind;

    public:
        FrontendAction(FrontendKind kind, std::string name, std::string desc);

        virtual ~FrontendAction() = default;

        /**
         * @brief Gets the name of this action
         * @return The name of this action
         */
        [[nodiscard]]
        const std::string& Name() const;

        /**
         * @brief Gets the description of this action
         * @return The description of this action
         */
        [[nodiscard]]
        const std::string& Desc() const;

        /**
         * @brief Gets the kind of this action
         * @return The kind of this action
         */
        [[nodiscard]]
        FrontendKind Kind() const;

        /**
         *
         * @param output A path to place output
         * @param argv Additional language-specific arguments
         * @return Zero if action completed successfully; otherwise, non-zero returned
         */
        [[nodiscard]]
        virtual int Run(
            std::string output,
            std::vector<std::string> argv) const = 0;

        /**
         * @brief Gets all registered actions
         * @return All registered actions
         */
        static const std::vector<FrontendAction*>& GetRegistered();
    };
}
