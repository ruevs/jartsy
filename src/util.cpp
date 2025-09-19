//-----------------------------------------------------------------------------
// Utility functions, mostly various kinds of vector math (working on real
// numbers, not working on quantities in the symbolic algebra system).
//
// Copyright 2008-2013 Jonathan Westhues.
//-----------------------------------------------------------------------------
#include "jartsy.h"

void JARTSy::AssertFailure(const char *file, unsigned line, const char *function,
                               const char *condition, const char *message) {
    std::string formattedMsg;
    formattedMsg += jartsyprintf("File %s, line %u, function %s:\n", file, line, function);
    formattedMsg += jartsyprintf("Assertion failed: %s.\n", condition);
    formattedMsg += jartsyprintf("Message: %s.\n", message);
    JARTSy::Platform::FatalError(formattedMsg);
}

std::string JARTSy::jartsyprintf(const char *fmt, ...) {
    va_list va;

    va_start(va, fmt);
    int size = vsnprintf(NULL, 0, fmt, va);
    jartsyassert(size >= 0, "vsnprintf could not encode string");
    va_end(va);

    std::string result;
    result.resize((size_t)size + 1u);

    va_start(va, fmt);
    vsnprintf(&result[0], size + 1u, fmt, va);
    va_end(va);

    result.resize((size_t)size);
    return result;
}

namespace JARTSy {
namespace Platform {

    void FatalError(const std::string &message) {
        fprintf(stderr, "%s", message.c_str());
        abort();
    }

} // namespace Platform
} // namespace JARTSy
