#include "pluginaction.h"

namespace lilac::cxx
{
    std::unique_ptr<clang::ASTConsumer> LilacAction::CreateASTConsumer(
        clang::CompilerInstance& ci,
        llvm::StringRef)
    {
        return std::make_unique<LilacASTConsumer>();
    }

    clang::PluginASTAction::ActionType LilacAction::getActionType()
    {
        return AddAfterMainAction;
    }

    bool LilacAction::ParseArgs(const clang::CompilerInstance& CI, const std::vector<std::string>& arg)
    {
        return true;
    }

    void LilacASTConsumer::InitializeSema(clang::Sema& sema)
    {
        m_Sema = &sema;
    }

    void LilacASTConsumer::HandleTranslationUnit(clang::ASTContext& context)
    {
        LilacASTVisitor visitor(*m_Sema, context.getTranslationUnitDecl());
        visitor.TraverseDecl(context.getTranslationUnitDecl());
    }

    LilacASTVisitor::LilacASTVisitor(clang::Sema& sema, clang::TranslationUnitDecl* unit)
        : m_Sema(sema),
          m_Diag(m_Sema.getDiagnostics())
    {
        const auto rOpt = llvm::cl::getRegisteredOptions().lookup("o");
        const auto opt  = dynamic_cast<llvm::cl::opt<std::string>*>(rOpt);

        if (!opt)
        {
            auto err = m_Diag.getCustomDiagID(
                Level::Fatal,
                "Couldn't get output file name; Did you specified '-o' option?");
            m_Sema.Diag(unit->getLocation(), err);
        }
        else
        {
            m_OutputFilename = opt->getValue();
        }
    }

    LilacASTVisitor::~LilacASTVisitor()
    {
        if (m_OutputFilename.empty())
            return;

        // TODO : Serialize into XML
    }

    }

    bool LilacASTVisitor::TraverseEnumDecl(clang::EnumDecl* decl)
    {
        std::vector<frxml::dom> children;
        EnumVisitor visitor = [&](clang::EnumConstantDecl* constant)
        {
            children.push_back()
        };
    }

    bool LilacASTVisitor::TraverseCXXRecordDecl(clang::CXXRecordDecl* decl)
    {
    }

    bool LilacASTVisitor::TraverseFunctionDecl(clang::FunctionDecl* decl)
    {
    }

    bool LilacASTVisitor::EnumVisitor::TraverseEnumConstantDecl(clang::EnumConstantDecl* decl)
    {
        m_Delegate(decl);
        return true;
    }
}

[[maybe_unused]]
static clang::FrontendPluginRegistry::Add<lilac::cxx::LilacAction> X(
    "lilac-cxx-frontend",
    "A plugin of clang-frontend for LILAC system"
);
