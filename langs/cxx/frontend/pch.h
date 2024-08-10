#pragma once

/********************************
 * Clang/LLVM                   *
 ********************************/
#include <clang/AST/ASTContext.h>
#include <clang/AST/ASTConsumer.h>
#include <clang/AST/Decl.h>
#include <clang/AST/Mangle.h>
#include <clang/AST/RecursiveASTVisitor.h>
#include <clang/CodeGen/BackendUtil.h>
#include <clang/CodeGen/CodeGenAction.h>
#include <clang/Frontend/CompilerInstance.h>
#include <clang/Frontend/FrontendAction.h>
#include <clang/Frontend/FrontendPluginRegistry.h>
#include <clang/Tooling/CommonOptionsParser.h>
#include <clang/Tooling/Tooling.h>
#include <clang/Tooling/Tooling.h>
#include <clang/Tooling/JSONCompilationDatabase.h>
#include <clang/Sema/ParsedAttr.h>
#include <clang/Sema/Sema.h>
#include <clang/Sema/SemaConsumer.h>
#include <clang/Sema/SemaDiagnostic.h>
#include <llvm/IR/Attributes.h>
#include <llvm/Linker/Linker.h>
#include <llvm/Support/CommandLine.h>

/********************************
 * External Dependencies        *
 ********************************/
#include <frxml.h>

/********************************
 * Standard Libraries           *
 ********************************/
#include <algorithm>
#include <fstream>
#include <utility>
#include <memory>
#include <ranges>
#include <sstream>
#include <stack>
#include <variant>
#include <vector>
