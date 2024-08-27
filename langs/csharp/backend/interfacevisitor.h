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
        std::ostream& Output;
    };

    class CSharpAssemblyInterfaceVisitor final : public shared::AssemblyVisitor<VisitContext>
    {
    public:
        void Begin(VisitContext& ctx, const frxml::dom& parent, const frxml::dom& current, int depth) override;

        void End(VisitContext& ctx, const frxml::dom& parent, const frxml::dom& current, int depth) override;
    };

    class NamespaceVisitor final : public shared::InterfaceVisitor<VisitContext>
    {
    public:
        [[nodiscard]]
        std::string getName() const override;

        void Begin(VisitContext& ctx, const frxml::dom& parent, const frxml::dom& current, int depth) override;

        void End(VisitContext& ctx, const frxml::dom& parent, const frxml::dom& current, int depth) override;
    };

    class RecordVisitor final : public shared::InterfaceVisitor<VisitContext>
    {
    public:
        [[nodiscard]]
        std::string getName() const override;

        void Begin(VisitContext& ctx, const frxml::dom& parent, const frxml::dom& current, int depth) override;

        void End(VisitContext& ctx, const frxml::dom& parent, const frxml::dom& current, int depth) override;
    };

    class EnumVisitor final : public shared::InterfaceVisitor<VisitContext>
    {
    public:
        [[nodiscard]]
        std::string getName() const override;

        void Begin(VisitContext& ctx, const frxml::dom& parent, const frxml::dom& current, int depth) override;

        void End(VisitContext& ctx, const frxml::dom& parent, const frxml::dom& current, int depth) override;
    };

    class EnumConstantVisitor final : public shared::InterfaceVisitor<VisitContext>
    {
    public:
        [[nodiscard]]
        std::string getName() const override;

        void Begin(VisitContext& ctx, const frxml::dom& parent, const frxml::dom& current, int depth) override;

        void End(VisitContext& ctx, const frxml::dom& parent, const frxml::dom& current, int depth) override;
    };

    class FunctionVisitor : public shared::InterfaceVisitor<VisitContext>
    {
    public:
        [[nodiscard]]
        std::string getName() const override;

        void Begin(VisitContext& ctx, const frxml::dom& parent, const frxml::dom& current, int depth) final;

        void End(VisitContext& ctx, const frxml::dom& parent, const frxml::dom& current, int depth) final;
    };

    class MethodVisitor : public FunctionVisitor
    {
    public:
        [[nodiscard]]
        std::string getName() const override;
    };

    class CtorVisitor final : public MethodVisitor
    {
    public:
        [[nodiscard]]
        std::string getName() const override;
    };

    class DtorVisitor final : public MethodVisitor
    {
    public:
        [[nodiscard]]
        std::string getName() const override;
    };
}
