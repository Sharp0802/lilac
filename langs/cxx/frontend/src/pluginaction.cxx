#include "pluginaction.h"
#include "typemap.h"

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

    frxml::dom* LilacASTVisitor::GetNamespaceDOM(clang::NamedDecl* decl)
    {
        const auto ctx = decl->getDeclContext();

        if (ctx->getParent()->getDeclKind() != clang::Decl::Namespace)
        {
            static auto err = m_Diag.getCustomDiagID(
                Level::Error,
                "Couldn't handle a nested declaration");

            m_Sema.Diag(decl->getLocation(), err);

            return nullptr;
        }

        std::stack<clang::NamespaceDecl*> stk;
        for (
            auto cur = ctx->getParent();
            cur != nullptr && cur->getDeclKind() == clang::Decl::Namespace;
            cur = cur->getParent())
        {
            stk.emplace(clang::cast<clang::NamespaceDecl>(cur));
        }

        frxml::dom* cur = &m_Root;
        for (; !stk.empty(); stk.pop())
        {
            frxml::dom* node = nullptr;

            auto ns = stk.top()->getNameAsString();
            for (auto& child: cur->children())
            {
                if (child.tag().view() != "namespace")
                    continue;
                if (child.attr()["name"].view() == ns)
                    node = &child;
            }
            if (!node)
            {
                node = &cur->children().emplace_back(frxml::dom::element(
                    "namespace",
                    { { "name", ns } }));
            }

            cur = node;
        }

        return cur;
    }

    LilacASTVisitor::LilacASTVisitor(clang::Sema& sema, clang::TranslationUnitDecl* unit)
        : m_Sema(sema),
          m_Diag(m_Sema.getDiagnostics()),
          m_Root(frxml::dom::element("assembly"))
    {
        const auto rOpt = llvm::cl::getRegisteredOptions().lookup("o");

        if (const auto opt = dynamic_cast<llvm::cl::opt<std::string>*>(rOpt); !opt)
        {
            const auto err = m_Diag.getCustomDiagID(
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

        std::ofstream ofs(m_OutputFilename + ".xml");
        ofs << static_cast<std::string>(frxml::doc{ m_Root });
    }

    const clang::Type* GetUnderlyingType(clang::Sema& sema, clang::SourceLocation loc, const clang::Type* t)
    {
        while (true)
        {
            if (const auto usingT = clang::dyn_cast<clang::UsingType>(t))
                t = usingT->getUnderlyingType().getTypePtr();
            else if (const auto decltypeT = clang::dyn_cast<clang::DecltypeType>(t))
                t = decltypeT->getUnderlyingType().getTypePtr();
            else if (auto macroT = clang::dyn_cast<clang::MacroQualifiedType>(t))
                t = macroT->getUnderlyingType().getTypePtr();
            else if (clang::isa<clang::TypedefType>(t))
            {
                static auto err = sema.getDiagnostics().getCustomDiagID(
                    clang::DiagnosticsEngine::Error,
                    "Couldn't retrieve underlying type from typedef declaration");
                sema.Diag(loc, err);
                return nullptr;
            }

            return t;
        }
    }

    bool LilacASTVisitor::TraverseEnumDecl(clang::EnumDecl* decl)
    {
        std::vector<frxml::dom> children;

        EnumVisitor visitor{
            [&](const clang::EnumConstantDecl* constant)
            {
                const auto value    = constant->getValue();
                const auto valueStr = decl->getIntegerType()->isSignedIntegerType()
                    ? std::to_string(value.getSExtValue())
                    : std::to_string(value.getZExtValue());

                children.push_back(frxml::dom::element(
                    "constant",
                    {
                        { "name", constant->getNameAsString() },
                        { "value", valueStr }
                    }));
            }
        };
        visitor.TraverseDecl(decl);

        const auto underlyingT = decl->getIntegerType();
        const auto typeName    = GetBuiltinTypeName(underlyingT.isNull()
            ? clang::BuiltinType::Kind::Int
            : clang::cast<clang::BuiltinType>(GetUnderlyingType(
                    m_Sema,
                    decl->getLocation(),
                    underlyingT.getTypePtr())
            )->getKind());

        const auto ns = GetNamespaceDOM(decl);
        if (!ns) return true;
        ns->children().push_back(frxml::dom::element(
            "enum",
            {
                { "name", decl->getNameAsString() },
                { "type", typeName }
            },
            children
        ));
        return true;
    }

    bool LilacASTVisitor::TraverseCXXRecordDecl(clang::CXXRecordDecl* decl)
    {
        std::vector<frxml::dom> children;

        CXXRecordVisitor visitor{
            [&](clang::CXXMethodDecl* method)
            {
                // TODO
            }
        };
        visitor.TraverseDecl(decl);

        const auto ns = GetNamespaceDOM(decl);
        if (!ns) return true;

        const auto typeInfo = decl->getASTContext().getTypeInfo(decl->getTypeForDecl());
        ns->children().push_back(frxml::dom::element(
            "record",
            {
                { "name", decl->getNameAsString() },
                { "size", std::to_string(typeInfo.Width / 8) },
                { "align", std::to_string(typeInfo.Align) }
            },
            children
        ));
        return true;
    }

    bool LilacASTVisitor::TraverseFunctionDecl(clang::FunctionDecl* decl)
    {
        if (clang::isa<clang::CXXMethodDecl>(decl))
            return true;

        // TODO

        return true;
    }

    LilacASTVisitor::EnumVisitor::EnumVisitor(std::function<void(clang::EnumConstantDecl* constant)> delegate)
        : m_Delegate(std::move(delegate))
    {
    }

    bool LilacASTVisitor::EnumVisitor::TraverseEnumConstantDecl(clang::EnumConstantDecl* decl)
    {
        m_Delegate(decl);
        return true;
    }

    LilacASTVisitor::CXXRecordVisitor::CXXRecordVisitor(std::function<void(clang::CXXMethodDecl*)> delegate)
        : m_Delegate(std::move(delegate))
    {
    }

    bool LilacASTVisitor::CXXRecordVisitor::TraverseCXXMethodDecl(clang::CXXMethodDecl* decl)
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
