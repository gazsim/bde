// bsla_printf.h                                                      -*-C++-*-
#ifndef INCLUDED_BSLA_PRINTF
#define INCLUDED_BSLA_PRINTF

#include <bsls_ident.h>
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide a macro to indicate 'printf'-style arguments.
//
//@CLASSES:
//
//@MACROS:
//  BSLA_PRINTF(FMTIDX, STARTIDX): validate 'printf' format and arguments
//  BSLA_PRINTF_IS_ACTIVE: 1 if 'BSLA_PRINTF' is active and 0 otherwise
//
//@SEE ALSO: bsla_annotations
//
//@AUTHOR: Andrew Paprocki (apaprock), Bill Chapman (bchapman2)
//
//@DESCRIPTION: This component provides a preprocessor macro that allows the
// designation of a given function argument as a 'printf'-style format string,
// and arguments starting at a certain index in the argument list to be
// formatted according to that string.
//
///Macro Reference
///---------------
//: o BBSLA_PRINTF(FMTIDX, STARTIDX)
//:
//: o This annotation instructs the compiler to perform additional compile-time
//:   checks on so-annotated functions that take 'printf'-style arguments,
//:   which should be type-checked against a format string.
//: o The 'FMTIDX' parameter is the one-based index to the 'const char *'
//:   format string.  The 'STARTIDX' parameter is the one-based index to the
//:   first variable argument to type-check against that format string.  For
//:   example:
//..
//  extern int my_printf(void *obj, const char *format, ...) BSLA_PRINTF(2, 3);
//..
//
//: o BSLA_PRINTF_IS_ACTIVE
//:
//: The macro 'BSLA_PRINTF_IS_ACTIVE' is defined to 0 on platforms or compilers
//: where 'BSLA_PRINTF' expands to nothing, and 1 otherwise.
//
///Usage
///-----
//
///Example 1: String Printf:
/// - - - - - - - - - - - -
// First, we define a function 'strPrintf' which takes a variable number of
// arguments.  The second argument is the format string, and we annnotate it
// with 'BSLA_PRINTF'
//..
//  std::string strPrintf(size_t *numChars, const char *format, ...)
//                                                           BSLA_PRINTF(2, 3);
//  std::string strPrintf(size_t *numChars, const char *format, ...)
//      // Do an 'sprintf' write to a 'std::string' and return the string by
//      // value.  Ensure that the write can't overflow unless memory or
//      // address space is exhausted.  The specified '*numChars' is the number
//      // of characters written, the specified 'format' is the 'printf'-style
//      // format string, and the specified '...' is the variable-length list
//      // of arguments to be formatted.
//  {
//      std::string ret = " ";
//
//      va_list ap;
//      va_start(ap, format);
//
//      // 'vnsprintf' returns the number of chars that WOULD have been written
//      // (not including the terminating '\0') had the buffer been long
//      // enough.
//
//      *numChars = ::vsnprintf(&ret[0], 1, format, ap);
//      va_end(ap);
//
//      ret.resize(*numChars + 1);
//
//      va_start(ap, format);
//      *numChars = ::vsnprintf(&ret[0], *numChars + 1, format, ap);
//      va_end(ap);
//
//      BSLS_ASSERT(::strlen(ret.c_str()) == *numChars);
//
//      ret.resize(*numChars);
//      return ret;
//  }
//..
// Then, in 'main', we call the function correctly a couple of times:
//..
//  size_t len;
//  std::string s;
//
//  s = strPrintf(&len, "%s %s %s %g\n", "woof", "meow", "arf", 23.5);
//  assert("woof meow arf 23.5\n" == s);
//  assert(19 == len);
//  assert(len == s.length());
//
//  s = strPrintf(&len, "%s %s %s %s %s %s %s %s %s\n",
//      "The", "rain", "in", "Spain", "falls", "mainly", "in", "the", "plain");
//  assert("The rain in Spain falls mainly in the plain\n" == s);
//  assert(44 == len);
//  assert(len == s.length());
//..
// Now, we call it with too many arguments and of the wrong type:
//..
//  s = strPrintf(&len, "%c %f %g", "arf", 27, 32, 65, 27);
//..
// Finally, we observe the compiler warnings on g++.
//..
//  .../bsla/bsla_printf.t.cpp:324:58: warning: format '%c' expects argument of
//   type 'int', but argument 3 has type 'const char*' [-Wformat=]
//       s = strPrintf(&len, "%c %f %g", "arf", 27, 32, 65, 27);
//                                                            ^
//  .../bsla/bsla_printf.t.cpp:324:58: warning: format '%f' expects argument of
//   type 'double', but argument 4 has type 'int' [-Wformat=]
//  .../bsla/bsla_printf.t.cpp:324:58: warning: format '%g' expects argument of
//   type 'double', but argument 5 has type 'int' [-Wformat=]
//  .../bsla/bsla_printf.t.cpp:324:58: warning: too many arguments for format [
//  -Wformat-extra-args]
//..

#include <bsls_platform.h>

#if defined(BSLS_PLATFORM_CMP_GNU)   ||                                      \
    defined(BSLS_PLATFORM_CMP_CLANG) ||                                      \
    defined(BSLS_PLATFORM_CMP_HP)    ||                                      \
    defined(BSLS_PLATFORM_CMP_IBM)
    #define BSLA_PRINTF(fmt, arg) __attribute__((format(printf, fmt, arg)))

    #define BSLA_PRINTF_IS_ACTIVE 1
#else
    #define BSLA_PRINTF(fmt, arg)

    #define BSLA_PRINTF_IS_ACTIVE 0
#endif

#endif

// ----------------------------------------------------------------------------
// Copyright 2019 Bloomberg Finance L.P.
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
// ----------------------------- END-OF-FILE ----------------------------------
