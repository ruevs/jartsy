#ifndef JARTSY_H
#define JARTSY_H

#include <cstdarg>
#include <algorithm>
#include <string>
#include <vector>
#include <functional>

#include "platform/platform.h"

// The type of floating point number used throughout (float or double or whatever)
using Float = float;
static constexpr Float LENGTH_EPS = 1e-6;

// The few floating-point equality comparisons in jartsy should have been
// carefully considered, so we disable the -Wfloat-equal warning for them
#ifdef __clang__
#    define EXACT(expr) \
        (_Pragma("clang diagnostic push") _Pragma("clang diagnostic ignored \"-Wfloat-equal\"")( \
            expr) _Pragma("clang diagnostic pop"))
#else
#    define EXACT(expr) (expr)
#endif

template<typename T>
inline T Sqr(const T v) {
    return v * v;
}


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
#define CO(v) (v).x, (v).y, (v).z
#define DBPTRI(a,op,b, eq, c) \
    dbp("(%.3f %.3f %.3f) %s (%.3f %.3f %.3f) %s (%.3f %.3f %.3f)", CO(a), op, CO(b), eq,\
        CO(c))

namespace JARTSy {

using std::min;
using std::max;

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

#endif
