// balxml_base64parser.h                                              -*-C++-*-
#ifndef INCLUDED_BALXML_BASE64PARSER
#define INCLUDED_BALXML_BASE64PARSER

#include <bsls_ident.h>
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide push parser for Base64 types.
//
//@DEPRECATED: Use bdlde_base64decoder instead.
//
//@CLASSES:
//  balxml::Base64Parser: push parser for Base64 types
//
//@DESCRIPTION: The 'balxml::Base64Parser' class template provided by this
// component can be used to parse Base64 characters into one of the supported
// Base64 types, which are 'bsl::vector<char>' and 'bsl::string'.  The 'TYPE'
// parameter can be one of these two types.
//
// This class template is a model of the 'PushParser' concept, which contains
// the following methods:
//..
//  int beginParse(TYPE *object);
//      // Prepare the parser to start parsing a new value and associate the
//      // specified 'object' with the parser.  Return 0 if successful and
//      // non-zero otherwise.
//
//  int endParse();
//      // Ends the parse operation and store the value parsed from the pushed
//      // characters into the associated object.  Return 0 if successful and
//      // non-zero otherwise.  The behavior is undefined unless an object is
//      // associated with this parser.  Upon successful completion, the parser
//      // will be disassociated with the object.
//
//  template <typename INPUT_ITERATOR>
//  int pushCharacters(INPUT_ITERATOR begin, INPUT_ITERATOR end);
//      // Push the characters ranging from the specified 'begin' up to (but
//      // not including) the specified 'end' into this parser.  Return 0 if
//      // successful and non-zero otherwise.  The parameterized
//      // 'INPUT_ITERATOR' must be dereferenceable to a 'char' value.  The
//      // behavior is undefined unless an object is associated with this
//      // parser.
//..
//
///Usage
///-----
// The following snippets of code illustrate the usage of this component.
// Suppose you had an input stream that contained Base64 data.  The following
// 'loadFromBase64Stream' function loads this data into an 'bsl::vector<char>'
// blob:
//..
//  #include <balxml_base64parser.h>
//
//  #include <istream>
//  #include <iterator>
//  #include <vector>
//
//  using namespace BloombergLP;
//
//  int loadFromBase64Stream(bsl::vector<char> *result, bsl::istream& stream)
//  {
//      enum { FAILURE = -1 };
//
//      balxml::Base64Parser<bsl::vector<char> > parser;
//
//      if (0 != parser.beginParse(result)) {
//          return FAILURE;
//      }
//
//      if (0 != parser.pushCharacters(bsl::istreambuf_iterator<char>(stream),
//                                     bsl::istreambuf_iterator<char>())) {
//          return FAILURE;
//      }
//
//      return parser.endParse();
//  }
//..
// The following function demonstrates the 'loadFromBase64Stream' function:
//..
//  #include <sstream>
//
//  void usageExample()
//  {
//      const char INPUT[] = "YWJjZA==";  // "abcd" in Base64
//
//      bsl::vector<char>  vec;
//      bsl::istringstream iss(INPUT);
//
//      int result = loadFromBase64Stream(&vec, iss);
//
//      assert(0   == result);
//      assert(4   == vec.size());
//      assert('a' == vec[0]);
//      assert('b' == vec[1]);
//      assert('c' == vec[2]);
//      assert('d' == vec[3]);
//  }
//..

#include <balscm_version.h>

#include <bdlde_base64decoder.h>

#include <bdlat_valuetypefunctions.h>

#include <bsl_iterator.h>

#include <bsls_assert.h>
#include <bsls_review.h>

namespace BloombergLP {
namespace balxml {

                          // ========================
                          // class Base64Parser<TYPE>
                          // ========================

template <class TYPE>
class Base64Parser {
    // This is a push parser for supported Base64 types ('bsl::vector<char>' or
    // 'bsl::string').

    // PRIVATE DATA MEMBERS
    bdlde::Base64Decoder  d_base64Decoder;   // decoder
    TYPE                 *d_object_p;        // associated object

  private:
    // NOT IMPLEMENTED
    Base64Parser(const Base64Parser&);
    Base64Parser& operator=(const Base64Parser&);

  public:
    // CREATORS
    Base64Parser();
        // Create a parser for parsing Base64 types.

#ifdef DOXYGEN    // Generated by compiler:

    ~Base64Parser();
        // Destroy this parser object.
#endif

    // MANIPULATORS
    int beginParse(TYPE *object);
        // Prepare the parser to start parsing a new value and associate the
        // specified 'object' with the parser.  Return 0 if successful and
        // non-zero otherwise.

    int endParse();
        // Ends the parse operation and store the value parsed from the pushed
        // characters into the associated object.  Return 0 if successful and
        // non-zero otherwise.  The behavior is undefined unless an object is
        // associated with this parser.  Upon successful completion, the parser
        // will be disassociated with the object.

    template <class INPUT_ITERATOR>
    int pushCharacters(INPUT_ITERATOR begin, INPUT_ITERATOR end);
        // Push the characters ranging from the specified 'begin' up to (but
        // not including) the specified 'end' into this parser.  Return 0 if
        // successful and non-zero otherwise.  The parameterized
        // 'INPUT_ITERATOR' must be dereferenceable to a 'char' value.  The
        // behavior is undefined unless an object is associated with this
        // parser.
};

// ============================================================================
//                            INLINE DEFINITIONS
// ============================================================================

                          // ------------------------
                          // class Base64Parser<TYPE>
                          // ------------------------

// CREATORS
template <class TYPE>
Base64Parser<TYPE>::Base64Parser()
: d_base64Decoder(true)  // 'true' indicates report errors
, d_object_p(0)
{
}

// MANIPULATORS
template <class TYPE>
int Base64Parser<TYPE>::beginParse(TYPE *object)
{
    BSLS_REVIEW(object);

    enum { k_SUCCESS = 0 };

    d_base64Decoder.resetState();
    d_object_p = object;

    bdlat_ValueTypeFunctions::reset(d_object_p);

    return k_SUCCESS;
}

template <class TYPE>
int Base64Parser<TYPE>::endParse()
{
    BSLS_REVIEW(d_object_p);

    enum { k_SUCCESS = 0, k_FAILURE = -1 };

    bsl::back_insert_iterator<TYPE> outputIterator(*d_object_p);

    int status = d_base64Decoder.endConvert(outputIterator);

    if (0 > status) {
        return k_FAILURE;                                             // RETURN
    }

    BSLS_REVIEW(0 == status);  // nothing should be retained by decoder

    d_object_p = 0;

    return k_SUCCESS;
}

template <class TYPE>
template <class INPUT_ITERATOR>
int Base64Parser<TYPE>::pushCharacters(INPUT_ITERATOR begin,
                                       INPUT_ITERATOR end)
{
    BSLS_REVIEW(d_object_p);

    enum { k_SUCCESS = 0, k_FAILURE = -1 };

    bsl::back_insert_iterator<TYPE> outputIterator(*d_object_p);

    int status = d_base64Decoder.convert(outputIterator, begin, end);

    if (0 > status) {
        return k_FAILURE;                                             // RETURN
    }

    BSLS_REVIEW(0 == status);  // nothing should be retained by decoder

    return k_SUCCESS;
}

}  // close package namespace
}  // close enterprise namespace

#endif

// ----------------------------------------------------------------------------
// Copyright 2015 Bloomberg Finance L.P.
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
