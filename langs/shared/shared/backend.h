#pragma once

#include <string>

#define MODULE(name) int module__##name(const std::string& xmlPath, const std::string& libPath, const std::string& outPath)
#define MODULE_REF(name) module__##name
