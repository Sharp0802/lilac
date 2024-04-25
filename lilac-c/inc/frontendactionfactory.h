#ifndef FRONTENDACTIONFACTORY_H
#define FRONTENDACTIONFACTORY_H
#include <clang/Tooling/Tooling.h>

namespace lilac
{
    class FrontendActionFactory : public clang::tooling::FrontendActionFactory
    {
    public:
        FrontendActionFactory();

        std::unique_ptr<clang::FrontendAction> create() override;
    };
}

#endif //FRONTENDACTIONFACTORY_H
