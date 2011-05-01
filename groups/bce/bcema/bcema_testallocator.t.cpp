// bcema_testallocator.t.cpp                                          -*-C++-*-

#include <bcema_testallocator.h>

#include <bslma_testallocator.h>
#include <bslma_testallocatorexception.h>

#include <bsls_platform.h>

#include <bsl_cstdio.h>               // printf()
#include <bsl_cstring.h>              // memset(), strlen()
#include <bsl_cstdlib.h>              // atoi()
#include <bsl_iostream.h>
#include <bsl_new.h>                  // placement syntax
#include <bsl_strstream.h>            // ostrstream

#if defined(BSLS_PLATFORM__OS_SOLARIS)
#include <sys/resource.h>       // for setrlimit, etc
#endif

using namespace BloombergLP;
using namespace bsl;  // automatically added by script

//=============================================================================
//                              TEST PLAN
//-----------------------------------------------------------------------------
//                              Overview
//                              --------
// We are testing a concrete implementation of a protocol.  This "test"
// implementation calls 'malloc' and 'free' directly.  We must verify that
// instances of this concrete class do not call global operators 'new' and
// 'delete'.  We can do that by redefining these global operators and
// instrumenting them to be sure that these operators are in fact not called.
//
// We also need to verify that when bad addresses are supplied that we can
// detect them and report the problem to 'stdout'.  Since this behavior is
// not an error during the testing of this component, we will first set the
// quiet flag to suppress the output part, but will still verify the status
// to ensure that the problem was in fact detected.
//
// We must also verify that when exceptions are enabled, the test allocator
// throws an exception after the number of requests exceeds the allocator's
// (non-negative) allocation limit.  We achieve this by repeatedly allocating
// memory from the test allocator until the number of requests exceeds the
// allocation limit, then verify that exceptions thrown by the allocator are
// caught and that they contain the expected contents.
//-----------------------------------------------------------------------------
// [ 1] bcema_TestAllocator(bool verboseFlag = 0);
// [ 6] bcema_TestAllocator(const char *name, bool verboseFlag = 0);
// [ 2] ~bcema_TestAllocator();
// [ 3] void *allocate(int size);
// [ 3] void deallocate(void *address);
// [ 2] void setNoAbort(bool flagValue);
// [ 2] void setQuiet(bool flagValue);
// [ 2] void setVerbose(bool flagValue);
// [ 2] void setAllocationLimit(int numAlloc);
// [ 2] bool isNoAbort() const;
// [ 2] bool isQuiet() const;
// [ 2] bool isVerbose() const;
// [ 1] int numBytesInUse() const;
// [ 1] int numBlocksInUse() const;
// [ 1] int numBytesMax() const;
// [ 1] int numBlocksMax() const;
// [ 1] int numBytesTotal() const;
// [ 1] int numBlocksTotal() const;
// [ 1] int numMismatches() const;
// [ 2] int status() const;
// [ 2] int allocationLimit() const;
// [ 1] int lastAllocatedNumBytes() const;
// [ 1] int lastDeallocatedNumBytes() const;
// [ 1] void *lastAllocatedAddress() const;
// [ 1] void *lastDeallocatedAddress() const;
// [ 1] int numAllocations() const;
// [ 1] int numDeallocations() const;
// [ 6] const char *name() const;
//
// [ 4] ostream& operator<<(ostream& lhs, const bcema_TestAllocator& rhs);
//-----------------------------------------------------------------------------
// [ 8] USAGE TEST - Make sure usage example for exception neutrality works.
// [ 5] Ensure that exception is thrown after allocation limit is exceeded.
// [ 1] Make sure that all counts are initialized to zero (placement new).
// [ 1] Make sure that global operators new and delete are *not* called.
// [ 3] Ensure that the allocator is incompatible with new/delete.
// [ 3] Ensure that the allocator is incompatible with malloc/free.
// [ 3] Ensure that mismatched deallocations are detected/reported.
// [ 3] Ensure that repeated deallocations are detected/reported.
// [ 3] Ensure that an invalid cached length is reported.
// [ 3] Ensure that deallocated memory is scribbled.
// [ 3] Ensure that memory leaks (byte/block) are detected/reported.
// [ 7] Ensure that memory allocation list is kept track of properly.
// [ 8] Ensure that writing to the segment does not trigger over or under runs.
//=============================================================================
//                    STANDARD BDE ASSERT TEST MACRO
//-----------------------------------------------------------------------------
static int testStatus = 0;
static void aSsErT(int c, const char *s, int i)
{
    if (c) {
        cout << "Error " << __FILE__ << "(" << i << "): " << s
             << "    (failed)" << endl;
        if (testStatus >= 0 && testStatus <= 100) ++testStatus;
    }
}
#define ASSERT(X) { aSsErT(!(X), #X, __LINE__); }

//=============================================================================
//                    STANDARD BDE LOOP-ASSERT TEST MACROS
//-----------------------------------------------------------------------------

