#include "pluginaction.h"
#include "codegen.h"

namespace lilac::cxx
{
    std::unique_ptr<clang::ASTConsumer> LilacAction::CreateASTConsumer(
        clang::CompilerInstance& ci,
        llvm::StringRef)
    {
        return std::make_unique<LilacASTConsumer>(m_Context);
    }

    bool LilacAction::ParseArgs(const clang::CompilerInstance& ci, const std::vector<std::string>&)
    {
        if (ci.getCodeGenOpts().DiscardValueNames)
        {
            llvm::errs() << "cannot use LILAC with discard-value-name; may '-fno-discard-value-names' not set?\n";
            return false;
        }

        return true;
    }

    void LilacAction::EndSourceFileAction()
    {
        PluginASTAction::EndSourceFileAction();

        auto& ci = getCompilerInstance();

        clang::CompilerInstance fci;
        fci.setSourceManager(ci.getSourceManagerPtr().get());
        fci.setInvocation(ci.getInvocationPtr());
        fci.createDiagnostics();

        clang::EmitLLVMOnlyAction action;
        fci.ExecuteAction(action);

        std::error_code      ec;
        llvm::raw_fd_ostream os(fci.getFrontendOpts().OutputFile, ec);
        if (!ec)
        {
            llvm::errs()
                << "Error occurred opening file '" << fci.getFrontendOpts().OutputFile
                << "': " << std::system_error(ec).what() << '\n';
        }

        const auto module = action.takeModule();

        LilacCodeGen lcg(m_Context, *module);
        lcg.Execute();

        module->print(os, nullptr);
    }

    clang::PluginASTAction::ActionType LilacAction::getActionType()
    {
        return ReplaceAction;
    }

    LilacASTConsumer::LilacASTConsumer(LilacContext& context): m_Visitor(context)
    {
    }

    void LilacASTConsumer::InitializeSema(clang::Sema& sema)
    {
        m_Visitor.SetSema(sema);
    }

    void LilacASTConsumer::HandleTranslationUnit(clang::ASTContext& context)
    {
        m_Visitor.TraverseDecl(context.getTranslationUnitDecl());
    }
}

[[maybe_unused]]
static clang::FrontendPluginRegistry::Add<lilac::cxx::LilacAction> X(
    "lilac-cxx-frontend",
    "A plugin of clang-frontend for LILAC system"
);
