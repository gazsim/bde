// bdede_utf8util.t.cpp       -*-C++-*-
#include <bdede_utf8util.h>

#include <bsl_string.h>
#include <bslma_testallocator.h>
#include <bslma_defaultallocatorguard.h>
#include <bsls_platformutil.h>

#include <bsl_iostream.h>
#include <bsl_string.h>

#include <bsl_cstdlib.h>
#include <bsl_cstring.h>

using namespace BloombergLP;
using bsl::cout;
using bsl::cerr;
using bsl::endl;

//=============================================================================
//                             TEST PLAN
//-----------------------------------------------------------------------------
//                              Overview
//                              --------
//
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
// [ 1] BREATHING TEST
// [-1] random number generator
// [-2] 'utf8Encode', 'decode'

//=============================================================================
//                      STANDARD BDE ASSERT TEST MACRO
//-----------------------------------------------------------------------------

namespace {

int testStatus = 0;

void aSsErT(int c, const char *s, int i)
{
    if (c) {
        cout << "Error " << __FILE__ << "(" << i << "): " << s
             << "    (failed)" << endl;
        if (0 <= testStatus && testStatus <= 100) ++testStatus;
    }
}

}  // close unnamed namespace

#define ASSERT(X) { aSsErT(!(X), #X, __LINE__); }

