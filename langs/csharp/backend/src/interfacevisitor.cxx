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

#include "interfacevisitor.h"
#include "shared/char.h"

#define TAB "\t"

static lilac::shared::GenericInterfaceVisitor<
    lilac::csharp::VisitContext,
    lilac::csharp::NamespaceVisitor,
    lilac::csharp::RecordVisitor,
    lilac::csharp::EnumVisitor,
    lilac::csharp::EnumConstantVisitor,
    lilac::csharp::FunctionVisitor,
    lilac::csharp::MethodVisitor,
    lilac::csharp::CtorVisitor,
    lilac::csharp::DtorVisitor
> s_GenericInterfaceVisitor;

void ForeachChildren(lilac::csharp::VisitContext& ctx, const frxml::dom& current, int depth)
{
    for (auto i = 0; i < current.children().size(); ++i)
    {
        if (i)
            ctx.Output << lilac::shared::endl;
        s_GenericInterfaceVisitor.Begin(ctx, current, current.children()[i], depth);
    }
}

#pragma region AssemblyInterfaceVisitor

void lilac::csharp::CSharpAssemblyInterfaceVisitor::Begin(
    VisitContext&     ctx,
    const frxml::dom& parent,
    const frxml::dom& current,
    int               depth)
{
    if (current.tag().view() != "assembly")
        throw shared::exception("AssemblyInterfaceVisitor should deal with assembly element only", current);

    const auto indent = shared::GetIndent(depth);

    auto addDepth = 0;
    if (!ctx.RootNamespace.empty())
    {
        ctx.Output
            << indent << "namespace " << ctx.RootNamespace << shared::endl
            << indent << "{" << shared::endl;
        addDepth++;
    }

    ForeachChildren(ctx, current, depth + addDepth);
}

void lilac::csharp::CSharpAssemblyInterfaceVisitor::End(
    VisitContext&     ctx,
    const frxml::dom& parent,
    const frxml::dom& current,
    int               depth)
{
    if (ctx.RootNamespace.empty())
        return;

    const auto indent = shared::GetIndent(depth);
    ctx.Output << indent << "}" << shared::endl;
}

#pragma endregion

#pragma region NamespaceVisitor

std::string lilac::csharp::NamespaceVisitor::getName() const
{
    return "namespace";
}

void lilac::csharp::NamespaceVisitor::Begin(
    VisitContext&     ctx,
    const frxml::dom& parent,
    const frxml::dom& current,
    int               depth)
{
    const auto indent = shared::GetIndent(depth);

    ctx.Output
        << indent << "namespace " << current.attr().at("name").view() << shared::endl
        << indent << '{' << shared::endl;

    ForeachChildren(ctx, current, depth + 1);
}

void lilac::csharp::NamespaceVisitor::End(
    VisitContext&     ctx,
    const frxml::dom& parent,
    const frxml::dom& current,
    int               depth)
{
    const auto indent = shared::GetIndent(depth);
    ctx.Output << indent << "}" << shared::endl;
}

#pragma endregion

#pragma region RecordVisitor

std::string lilac::csharp::RecordVisitor::getName() const
{
    return "record";
}

void lilac::csharp::RecordVisitor::Begin(
    VisitContext&     ctx,
    const frxml::dom& parent,
    const frxml::dom& current,
    int               depth)
{
    const auto indent = shared::GetIndent(depth);

    const auto size  = current.attr().at("size").view();
    const auto align = current.attr().at("align").view();

#define INTEROP_NS "global::System.Runtime.InteropServices."
#define COMPILER_NS "global::System.Runtime.CompilerServices."

    ctx.Output
        << indent << "[" INTEROP_NS"StructLayout(" INTEROP_NS"Explicit, Size=" << size << ", Pack=" << align << ")]\n"
        << indent << "public struct InteropTest\n"
        << indent << "{\n"
        << indent << TAB "[" INTEROP_NS"FieldOffset(0)]\n"
        << indent << TAB "private byte __ref;\n"
        << indent << "\n"
        << indent << TAB "private unsafe void* This => " COMPILER_NS"Unsafe.AsPointer(ref __ref);\n";

#undef COMPILER_NS
#undef INTEROP_NS

    if (!current.children().empty())
        ctx.Output << shared::endl;

    ForeachChildren(ctx, current, depth + 1);
}

void lilac::csharp::RecordVisitor::End(
    VisitContext& ctx,
    const frxml::dom& parent,
    const frxml::dom& current,
    int depth)
{
    const auto indent = shared::GetIndent(depth);
    ctx.Output << indent << "}" << shared::endl;
}

#pragma endregion

std::string lilac::csharp::EnumVisitor::getName() const
{
    return "enum";
}

void lilac::csharp::EnumVisitor::Begin(
    VisitContext& ctx,
    const frxml::dom& parent,
    const frxml::dom& current,
    int depth)
{
}

void lilac::csharp::EnumVisitor::End(
    VisitContext& ctx,
    const frxml::dom& parent,
    const frxml::dom& current,
    int depth)
{
}

std::string lilac::csharp::EnumConstantVisitor::getName() const
{
    return "constant";
}

std::string lilac::csharp::FunctionVisitor::getName() const
{
    return "function";
}

std::string lilac::csharp::MethodVisitor::getName() const
{
    return "method";
}

std::string lilac::csharp::CtorVisitor::getName() const
{
    return "ctor";
}

std::string lilac::csharp::DtorVisitor::getName() const
{
    return "dtor";
}
