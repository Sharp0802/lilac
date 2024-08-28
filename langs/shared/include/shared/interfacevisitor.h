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

#include <functional>
#include <utility>

#include "exception.h"
#include "frxml.h"

namespace lilac::shared
{
    /**
     * @brief A helper class to traverse interface graph
     *
     * To implement transformer of interface graph,
     * Implement visitor derived from lilac::shared::InterfaceVisitor.
     *
     * @tparam TContext Type of context object
     */
    template<typename TContext>
    class InterfaceVisitor
    {
    public:
        virtual ~InterfaceVisitor() = default;

        /**
         * @brief Gets the name of this interface visitor
         *
         * To traverse interface graph and matching proper visitor for XML tag,
         * Interface visitors should have key corresponding to XML tag.
         *
         * Interface visitor is reserved for special purpose if its name starts with `__'.
         *
         * @return Name of this interface visitor
         */
        [[nodiscard]]
        virtual std::string GetName() const = 0;

        /**
         * @param ctx Context object for interface visitors
         * @param parent Parent DOM object
         * @param current Current DOM object
         * @param depth Depth of current DOM object
         */
        virtual void Begin(TContext& ctx, const frxml::dom& parent, const frxml::dom& current, int depth) = 0;

        /**
         * @param ctx Context object for interface visitors
         * @param parent Parent DOM object
         * @param current Current DOM object
         * @param depth Depth of current DOM object
         */
        virtual void End(TContext& ctx, const frxml::dom& parent, const frxml::dom& current, int depth) = 0;
    };

    template<typename T, typename TContext>
    concept TVisitor = std::is_base_of_v<InterfaceVisitor<TContext>, T>;

    /**
     * @brief A composite ASTVisitor wrapping other ASTVisitor(s)
     *
     * @tparam TContext Type of context object
     * @tparam TVisitor Type derived from lilac::shared::ASTVisitor
     */
    template<typename TContext, TVisitor<TContext>... TVisitor>
    class GenericInterfaceVisitor final : public InterfaceVisitor<TContext>
    {
        std::map<std::string, std::shared_ptr<InterfaceVisitor<TContext>>> m_Visitors;

    public:
        GenericInterfaceVisitor()
        {
            for (std::shared_ptr<InterfaceVisitor<TContext>> visitor: { std::make_shared<TVisitor>()... })
                m_Visitors.emplace(visitor->getName(), std::move(visitor));
        }

        [[nodiscard]]
        std::string GetName() const override
        {
            return "__generic__";
        }

        void Begin(TContext& ctx, const frxml::dom& parent, const frxml::dom& current, int depth) override
        {
            auto v = static_cast<std::string>(current.tag().view());
            if (!m_Visitors.contains(v))
                throw exception(std::format("undefined tag '{}'", v), current);

            m_Visitors.at(v)->Begin(ctx, parent, current, depth);
        }

        void End(TContext& ctx, const frxml::dom& parent, const frxml::dom& current, int depth) override
        {
            auto v = static_cast<std::string>(current.tag().view());
            if (!m_Visitors.contains(v))
                throw exception(std::format("undefined tag '{}'", v), current);

            m_Visitors.at(v)->End(ctx, parent, current, depth);
        }
    };

    /**
     * @brief An interface visitor for a whole assembly.
     *
     * To traverse whole assembly,
     * Implement visitor derived from lilac::shared::AssemblyVisitor with your context object.
     * Use lilac::shared::GenericInterfaceVisitor to traverse XML elements tagged as `assembly'
     * is not intended usage of lilac::shared::GenericInterfaceVisitor.
     * Do **NOT** use lilac::shared::GenericInterfaceVisitor to traverse assembly element.
     *
     * @tparam TContext Type of context object
     */
    template<typename TContext>
    class AssemblyVisitor : public InterfaceVisitor<TContext>
    {
    public:
        [[nodiscard]]
        std::string GetName() const final
        {
            return "assembly";
        }

        void Begin(TContext& ctx, const frxml::dom& parent, const frxml::dom& current, int depth) override = 0;

        void End(TContext& ctx, const frxml::dom& parent, const frxml::dom& current, int depth) override = 0;
    };
}
