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
#include "shared/interfacevisitor.h"

namespace lilac::csharp
{
    struct VisitContext
    {
        std::string   RootNamespace;
        std::string   LibraryName;
        std::ostream& Output;
    };

    /**
     * @brief A visitor that creates bridge of `assembly` elements with C#
     */
    class CSharpAssemblyInterfaceVisitor final : public shared::AssemblyVisitor<VisitContext>
    {
    public:
        void Begin(VisitContext& ctx, const frxml::dom& parent, const frxml::dom& current, int depth) override;

        void End(VisitContext& ctx, const frxml::dom& parent, const frxml::dom& current, int depth) override;
    };

    /**
     * @brief A visitor that creates bridge of `namespace` elements with C#
     */
    class NamespaceVisitor final : public shared::InterfaceVisitor<VisitContext>
    {
    public:
        [[nodiscard]]
        std::string GetName() const override;

        void Begin(VisitContext& ctx, const frxml::dom& parent, const frxml::dom& current, int depth) override;

        void End(VisitContext& ctx, const frxml::dom& parent, const frxml::dom& current, int depth) override;
    };

    /**
     * @brief A visitor that creates bridge of `record` elements with C#
     */
    class RecordVisitor final : public shared::InterfaceVisitor<VisitContext>
    {
    public:
        [[nodiscard]]
        std::string GetName() const override;

        void Begin(VisitContext& ctx, const frxml::dom& parent, const frxml::dom& current, int depth) override;

        void End(VisitContext& ctx, const frxml::dom& parent, const frxml::dom& current, int depth) override;
    };

    /**
     * @brief A visitor that creates bridge of `enum` elements with C#
     */
    class EnumVisitor final : public shared::InterfaceVisitor<VisitContext>
    {
    public:
        [[nodiscard]]
        std::string GetName() const override;

        void Begin(VisitContext& ctx, const frxml::dom& parent, const frxml::dom& current, int depth) override;

        void End(VisitContext& ctx, const frxml::dom& parent, const frxml::dom& current, int depth) override;
    };

    /**
     * @brief A visitor that creates bridge of `constant` elements with C#
     */
    class EnumConstantVisitor final : public shared::InterfaceVisitor<VisitContext>
    {
    public:
        [[nodiscard]]
        std::string GetName() const override;

        void Begin(VisitContext& ctx, const frxml::dom& parent, const frxml::dom& current, int depth) override;

        void End(VisitContext& ctx, const frxml::dom& parent, const frxml::dom& current, int depth) override;
    };

    /**
     * @brief A visitor that creates bridge of `function` elements with C#
     */
    class FunctionVisitor : public shared::InterfaceVisitor<VisitContext>
    {
    public:
        [[nodiscard]]
        std::string GetName() const override;

        void Begin(VisitContext& ctx, const frxml::dom& parent, const frxml::dom& current, int depth) final;

        void End(VisitContext& ctx, const frxml::dom& parent, const frxml::dom& current, int depth) final;
    };

    /**
     * @brief A visitor that creates bridge of `method` elements with C#
     */
    class MethodVisitor : public FunctionVisitor
    {
    public:
        [[nodiscard]]
        std::string GetName() const override;
    };

    /**
     * @brief A visitor that creates bridge of `ctor` elements with C#
     */
    class CtorVisitor final : public MethodVisitor
    {
    public:
        [[nodiscard]]
        std::string GetName() const override;
    };

    /**
     * @brief A visitor that creates bridge of `dtor` elements with C#
     */
    class DtorVisitor final : public MethodVisitor
    {
    public:
        [[nodiscard]]
        std::string GetName() const override;
    };
}