#define LOOP_ASSERT(I,X) { \
    if (!(X)) { cout << #I << ": " << I << "\n"; aSsErT(1, #X, __LINE__);}}

#define LOOP2_ASSERT(I,J,X) { \
    if (!(X)) { cout << #I << ": " << I << "\t" << #J << ": " \
              << J << "\n"; aSsErT(1, #X, __LINE__); } }

//=============================================================================
//                      SEMI-STANDARD TEST OUTPUT MACROS
//-----------------------------------------------------------------------------

#define P(X) cout << #X " = " << (X) << endl; // Print identifier and value.
#define Q(X) cout << "<| " #X " |>" << endl;  // Quote identifier literally.
#define Q_(X) cout << "<| " #X " |>" << flush;  // Q(X) without '\n'
#define P_(X) cout << #X " = " << (X) << ", " << flush; // P(X) without '\n'
#define L_ __LINE__                           // current Line number

//=============================================================================
//                              GLOBAL CONSTANTS
//-----------------------------------------------------------------------------

// This is copied from 'bcema_testallocator.cpp' to compare with scribbled
// deallocated memory.
const unsigned char SCRIBBLED_MEMORY = 0xA5;   // byte used to scribble
                                               // deallocated memory

//=============================================================================
//                                USAGE EXAMPLE
//-----------------------------------------------------------------------------
// my_shortarray.h

class my_ShortArray {
    short *d_array_p; // dynamically-allocated array of short integers
    int d_size;       // physical size of the 'd_array_p' array (elements)
    int d_length;     // logical length of the 'd_array_p' array (elements)
    bslma_Allocator *d_allocator_p; // holds (but does not own) allocator

  private:
    void increaseSize(); // Increase the capacity by at least one element.

  public:
    // CREATORS
    my_ShortArray(bslma_Allocator *basicAllocator = 0);
        // Create an empty array.  Optionally specify a 'basicAllocator'
        // used to supply memory.  If 'basicAllocator' is 0, global
        // operators 'new' and 'delete' are used.
     // ...

    ~my_ShortArray();
    void append(int value);
    const short& operator[](int index) const { return d_array_p[index]; }
    int length() const { return d_length; }
    operator const short *() const { return d_array_p; }
};

enum { INITIAL_SIZE = 1, GROW_FACTOR = 2 };

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// my_shortarray.cpp

my_ShortArray::my_ShortArray(bslma_Allocator *basicAllocator)
: d_size(INITIAL_SIZE)
, d_length(0)
, d_allocator_p(basicAllocator)
{
    int sz = d_size * sizeof *d_array_p;
    if (basicAllocator) {
        d_array_p = (short *) d_allocator_p->allocate(sz);
    }
    else {
        d_array_p = (short *) operator new(sz);
    }
}

my_ShortArray::~my_ShortArray()
{
    // CLASS INVARIANTS
    ASSERT(d_array_p);
    ASSERT(0 <= d_size);
    ASSERT(0 <= d_length); ASSERT(d_length <= d_size);

    if (d_allocator_p) {
        d_allocator_p->deallocate(d_array_p);
    }
    else {
        operator delete(d_array_p);
    }
}

inline void my_ShortArray::append(int value)
{
    if (d_length >= d_size) {
        increaseSize();
    }
    d_array_p[d_length++] = value;
}

inline static
void reallocate(short **array, int newSize, int length,
                bslma_Allocator *basicAllocator)
    // Reallocate memory in the specified 'array' to the specified 'newSize'
    // using the specified 'basicAllocator', or, if 'basicAllocator' is 0,
    // global operators 'new' and 'delete'.  The specified 'length' number of
    // leading elements are preserved.  Since the class invariant requires
    // that the physical capacity of the container may grow but never shrink;
    // the behavior is undefined unless length <= newSize.
{
    ASSERT(array);
    ASSERT(1 <= newSize);
    ASSERT(0 <= length);
    ASSERT(length <= newSize);          // enforce class invariant

    short *tmp = *array;                // support exception neutrality
    int sz = newSize * sizeof **array;
    if (basicAllocator) {
        *array = (short *) basicAllocator->allocate(sz);
    }
    else {
        *array = (short *) operator new(sz);
    }

    // COMMIT POINT

    memcpy(*array, tmp, length * sizeof **array);

    if (basicAllocator) {
        basicAllocator->deallocate(tmp);
    }
    else {
        operator delete(tmp);
    }
}

void my_ShortArray::increaseSize()
{
     int proposedNewSize = d_size * GROW_FACTOR;      // reallocate can throw
     ASSERT(proposedNewSize > d_length);
     reallocate(&d_array_p, proposedNewSize, d_length, d_allocator_p);
     d_size = proposedNewSize;                        // we're committed
}

ostream& operator<<(ostream& stream, const my_ShortArray& array)
{
    stream << '[';
    const int len = array.length();
    for (int i = 0; i < len; ++i) {
        stream << ' ' << array[i];
    }
    return stream << " ]" << flush;
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// my_shortarray.t.cpp

static int areEqual(const short *array1, const short *array2, int numElement)
    // Return 1 if the specified initial 'numElement' in the specified 'array1'
    // and 'array2' have the same values, and 0 otherwise.
{
    for (int i = 0; i < numElement; ++i) {
        if (array1[i] != array2[i]) return 0;
    }
    return 1;
}

static int verifyPrint(const bcema_TestAllocator& ta,
                       const char* const          FMT,
                       int                        verboseFlag)
    // Return 0 if the specified 'ta' prints the same message as 'FMT'.
{
    int verbose = verboseFlag > 2;
    int veryVerbose = verboseFlag > 3;
    // int veryVeryVerbose = verboseFlag > 4;

    if (verbose) cout << "\tCompare with expected result." <<endl;

    const int  SIZE = 2000;     // Must be big enough to hold output string.
    const char XX   = (char) 0xFF;  // Value used to represent an unset char.

    char mCtrlBuf[SIZE];  memset(mCtrlBuf, XX, SIZE);
    const char *CTRL_BUF = mCtrlBuf; // Used to check for extra characters.

    char buf[SIZE];
    memcpy(buf, CTRL_BUF, SIZE); // Preset buf to 'unset' char values.

    if (veryVerbose) cout << "\nEXPECTED FORMAT:" << endl << FMT << endl;
    ostrstream out(buf, SIZE);  out << ta << ends;
    if (veryVerbose) cout << "\nACTUAL FORMAT:" << endl << buf << endl;

    const int SZ = strlen(FMT) + 1;
    ASSERT(SZ < SIZE);  // Check buffer is large enough.
    ASSERT(XX == buf[SIZE - 1]);  // Check for overrun.
    ASSERT(0 == memcmp(buf + SZ, CTRL_BUF + SZ, SIZE - SZ));

    return memcmp(buf, FMT, SZ);
}

//-----------------------------------------------------------------------------
//                      REDEFINED GLOBAL OPERATOR NEW
//-----------------------------------------------------------------------------

static int globalNewCalledCount = 0;
static int globalNewCalledCountIsEnabled = 0;

static int globalDeleteCalledCount = 0;
static int globalDeleteCalledCountIsEnabled = 0;

#ifdef BDE_BUILD_TARGET_EXC
void *operator new(size_t size) throw(bsl::bad_alloc)
#else
void *operator new(size_t size)
#endif
    // Trace use of global operator new.  Note that we must use printf
    // to avoid recursion.
{
    void *addr = malloc(size);

    if (globalNewCalledCountIsEnabled) {
        ++globalNewCalledCount;
        printf ("globle new called, count = %d: %p\n",
                                                globalNewCalledCount, addr);
    }

    return addr;
}

#ifdef BDE_BUILD_TARGET_EXC
void operator delete(void *address) throw()
#else
void operator delete(void *address)
#endif
    // Trace use of global operator delete.
{
    if (globalDeleteCalledCountIsEnabled) {
        ++globalDeleteCalledCount;
        printf("global delete freeing: %p\n", address);
    }

    free(address);
}

//=============================================================================
//                      MAIN PROGRAM
//-----------------------------------------------------------------------------

int main(int argc, char *argv[]) {

    int test = argc > 1 ? atoi(argv[1]) : 0;
    int verbose = argc > 2;
    int veryVerbose = argc > 3;
    int veryVeryVerbose = argc > 4;

    cout << "TEST " << __FILE__ << " CASE " << test << endl;

    // Additional code for usage test:
    bcema_TestAllocator testAllocator(veryVeryVerbose);

    switch (test) { case 0:
      case 10: {
        // --------------------------------------------------------------------
        // TEST USAGE
        //   Verify that the usage example for testing exception neutrality is
        //   free of syntax errors and works as advertised.
        //
        // Testing:
        //   USAGE TEST - Make sure usage example compiles and works.
        // --------------------------------------------------------------------

        if (verbose) cout << endl << "TEST USAGE" << endl
                                  << "==========" << endl;

        typedef short Element;
        const Element VALUES[] = { 1, 2, 3, 4, -5 };
        const int NUM_VALUES = sizeof VALUES / sizeof *VALUES;

        const Element &V0 = VALUES[0],
                      &V1 = VALUES[1],
                      &V2 = VALUES[2],
                      &V3 = VALUES[3],
                      &V4 = VALUES[4];

        struct {
            int d_line;
            int d_numElem;
            short d_exp[NUM_VALUES];
        } DATA[] = {
            { L_, 0, { 0 } },
            { L_, 1, { V0 } },
            { L_, 5, { V0, V1, V2, V3, V4 } }
        };

        const int NUM_TEST = sizeof DATA / sizeof *DATA;

        for (int ti = 0; ti < NUM_TEST; ++ti) {
            const int    LINE     = DATA[ti].d_line;
            const int    NUM_ELEM = DATA[ti].d_numElem;
            const short *EXP      = DATA[ti].d_exp;

            BEGIN_BSLMA_EXCEPTION_TEST {
                my_ShortArray mA(&testAllocator);
                const my_ShortArray& A = mA;
                for (int ei = 0; ei < NUM_ELEM; ++ei) {
                    mA.append(VALUES[ei]);
                }
                if (veryVerbose) { P_(ti); P_(NUM_ELEM); P(A); }
                LOOP2_ASSERT(LINE, ti, areEqual(EXP, A, NUM_ELEM));
            } END_BSLMA_EXCEPTION_TEST
        }

        if (veryVerbose) cout << testAllocator << endl;
      } break;
      case 9: {
        // --------------------------------------------------------------------
        // TESTING BUFFER OVERRUN DETECTION
        //
        // Concerns:
        //   That the test allocator properly detects buffer over and underrun.
        //
        // Plan:
        //   Set the allocator in quiet mode, then deliberately cause buffer
        //   over and underruns to happen, verifying that they have happened
        //   by observer 'numMismatches()'.  First, verify that legitimate
        //   writing over the user segment does not trigger false errors.  At
        //   the end, verify 'status()' is what's expected and that
        //  'numMismatches()' is still 0.
        // --------------------------------------------------------------------

        if (verbose) cout << "Testing buffer over/underrun detection\n"
                             "======================================\n";

        enum { PADDING_SIZE = 8 };

        bcema_TestAllocator alloc(veryVeryVerbose);
        char *seg;

        alloc.setQuiet(true);

        // verify we can write all over the user segment without triggering
        // over/underrun fault
        for (int segLen = 1; segLen < 1000; ++segLen) {
            seg = (char *) alloc.allocate(segLen);
            memset(seg, 0x07, segLen);
            alloc.deallocate(seg);
            ASSERT(0 == alloc.numMismatches());
            ASSERT(0 == alloc.numBoundsErrors());

            seg = (char *) alloc.allocate(segLen);
            memset(seg, 0, segLen);
            alloc.deallocate(seg);
            ASSERT(0 == alloc.numMismatches());
            ASSERT(0 == alloc.numBoundsErrors());
        }

        if (veryVerbose) {
            cout << "Done verifying that writing over the segment does not\n"
                    "    trigger over/under run errors, numMistmatches = " <<
                    alloc.numMismatches() << endl;
        }

        int expectedBoundsErrors = 0;
        bool success;

        // Verify overrun is detected -- write on the trailing pad, deallocate,
        // and verify that numMismatches() has incremented to reflect the error
        // was caught.
        for (int segLen = 1; segLen < 1000; ++segLen) {
            for (int badByte = 0; badByte < PADDING_SIZE; ++badByte) {
                seg = (char *) alloc.allocate(segLen);
                seg[segLen + badByte] = 0x07;
                alloc.deallocate(seg);
                ASSERT(success =
                          (++expectedBoundsErrors == alloc.numBoundsErrors()));

                if (veryVerbose) {
                    cout << (success ? "S" : "Uns") <<
                            "uccessfully tested overrun at " << badByte <<
                        " bytes past the end of " << segLen <<
                        " length segment\n";
                }
            }
        }

        // Verify underrun is detected -- write on the trailing pad,
        // deallocate, and verify that numMismatches() has incremented to
        // reflect the error was caught.
        for (int segLen = 1; segLen < 1000; ++segLen) {
            for (int badByte = 0; badByte < PADDING_SIZE; ++badByte) {
                seg = (char *) alloc.allocate(segLen);
                seg[-(badByte + 1)] = 0x07;
                alloc.deallocate(seg);
                ASSERT(success =
                          (++expectedBoundsErrors == alloc.numBoundsErrors()));

                if (veryVerbose) {
                    cout << (success ? "S" : "Uns") <<
                            "uccessfully tested underrun at " << badByte <<
                        " bytes before the start of " << segLen <<
                        " length segment\n";
                }
            }
        }

        ASSERT(alloc.status() == expectedBoundsErrors);
        ASSERT(0 == alloc.numMismatches());
      } break;
      case 8: {
        // --------------------------------------------------------------------
        // TEST ALLOCATOR THROWS bsl::bad_alloc IF 'malloc' FAILS
        //
        // Testing:
        //   That the test allocator throws bsl::bad_alloc if it is unable
        //   to get memory.
        //
        // Concerns:
        //   That the test allocator will throw bsl::bad_alloc if 'malloc'
        //   returns 0.
        //
        // Plan:
        //   Set up a request for memory that fails, and verify that the
        //   exception occurs.
        // --------------------------------------------------------------------

        // A number of ways of getting malloc to fail were considered, all
        // had problems.  Asking for huge amounts of memory often took a lot
        // of time and wasted resources before 'malloc' gave up.  The
        // best approach turned out to be to set a low limit on the amount
        // of memory that could be obtained, and exceed that.  It was,
        // however, not portable, so we decided to implement the test only
        // on Solaris.
        // TBD: Implement this test on more platforms

        if (verbose) cout << "\nTEST THROWING STD::BAD_ALLOC\n"
                               "============================\n";

#ifdef BDE_BUILD_TARGET_EXC
// TBD This test is failing under gcc 4.3.2 with an uncaught exception.
// It does *not* appear to be an issue with EH support, but an issue with the
// test case proper.  In the debugger, it appeared that the runtime had
// insufficient resources to handle the exception, so 'abort()' was invoked.
#if defined(BSLS_PLATFORM__OS_SOLARIS) && !defined(BSLS_PLATFORM__CMP_GNU)
        if (verbose) cout << "\nTest throwing bsl::bad_alloc\n";

        rlimit rl = { 1 << 20, 1 << 20 };
        int sts = setrlimit(RLIMIT_AS, &rl);    // limit us to 1 MByte
        ASSERT(0 == sts);

        sts = getrlimit(RLIMIT_AS, &rl);
        ASSERT(0 == sts);
        if (verbose) cout << "Limits: " << rl.rlim_cur << ", " <<
                                           rl.rlim_max << endl;
        ASSERT(1 << 20 == rl.rlim_cur);

        bcema_TestAllocator ta;

        bool caught = false;
        void *p = (void *) 0x12345678;
        try {
            p = ta.allocate(1 << 21);
            cout << "Error: allocate() returned\n";
            ASSERT(0);
        } catch (bsl::bad_alloc) {
            caught = true;
            if (verbose) cout << "Exception caught, p = " << p << endl;
        }

        ASSERT(caught);
#else
        if (verbose) cout <<
                           "No testing.  Testing skipped on this platform.\n";
#endif
#else
        if (verbose) cout << "No testing.  Exceptions are not enabled.\n";
#endif
      } break;
      case 7: {
        // --------------------------------------------------------------------
        // TEST MEMORY ALLOCATION LIST
        //   Testing the memory allocation list managed internally within the
        //   'bcema_TestAllocator'.  Ensures that the list of outstanding
        //   memory blocks are kept track of.  Test the case where there's no
        //   allocated memory block outstanding, all allocated memory
        //   blocks outstanding and some allocated memory blocks outstanding.
        //
        // Testing:
        //   'd_list' inside 'bcema_TestAllocator'
        // --------------------------------------------------------------------

        if (verbose) cout << endl << "TEST MEMORY ALLOCATION LIST"
                          << endl << "===========================" << endl;

        {
        if (verbose) cout << "\tTest empty memory allocation list" <<endl;

        bcema_TestAllocator a;

        const char* const FMT =
            "\n"
            "==================================================\n"
            "                TEST ALLOCATOR STATE\n"
            "--------------------------------------------------\n"
            "        Category\tBlocks\tBytes\n"
            "        --------\t------\t-----\n"
            "          IN USE\t0\t0\n"
            "             MAX\t0\t0\n"
            "           TOTAL\t0\t0\n"
            "  NUM MISMATCHES\t0\n"
            "--------------------------------------------------\n"
            ;

        ASSERT(0 == verifyPrint(a, FMT, argc));
        }

        {
        if (verbose) cout << "\tTest full memory allocation list" <<endl;

        bcema_TestAllocator a;

        void *p1 = a.allocate(40);
        void *p2 = a.allocate(30);
        void *p3 = a.allocate(20);
        void *p4 = a.allocate(10);
        void *p5 = a.allocate( 1);
        void *p6 = a.allocate( 1);
        void *p7 = a.allocate( 1);
        void *p8 = a.allocate( 1);
        void *p9 = a.allocate( 1);

        const char* const FMT =
            "\n"
            "==================================================\n"
            "                TEST ALLOCATOR STATE\n"
            "--------------------------------------------------\n"
            "        Category\tBlocks\tBytes\n"
            "        --------\t------\t-----\n"
            "          IN USE\t9\t105\n"
            "             MAX\t9\t105\n"
            "           TOTAL\t9\t105\n"
            "  NUM MISMATCHES\t0\n"
            "--------------------------------------------------\n"
            ;

        ASSERT(0 == verifyPrint(a, FMT, argc));

        a.deallocate(p1);
        a.deallocate(p2);
        a.deallocate(p3);
        a.deallocate(p4);
        a.deallocate(p5);
        a.deallocate(p6);
        a.deallocate(p7);
        a.deallocate(p8);
        a.deallocate(p9);
        }

        {
        if (verbose) cout << "\tTest partial memory allocation list" <<endl;

        bcema_TestAllocator a;

        void *p1 = a.allocate(40);
        void *p2 = a.allocate(30);
        void *p3 = a.allocate(20);
        void *p4 = a.allocate(10);
        void *p5 = a.allocate( 1);

        a.deallocate(p3);
        a.deallocate(p5);

        void *p6 = a.allocate(10);

        const char* const FMT =
            "\n"
            "==================================================\n"
            "                TEST ALLOCATOR STATE\n"
            "--------------------------------------------------\n"
            "        Category\tBlocks\tBytes\n"
            "        --------\t------\t-----\n"
            "          IN USE\t4\t90\n"
            "             MAX\t5\t101\n"
            "           TOTAL\t6\t111\n"
            "  NUM MISMATCHES\t0\n"
            "--------------------------------------------------\n"
            ;

        ASSERT(0 == verifyPrint(a, FMT, argc));

        a.deallocate(p1);
        a.deallocate(p2);
        a.deallocate(p4);
        a.deallocate(p6);
        }

        {
        if (verbose) cout << "\tTest empty memory allocation list" <<endl;

        bcema_TestAllocator a;

        void *p1 = a.allocate(40);
        void *p2 = a.allocate(30);

        a.deallocate(p1);
        a.deallocate(p2);

        const char* const FMT =
            "\n"
            "==================================================\n"
            "                TEST ALLOCATOR STATE\n"
            "--------------------------------------------------\n"
            "        Category\tBlocks\tBytes\n"
            "        --------\t------\t-----\n"
            "          IN USE\t0\t0\n"
            "             MAX\t2\t70\n"
            "           TOTAL\t2\t70\n"
            "  NUM MISMATCHES\t0\n"
            "--------------------------------------------------\n"
            ;

        ASSERT(0 == verifyPrint(a, FMT, argc));
        }
      } break;
      case 6: {
        // --------------------------------------------------------------------
        // TEST NAMED CONSTRUCTOR AND NAME ACCESS
        //   Testing the 'bcema_TestAllocator' constructed with a 'name'.
        //   Ensures that the 'name' is accessible through the 'name'
        //   function.
        //
        // Testing:
        //   bcema_TestAllocator(const char* name, bool verboseFlag = 0);
        //   const char *name() const;
        // --------------------------------------------------------------------

        if (verbose) cout << endl << "TEST NAMED CONSTRUCTOR AND NAME ACCESS"
                          << endl << "=====================" << endl;

        const char *NAME   = "Test Allocator";
        const int   length = strlen(NAME);
        bcema_TestAllocator a(NAME, veryVeryVerbose);

        if (verbose) cout << "Make sure all internal states initialized."
                          << endl;
        ASSERT(0 == a.numBlocksInUse());
        ASSERT(0 == a.numBytesInUse());
        ASSERT(0 == a.numBlocksMax());
        ASSERT(0 == a.numBytesMax());
        ASSERT(0 == a.numBlocksTotal());
        ASSERT(0 == a.numBytesTotal());
        ASSERT(0 == a.lastAllocatedNumBytes());
        ASSERT(0 == a.lastAllocatedAddress());
        ASSERT(0 == a.lastDeallocatedNumBytes());
        ASSERT(0 == a.lastDeallocatedAddress());
        ASSERT(0 == a.numAllocations());
        ASSERT(0 == a.numDeallocations());

        if (verbose) cout << "Make sure name is correct."
                          << endl;

        ASSERT(0 == bsl::memcmp(NAME, a.name(), length));
      } break;
      case 5: {
        // --------------------------------------------------------------------
        // TEST ALLOCATION LIMIT
        //   Create a test allocator and set its allocation limit to varying
        //   values.  Verify that exception is thrown with the correct content
        //   for every allocation request that occurs after the number of
        //   requests exceeds the current allocation limit.  Also verify that
        //   exception is never thrown for negative allocation limits.
        //
        // Testing:
        //   Ensure that exception is thrown after allocation limit is exceeded
        // --------------------------------------------------------------------

        if (verbose) cout << endl << "TEST ALLOCATION LIMIT" << endl
                                  << "=====================" << endl;

#ifdef BDE_BUILD_TARGET_EXC
        if (verbose) cout << "Testing for exception thrown after exceeding "
                          << "allocation limit." << endl;

        const int NUM_ALLOC = 5;
        const int LIMIT[] = { 0, 1, 4, 5, -1, -100 };
        const int NUM_TEST = sizeof LIMIT / sizeof *LIMIT;

        bcema_TestAllocator mX(veryVeryVerbose);

        for (int ti = 0; ti < NUM_TEST; ++ti) {
            mX.setAllocationLimit(LIMIT[ti]);

            for (int ai = 0; ai < NUM_ALLOC; ++ai) {
                const int SIZE = ai + 1; // alloc size varies for each test
                if (veryVerbose) { P_(ti); P_(ai); P_(SIZE); P(LIMIT[ti]); }
                try {
                    void *p = mX.allocate(SIZE);
                    mX.deallocate(p);
                }
                catch (bslma_TestAllocatorException& e) {
                    int numBytes = e.numBytes();
                    if (veryVerbose) { cout << "Caught: "; P(numBytes); }
                    LOOP2_ASSERT(ti, ai, LIMIT[ti] == ai);
                    LOOP2_ASSERT(ti, ai, SIZE == numBytes);
                    continue;
                }
                LOOP2_ASSERT(ti, ai, LIMIT[ti] != ai);
            }
        }
#else
        if (verbose) cout << "No testing.  Exceptions are not enabled."
                          << endl;
#endif
      } break;
      case 4: {
        // --------------------------------------------------------------------
        // TEST OUTPUT OPERATOR (<<)
        //   Lightly verify that the output operator works.
        //
        // Testing:
        //   ostream& operator<<(ostream& lhs, const bcema_TestAllocator& rhs);
        // --------------------------------------------------------------------

        if (verbose) cout << endl << "TESTING OUTPUT OPERATOR (<<)" << endl
                                  << "============================" << endl;

        if (verbose) cout << "\nTest a single case with unique fields." <<endl;

        bcema_TestAllocator a;

        if (verbose) cout << "\tSet up unique fields." <<endl;
        void *p1 = a.allocate(40);
        void *p2 = a.allocate(30);
        void *p3 = a.allocate(20);
        void *p4 = a.allocate( 1);

        a.deallocate(p4);
        a.deallocate(p1);

        p1 = a.allocate(10);

        ASSERT(3 == a.numBlocksInUse());  ASSERT( 60 == a.numBytesInUse());
        ASSERT(4 == a.numBlocksMax());    ASSERT( 91 == a.numBytesMax());
        ASSERT(5 == a.numBlocksTotal());  ASSERT(101 == a.numBytesTotal());
        ASSERT(0 == a.numMismatches());

        const char* const FMT =
            "\n"
            "==================================================\n"
            "                TEST ALLOCATOR STATE\n"
            "--------------------------------------------------\n"
            "        Category\tBlocks\tBytes\n"
            "        --------\t------\t-----\n"
            "          IN USE\t3\t60\n"
            "             MAX\t4\t91\n"
            "           TOTAL\t5\t101\n"
            "  NUM MISMATCHES\t0\n"
            "--------------------------------------------------\n"
            ;

        ASSERT(0 == verifyPrint(a, FMT, argc));

        a.deallocate(p1);
        a.deallocate(p2);
        a.deallocate(p3);

      } break;
      case 3: {
        // --------------------------------------------------------------------
        // TEST ERROR COUNTS
        //   Disable the abort mode and ensure that attempting failure modes
        //   is correctly detected and logged in the numMismatches counter.
        //   Also verify that status is correct.  Note that this test case
        //   will cause purify errors.
        //
        // Testing:
        //   ~bcema_TestAllocator();
        //   void *allocate(int size);
        //   void deallocate(void *address);
        //
        //   Ensure that the allocator is incompatible with new/delete.
        //   Ensure that the allocator is incompatible with malloc/free.
        //   Ensure that mismatched deallocations are detected/reported.
        //   Ensure that repeated deallocations are detected/reported.
        //   Ensure that an invalid cached length is reported.
        //   Ensure that deallocated memory is scribbled.
        //   Ensure that memory leaks (byte/block) are detected/reported.
        // --------------------------------------------------------------------

        if (verbose) cout << endl << "TEST ERROR COUNTS" << endl
                                  << "=================" << endl;

        if (verbose) cout << endl
            << "Note:" << endl
            << "  Error messages can be viewed in veryVerbose mode" << endl
            << "  and Memory trace messages in veryVeryVerbose mode." << endl;

        const char *const LINE =
        "-------------------------------------------------------------------";

        {
            int i;
            if (verbose) cout <<
                "\nEnsure incompatibility with new/delete." << endl;

            bcema_TestAllocator a(veryVeryVerbose);
            a.setNoAbort(verbose); a.setQuiet(!veryVerbose);
            void *p = operator new(100);
            for (i = 0; i < 100; ++i) { ((char *)p)[i] = i; }
                                ASSERT(0 == a.numBlocksInUse());
                                ASSERT(0 == a.numBytesInUse());
                                ASSERT(0 == a.status());
                                ASSERT(0 == a.numMismatches());

            if (verbose) cout << "\t[deallocate unallocated pointer]" << endl;
            if (veryVerbose) cout << LINE << endl;
            a.deallocate(p);
            if (veryVerbose) cout << LINE << endl;
                                ASSERT(0 == a.numBlocksInUse());
                                ASSERT(0 == a.numBytesInUse());
                                ASSERT(1 == a.status());
                                ASSERT(1 == a.numMismatches());
            operator delete(p);
            p = a.allocate(7);  ((char *)p)[0] = (char) 0xA7;
                                ASSERT(1 == a.numBlocksInUse());
                                ASSERT(7 == a.numBytesInUse());
                                ASSERT(0  < a.status());
                                ASSERT(1 == a.numMismatches());

            a.deallocate(p);
                                ASSERT(0 == a.numBlocksInUse());
                                ASSERT(0 == a.numBytesInUse());
                                ASSERT(1 == a.status());
                                ASSERT(1 == a.numMismatches());

            if (verbose) cout <<
              "\nEnsure repeated deallocations are detected/reported." << endl;

            if (verbose) cout << "\t[deallocate deallocated pointer]" << endl;
            if (veryVerbose) cout << LINE << endl;
            a.deallocate(p);
            if (veryVerbose) cout << LINE << endl;
                                ASSERT(0 == a.numBlocksInUse());
                                ASSERT(0 == a.numBytesInUse());
                                ASSERT(2 == a.status());
                                ASSERT(2 == a.numMismatches());

            if (verbose) cout <<
                "\nEnsure incompatibility with malloc/free." << endl;
            p = malloc(200);
            for (i = 0; i < 200; ++i) { ((char *)p)[i] = i; }
                                ASSERT(0 == a.numBlocksInUse());
                                ASSERT(0 == a.numBytesInUse());
                                ASSERT(2 == a.status());
                                ASSERT(2 == a.numMismatches());
            if (verbose) cout << "\t[deallocate unallocated pointer]" << endl;
            if (veryVerbose) cout << LINE << endl;
            a.deallocate(p);
            if (veryVerbose) cout << LINE << endl;
                                ASSERT(0 == a.numBlocksInUse());
                                ASSERT(0 == a.numBytesInUse());
                                ASSERT(3 == a.status());
                                ASSERT(3 == a.numMismatches());
            free(p);
            p = a.allocate(5);  ((char *)p)[0] = (char) 0xA5;
                                ASSERT(1 == a.numBlocksInUse());
                                ASSERT(5 == a.numBytesInUse());
                                ASSERT(0  < a.status());
                                ASSERT(3 == a.numMismatches());
            if (verbose) cout <<
          "\nEnsure immediately repeated deallocations are detected/reported."
                                                                       << endl;
            if (verbose) cout << "\t[deallocate deallocated pointer]" << endl;
            if (veryVerbose) cout << LINE << endl;
            a.deallocate(p);
            a.deallocate(p);
            if (veryVerbose) cout << LINE << endl;
                                ASSERT(0 == a.numBlocksInUse());
                                ASSERT(0 == a.numBytesInUse());
                                ASSERT(4 == a.status());
                                ASSERT(4 == a.numMismatches());

            p = a.allocate(3);  ((char *)p)[0] = (char) 0xA3;
                                ASSERT(1 == a.numBlocksInUse());
                                ASSERT(3 == a.numBytesInUse());
                                ASSERT(1 == a.numBlocksMax());
                                ASSERT(7 == a.numBytesMax());
                                ASSERT(3 == a.numBlocksTotal());
                                ASSERT(15 == a.numBytesTotal());
                                ASSERT(4 == a.status());
                                ASSERT(4 == a.numMismatches());

            if (verbose) cout <<
            "\nEnsure mismatched deallocations are detected/reported." << endl;

            if (verbose) cout << "\t[deallocate pointer + sizeof(size_type)]"
                              << endl;
            if (veryVerbose) cout << LINE << endl;
            a.deallocate((bslma_Allocator::size_type *)p + 1);
            if (veryVerbose) cout << LINE << endl;
                                ASSERT(1 == a.numBlocksInUse());
                                ASSERT(3 == a.numBytesInUse());
                                ASSERT(0  < a.status());
                                ASSERT(5 == a.numMismatches());

            if (verbose) cout << "\t[deallocate pointer - sizeof(size_type)]"
                              << endl;
            if (veryVerbose) cout << LINE << endl;
            a.deallocate((bslma_Allocator::size_type *)p - 1);
            if (veryVerbose) cout << LINE << endl;
                                ASSERT(1 == a.numBlocksInUse());
                                ASSERT(3 == a.numBytesInUse());
                                ASSERT(0  < a.status());
                                ASSERT(6 == a.numMismatches());
            a.deallocate(p);
                                ASSERT(0 == a.numBlocksInUse());
                                ASSERT(0 == a.numBytesInUse());
                                ASSERT(6 == a.status());
                                ASSERT(6 == a.numMismatches());

            if (verbose) cout <<
               "\nEnsure an invalid cached length is reported." << endl;

            struct B {
                int d_m;
                int d_s;
            } b[2] = { { 0xdeadbeef, -1 }, { 0x11, 0x22 } };

            if (verbose) cout <<"\t[deallocate memory with bad length]" <<endl;
            if (veryVerbose) cout << LINE << endl;
            a.deallocate(b + 1);
            if (veryVerbose) cout << LINE << endl;
                                ASSERT(0 == a.numBlocksInUse());
                                ASSERT(0 == a.numBytesInUse());
                                ASSERT(7 == a.status());
                                ASSERT(7 == a.numMismatches());

#if 0
    // Accessing deallocated memory can result in errors on some platforms.
    // For this reason, this part of the test has been removed for now.
            if (verbose) cout <<
                         "\nEnsure deallocated memory is scribbled." << endl;
            unsigned char *q = (unsigned char *) a.allocate(9);
            memset(q, 0, 9);
            a.deallocate(q);
            if (verbose) cout << "\t[deallocate memory scribbled]" << endl;
            for (int mi = 0; mi < 9; ++mi) {
                LOOP_ASSERT(mi, SCRIBBLED_MEMORY == q[mi]);
            }
            if (veryVerbose) cout << LINE << endl;
#endif

            p = a.allocate(9);  ((char *)p)[0] = (char) 0xA9;
                                ASSERT(1 == a.numBlocksInUse());
                                ASSERT(9 == a.numBytesInUse());
                                ASSERT(0 <  a.status());
                                ASSERT(7 == a.numMismatches());

            if (verbose) cout <<
           "\nEnsure memory leaks (byte/block) are detected/reported." << endl;

            if (verbose) cout << "\t[memory leak]" << endl;
            if (veryVerbose) cout << LINE << endl;
        }
        if (veryVerbose) cout << LINE << endl;

      } break;
      case 2: {
        // --------------------------------------------------------------------
        // GET/SET FLAGS
        //   Verify that each of the get/set flags is working properly.
        //   Also verify that status correctly returns 0.
        //
        // Testing:
        //   void setVerbose(bool flagValue);
        //   bool isVerbose() const;
        //   void setNoAbort(bool flagValue);
        //   bool isNoAbort() const;
        //   void setQuiet(bool flagValue);
        //   bool isQuiet() const;
        //   void setAllocationLimit(int numAlloc);
        //   int allocationLimit() const;
        //   int status() const;
        // --------------------------------------------------------------------

        if (verbose) cout << endl << "GET/SET FLAGS" << endl
                                  << "=============" << endl;

        if (verbose) cout << "\nTest get/set flags" << endl;
        {

            bcema_TestAllocator a(veryVeryVerbose);

            ASSERT(false == a.isQuiet());
            ASSERT(false == a.isNoAbort());
            ASSERT(veryVeryVerbose == a.isVerbose());
            ASSERT(0 >  a.allocationLimit());

            if (verbose) cout << "\tVerbose" << endl;

            a.setVerbose(10);   ASSERT(false == a.isQuiet());
                                ASSERT(false == a.isNoAbort());
                                ASSERT(true  == a.isVerbose());
                                ASSERT(0 >  a.allocationLimit());

            a.setVerbose(0);    ASSERT(false == a.isQuiet());
                                ASSERT(false == a.isNoAbort());
                                ASSERT(false == a.isVerbose());
                                ASSERT(0 >  a.allocationLimit());

            if (verbose) cout << "\tQuite" << endl;

            a.setQuiet(10);     ASSERT(true  == a.isQuiet());
                                ASSERT(false == a.isNoAbort());
                                ASSERT(false == a.isVerbose());
                                ASSERT(0 >  a.allocationLimit());

            a.setQuiet(0);      ASSERT(false == a.isQuiet());
                                ASSERT(false == a.isNoAbort());
                                ASSERT(false == a.isVerbose());
                                ASSERT(0 >  a.allocationLimit());

            if (verbose) cout << "\tNoAbort" << endl;

            a.setNoAbort(10);   ASSERT(false == a.isQuiet());
                                ASSERT(true  == a.isNoAbort());
                                ASSERT(false == a.isVerbose());
                                ASSERT(0 >  a.allocationLimit());

            a.setNoAbort(0);    ASSERT(false == a.isQuiet());
                                ASSERT(false == a.isNoAbort());
                                ASSERT(false == a.isVerbose());
                                ASSERT(0 >  a.allocationLimit());

            if (verbose) cout << "\tAllocationLimit" << endl;

            a.setAllocationLimit(5);
                                ASSERT(false == a.isQuiet());
                                ASSERT(false == a.isNoAbort());
                                ASSERT(false == a.isVerbose());
                                ASSERT(5 ==  a.allocationLimit());

            a.setAllocationLimit(0);
                                ASSERT(false == a.isQuiet());
                                ASSERT(false == a.isNoAbort());
                                ASSERT(false == a.isVerbose());
                                ASSERT(0 == a.allocationLimit());

            a.setAllocationLimit(-1);
                                ASSERT(false == a.isQuiet());
                                ASSERT(false == a.isNoAbort());
                                ASSERT(false == a.isVerbose());
                                ASSERT(-1 == a.allocationLimit());

            if (verbose) cout << "\tStatus" << endl;

                                ASSERT(0 == a.status());
        }

      } break;
      case 1: {
        // --------------------------------------------------------------------
        // BASIC TEST:
        //   Create a test allocator in as buffer to make sure each field
        //   is properly initialized (with no verbose flag).  Then create a
        //   test allocator on the program stack and verify that all of the
        //   non-error counts are working properly.  Ensure that neither
        //   'new' or 'delete' are called.
        //
        // Testing:
        //   Make sure that all counts are initialized to zero (placement new).
        //
        //   bcema_TestAllocator(bool verboseFlag = 0);
        //   int numBytesInUse() const;
        //   int numBlocksInUse() const;
        //   int numBytesMax() const;
        //   int numBlocksMax() const;
        //   int numBytesTotal() const;
        //   int numBlocksTotal() const;
        //   int numMismatches() const;
        //   int lastAllocatedNumBytes() const;
        //   int lastDeallocatedNumBytes() const;
        //   void *lastAllocatedAddress() const;
        //   void *lastDeallocatedAddress() const;
        //   int numAllocations() const;
        //   int numDeallocations() const;
        //
        //   Make sure that global operators new and delete are *not* called.
        // --------------------------------------------------------------------

        if (verbose) cout << endl << "BASIC TEST" << endl
                                  << "==========" << endl;

        if (verbose) cout << "\nCreate an allocator in a buffer" << endl;

        union {
            int d_int; double d_double; void *d_ptr;
            char d_space[sizeof(bcema_TestAllocator)];
        } arena;

        memset(arena.d_space, 0xA5, sizeof arena.d_space);
        bcema_TestAllocator *p = new(&arena) bcema_TestAllocator;

        if (verbose) cout <<
            "\nMake sure all counts/and flags are initialized" << endl;

        if (veryVerbose) cout << *p << endl;

        ASSERT(0 == globalNewCalledCount);
        ASSERT(0 == globalDeleteCalledCount);
        ASSERT(0 == p->numBlocksInUse());
        ASSERT(0 == p->numBytesInUse());
        ASSERT(0 == p->numBlocksMax());
        ASSERT(0 == p->numBytesMax());
        ASSERT(0 == p->numBlocksTotal());
        ASSERT(0 == p->numBytesTotal());
        ASSERT(false == p->isQuiet());
        ASSERT(false == p->isNoAbort());
        ASSERT(false == p->isVerbose());
        ASSERT(0 == p->status());
        ASSERT(0 == p->lastAllocatedNumBytes());
        ASSERT(0 == p->lastAllocatedAddress());
        ASSERT(0 == p->lastDeallocatedNumBytes());
        ASSERT(0 == p->lastDeallocatedAddress());
        ASSERT(0 == p->numAllocations());
        ASSERT(0 == p->numDeallocations());

        if (verbose) cout << "\nCreate an allocator" << endl;

        bcema_TestAllocator a(veryVeryVerbose);

        if (verbose) cout << "\nMake sure counts work properly" << endl;

        if (veryVerbose) cout << a << endl;

        ASSERT(0 == globalNewCalledCount);
        ASSERT(0 == globalDeleteCalledCount);
        ASSERT(0 == a.numBlocksInUse());
        ASSERT(0 == a.numBytesInUse());
        ASSERT(0 == a.numBlocksMax());
        ASSERT(0 == a.numBytesMax());
        ASSERT(0 == a.numBlocksTotal());
        ASSERT(0 == a.numBytesTotal());
        ASSERT(0 == a.lastAllocatedNumBytes());
        ASSERT(0 == a.lastAllocatedAddress());
        ASSERT(0 == a.lastDeallocatedNumBytes());
        ASSERT(0 == a.lastDeallocatedAddress());
        ASSERT(0 == a.numAllocations());
        ASSERT(0 == a.numDeallocations());

        if (verbose) cout << "\tallocate 1" << endl;
        globalNewCalledCountIsEnabled = 1;
        void *addr1 = a.allocate(1);
        globalNewCalledCountIsEnabled = 0;
        ASSERT(1 == a.numBlocksInUse());
        ASSERT(1 == a.numBytesInUse());
        ASSERT(1 == a.numBlocksMax());
        ASSERT(1 == a.numBytesMax());
        ASSERT(1 == a.numBlocksTotal());
        ASSERT(1 == a.numBytesTotal());
        ASSERT(1 == a.lastAllocatedNumBytes());
        ASSERT(addr1 == a.lastAllocatedAddress());
        ASSERT(0 == a.lastDeallocatedNumBytes());
        ASSERT(0 == a.lastDeallocatedAddress());
        ASSERT(1 == a.numAllocations());
        ASSERT(0 == a.numDeallocations());

        if (verbose) cout << "\tallocate 20" << endl;
        globalNewCalledCountIsEnabled = 1;
        void *addr2 = a.allocate(20);
        globalNewCalledCountIsEnabled = 0;
        ASSERT( 2 == a.numBlocksInUse());
        ASSERT(21 == a.numBytesInUse());
        ASSERT( 2 == a.numBlocksMax());
        ASSERT(21 == a.numBytesMax());
        ASSERT( 2 == a.numBlocksTotal());
        ASSERT(21 == a.numBytesTotal());
        ASSERT(20 == a.lastAllocatedNumBytes());
        ASSERT(addr2 == a.lastAllocatedAddress());
        ASSERT( 0 == a.lastDeallocatedNumBytes());
        ASSERT( 0 == a.lastDeallocatedAddress());
        ASSERT( 2 == a.numAllocations());
        ASSERT( 0 == a.numDeallocations());

        if (verbose) cout << "\tdeallocate 20" << endl;
        globalDeleteCalledCountIsEnabled = 1;
        a.deallocate(addr2);
        globalDeleteCalledCountIsEnabled = 0;
        ASSERT( 1 == a.numBlocksInUse());
        ASSERT( 1 == a.numBytesInUse());
        ASSERT( 2 == a.numBlocksMax());
        ASSERT(21 == a.numBytesMax());
        ASSERT( 2 == a.numBlocksTotal());
        ASSERT(21 == a.numBytesTotal());
        ASSERT(20 == a.lastAllocatedNumBytes());
        ASSERT(addr2 == a.lastAllocatedAddress());
        ASSERT(20 == a.lastDeallocatedNumBytes());
        ASSERT(addr2 == a.lastDeallocatedAddress());
        ASSERT( 2 == a.numAllocations());
        ASSERT( 1 == a.numDeallocations());

        if (verbose) cout << "\tallocate 300" << endl;
        globalNewCalledCountIsEnabled = 1;
        void *addr3 = a.allocate(300);
        globalNewCalledCountIsEnabled = 0;
        ASSERT(  2 == a.numBlocksInUse());
        ASSERT(301 == a.numBytesInUse());
        ASSERT(  2 == a.numBlocksMax());
        ASSERT(301 == a.numBytesMax());
        ASSERT(  3 == a.numBlocksTotal());
        ASSERT(321 == a.numBytesTotal());
        ASSERT(300 == a.lastAllocatedNumBytes());
        ASSERT(addr3 == a.lastAllocatedAddress());
        ASSERT(20 == a.lastDeallocatedNumBytes());
        ASSERT(addr2 == a.lastDeallocatedAddress());
        ASSERT(  3 == a.numAllocations());
        ASSERT(  1 == a.numDeallocations());

        if (verbose) cout << "\tdeallocate 300" << endl;
        globalDeleteCalledCountIsEnabled = 1;
        a.deallocate(addr3);
        globalDeleteCalledCountIsEnabled = 0;
        ASSERT(  1 == a.numBlocksInUse());
        ASSERT(  1 == a.numBytesInUse());
        ASSERT(  2 == a.numBlocksMax());
        ASSERT(301 == a.numBytesMax());
        ASSERT(  3 == a.numBlocksTotal());
        ASSERT(321 == a.numBytesTotal());
        ASSERT(300 == a.lastAllocatedNumBytes());
        ASSERT(addr3 == a.lastAllocatedAddress());
        ASSERT(300 == a.lastDeallocatedNumBytes());
        ASSERT(addr3 == a.lastDeallocatedAddress());
        ASSERT(  3 == a.numAllocations());
        ASSERT(  2 == a.numDeallocations());

        if (verbose) cout << "\tdeallocate 1" << endl;
        globalDeleteCalledCountIsEnabled = 1;
        a.deallocate(addr1);
        globalDeleteCalledCountIsEnabled = 0;
        ASSERT(  0 == a.numBlocksInUse());
        ASSERT(  0 == a.numBytesInUse());
        ASSERT(  2 == a.numBlocksMax());
        ASSERT(301 == a.numBytesMax());
        ASSERT(  3 == a.numBlocksTotal());
        ASSERT(321 == a.numBytesTotal());
        ASSERT(300 == a.lastAllocatedNumBytes());
        ASSERT(addr3 == a.lastAllocatedAddress());
        ASSERT(1 == a.lastDeallocatedNumBytes());
        ASSERT(addr1 == a.lastDeallocatedAddress());
        ASSERT(  3 == a.numAllocations());
        ASSERT(  3 == a.numDeallocations());

        if (verbose) cout << "\nMake sure allocate/deallocate invalid "
                          << "size/address is recorded." << endl;
        a.setNoAbort(1);
        a.setQuiet(1);

        if (verbose) cout << "\tallocate 0" << endl;
        a.allocate(0);
        ASSERT(0 == a.lastAllocatedNumBytes());
        ASSERT(0 == a.lastAllocatedAddress());
        ASSERT(1 == a.lastDeallocatedNumBytes());
        ASSERT(addr1 == a.lastDeallocatedAddress());
        ASSERT(4 == a.numAllocations());
        ASSERT(3 == a.numDeallocations());

        if (verbose) cout << "\tallocate -1" << endl;
        void *addr5 = a.allocate(-1);
        ASSERT(-1 == a.lastAllocatedNumBytes());
        ASSERT( 0 == a.lastAllocatedAddress());
        ASSERT( 1 == a.lastDeallocatedNumBytes());
        ASSERT(addr1 == a.lastDeallocatedAddress());
        ASSERT( 5 == a.numAllocations());
        ASSERT( 3 == a.numDeallocations());

        if (verbose) cout << "\tdeallocate -1" << endl;
        a.deallocate(addr5);
        ASSERT(-1 == a.lastAllocatedNumBytes());
        ASSERT( 0 == a.lastAllocatedAddress());
        ASSERT( 0 == a.lastDeallocatedNumBytes());
        ASSERT( 0 == a.lastDeallocatedAddress());
        ASSERT( 5 == a.numAllocations());
        ASSERT( 4 == a.numDeallocations());

        if (verbose) cout << "\tallocate 0" << endl;
        a.deallocate(addr5);
        ASSERT(-1 == a.lastAllocatedNumBytes());
        ASSERT( 0 == a.lastAllocatedAddress());
        ASSERT( 0 == a.lastDeallocatedNumBytes());
        ASSERT( 0 == a.lastDeallocatedAddress());
        ASSERT( 5 == a.numAllocations());
        ASSERT( 5 == a.numDeallocations());

        if (verbose) cout << "\nEnsure new and delete are not called." << endl;
        ASSERT(0 == globalNewCalledCount);
        ASSERT(0 == globalDeleteCalledCount);

      } break;
      default: {
        cerr << "WARNING: CASE `" << test << "' NOT FOUND." << endl;
        testStatus = -1;
      }
    }

    if (testStatus > 0) {
        cerr << "Error, non-zero test status = " << testStatus << "." << endl;
    }
    return testStatus;
}

// ---------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2002
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ---------------------------------