//=============================================================================
//                  STANDARD BDE LOOP-ASSERT TEST MACROS
//-----------------------------------------------------------------------------
#define LOOP_ASSERT(I,X) { \
   if (!(X)) { cout << #I << ": " << I << "\n"; aSsErT(1, #X, __LINE__); }}

#define LOOP2_ASSERT(I,J,X) { \
   if (!(X)) { cout << #I << ": " << I << "\t" << #J << ": " \
              << J << "\n"; aSsErT(1, #X, __LINE__); } }

#define LOOP3_ASSERT(I,J,K,X) { \
   if (!(X)) { cout << #I << ": " << I << "\t" << #J << ": " << J << "\t" \
              << #K << ": " << K << "\n"; aSsErT(1, #X, __LINE__); } }

#define LOOP4_ASSERT(I,J,K,L,X) { \
   if (!(X)) { cout << #I << ": " << I << "\t" << #J << ": " << J << "\t" << \
       #K << ": " << K << "\t" << #L << ": " << L << "\n"; \
       aSsErT(1, #X, __LINE__); } }

#define LOOP5_ASSERT(I,J,K,L,M,X) { \
   if (!(X)) { cout << #I << ": " << I << "\t" << #J << ": " << J << "\t" << \
       #K << ": " << K << "\t" << #L << ": " << L << "\t" << \
       #M << ": " << M << "\n"; \
       aSsErT(1, #X, __LINE__); } }

#define LOOP6_ASSERT(I,J,K,L,M,N,X) { \
   if (!(X)) { cout << #I << ": " << I << "\t" << #J << ": " << J << "\t" << \
       #K << ": " << K << "\t" << #L << ": " << L << "\t" << \
       #M << ": " << M << "\t" << #N << ": " << N << "\n"; \
       aSsErT(1, #X, __LINE__); } }

//=============================================================================
//                  SEMI-STANDARD TEST OUTPUT MACROS
//-----------------------------------------------------------------------------
#define P(X) cout << #X " = " << (X) << endl; // Print identifier and value.
#define Q(X) cout << "<| " #X " |>" << endl;  // Quote identifier literally.
#define P_(X) cout << #X " = " << (X) << ", "<< flush; // P(X) without '\n'
#define L_ __LINE__                           // current Line number
#define T_()  cout << '\t' << flush;          // Print tab w/o newline
#define PP(X) (cout << #X " = " << (X) << endl, 0) // Print name and
                                                   // value, then return false.

//=============================================================================
//              GLOBAL TYPEDEFS, CONSTANTS, ROUTINES & MACROS FOR TESTING
//-----------------------------------------------------------------------------

namespace {

typedef bdede_Utf8Util Obj;

// Have the default allocator be of different type than the allocator usually
// used -- then we can put breakpoints in bslma_TestAllocator code to find
// unintentional uses of the default allocator.

bslma_TestAllocator taDefault;
bslma_TestAllocator ta;

int verbose;
int veryVerbose;
int veryVeryVerbose;
int veryVeryVeryVerbose;

bsl::string code8(int b)
{
    bsl::string ret;

    ASSERT(0 == (b & ~0x7f));

    ret += (char) b;

    return ret;
}

bsl::string code16(int b)
{
    ASSERT(0 == (b & ~0x7ff));

    unsigned char buf[3];
    buf[0] = ((b & 0x7c0) >> 6) | 0xc0;
    buf[1] =  (b &  0x3f)       | 0x80;
    buf[2] = 0;

    return (char *) buf;
}

bsl::string code24(int b)
{
    ASSERT(0 == (b & ~0xffff));

    unsigned char buf[4];
    buf[0] = ((b & 0xf000) >> 12) | 0xe0;
    buf[1] = ((b &  0xfc0) >>  6) | 0x80;
    buf[2] =  (b &   0x3f)        | 0x80;
    buf[3] = 0;

    return (char *) buf;
}

bsl::string code32(int b)
{
    ASSERT((unsigned) b <= 0x10ffff);

    unsigned char buf[5];
    buf[0] = ((b & 0x1c0000) >> 18) | 0xf0;
    buf[1] = ((b &  0x3f000) >> 12) | 0x80;
    buf[2] = ((b &    0xfc0) >>  6) | 0x80;
    buf[3] =  (b &     0x3f)        | 0x80;
    buf[4] = 0;

    return (char *) buf;
}

bsl::string utf8Encode(int b)
{
    ASSERT((unsigned) b <= 0x10ffff);

    if (b <= 0x7f) {
        return code8(b);
    }
    if (b <= 0x7ff) {
        return code16(b);
    }
    if (b <= 0xffff) {
        return code24(b);
    }

    return code32(b);
}

bsl::string codeLongZero()
{
    unsigned char buf[3];
    buf[0] = 0xc0;
    buf[1] = 0x80;
    buf[2] = 0;

    return (char *) buf;
}

bsl::string codeBOM()
{
    unsigned char buf[4];
    buf[0] = 0xef;
    buf[1] = 0xbb;
    buf[2] = 0xbf;
    buf[3] = 0;

    return (char *) buf;
}

// note these decoders all assume they can look as far as they want down
// the stream of chars without provoking a segfault.

int decode8(const char *pc)
{
    ASSERT(!(~0x7f & *pc));

    return *pc;
}

int decode16(const char *pc)
{
    ASSERT(0xc0 == (*pc & 0xe0) && 0x80 == (pc[1] & 0xc0));

    return ((0x1f & *pc) << 6) | (0x3f & pc[1]);
}

int decode24(const char *pc)
{
    ASSERT(0xe0 == (*pc & 0xf0) && 0x80 == (pc[1] & 0xc0) &&
                                   0x80 == (pc[2] & 0xc0));

    return ((0xf & *pc) << 12) | ((0x3f & pc[1]) << 6) | (0x3f & pc[2]);
}

int decode32(const char *pc)
{
    ASSERT(0xf0 == (*pc & 0xf8) && 0x80 == (pc[1] & 0xc0) &&
                             0x80 == (pc[2] & 0xc0) && 0x80 == (pc[3] & 0xc0));

    return (( 0x7 & *pc)   << 18) | ((0x3f & pc[1]) << 12) |
           ((0x3f & pc[2]) <<  6) |  (0x3f & pc[3]);
}

int decode(const char **pc)
{
    int ret;

    char c = **pc;
    if (0    == (~0x7f & c)) {
        ret = decode8(*pc);
        ++ *pc;
    } else if (0xc0 == (0xe0 & c)) {
        ret = decode16(*pc);
        *pc += 2;
    } else if (0xe0 == (0xf0 & c)) {
        ret = decode24(*pc);
        *pc += 3;
    } else if (0xf0 == (0xf8 & c)) {
        ret = decode32(*pc);
        *pc += 4;
    } else {
        ASSERT(0);

        return -1;
    }

    return ret;
}

bsls_PlatformUtil::Int64 randAccum = 0;
int randNum()
    // MMIX Linear Congruential Generator algorithm by Donald Knuth
{
    randAccum = randAccum * 6364136223846793005LL + 1442695040888963407LL;
    return randAccum >> 32;
}

int randVal()
{
    return (randNum() >> 9) & 0x1fffff;
}

int randVal8(bool never0 = false)
{
    int ret;
    do {
        ret = randVal() & 0x7f;
    } while (never0 && 0 == ret);

    return ret;
}

int randVal16()
{
    int ret;
    do {
        ret = randVal() & 0x7ff;
    } while (ret < 0x80);

    return ret;
}

int randVal24(bool neverSurrogates = false)
{
    int ret;
    do {
        ret = randVal() & 0xffff;
    } while (ret < 0x800 ||
                          (neverSurrogates && ret >= 0xd800 && ret <= 0xdfff));

    return ret;
}

int randVal32()
{
    int ret;
    do {
        ret = (randVal() & 0xfffff) + (randVal() & 0x7ffff);
    } while (ret > 0x10ffff || ret < 0x10000);

    return ret;
}

int randValue(bool strict = false, bool never0 = false)
{
    int type = randVal();
    switch (type & 3) {
      case 0: {
        return randVal8(never0);
      } break;
      case 1: {
        return randVal16();
      } break;
      case 2: {
        return randVal24(strict);
      } break;
      case 3: {
        return randVal32();
      }
    }

    ASSERT(0);
    return 0;
}

bsl::string randEncode(bool strict = false, bool modifiedUtf8 = false)
{
    int type = randVal();
    switch (type & 3) {
      case 0: {
        int j = randVal8(false);
        return (0 == j && modifiedUtf8) ? code16(0) : code8(j);
      } break;
      case 1: {
        return code16(randVal16());
      } break;
      case 2: {
        return code24(randVal24(strict));
      } break;
      case 3: {
        return code32(randVal32());
      }
    }

    ASSERT(0);
    return "";
}

bool allValid(bsl::string& str)
{
    bool a = Obj::isValid(str.c_str());
    ASSERT(Obj::isValid(str.data(), str.length()) == a);

    return a;
}

int allNumChars(bsl::string& str)
{
    int len = Obj::numCharacters(str.data(), str.length());
    ASSERT(Obj::numCharacters(str.c_str()) == len);

    return len;
}

bsl::string clone(const char *pc, int length)
{
    bsl::string ret;

    ret.insert(0, pc, length);
    return ret;
}

} // close unnamed namespace

//=============================================================================
//                             Usage Example
//-----------------------------------------------------------------------------

namespace USAGE {

void utf8Cat8(bsl::string *str, int b)
    // Concatenate the utf8-encoded value of the specified 'b' in 1 byte
    // to the end of the specified 'str'.
{
    ASSERT(0 == (b & ~0x7f));

    *str += (char) b;
}

void utf8Cat16(bsl::string *str, int b)
    // Concatenate the utf8-encoded value of the specified 'b' in 2 bytes
    // to the end of the specified 'str'.
{
    ASSERT(0 == (b & ~0x7ff));

    unsigned char buf[3];
    buf[0] = ((b & 0x7c0) >> 6) | 0xc0;
    buf[1] =  (b &  0x3f)       | 0x80;
    buf[2] = 0;

    *str += (char *) buf;
}

void utf8Cat24(bsl::string *str, int b)
    // Concatenate the utf8-encoded value of the specified 'b' in 3 bytes
    // to the end of the specified 'str'.
{
    ASSERT(0 == (b & ~0xffff));

    unsigned char buf[4];
    buf[0] = ((b & 0xf000) >> 12) | 0xe0;
    buf[1] = ((b &  0xfc0) >>  6) | 0x80;
    buf[2] =  (b &   0x3f)        | 0x80;
    buf[3] = 0;

    *str += (char *) buf;
}

void utf8Cat32(bsl::string *str, int b)
    // Concatenate the utf8-encoded value of the specified 'b' in 4 bytes
    // to the end of the specified 'str'.
{
    ASSERT((unsigned) b <= 0x10ffff);

    unsigned char buf[5];
    buf[0] = ((b & 0x1c0000) >> 18) | 0xf0;
    buf[1] = ((b &  0x3f000) >> 12) | 0x80;
    buf[2] = ((b &    0xfc0) >>  6) | 0x80;
    buf[3] =  (b &     0x3f)        | 0x80;
    buf[4] = 0;

    *str += (char *) buf;
}

void utf8Cat(bsl::string *str, int b)
    // Concatenate the utf8-encoded value of the specified 'b' in the
    // minimum needed number of bytes to the end of the specified 'str'.
{
    ASSERT((unsigned) b <= 0x10ffff);

    if (b <= 0x7f) {
        utf8Cat8(str, b);
        return;                                                       // RETURN
    }
    if (b <= 0x7ff) {
        utf8Cat16(str, b);
        return;                                                       // RETURN
    }
    if (b <= 0xffff) {
        utf8Cat24(str, b);
        return;                                                       // RETURN
    }

    utf8Cat32(str, b);
}

}  // close namespace USAGE


//=============================================================================
//                      HELPER DEFINITIONS FOR TEST 4
//-----------------------------------------------------------------------------

namespace BDEDE_UTF8UTIL_CASE_4 {

const int surrogates[] = { 0xd800, 0xd811, 0xd9a3, 0xd9ff,
                           0xda00, 0xda35, 0xdb80, 0xdbff,
                           0xdc00, 0xdc48, 0xdd84, 0xddff,
                           0xde00, 0xde73, 0xdf24, 0xdfff };
const int *loSgates = surrogates;
const int *hiSgates = surrogates + 8;

bsl::string codeRandSurrogatePair()
{
    int val = randNum();
    int loSgate, hiSgate;
    if (val & (3 << 20)) {
        loSgate = 0xd800 + (val & 0x3ff);
        hiSgate = 0xdc00 + ((val >> 10) & 0x3ff);
    }
    else {
        loSgate = loSgates[val & 7];
        hiSgate = hiSgates[(val >> 3) & 7];
    }

    return code24(loSgate) + code24(hiSgate);
}

bsl::string codeRandSurrogate()
{
    int val = randNum();
    if (val &0x1800) {
        val = (val & 0x7ff) + 0xd800;
    }
    else {
        val = surrogates[val & 0xf];
    }

    return code24(val);
}

// encode with no zeroes or surrogates
bsl::string codeRandBenign()
{
    int val;
    do {
        val = randVal();
    } while (0 == val || (0xd800 <= val && 0xdfff >= val) || val > 0x10ffff);

    return utf8Encode(val);
}

} // namespace BDEDE_UTF8UTIL_CASE_4

//=============================================================================
//                      HELPER DEFINITIONS FOR TEST 2
//-----------------------------------------------------------------------------

namespace BDEDE_UTF8UTIL_CASE_2 {

bsl::string makeString(const char *pc, int len)
{
    bsl::string ret;

    ret.insert(0, pc, len);

    return ret;
}

#define STR(testId)  makeString(encode ## testId, sizeof(encode ## testId))

} // close namespace BDEDE_UTF8UTIL_CASE_2

//=============================================================================
//                              MAIN PROGRAM
//-----------------------------------------------------------------------------

int main(int argc, char *argv[])
{
    int test = argc > 1 ? bsl::atoi(argv[1]) : 0;

    verbose = argc > 2;
    veryVerbose = argc > 3;
    veryVeryVerbose = argc > 4;
    veryVeryVeryVerbose = argc > 5;

    bslma_DefaultAllocatorGuard guard(&taDefault);
    ASSERT(&taDefault == bslma_Default::defaultAllocator());

    cout << "TEST " << __FILE__ << " CASE " << test << endl;;

    switch (test) { case 0:  // Zero is always the leading case.
      case 5: {
        // --------------------------------------------------------------------
        // USAGE EXAMPLE
        //
        // Concerns:
        //   Demonstrate the routines by encoding some UTF-8 strings and
        //   validating them.
        //
        // Plan:
        //   Create both UTF-8 and modified UTF-8 strings and validate them.
        // --------------------------------------------------------------------

        using namespace USAGE;

        if (verbose) cout << "USAGE EXAMPLE\n"
                             "=============\n";

        // In this usage example, we will encode some utf8 strings and
        // demonstrate which ones are valid and which ones are not.

        // First, let's build an unquestionably valid UTF-8 string:

        bsl::string str;
        utf8Cat(&str, 0xff00);
        utf8Cat(&str, 0x856);
        utf8Cat(&str, 'a');
        utf8Cat(&str, 0x1008aa);
        utf8Cat(&str, 0xfff);
        utf8Cat(&str, 'w');
        utf8Cat(&str, 0x1abcd);
        utf8Cat(&str, '.');
        utf8Cat(&str, '\n');

        // Check its validity and measure its length.

        ASSERT(true == bdede_Utf8Util::isValid(str.data(), str.length()));
        ASSERT(true == bdede_Utf8Util::isValid(str.c_str()));

        ASSERT(9 == bdede_Utf8Util::numCharacters(str.data(), str.length()));
        ASSERT(9 == bdede_Utf8Util::numCharacters(str.c_str()));

        // Encode lone surrogate value, which is not allowed.

        bsl::string m3Str = str;
        utf8Cat(&m3Str, 0xd8ab);

        ASSERT(false == bdede_Utf8Util::isValid(m3Str.data(), m3Str.length()));
        ASSERT(false == bdede_Utf8Util::isValid(m3Str.c_str()));

        // We can't measure the length, the behavior of 'numCharacters' is
        // undefined for an invalid string.

        // Encode 0, which is allowed.  We can't use the zero terminated
        // string interfaces for this one.

        bsl::string zStr = str;
        utf8Cat8(&zStr, 0);

        ASSERT(true == bdede_Utf8Util::isValid(zStr.data(), zStr.length()));

        ASSERT(10 == bdede_Utf8Util::numCharacters(zStr.data(),zStr.length()));

        // Encode an overlong value - 0x61 == 'a' encoded over 2 bytes,
        // which is not allowed.

        bsl::string oStr = str;
        utf8Cat16(&oStr, 'a');

        ASSERT(false == bdede_Utf8Util::isValid(oStr.data(), oStr.length()));
        ASSERT(false == bdede_Utf8Util::isValid(oStr.c_str()));
      } break;
      case 4: {
        // --------------------------------------------------------------------
        // TESTING SURROGATES
        //
        // Concerns:
        //   Create some strings containing surrogate pairs and verify that
        //   the validation routines interpret them properly.
        //
        // Plan:
        //   Create a large number of strings containing valid and invalid
        //   surrogates and verify that the the validation routines handle
        //   them properly.
        // --------------------------------------------------------------------

        if (verbose) cout << "TESTING SURROGATES\n"
                             "==================\n";

        using namespace BDEDE_UTF8UTIL_CASE_4;

        bsl::string str;
        str.reserve(6 * 4 + 6 + 1);

        randAccum = 0;

        // test strings with surrogate pairs
        for (int i = 5000; i > 0; -- i) {
            int rv = randVal();
            int numLeading = rv & 3;
            int numTrailing = (rv >> 2) & 3;
            str = "";
            for (int j = 0; j < numLeading; ++ j) {
                str += codeRandBenign();
            }
            str += codeRandSurrogatePair();
            for (int j = 0; j < numTrailing; ++ j) {
                str += codeRandBenign();
            }

            ASSERT(false == allValid(str));
        }

        randAccum = 0;

        // test strings with lone surrogates
        for (int i = 5000; i > 0; -- i) {
            int rv = randVal();
            int numLeading = rv & 3;
            int numTrailing = (rv >> 2) & 3;
            str = "";
            for (int j = 0; j < numLeading; ++ j) {
                str += codeRandBenign();
            }
            str += codeRandSurrogate();
            for (int j = 0; j < numTrailing; ++ j) {
                str += codeRandBenign();
            }

            ASSERT(false == allValid(str));
        }
      } break;
      case 3: {
        // --------------------------------------------------------------------
        // TESTING BYTE-ORDER MARK
        //
        // Concerns:
        //   Verify byte-order mark is accepted by all the validation routines.
        //
        // Plan:
        //   Create a byte-order mark and feed it to the validation routines,
        //   it should always be acceptable.  Also create UTF-16 BOM's of both
        //   byte orders, they should be rejected.
        // --------------------------------------------------------------------

        if (verbose) cout << "TESTING BYTE-ORDER MARK\n"
                             "=======================\n";

        using namespace BDEDE_UTF8UTIL_CASE_2;

        typedef const char Char;

        Char c    = (unsigned char) 0x80;    // 'c'ontinuation
        Char b3   = (unsigned char) 0xe0;    // 'b'eginning of 3-byte
        Char b4   = (unsigned char) 0xf0;    // 'b'eginning of 4-byte

        Char encode4a[] = { b4, c | 0x10, c, c };       int val4a = 0x10000;

        bsl::string s4a = STR(4a);

        bsl::string s1b = code8(47);
        ASSERT(1 == s1b.length());

        ASSERT(allValid(s4a));

        unsigned char uBom[] = { 0xef, 0xbb, 0xbf };
        Char bom[] = { b3 | 0xf, c | 0x3b, c | 0x3f };

        const int bomVal = (0xf << 12) + (0x3b << 6) + 0x3f;

        ASSERT(0 == bsl::memcmp(uBom, bom, 3));

        bsl::string sBom = makeString(bom, sizeof(bom));
        ASSERT(codeBOM() == sBom);
        ASSERT(code24(bomVal) == sBom);

        const char *pc = bom;
        ASSERT(bomVal == decode(&pc));
        ASSERT(bom + sizeof(bom) == pc);

        ASSERT(allValid(sBom));

        bsl::string lStr = sBom + s4a + s4a;

        ASSERT(allValid(lStr));

        lStr = s4a + s4a + sBom + s4a + s4a;

        ASSERT(allValid(lStr));

        lStr = s4a + s4a + sBom;

        ASSERT(allValid(lStr));

        lStr = s4a + s4a + sBom + s1b;

        ASSERT(allValid(lStr));

        Char encode16BomLe[] = { (unsigned char) 0xff, (unsigned char) 0xfe };
        Char encode16BomBe[] = { (unsigned char) 0xfe, (unsigned char) 0xff };

        bsl::string bomLe = STR(16BomLe);
        bsl::string bomBe = STR(16BomBe);

        ASSERT(! allValid(bomLe));
        ASSERT(! allValid(bomBe));

        lStr = s4a + bomLe;
        ASSERT(! allValid(lStr));

        lStr = s4a + bomBe;
        ASSERT(! allValid(lStr));

        lStr = s4a + bomLe + s4a;
        ASSERT(! allValid(lStr));

        lStr = s4a + bomBe + s4a;
        ASSERT(! allValid(lStr));

        lStr = s4a + bomLe + s1b;
        ASSERT(! allValid(lStr));

        lStr = s4a + bomBe + s1b;
        ASSERT(! allValid(lStr));
      } break;
      case 2: {
        // --------------------------------------------------------------------
        // TABLE-DRIVEN ENCODING / DECODING / VALIDATION TEST
        //
        // Concerns:
        //   Test encoding, decoding, and validation on data that is table
        //   driven rather than randomly generated.
        //
        // Plan:
        //   Encode some const char strings with specific values, try encoding
        //   them and verify the expected results are yields.  Run the
        //   validation routines on them and verify the expected results.  Code
        //   various forms of invalid strings and observe that the validators
        //   reject them appropriately.  Several types of encodings are
        //   represented.  UTF-8 zero, Modified UTF-8 zero, many values that
        //   are valid in both UTF-8 and modified UTF-8, overlong values other
        //   than zero, blatantly invalid values, and short values.
        // --------------------------------------------------------------------

        if (verbose) cout << "TABLE-DRIVEN ENCODING / DECODING TEST\n"
                             "=====================================\n";

        using namespace BDEDE_UTF8UTIL_CASE_2;

        typedef const char Char;

        Char c    = (unsigned char) 0x80;    // 'c'ontinuation
        Char b2   = (unsigned char) 0xc0;    // 'b'eginning of 2-byte
        Char b3   = (unsigned char) 0xe0;    // 'b'eginning of 3-byte
        Char b4   = (unsigned char) 0xf0;    // 'b'eginning of 4-byte
        Char b5   = (unsigned char) 0xf8;    // 'b'eginning of 5-byte
        Char b6   = (unsigned char) 0xfc;    // 'b'eginning of 6-byte
        Char b7   = (unsigned char) 0xfe;    // 'b'eginning of 6-byte

        Char encode1a[] = { 0 };                        int val1a = 0;
        Char encode1b[] = { 0x35 };                     int val1b = 0x35;
        Char encode1c[] = { 0x47 };                     int val1c = 0x47;
        Char encode1d[] = { 0x63 };                     int val1d = 0x63;
        Char encode1e[] = { 0x7f };                     int val1e = 0x7f;

        Char encode2a[] = { b2 | 2, c };                int val2a = 0x80;
        Char encode2b[] = { b2 | 3, c | 3 };            int val2b = 3*64 + 3;
        Char encode2c[] = { b2 | 0xf, c | 0xf };        int val2c = 15*64 + 15;
        Char encode2d[] = { b2 | 0x10, c | 7 };         int val2d = 16*64 + 7;
        Char encode2e[] = { b2 | 0x1f, c | 0x3f };      int val2e = 0x7ff;

        Char encode3a[] = { b3, c | 0x20, c | 0 };      int val3a = 0x800;
        Char encode3b[] = { b3, c | 58, c | 0 };        int val3b = 58 * 64;
        Char encode3c[] = { b3, c | 0x3f, c | 0x3f };   int val3c = 0xfff;
        Char encode3d[] = { b3 | 7, c | 0x3f, c | 0x3f };
                                                        int val3d = 0x7fff;
        Char encode3e[] = { b3 | 0xf, c | 0x3f, c | 0x3f };
                                                        int val3e = 0xffff;

        Char encode4a[] = { b4, c | 0x10, c, c };       int val4a = 0x10000;
        Char encode4b[] = { b4, c | 0x10, c, c | 0xf }; int val4b = 0x1000f;
        Char encode4c[] = { b4, c | 0x10, c | 0xf, c }; int val4c = 0x103c0;
        Char encode4d[] = { b4, c | 0x10, c | 0x30, c };int val4d = 0x10c00;
        Char encode4e[] = { b4, c | 0x13, c, c };       int val4e = 0x13000;
        Char encode4f[] = { b4, c | 0x1c, c, c };       int val4f = 0x1c000;
        Char encode4g[] = { b4 | 1, c, c, c };          int val4g = 0x40000;
        Char encode4h[] = { b4 | 3, c, c, c };          int val4h = 0xc0000;
        Char encode4i[] = { b4 | 4, c, c, c };          int val4i = 0x100000;
        Char encode4j[] = { b4 | 4, c | 0xf, c | 0x3f, c | 0x3f };
                                                        int val4j = 0x10ffff;

#define TEST(testId) do {                                                   \
            bsl::string str;                                                \
            str.insert(0, encode ## testId, sizeof(encode ## testId));      \
            Char *pc = str.data();                                          \
            const int val = decode(&pc);                                    \
            ASSERT(pc == str.data() + str.length() + 0 * val ## testId);    \
            ASSERT(val == val ## testId);                                   \
            ASSERT(str == utf8Encode(val ## testId));                       \
            ASSERT(allValid(str));                                          \
        } while(false);

        TEST(1a);       TEST(1b);       TEST(1c);       TEST(1d);     TEST(1e);
        TEST(2a);       TEST(2b);       TEST(2c);       TEST(2d);     TEST(2e);
        TEST(3a);       TEST(3b);       TEST(3c);       TEST(3d);     TEST(3e);
        TEST(4a);       TEST(4b);       TEST(4c);       TEST(4d);     TEST(4e);
        TEST(4f);       TEST(4g);       TEST(4h);       TEST(4i);     TEST(4j);
#undef TEST

#define TEST(testId) do {                                                   \
            bsl::string str;                                                \
            str.insert(0, encode ## testId, sizeof(encode ## testId));      \
            Char *pc = str.data();                                          \
            const int val = decode(&pc);                                    \
            ASSERT(pc == str.data() + str.length() + 0 * val ## testId);    \
            ASSERT(val == val ## testId);                                   \
            ASSERT(str == utf8Encode(val ## testId));                       \
            ASSERT(allValid(str));                                          \
            ASSERT(1 == allNumChars(str));                                  \
            ASSERT(bsl::strlen(str.c_str()) == str.length());               \
        } while(false);

                        TEST(1b);       TEST(1c);       TEST(1d);     TEST(1e);
        TEST(2a);       TEST(2b);       TEST(2c);       TEST(2d);     TEST(2e);
        TEST(3a);       TEST(3b);       TEST(3c);       TEST(3d);     TEST(3e);
        TEST(4a);       TEST(4b);       TEST(4c);       TEST(4d);     TEST(4e);
        TEST(4f);       TEST(4g);       TEST(4h);       TEST(4i);     TEST(4j);
#undef TEST

        bsl::string s1 = STR(1a)+STR(2a)+STR(3a)+STR(4a)+STR(4f);
        Char *ps = s1.data();
        ASSERT(val1a == decode(&ps));
        ASSERT(val2a == decode(&ps));
        ASSERT(val3a == decode(&ps));
        ASSERT(val4a == decode(&ps));
        ASSERT(val4f == decode(&ps));
        ASSERT(ps == s1.data() + s1.length());
        ASSERT(allValid(s1));

        s1 = STR(2a)+STR(3a)+STR(4a)+STR(4f);
        ps = s1.data();
        ASSERT(val2a == decode(&ps));
        ASSERT(val3a == decode(&ps));
        ASSERT(val4a == decode(&ps));
        ASSERT(val4f == decode(&ps));
        ASSERT(ps == s1.data() + s1.length());
        ASSERT(allValid(s1));
        ASSERT(bsl::strlen(s1.c_str()) == s1.length());

        s1 = STR(1b)+STR(2b)+STR(3b)+STR(4b)+STR(4g);
        ps = s1.data();
        ASSERT(val1b == decode(&ps));
        ASSERT(val2b == decode(&ps));
        ASSERT(val3b == decode(&ps));
        ASSERT(val4b == decode(&ps));
        ASSERT(val4g == decode(&ps));
        ASSERT(ps == s1.data() + s1.length());
        ASSERT(allValid(s1));
        ASSERT(bsl::strlen(s1.c_str()) == s1.length());

        s1 = STR(1c)+STR(2c)+STR(3c)+STR(4c)+STR(4h);
        ps = s1.data();
        ASSERT(val1c == decode(&ps));
        ASSERT(val2c == decode(&ps));
        ASSERT(val3c == decode(&ps));
        ASSERT(val4c == decode(&ps));
        ASSERT(val4h == decode(&ps));
        ASSERT(ps == s1.data() + s1.length());
        ASSERT(allValid(s1));
        ASSERT(bsl::strlen(s1.c_str()) == s1.length());

        s1 = STR(1d)+STR(2d)+STR(3d)+STR(4d)+STR(4i);
        ps = s1.data();
        ASSERT(val1d == decode(&ps));
        ASSERT(val2d == decode(&ps));
        ASSERT(val3d == decode(&ps));
        ASSERT(val4d == decode(&ps));
        ASSERT(val4i == decode(&ps));
        ASSERT(ps == s1.data() + s1.length());
        ASSERT(allValid(s1));
        ASSERT(bsl::strlen(s1.c_str()) == s1.length());

        s1 = STR(1e)+STR(2e)+STR(3e)+STR(4e)+STR(4j);
        ps = s1.data();
        ASSERT(val1e == decode(&ps));
        ASSERT(val2e == decode(&ps));
        ASSERT(val3e == decode(&ps));
        ASSERT(val4e == decode(&ps));
        ASSERT(val4j == decode(&ps));
        ASSERT(ps == s1.data() + s1.length());
        ASSERT(allValid(s1));
        ASSERT(bsl::strlen(s1.c_str()) == s1.length());

        // random overlong values
        Char encode2o1[] = { b2 | 1, c | 0x3f };        int val2o1 = 0x7f;
        Char encode2o2[] = { b2 | 0, c | 3 };           int val2o2 = 0x3;
        Char encode3o1[] = { b3, c | 0x1f, c | 0x3f };  int val3o1 = 0x7ff;
        Char encode3o2[] = { b3, c | 0, c | 3 };        int val3o2 = 0x3;
        Char encode4o1[] = { b4, c | 0xf, c | 0x3f, c | 0x3f };
                                                        int val4o1 = 0xffff;
        Char encode4o2[] = { b4, c , c, c | 3 };        int val4o2 = 0x3;

#define TEST(testId) do {                                                   \
            bsl::string str = STR(testId);                                  \
            Char *pc = str.data();                                          \
            ASSERT(val ## testId == decode(&pc));                           \
            ASSERT(pc == str.data() + str.length());                        \
            ASSERT(false == allValid(str));                                 \
        } while(false)

        TEST(2o1);          TEST(2o2);
        TEST(3o1);          TEST(3o2);
        TEST(4o1);          TEST(4o2);
#undef TEST

        // blatantly invalid strings
        Char encode1i1[] = { c };
        Char encode1i2[] = { c | 0x3f };

        Char encode2i1[] = { b2 | 2, 0 };
        Char encode2i2[] = { b2 | 2, b2 };
        Char encode2i3[] = { b2 | 2, b3 };
        Char encode2i4[] = { b2 | 2, b4 };
        Char encode2i5[] = { b2 | 2, b5 };
        Char encode2i6[] = { b2 | 2, b6 };
        Char encode2i7[] = { b2 | 2, b7 };
        Char encode2i8[] = { b2 | 2, (unsigned char) 0xff };

        Char encode3i1[] = { b3, c | 0x20, 0 };
        Char encode3i2[] = { b3, c | 0x20, b2 };
        Char encode3i3[] = { b3, c | 0x20, b3 };
        Char encode3i4[] = { b3, c | 0x20, b4 };
        Char encode3i5[] = { b3, c | 0x20, b5 };
        Char encode3i6[] = { b3, c | 0x20, b6 };
        Char encode3i7[] = { b3, c | 0x20, b7 };
        Char encode3i8[] = { b3, c | 0x20, (unsigned char) 0xff };
        Char encode3i9[] = { b3, 0x3f, c };
        Char encode3ia[] = { b3, b2, c };
        Char encode3ib[] = { b3, b3, c };
        Char encode3ic[] = { b3, b4, c };
        Char encode3id[] = { b3, b5, c };
        Char encode3ie[] = { b3, b6, c };
        Char encode3if[] = { b3, b7, c };
        Char encode3ig[] = { b3, (unsigned char) 0xff, c };

        Char encode4i1[] = { b4, c | 0x10, c, 0 };
        Char encode4i2[] = { b4, c | 0x10, c, b2 };
        Char encode4i3[] = { b4, c | 0x10, c, b3 };
        Char encode4i4[] = { b4, c | 0x10, c, b4 };
        Char encode4i5[] = { b4, c | 0x10, c, b5 };
        Char encode4i6[] = { b4, c | 0x10, c, b6 };
        Char encode4i7[] = { b4, c | 0x10, c, b7 };
        Char encode4i8[] = { b4, c | 0x10, c, (unsigned char) 0xff };
        Char encode4i9[] = { b4, c | 0x10, 0, c };
        Char encode4ia[] = { b4, c | 0x10, b2, c };
        Char encode4ib[] = { b4, c | 0x10, b3, c };
        Char encode4ic[] = { b4, c | 0x10, b4, c };
        Char encode4id[] = { b4, c | 0x10, b5, c };
        Char encode4ie[] = { b4, c | 0x10, b6, c };
        Char encode4if[] = { b4, c | 0x10, b7, c };
        Char encode4ig[] = { b4, c | 0x10, (unsigned char) 0xff, c };
        Char encode4ih[] = { b4, 0, c, c };
        Char encode4ii[] = { b4, b2, c, c };
        Char encode4ij[] = { b4, b3, c, c };
        Char encode4ik[] = { b4, b4, c, c };
        Char encode4il[] = { b4, b5, c, c };
        Char encode4im[] = { b4, b6, c, c };
        Char encode4in[] = { b4, b7, c, c };
        Char encode4io[] = { b4, (unsigned char) 0xff, c, c };

        Char encode5i1[] = { b5, c, c, c, c };

        Char encode6i1[] = { b6, c, c, c, c, c };

        Char encode7i1[] = { b7, c, c, c, c, c, c };

        Char encode8i1[] = { (unsigned char) 0xff, c, c, c, c, c, c, c };

#define TEST(testId) do {                                                   \
            bsl::string str = STR(testId);                                  \
            ASSERT(! allValid(str));                                        \
        } while(false)

        TEST(1i1);      TEST(1i2);

        TEST(2i1);      TEST(2i2);      TEST(2i3);      TEST(2i4);
        TEST(2i5);      TEST(2i6);      TEST(2i7);      TEST(2i8);

        TEST(3i1);      TEST(3i2);      TEST(3i3);      TEST(3i4);
        TEST(3i5);      TEST(3i6);      TEST(3i7);      TEST(3i8);
        TEST(3i9);      TEST(3ia);      TEST(3ib);      TEST(3ic);
        TEST(3id);      TEST(3ie);      TEST(3if);      TEST(3ig);

        TEST(4i1);      TEST(4i2);      TEST(4i3);      TEST(4i4);
        TEST(4i5);      TEST(4i6);      TEST(4i7);      TEST(4i8);
        TEST(4i9);      TEST(4ia);      TEST(4ib);      TEST(4ic);
        TEST(4id);      TEST(4ie);      TEST(4if);      TEST(4ig);
        TEST(4ih);      TEST(4ii);      TEST(4ij);      TEST(4ik);
        TEST(4il);      TEST(4im);      TEST(4in);      TEST(4io);

        TEST(5i1);

        TEST(6i1);

        TEST(7i1);

        TEST(8i1);
#undef TEST

        // short strings, strings that are valid until the end too abruptly

        Char encode2s1[] = { b2 };

        Char encode3s1[] = { b3, c | 0x20 };
        Char encode3s2[] = { b3 };

        Char encode4s1[] = { b4, c | 0x10, c };
        Char encode4s2[] = { b4, c | 0x10 };
        Char encode4s3[] = { b4 };

#define TEST(testId) do {                                                   \
            bsl::string str = STR(testId);                                  \
            ASSERT(! allValid(str));                                        \
                                                                            \
            str = STR(4a) + STR(testId) + STR(4a);                          \
            ASSERT(! allValid(str));                                        \
                                                                            \
            str = STR(4a) + STR(testId);                                    \
            ASSERT(! allValid(str));                                        \
                                                                            \
            str = STR(3a) + STR(testId) + STR(3a);                          \
            ASSERT(! allValid(str));                                        \
                                                                            \
            str = STR(3a) + STR(testId);                                    \
            ASSERT(! allValid(str));                                        \
                                                                            \
            str = STR(2a) + STR(testId) + STR(2a);                          \
            ASSERT(! allValid(str));                                        \
                                                                            \
            str = STR(2a) + STR(testId);                                    \
            ASSERT(! allValid(str));                                        \
                                                                            \
            str = STR(1b) + STR(testId) + STR(1b);                          \
            ASSERT(! allValid(str));                                        \
                                                                            \
            str = STR(1b) + STR(testId);                                    \
            ASSERT(! allValid(str));                                        \
        } while(false)

        TEST(2s1);
        TEST(3s1);      TEST(3s2);
        TEST(4s1);      TEST(4s2);      TEST(4s3);
#undef TEST

        // s'u'rrogate values
        Char encode3u1[] = { b3 | 0xd, c | 0x20, c };   int val3u1 = 0xd800;
        Char encode3u2[] = { b3 | 0xd, c | 0x20, c | 0x37 };
                                                        int val3u2 = 0xd837;
        Char encode3u3[] = { b3 | 0xd, c | 0x30, c };   int val3u3 = 0xdc00;
        Char encode3u4[] = { b3 | 0xd, c | 0x33, c | 0x3f };
                                                        int val3u4 = 0xdcff;

        int zero = 0;

#define TEST(testId) do {                                                   \
            bsl::string str = STR(testId);                                  \
            ASSERT(false == allValid(str));                                 \
                                                                            \
            str = STR(1b) + STR(testId) + STR(1b);                          \
                                                                            \
            ASSERT(! allValid(str));                                        \
                                                                            \
            str = STR(2b) + STR(testId) + STR(2b);                          \
                                                                            \
            ASSERT(! allValid(str));                                        \
                                                                            \
            str = STR(3b) + STR(testId) + STR(3b);                          \
                                                                            \
            ASSERT(! allValid(str));                                        \
                                                                            \
            str = STR(4b) + STR(testId) + STR(4b);                          \
                                                                            \
            ASSERT(! allValid(str));                                        \
        } while (false)

        TEST(3u1);
        TEST(3u2);
        TEST(3u3);
        TEST(3u4);
#undef TEST
      } break;
      case 1: {
        // --------------------------------------------------------------------
        // BREATHING TEST
        //
        // Concerns:
        //   Test basic methods
        //
        // Plan:
        //   Test the various test functions and verify that they work as
        //   designed.
        // --------------------------------------------------------------------

        if (verbose) cout << "Breathing test\n"
                             "==============\n";

        randAccum = 0;
        bsl::string str;
        str.reserve(41);
        for (int i = 0; i < 40; i += 4) {
            str += code32(randVal32());
        }

        ASSERT(40 == str.length());
        ASSERT(10 == allNumChars(str));

        ASSERT(allValid(str));

        for (int i = 0; i <= 40; i += 4) {
            ASSERT(Obj::isValid(str.data(), i));
            bsl::string c = clone(str.data(), i);
            ASSERT(Obj::isValid(c.c_str()));

            ASSERT(i / 4 == Obj::numCharacters(str.data(), i));
            ASSERT(i / 4 == Obj::numCharacters(c.c_str()));

            if (40 == i) break;

            for (int j = i + 1; j < i + 4; ++ j) {
                ASSERT(false == Obj::isValid(str.data(), j));
                ASSERT(false == Obj::isValid(clone(str.data(), j).c_str()));
            }
        }

        {
            bsl::string overStr = str + code32(0xffff);

            ASSERT(44 == overStr.length());
            ASSERT(11 == allNumChars(overStr));

            ASSERT(! allValid(overStr));

            for (int i = 0; i <= 40; i += 4) {
                ASSERT(Obj::isValid(str.data(), i));
                bsl::string c = clone(str.data(), i);
                ASSERT(Obj::isValid(c.c_str()));

                ASSERT(i / 4 == Obj::numCharacters(str.data(), i));
                ASSERT(i / 4 == Obj::numCharacters(c.c_str()));

                if (40 == i) break;

                for (int j = i + 1; j < i + 4; ++ j) {
                    ASSERT(false == Obj::isValid(str.data(), j));
                    ASSERT(false == Obj::isValid(clone(str.data(),j).c_str()));
                }
            }
        }

        str = "";
        str.reserve(34);
        for (int i = 0; i < 30; i += 3) {
            str += code24(randVal24(true));
        }

        ASSERT(30 == str.length());
        ASSERT(10 == allNumChars(str));

        ASSERT(allValid(str));

        for (int i = 0; i <= 30; i += 3) {
            ASSERT(Obj::isValid(str.data(), i));
            bsl::string c = clone(str.data(), i);
            ASSERT(Obj::isValid(c.c_str()));

            ASSERT(i / 3 == Obj::numCharacters(str.data(), i));
            ASSERT(i / 3 == Obj::numCharacters(c.c_str()));

            if (30 == i) break;

            for (int j = i + 1; j < i + 3; ++ j) {
                ASSERT(false == Obj::isValid(str.data(), j));
                ASSERT(false == Obj::isValid(clone(str.data(), j).c_str()));
            }
        }

        {
            bsl::string surStr = str + code24(0xdddd);    // surrogate value

            ASSERT(33 == surStr.length());
            ASSERT(11 == allNumChars(surStr));

            ASSERT(! Obj::isValid(surStr.data(), surStr.length()));
            ASSERT(! Obj::isValid(surStr.c_str()));

            for (int i = 0; i <= 33; i += 3) {
                bool tst = i < 33;
                ASSERT(tst == Obj::isValid(surStr.data(), i));
                bsl::string c = clone(surStr.data(), i);
                ASSERT(tst == Obj::isValid(c.c_str()));

                ASSERT(i / 3 == Obj::numCharacters(surStr.data(), i));
                ASSERT(i / 3 == Obj::numCharacters(c.c_str()));

                if (!tst) break;

                for (int j = i + 1; j < i + 3; ++ j) {
                    ASSERT(! Obj::isValid(surStr.data(), j));
                    ASSERT(! Obj::isValid(clone(surStr.data(), j).c_str()));
                }
            }
        }

        {
            bsl::string overStr = str + code24(0x7ff);    // overlong value

            ASSERT(33 == overStr.length());
            ASSERT(11 == allNumChars(overStr));

            ASSERT(! allValid(overStr));

            for (int i = 0; i <= 33; i += 3) {
                bool tst = i < 33;
                bsl::string c = clone(overStr.data(), i);
                ASSERT(tst == allValid(c));

                ASSERT(i / 3 == Obj::numCharacters(overStr.data(), i));
                ASSERT(i / 3 == Obj::numCharacters(
                                            clone(overStr.data(), i).c_str()));

                if (!tst) break;

                for (int j = i + 1; j < i + 3; ++ j) {
                    ASSERT(! Obj::isValid(overStr.data(), j));
                    ASSERT(! Obj::isValid(clone(overStr.data(), j).c_str()));
                }
            }
        }

        str = "";
        str.reserve(23);
        for (int i = 0; i < 20; i += 2) {
            int val;
            do {
                val = randVal16();
            } while(0 == val);
            str += code16(val);
        }

        ASSERT(20 == str.length());
        ASSERT(10 == allNumChars(str));

        ASSERT(allValid(str));

        for (int i = 0; i <= 20; i += 2) {
            ASSERT(Obj::isValid(str.data(), i));
            bsl::string c = clone(str.data(), i);
            ASSERT(Obj::isValid(c.c_str()));

            ASSERT(i / 2 == Obj::numCharacters(str.data(), i));
            ASSERT(i / 2 == Obj::numCharacters(c.c_str()));

            if (20 == i) break;

            for (int j = i + 1; j < i + 2; ++ j) {
                ASSERT(false == Obj::isValid(str.data(), j));
                ASSERT(false == Obj::isValid(clone(str.data(), j).c_str()));
            }
        }

        {
            bsl::string overStr = str + code16(0);       // overlong zero value

            ASSERT(22 == overStr.length());
            ASSERT(11 == allNumChars(overStr));

            ASSERT(false == allValid(overStr));

            for (int i = 0; i <= 22; i += 2) {
                bool tst = i < 22;
                ASSERT(tst == Obj::isValid(overStr.data(), i));
                bsl::string c = clone(overStr.data(), i);
                ASSERT(tst == Obj::isValid(c.c_str()));

                ASSERT(i / 2 == Obj::numCharacters(overStr.data(), i));
                ASSERT(i / 2 == Obj::numCharacters(c.c_str()));

                if (!tst) break;

                for (int j = i + 1; j < i + 2; ++ j) {
                    ASSERT(false == Obj::isValid(overStr.data(), j));
                    ASSERT(false == Obj::isValid(
                                            clone(overStr.data(), j).c_str()));
                }
            }
        }

        {
            bsl::string overStr = str + code16(0x7f);// overlong non-zero value

            ASSERT(22 == overStr.length());
            ASSERT(11 == allNumChars(overStr));

            ASSERT(false == allValid(overStr));

            for (int i = 0; i <= 22; i += 2) {
                bool tst = i < 22;
                ASSERT(tst == Obj::isValid(overStr.data(), i));
                bsl::string c = clone(overStr.data(), i);
                ASSERT(tst == Obj::isValid(c.c_str()));

                ASSERT(i / 2 == Obj::numCharacters(overStr.data(), i));
                ASSERT(i / 2 == Obj::numCharacters(c.c_str()));

                if (!tst) break;

                for (int j = i + 1; j < i + 2; ++ j) {
                    ASSERT(false == Obj::isValid(overStr.data(), j));
                    ASSERT(false == Obj::isValid(
                                            clone(overStr.data(), j).c_str()));
                }
            }
        }

        str = "";
        str.reserve(14);
        for (int i = 0; i < 10; ++ i) {
            str += code8(randVal8(true));
        }

        ASSERT(10 == str.length());
        ASSERT(10 == allNumChars(str));

        ASSERT(allValid(str));

        for (int i = 0; i <= 10; ++ i) {
            ASSERT(Obj::isValid(str.data(), i));
            bsl::string c = clone(str.data(), i);
            ASSERT(Obj::isValid(c.c_str()));

            ASSERT(i == Obj::numCharacters(str.data(), i));
            ASSERT(i == Obj::numCharacters(c.c_str()));
        }

        {
            bsl::string zStr = str + code8(0);
            zStr += code8(randVal8());
            zStr += code8(randVal8());
            zStr += code8(randVal8());

            ASSERT(14 == zStr.length());
            ASSERT(14 == Obj::numCharacters(zStr.data(), 14));
            ASSERT(10 == Obj::numCharacters(zStr.data()));

            ASSERT(allValid(zStr));

            for (int i = 0; i <= 14; ++ i) {
                ASSERT(Obj::isValid(zStr.data(), i));
                bsl::string c = clone(zStr.data(), i);
                ASSERT(Obj::isValid(c.c_str()));

                ASSERT(i == Obj::numCharacters(zStr.data(), i));
                ASSERT((i <= 10 ? i : 10) == Obj::numCharacters(c.c_str()));
            }
        }
      }  break;
      case -1: {
        // --------------------------------------------------------------------
        // RANDOM NUMBER GENERATORS TEST
        //
        // Concerns:
        //   Random number generators work properly.
        //
        // Plan:
        //   Test the various random number generators 2 ways -- print out a
        //   bunch of values for visual inspection, and run a much larger
        //   number of values for programmatic verification.
        // --------------------------------------------------------------------

        if (verbose) cout << "Random number generators test\n"
                             "=============================\n";

        cout << "randVal8()\n";
        for (int i = 0; i < 32; ++i) {
            for (int j = 0; j < 16; ++j) {
                cout << (j ? ", " : "") << randVal8();
            }
            cout << endl;
        }

        randAccum = 0;
        cout << "\nrandVal8(true)\n";
        for (int i = 0; i < 32; ++i) {
            for (int j = 0; j < 16; ++j) {
                cout << (j ? ", " : "") << randVal8(true);
            }
            cout << endl;
        }
        for (int i = 10 * 1000; i > 0; --i) {
            int j = randVal8(true);
            ASSERT((0 != j) & (0 == (~0x7f & j)));
        }

        randAccum = 0;
        cout << "\nrandVal16()\n";
        for (int i = 0; i < 60; ++i) {
            for (int j = 0; j < 10; ++j) {
                cout << (j ? ", " : "") << "0x" << bsl::hex << randVal16();
            }
            cout << endl;
        }
        for (int i = 100 * 1000; i > 0; --i) {
            int j = randVal16();
            ASSERT((j >= 0x80) & (j <= 0x7ff));
        }

        randAccum = 0;
        cout << "\nrandVal24(false)\n";
        for (int i = 0; i < 60; ++i) {
            for (int j = 0; j < 10; ++j) {
                cout << (j ? ", " : "") << "0x" << bsl::hex << randVal24();
            }
            cout << endl;
        }
        for (int i = 100 * 1000; i > 0; --i) {
            int j = randVal24();
            ASSERT((j >= 0x800) & (j <= 0xffff));
        }

        randAccum = 0;
        cout << "\nrandVal24(true)\n";
        for (int i = 0; i < 60; ++i) {
            for (int j = 0; j < 10; ++j) {
                cout << (j ? ", " : "") << "0x" << bsl::hex << randVal24(true);
            }
            cout << endl;
        }
        for (int i = 100 * 1000; i > 0; --i) {
            int j = randVal24(true);
            ASSERT((j >= 0x800) & (j <= 0xffff) &
                                             !((j >= 0xd800) & (j <= 0xdfff)));
        }

        randAccum = 0;
        cout << "\nrandVal32()\n";
        for (int i = 0; i < 60; ++i) {
            for (int j = 0; j < 8; ++j) {
                cout << (j ? ", " : "") << "0x" << bsl::hex << randVal32();
            }
            cout << endl;
        }
        int highest = 0;
        for (int i = 1000 * 1000; i > 0; --i) {
            int j = randVal32();
            ASSERT((j >= 0x10000) & (j <= 0x10ffff));
            if (j > highest) highest = j;
        }
        cout << "highest randVal32: " << highest << endl;
      }  break;
      case -2: {
        // --------------------------------------------------------------------
        // Test Apparatuses Test
        //
        // Concerns:
        //   Test apparatuses works properly.
        //
        // Plan:
        //   Test the various test functions and verify that they work as
        //   designed.
        // --------------------------------------------------------------------

        if (verbose) cout << "Test Apparatuses Test\n"
                             "=====================\n";

        cout << "Encode / Decode cycle\n";

        randAccum = 0;
        bsl::string str;

        {
            randAccum = 0;
            cout << "Encode:\n";
            str.reserve(7 * 7 * 4 + 1);
            int valStore[7][7];
            for (int row = 0; row < 7; ++ row) {
                for (int col = 0; col < 7; ++ col) {
                    int val = randValue(true);
                    cout << (col ? ", " : "") << "0x" << bsl::hex << val;
                    str += utf8Encode(val);
                    valStore[row][col] = val;
                }
                cout << endl;
            }

            ASSERT(str.length() < 7 * 7 * 4 + 1);

            cout << "Decode:\n";
            const char *pc = str.data();
            for (int row = 0; row < 7; ++ row) {
                for (int col = 0; col < 7; ++ col) {
                    int val = decode(&pc);
                    cout << (col ? ", " : "") << "0x" << bsl::hex << val;
                    ASSERT(valStore[row][col] == val);
                }
                cout << endl;
            }
            ASSERT(pc == str.data() + str.length());
        }

        {
            randAccum = 0;
            str = "";
            str.reserve(100 * 100 * 4 + 1);
            int valStore[100][100];
            for (int row = 0; row < 100; ++ row) {
                for (int col = 0; col < 100; ++ col) {
                    int val = randValue(true);
                    str += utf8Encode(val);
                    valStore[row][col] = val;
                }
            }

            ASSERT(str.length() < 100 * 100 * 4 + 1);

            const char *pc = str.data();
            for (int row = 0; row < 100; ++ row) {
                for (int col = 0; col < 100; ++ col) {
                    int val = decode(&pc);
                    ASSERT(valStore[row][col] == val);
                }
            }
            ASSERT(pc == str.data() + str.length());
            ASSERT(bsl::strlen(str.c_str()) < str.length());
        }

        {
            randAccum = 0;
            str = "";
            str.reserve(100 * 100 * 4 + 1);
            int valStore[100][100];
            for (int row = 0; row < 100; ++ row) {
                for (int col = 0; col < 100; ++ col) {
                    int val = randValue(true, true);
                    str += utf8Encode(val);
                    valStore[row][col] = val;
                }
            }

            ASSERT(str.length() < 100 * 100 * 4 + 1);

            const char *pc = str.data();
            for (int row = 0; row < 100; ++ row) {
                for (int col = 0; col < 100; ++ col) {
                    int val = decode(&pc);
                    ASSERT(valStore[row][col] == val);
                }
            }
            ASSERT(pc == str.data() + str.length());
            ASSERT(bsl::strlen(str.c_str()) == str.length());
        }
      } break;
      default: {
        cerr << "WARNING: CASE `" << test << "' NOT FOUND." << endl;
        testStatus = -1;
      }
    }  // switch (test)
    if (testStatus > 0) {
        cerr << "Error, non-zero test status = " << testStatus << "." << endl;
    }
    return testStatus;
}

// ---------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2009
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ---------------------------------
