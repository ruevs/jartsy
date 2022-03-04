#include <cstdarg>
#include <algorithm>
#include <string>
#include <vector>
#include <functional>

#include "platform/platform.h"


// Debugging functions
#if defined(__GNUC__)
#    define jartsyassert(condition, message) \
        do { \
            if(__builtin_expect((condition), true) == false) { \
                JARTSy::AssertFailure(__FILE__, __LINE__, __func__, #condition, message); \
                __builtin_unreachable(); \
            } \
        } while(0)
#else
#    define jartsyassert(condition, message) \
        do { \
            if((condition) == false) { \
                JARTSy::AssertFailure(__FILE__, __LINE__, __func__, #condition, message); \
                abort(); \
            } \
        } while(0)
#endif

#define dbp JARTSy::Platform::DebugPrint
#define DBPTRI(tri) \
    dbp("tri: (%.3f %.3f %.3f) (%.3f %.3f %.3f) (%.3f %.3f %.3f)", CO((tri).a), CO((tri).b), \
        CO((tri).c))

namespace JARTSy {

[[noreturn]] void AssertFailure(const char *file, unsigned line, const char *function,
                                const char *condition, const char *message);
namespace Platform {
    // Handling fatal errors.
    [[noreturn]] void FatalError(const std::string &message);
}

std::string jartsyprintf(const char *fmt, ...);

} // namespace JARTSy


#ifndef __OBJC__
using namespace JARTSy;
#endif