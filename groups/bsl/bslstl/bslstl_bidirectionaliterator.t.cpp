// bslstl_bidirectionaliterator.t.cpp                  -*-C++-*-

#include <bslstl_bidirectionaliterator.h>

#include <bslstl_iterator.h>   // for testing only
#include <bslmf_issame.h>

#include <climits>
#include <cstdlib>
#include <iostream>
#include <sstream>

using namespace BloombergLP;
using namespace std;

//=============================================================================
//                             TEST PLAN
//-----------------------------------------------------------------------------
//                              Overview
//                              --------
// 'bslstl_BidirectionalIterator' is an in-core value-semantic type that that
// adapts a more limitted type, which offers a basic set of operations, so that
// the resulting 'bslstl_BidirectionalIterator' object meets all the
// requirements of a standard Bidirectional Iterator.  These requirements are
// spelled out in [forward.iterators], Table 104 - Bidirectional iterator
// requirements.  The primary manipulator of an iterator is the pre-increment
// operator which, together with a function that returns an iterator to the
// start of a sequence, and a second function to return an iterator to the end
// of the same sequence, is sufficient to attain any achievable state.  While
// an iterator offers no direct accessors, its state can be reasonably inferred
// by inspecting the result of the dereference operator.
//
// In order to test this iterator adaptor, a simple container supporting
// forward iterators will be implemented, to provide the basic type to be
// adapted.  This container will use the 'bslstl_BidirectionalIterator'
// template to declare its iterators, as suggested in the usage example.
//
//  SKETCH NOTES FOR A PLAN THAT NEEDS UPDATING
//
// The following is the set of direct accessors (i.e. accessors that have
// direct contact with the physical state of the object):
// - int value() const;
// - ...;
//-----------------------------------------------------------------------------
// [ 2] bslstl_BidirectionalIterator();
// [ 3] bslstl_BidirectionalIterator(IMPL);
// [ 7] bslstl_BidirectionalIterator(const bslstl_BidirectionalIterator&);
// [ 2] ~bslstl_BidirectionalIterator();
// [ 9] bslstl_BidirectionalIterator& operator=(
//                                        const bslstl_BidirectionalIterator&);
// [ 2] bslstl_BidirectionalIterator& operator++();
// [12] bslstl_BidirectionalIterator  operator++(
//                                         bslstl_BidirectionalIterator&, int);
// [11] bslstl_BidirectionalIterator& operator--();
// [12] bslstl_BidirectionalIterator  operator--(
//                                         bslstl_BidirectionalIterator&, int);
// [ 4] T& operator*() const;
// [10] T *operator->() const;
// [ 6] bool operator==(const bslstl_BidirectionalIterator&,
//                      const bslstl_BidirectionalIterator&);
// [ 6] bool operator!=(const bslstl_BidirectionalIterator&,
//                      const bslstl_BidirectionalIterator&);
//-----------------------------------------------------------------------------
// [ 1] BREATHING TEST
// [  ] USAGE EXAMPLE is informative only, and does not compile independantly
//-----------------------------------------------------------------------------

//=============================================================================
//                      STANDARD BDE ASSERT TEST MACRO
//-----------------------------------------------------------------------------
static int testStatus = 0;

static void aSsErT(int c, const char *s, int i)
{
    if (c) {
        cout << "Error " << __FILE__ << "(" << i << "): " << s
             << "    (failed)" << endl;
        if (0 <= testStatus && testStatus <= 100) ++testStatus;
    }
}

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
#define T_ cout << "\t" << flush;             // Print tab w/o newline
#define L_ __LINE__                           // current Line number

//=============================================================================
//                ALGORITHMS FOR TESTING USAGE EXAMPLES
//-----------------------------------------------------------------------------

namespace
{

template<typename Iter>
int testDistance( Iter first, Iter last ) {
    //  A basic algorithm to verify the iterator can type walk the range
    //  specified by the pair of iterators 'first' and 'last', and return at
    //  end of the range.  We choose to calculate distance as this might prove
    //  useful in verifying the number of iterations and operations in further
    //  tests.
    int result = 0;
    while( first != last ) {
        ++result;
        ++first;
    }
    return result;
}


//  Create an overload set that can determine, using tag dispatch techniques,
//  exactly which iterator tag is declared for this iterator adaptor
enum iter_tag_type {
    output_iterator,
    input_iterator,
    forward_iterator,
    bidirectional_iterator,
    random_access_iterator
};

template<typename Iter>
iter_tag_type
testTagDispatch( Iter, Iter, std::output_iterator_tag* =
                                         (typename Iter::iterator_category*)0 )
{
    return output_iterator;
}

template<typename Iter>
iter_tag_type
testTagDispatch( Iter, Iter, std::input_iterator_tag* =
                                         (typename Iter::iterator_category*)0 )
{
    return input_iterator;
}

template<typename Iter>
iter_tag_type
testTagDispatch( Iter, Iter, std::forward_iterator_tag* =
                                         (typename Iter::iterator_category*)0 )
{
    return forward_iterator;
}

template<typename Iter>
iter_tag_type
testTagDispatch( Iter, Iter, std::bidirectional_iterator_tag* =
                                         (typename Iter::iterator_category*)0 )
{
    return bidirectional_iterator;
}

template<typename Iter>
iter_tag_type
testTagDispatch( Iter, Iter, std::random_access_iterator_tag* =
                                         (typename Iter::iterator_category*)0 )
{
    return random_access_iterator;
}


//  Create a set of overloads to verify that appropriately tagged algorithms
//  can be called with this iterator adaptor, but not other categories.
//  To an extend this is duplicating work that should be part of the
//  bslstl_iterator test driver, consider moving as appropriate.

template<typename Iter>
bool testOutputTag( Iter, Iter ) { return true; }

template<typename Iter>
bool testOutputTag( Iter, Iter, std::output_iterator_tag* =
                                         (typename Iter::iterator_category*)0 )
{
    return false;
}

template<typename Iter>
bool testInputTag( Iter, Iter ) { return false; }

template<typename Iter>
bool testInputTag( Iter, Iter, std::input_iterator_tag* =
                                         (typename Iter::iterator_category*)0 )
{
    return true;
}

template<typename Iter>
bool testForwardTag( Iter, Iter ) { return false; }

template<typename Iter>
bool testForwardTag( Iter, Iter, std::forward_iterator_tag* =
                                         (typename Iter::iterator_category*)0 )
{
    return true;
}

template<typename Iter>
bool testBidirectionalTag( Iter, Iter ) { return true; }

template<typename Iter>
bool testBidirectionalTag( Iter, Iter, std::bidirectional_iterator_tag* =
                                         (typename Iter::iterator_category*)0 )
{
    return false;
}

template<typename Iter>
bool testRandomAccessTag( Iter, Iter ) { return true; }

template<typename Iter>
bool testRandomAccessTag( Iter, Iter, std::random_access_iterator_tag* =
                                         (typename Iter::iterator_category*)0 )
{
    return false;
}

struct Wrap { int data; };

}
//=============================================================================
//                  CLASSES FOR TESTING USAGE EXAMPLES
//-----------------------------------------------------------------------------

class UnspecifiedBool {
    //  This class provides a value type that can be implicitly converted to
    //  'bool'.  This will allow us to validate the minimum guarantees on
    //  functions specified as returning a value "convertible to 'bool'".
private:
    struct impl { int data; };
    typedef int impl::*result_type;
    result_type d_result;

public:
    explicit UnspecifiedBool(bool b)
      : d_result(b ? &impl::data : result_type()) {}

    operator result_type() const { return d_result; }
};

//  Minimal implementation of a singly linked list to validate basic iterator
//  operations with a suitably resricted container.  The important feature for
//  validating the iterator adaptor is that the interator-like class being
//  adapted for the container's iterators support only the minimum set of
//  operations required for the adaptor.  If not for this requirement, we would
//  use 'slist' as an already well-tested component.  Note that this simple
//  list container supports appending items to the front of the list, but does
//  not support insertion at any other location, nor erasure.  This provides
//  the minimal set of facilities needed to validate the iterator under test.
template <class T>
class my_List
{
    // PRIVATE TYPES
    struct Node
    {
        Node *d_next;
        Node *d_prev;
        T     d_val;

        Node(const T& val, Node* next)
          : d_next(next), d_prev(), d_val(val)
        {
            if(d_next) {
              d_next->d_prev = this;
            }
        }
    };

    class IteratorImp
    {
        Node*   d_node;

      public:
        IteratorImp(Node* node = 0) : d_node(node) { }

        // Compiler-generated copy and destruction
        // IteratorImp(const IteratorImp&);
        // IteratorImp& operator=(const IteratorImp&);
        // ~IteratorImp();

        UnspecifiedBool operator==(const IteratorImp& rhs) const
            { return UnspecifiedBool(d_node == rhs.d_node); }

        T& operator*() const { return d_node->d_val; }

        void operator++() { d_node = d_node->d_next; }
        void operator--() { d_node = d_node->d_prev; }
    };

    friend class bslstl_BidirectionalIterator<T,IteratorImp>;

    friend class bslstl_BidirectionalIterator<const T,IteratorImp>;

    // Private copy constructor and copy assignment operator.
    // This class does not have value semantics.
    my_List(const my_List&);
    my_List& operator=(const my_List&);

    // PRIVATE DATA
    Node * d_head;
    Node * d_tail;

  public:
    // Declare iterator types:
    typedef bslstl_BidirectionalIterator<T, IteratorImp>       iterator;
    typedef bslstl_BidirectionalIterator<const T, IteratorImp> const_iterator;

  my_List() : d_head(new Node(T(),0)), d_tail(d_head) {}

    ~my_List() {
        while (d_head) {
            Node *x = d_head;
            d_head = d_head->d_next;
            delete x;
        }
    }

    void push(const T& v) {
        d_head = new Node(v, d_head);
    }

    iterator begin() { return IteratorImp(d_head); }
    iterator end()   { return IteratorImp(d_tail); }
    const_iterator begin() const { return IteratorImp(d_head); }
    const_iterator end() const { return IteratorImp(d_tail); }
};


//=============================================================================
//                              MAIN PROGRAM
//-----------------------------------------------------------------------------

int main(int argc, char *argv[])
{
    int test = argc > 1 ? atoi(argv[1]) : 0;
    int verbose = argc > 2;
    int veryVerbose = argc > 3;
    int veryVeryVerbose = argc > 4;
    int veryVeryVeryVerbose = argc > 5;

    cout << "TEST " << __FILE__ << " CASE " << test << endl;

    switch (test) { case 0:
      case 12: {
        // --------------------------------------------------------------------
        // TESTING POST-INCREMENT OPERATOR
        //
        // Concerns:
        //   The usage example provided in the component header file must
        //   compile, link, and run on all platforms as shown.
        //
        // Plan:
        //   Incorporate usage example from header into driver, remove leading
        //   comment characters, and replace 'assert' with 'ASSERT'.
        //
        // Testing:
        //   bslstl_BidirectionalIterator& operator++(int);
        //   bslstl_BidirectionalIterator& operator--(int);
        // --------------------------------------------------------------------

        if (verbose) cout << endl << "TESTING POST-*CREMENT OPERATOR" << endl
                                  << "==============================" << endl;

        int testData[4] = { 0, 1, 2, 3 };
        typedef bslstl_BidirectionalIterator<int, int*> iterator;
        typedef bslstl_BidirectionalIterator<const int, int*> const_iterator;

        if (verbose) cout << "\nConstruct basic iterator values" << endl;
        iterator it1 = testData;
        iterator it2 = it1;
        iterator it3 = it1;
        ++it2;

        if (verbose) cout << "\nvalidate post-increment operator" << endl;
        ASSERT( it3++ == it1);
        ASSERT( it3 == it2);

        if (verbose) cout << "\nvalidate post-decrement operator" << endl;
        ASSERT( it3-- == it2);
        ASSERT( it3 == it1);

      } break;
      case 11: {
        // --------------------------------------------------------------------
        // TESTING DECREMENT OPERATOR
        //
        // Concerns:
        //
        // Plan:
        //
        // Testing:
        //   bslstl_BidirectionalIterator& operator++();
        // --------------------------------------------------------------------

        if (verbose) cout << endl << "TESTING DECREMENT OPERATOR" << endl
                                  << "==========================" << endl;

        //  Declare test data and types
        int testData[4] = { 0, 1, 2, 3 };
        typedef bslstl_BidirectionalIterator<int, int*> iterator;
        typedef bslstl_BidirectionalIterator<const int, int*> const_iterator;

        //  Confirm that an iterator over our reference array does not have the
        //  same value as a default constructed iterator.  (Actually, this
        //  comparison is undefined in general.)
        const const_iterator itcOrigin = &testData[3];
        iterator itData = &testData[3];
        ASSERT( itcOrigin == itData);

        //  Confirm that an decremented iterator does not have the same value
        //  as the intitial iterator
        --itData;
        ASSERT( itcOrigin != itData);

        //  Confirm that decrementing a second copy of the intitial iterator
        //  has the same value as the first incremented iterator.
        const_iterator itcCopy = itcOrigin;
        ASSERT( itcOrigin == itcCopy);

        --itcCopy;
        ASSERT( itcOrigin != itcCopy);
        ASSERT( itData == itcCopy);

        const_iterator itReader = &testData[4];
        const int* itValidator = testData + 4;
        for(int i = 4; i; --itValidator,--itReader, --i) {
            if(4 == i) {
                LOOP_ASSERT(i, &*itReader == itValidator);
            }
            else {
                LOOP3_ASSERT(i, *itReader, *itValidator,
                                                    &*itReader == itValidator);
            }
        }
      } break;
      case 10: {
        // --------------------------------------------------------------------
        // TESTING OPERATOR->
        //
        //
        // Concerns:
        //
        // Plan:
        //
        // Testing:
        //    T *operator->() const
        // --------------------------------------------------------------------

        if (verbose) cout << endl << "TESTING OPERATOR->" << endl
                                  << "==================" << endl;

        {
        if (verbose) cout << "\nVefify iterator properties with a"
                             " directly examinable container" << endl;

        //  Declare test data and types
        Wrap testData[2] = { {13}, {99} };
        typedef bslstl_BidirectionalIterator<Wrap, Wrap*> iterator;
        typedef bslstl_BidirectionalIterator<const Wrap, Wrap*> const_iterator;

        const iterator itWritable = testData;
        // Obtain a fresh copy of an equivalent constant iterator
        const_iterator itReadable = testData;

        ASSERT(13 == itWritable->data);
        ASSERT(13 == itReadable->data);
        ASSERT(&itWritable->data == &testData[0].data);
        ASSERT(&itReadable->data == &testData[0].data);

        if (verbose) cout << "\nVerify can observe new value written through"
                          << " the iterator" << endl;
        itWritable->data = 42;

        //  Test itReadable before itWritable to be sure the update is not a
        //  side effect of the reading!
        ASSERT(42 == itReadable->data);
        ASSERT(42 == itWritable->data);
        ASSERT(42 == testData[0].data);

        testData[0].data = 13;

        ASSERT(13 == itWritable->data);
        ASSERT(13 == itReadable->data);
        ASSERT(&itWritable->data == &testData[0].data);
        ASSERT(&itReadable->data == &testData[0].data);

        iterator itNext = itWritable;
        ++itNext;
        ASSERT(99 == itNext->data);
        ASSERT(13 == itWritable->data);
        ASSERT(&itNext->data == &testData[1].data);

        itNext->data = 42;
        ASSERT(42 == itNext->data);
        ASSERT(13 == itWritable->data);
        ASSERT(42 == testData[1].data);
        }

      } break;
      case 9: {
        // --------------------------------------------------------------------
        // TESTING ASSIGNMENT OPERATOR
        //
        // Concerns:
        //
        // Plan:
        //
        // Testing:
        //   bslstl_BidirectionalIterator& operator=(
        //                                 const bslstl_BidirectionalIterator&)
        // --------------------------------------------------------------------

        if (verbose) cout << endl << "TESTING ASSIGNMENT OPERATOR" << endl
                                  << "===========================" << endl;

        if (verbose) cout << "\nTesting assignment u = v" << endl;

        typedef my_List<int> TestContainer;
        typedef TestContainer::iterator iterator;
        typedef TestContainer::const_iterator const_iterator;

        TestContainer testContainer;
        testContainer.push(42);
        testContainer.push(13);

        const iterator itBase = testContainer.begin();
        const iterator itSecond = ++testContainer.begin();

        iterator itTest = itBase;
        ASSERT(itBase == itTest);
        ASSERT(itSecond != itTest);

        itTest = itSecond;
        ASSERT(itBase != itTest);
        ASSERT(itSecond == itTest);

        itTest = itBase;
        ASSERT(itBase == itTest);
        ASSERT(itSecond != itTest);

        // Test const_iterator assignment
        const_iterator citTest = itBase;
        ASSERT(itBase == citTest);
        ASSERT(itSecond != citTest);

        citTest = itSecond;
        ASSERT(itBase != citTest);
        ASSERT(itSecond == citTest);

        citTest = itBase;
        ASSERT(itBase == citTest);
        ASSERT(itSecond != citTest);

      } break;
      case 8: {
        // --------------------------------------------------------------------
        // TESTING VALUE CONSTRUCTOR
        //  This iterator has two value constructors,
        //  (1) construct from an iterator-like value pointing to host sequence
        //  (2) const a constant iterator from a mutable iterator
        //  Note that (1) has been thoroughly tested by the generator functions
        //  in test [3], and (2) has been widely used in earlier tests to test
        //  interoperability.
        //
        // Concerns:
        //
        // Plan:
        //
        // Testing:
        // --------------------------------------------------------------------

        if (verbose) cout << endl << "TESTING VALUE CONSTRUCTOR" << endl
                                  << "=========================" << endl;

        //  Declare test data and types
        int testData[1] = { 13 };
        typedef bslstl_BidirectionalIterator<int, int*> iterator;
        typedef bslstl_BidirectionalIterator<const int, int*> const_iterator;

        const iterator itSource = testData;
        const_iterator itCopy = itSource;
        ASSERT(itSource == itCopy);

      } break;
       case 7: {
        // --------------------------------------------------------------------
       // TESTING COPY CONSTRUCTOR
        //
        // Concerns:
        //
        // Plan:
        //
        // Testing:
        // --------------------------------------------------------------------

        if (verbose) cout << endl << "TESTING COPY CONSTRUCTOR" << endl
                                  << "========================" << endl;

        //  Declare test data and types
        int testData[1] = { 13 };
        typedef bslstl_BidirectionalIterator<int, int*> iterator;
        typedef bslstl_BidirectionalIterator<const int, int*> const_iterator;

        const iterator itSource = testData;
        iterator itCopy = itSource;
        ASSERT(itSource == itCopy);

        const const_iterator citSource = testData;
        const_iterator citCopy = itSource;
        ASSERT(citSource == citCopy);

      } break;
      case 6: {
        // --------------------------------------------------------------------
        // TESTING EQUALITY OPERATOR
        //   The equality operator defines the notion of value for a type.  It
        //   shall return 'true' when two iterators have the same value, and
        //   'false' otherwise.  Likewise, operator!= shall return
        //
        // Concerns:
        //    Iterators must compare equal to themselves.
        //    constant iterators can be compared with mutable iterators
        //    contant and mutable iterators referring to the same element shall
        //    compare equal
        //    iterators that do not refer to the same element shall not compare
        //    equal
        //    tests based on the identity of the referenced element must be
        //    valid, even when the referenced element overloads operator&
        //
        // Plan:
        //   Create a list with a single element, so that the sequence is not
        //   empty, but two increments will take an iterator from 'begin' to
        //   'end'.  Validate that both the begin and end iterators compare
        //   equal to themselves, and do not compare equal to each other.  Then
        //   verify that an iterator copied from a 'begin' iterator compares
        //   equal to 'begin' and not 'end', after a single increment compares
        //   equal to neither, and after a third increment compares equal to
        //   the 'end' iterator.  Validating two iterators compare equal means
        //   asserting both the '==' and '!=' operators yield the correct
        //   values.
        //
        // Testing:
        //   bool operator==(const bslstl_BidirectionalIterator&,
        //                   const bslstl_BidirectionalIterator&);
        //   bool operator!=(const bslstl_BidirectionalIterator&,
        //                   const bslstl_BidirectionalIterator&);
        // --------------------------------------------------------------------
        if (verbose) cout << endl << "TESTING EQUALITY OPERATOR" << endl
                                  << "=========================" << endl;

        typedef my_List<int> TestContainer;
        typedef TestContainer::iterator iterator;
        typedef TestContainer::const_iterator const_iterator;

        TestContainer testContainer;
        testContainer.push(42);
        testContainer.push(13);

        if (verbose) cout << "\nvalidate self-equality" << endl;
        const iterator itBegin = testContainer.begin();
        ASSERT(itBegin == itBegin);
        ASSERT(!(itBegin != itBegin));
        ASSERT(testContainer.begin() == itBegin);

        if (verbose) cout << "\nvalidate inequality" << endl;
        const iterator itEnd = testContainer.end();
        ASSERT(itBegin != itEnd);
        ASSERT(!(itBegin == itEnd));
        ASSERT(itEnd == itEnd);
        ASSERT(!(itEnd != itEnd));

        if (verbose) cout << "\nvalidate interoperability of types" << endl;
        const const_iterator citBegin = testContainer.begin();
        const_iterator citEnd   = testContainer.end();
        ASSERT(citBegin == itBegin);
        ASSERT(citEnd == itEnd);

        if (verbose) cout << "\nvalidate transition to expected value" << endl;
        iterator itCursor = testContainer.begin();
        ASSERT(itBegin == itCursor);
        ASSERT(citEnd != itCursor);

        ++itCursor;
        ASSERT(itBegin != itCursor);
        ASSERT(citEnd != itCursor);

        ++itCursor;
        ASSERT(itBegin != itCursor);
        ASSERT(citEnd == itCursor);

      } break;
      case 5: {
        // --------------------------------------------------------------------
        // TESTING 'print' AND 'operator<<'
        //   N/A for this component, although a debug printer might be
        //   considered in the future.
        // --------------------------------------------------------------------
        if (verbose) cout <<"\nThis is trivially satisfied for iterator types."
                            "\nIterators do not support printing or streaming."
                          << endl;
      } break;
      case 4: {
        // --------------------------------------------------------------------
        // TESTING BASIC ACCESSORS
        //   While an iterator has no named attributes, and so no basic
        //   accessors, the -> operator acts as a basic accessor, returning the
        //
        //
        // Concerns:
        //   Derefencing an iterator should refer to the expected element
        //   Must be able to check dereferenced object's identity, even if it
        //   overloads operator&
        //   Should be able to write through a mutable iterator, and observe
        //   the effect through a constant iterator referring to the same
        //   element.
        //
        // Plan:
        //   Perform initial validation against an array of ints, that can be
        //   directly manipulated to confirm iterators are operating correctly.
        //   Repeat tests using the sample list container to vefify that the
        //   same operations work as advertised when the adapted iterator
        //   offers only the minimal set of operations.
        //
        // Testing:
        //   T& operator*() const;
        // --------------------------------------------------------------------

        if (verbose) cout << endl << "TESTING BASIC ACCESSORS" << endl
                                  << "=======================" << endl;


        {
        if (verbose) cout << "\nVefify iterator properties with a"
                             " directly examinable container" << endl;

        //  Declare test data and types
        int testData[1] = { 13 };
        typedef bslstl_BidirectionalIterator<int, int*> iterator;
        typedef bslstl_BidirectionalIterator<const int, int*> const_iterator;

        const iterator itWritable = testData;
        // Obtain a fresh copy of an equivalent constant iterator
        const_iterator itReadable = testData;

        ASSERT(13 == *itWritable);
        ASSERT(13 == *itReadable);
        ASSERT(&*itWritable == testData);
        ASSERT(&*itReadable == testData);

        if (verbose) cout << "\nVerify can observe new value written through"
                          << " the iterator" << endl;
        *itWritable = 42;

        //  Test itReadable before itWritable to be sure the update is not a
        //  side effect of the reading!
        ASSERT(42 == *itReadable);
        ASSERT(42 == *itWritable);
        ASSERT(42 == testData[0]);

        testData[0] = 13;

        ASSERT(13 == *itWritable);
        ASSERT(13 == *itReadable);
        ASSERT(&*itWritable == testData);
        ASSERT(&*itReadable == testData);
        }

        {
        if (verbose) cout << "\nRepeat tests with a minimal adapted iterator"
                          << endl;

        typedef my_List<int> test_container_type;
        typedef test_container_type::iterator iterator;
        typedef test_container_type::const_iterator const_iterator;

        test_container_type testContainer;
        testContainer.push(13);

        const iterator itWritable = testContainer.begin();
        // Obtain a fresh copy of an equivalent constant iterator
        const_iterator itReadable = testContainer.begin();

        ASSERT(13 == *itWritable);
        ASSERT(13 == *itReadable);
        ASSERT(&*itWritable == &*itReadable);

        if (verbose) cout << "\nVerify can observe new value written through"
                          << " the iterator" << endl;
        *itWritable = 42;

        //  Test itReadable before itWritable to be sure the update is not a
        //  side effect of the reading!
        ASSERT(&*itWritable == &*itReadable);
        ASSERT(42 == *itReadable);
        ASSERT(42 == *itWritable);
        }
      } break;
      case 3: {
        // --------------------------------------------------------------------
        // TESTING (PRIMITIVE) GENERATORS
        //   The primitive generators for our iterator, the primary piece of
        //   test apparatus, are the 'begin' and 'end' member functions of the
        //   sample container type.  It is difficult to test the value of these
        //   iterators without any basic accessors, but we can assert several
        //   basic properties:
        //   ..
        //    1/ No generated iterator shall have the value of a default-
        //       constructed iterator.
        //    2/ The generated iterators shall compare equal to themselves.
        //    3/ The iterators generated by 'begin' and 'end' shall compare
        //       equal if, and only if, the underlying container is empty.
        //   ..
        //   Note that while it is not possible to compare an iterator with
        //   the default-constructed iterator value in the general case, the
        //   specific cases we are testing ensure this is well defined.  This
        //   is another facility of the iterator adaptors.
        //
        // Concerns:
        //   1. The test container may have some latent bug that yields bad
        //      iterators.
        //
        // Testing:
        //   class UnspecifiedBool
        //   class my_List<T>
        //   my_List<T>::begin
        //   my_List<T>::end
        //   bslstl_BidirectionalIterator(IMPL);
        // --------------------------------------------------------------------

        if (verbose) cout << endl << "TESTING (PRIMITIVE) GENERATORS" << endl
                                  << "==============================" << endl;

        if (verbose) cout << "\nValidating primitive test machinery" << endl;

        ASSERT(UnspecifiedBool(true));
        ASSERT(!UnspecifiedBool(false));


        if (verbose) cout << "\nTesting class my_List<int>" << endl;

        typedef my_List<int> TestContainer;
        typedef TestContainer::iterator iterator;
        typedef TestContainer::const_iterator const_iterator;

        TestContainer testContainer;
        iterator itBegin = testContainer.begin();
        iterator itEnd   = testContainer.end();
        ASSERT(itBegin == itEnd);

        testContainer.push(3);
        itBegin = testContainer.begin();
        itEnd = testContainer.end();
        ASSERT(itBegin != itEnd);

        ++itBegin;
        ASSERT(itBegin == itEnd);

        testContainer.push(2);
        testContainer.push(1);

        if (verbose) cout << "\nCheck iterator range has right length" << endl;
        int length = 0;
        itBegin = testContainer.begin();
        while(itBegin != itEnd) {
            ++length;
            ++itBegin;
        }
        LOOP_ASSERT(length, 3 == length);

        if (verbose) cout << "\nCheck iterators refer to right values" << endl;
        itBegin = testContainer.begin();
        ASSERT(1 == *(itBegin++));
        ASSERT(2 == *(itBegin++));
        ASSERT(3 == *(itBegin++));

        if (verbose) cout << "\nRepeat the tests for const_iterators" << endl;
        const TestContainer& constContainer = testContainer;
        const_iterator itcBegin = testContainer.begin();
        const const_iterator itcEnd = testContainer.end();
        length = 0;
        while( itcBegin != itcEnd) {
            ++length;
            ++itcBegin;
        }
        ASSERT(3 == length);

        itcBegin = testContainer.begin();
        ASSERT(1 == *itcBegin++);
        ASSERT(2 == *itcBegin++);
        ASSERT(3 == *itcBegin++);
      } break;
      case 2: {
        // --------------------------------------------------------------------
        // TESTING PRIMARY MANIPULATORS
        //
        // Concerns:
        //
        // Plan:
        //
        // Testing:
        //   bslstl_BidirectionalIterator();
        //   ~bslstl_BidirectionalIterator();
        //   bslstl_BidirectionalIterator& operator++();
        // --------------------------------------------------------------------

        if (verbose) cout << endl << "TESTING PRIMARY MANIPULATORS" << endl
                                  << "============================" << endl;

        if (verbose) cout << "\nTesting default constructor, destructor, "
                             "and 'operator++'." << endl;

        //  Declare test data and types
        int testData[4] = { 0, 1, 2, 3 };
        typedef bslstl_BidirectionalIterator<int, int*> iterator;
        typedef bslstl_BidirectionalIterator<const int, int*> const_iterator;

        //  Confirm that we can default-intialize const iterators.
        //  Confirm that iterator and const_iterator can be compared.
        const iterator itDefault;
        const const_iterator itcDefault;
        ASSERT( itDefault == itcDefault);

        //  Confirm that an iterator over our reference array does not have the
        //  same value as a default constructed iterator.  (Actually, this
        //  comparison is undefined in general.)
        const const_iterator itcOrigin = testData;
        iterator itData = testData;
        ASSERT( itDefault != itData);
        ASSERT( itcOrigin == itData);

        //  Confirm that an incremented iterator does not have the same value
        //  as the intitial iterator
        ++itData;
        ASSERT( itcOrigin != itData);

        //  Confirm that incrementing a second copy of the intitial iterator
        //  has the same value as the first incremented iterator.
        const_iterator itcCopy = itcOrigin;
        ASSERT( itcOrigin == itcCopy);

        ++itcCopy;
        ASSERT( itcOrigin != itcCopy);
        ASSERT( itData == itcCopy);

        const_iterator itReader = testData;
        const int* itValidator = testData;
        for(int i = 0; i < sizeof(testData)/sizeof(int); ++itValidator,
                                                         ++itReader, ++i) {
            LOOP3_ASSERT(i, *itReader, *itValidator,&*itReader == itValidator);
        }
      } break;
      case 1: {
        // --------------------------------------------------------------------
        // BREATHING TEST
        //
        // Concerns:
        //   That basic functionality appears to work as advertised before
        //   before beginning testing in earnest:
        //   - default and copy constructors
        //   - assignment operator
        //   - primary manipulators, basic accessors
        //   - 'operator==', 'operator!='
        //
        // Plan:
        //
        // Testing:
        //   BREATHING TEST
        // --------------------------------------------------------------------

        if (verbose) cout << endl << "BREATHING TEST" << endl
                                  << "==============" << endl;

        int testData[4] = { 0, 1, 2, 3 };
        typedef bslstl_BidirectionalIterator<int, int*> iterator;
        typedef bslstl_BidirectionalIterator<const int, int*> const_iterator;

        if (verbose) cout << "\nConstruct a basic iterator value" << endl;
        iterator it1 = testData;
        LOOP_ASSERT( *it1, 0 == *it1);

        if (verbose) cout << "\nMake a copy of that iterator" << endl;
        iterator it2 = it1;
        LOOP_ASSERT(  *it2,            0 ==  *it2);
        LOOP2_ASSERT(&*it1, &*it2,   it1 ==   it2);
        LOOP2_ASSERT(&*it1, &*it2, &*it1 == &*it2);

        if (verbose) cout << "\nIncrement an iterator" << endl;
        ++it2;
        LOOP_ASSERT(  *it2,             1 == *it2);
        LOOP2_ASSERT(&*it1, &*it2,   it1 !=   it2);
        LOOP2_ASSERT(&*it1, &*it2, &*it1 != &*it2);

        //  Increment the other iterator iterator,
        //  verify both iterators have the same value again
        if (verbose) cout << "\nVerify multipass property of forward iterator"
                          << endl;
        ++it1;
        LOOP_ASSERT(  *it1,            1 ==  *it1);
        LOOP2_ASSERT(&*it1, &*it2,   it1 ==   it2);
        LOOP2_ASSERT(&*it1, &*it2, &*it1 == &*it2);

        if (verbose) cout << "\nConstruct a const_iterator" << endl;
        const_iterator itEnd = testData + 4;

        //  Verify const_iterator and iterator interoperate
        //  Verify the expected number of iterations are required to traverse
        //  the array.
        if (verbose) cout << "\nVerify multipass property of forward iterator"
                          << endl;

        const_iterator itBegin = it1;
        int distance = testDistance( itBegin, itEnd );
        LOOP_ASSERT(distance, 3 == distance);

        if (verbose) cout << "\nVerify writing through a dereferenced iterator"
                          << endl;
        *it1 = 42;
        LOOP_ASSERT( *it1, 42 == *it1);

        if (verbose) cout << "\nVerify that writes through one iterator are"
                          << "  visible through another" << endl;
        LOOP_ASSERT( *it2,         42 == *it2);
        LOOP2_ASSERT(*it1, *it2, *it1 == *it2);

        //  To test operator->, create an array of struct values
        Wrap testWrap[4] = { {0}, {1}, {2}, {3} };
        typedef bslstl_BidirectionalIterator<Wrap, Wrap*> wrap_iterator;
        typedef bslstl_BidirectionalIterator<const Wrap, Wrap*>
                                                           const_wrap_iterator;

        wrap_iterator wit1 = testWrap;
        LOOP_ASSERT(wit1->data, 0 == wit1->data );

        const_wrap_iterator wit2 = wit1;
        LOOP_ASSERT(wit2->data,           0 == wit2->data);
        LOOP2_ASSERT(&*wit1, &*wit2,   wit1 == wit2);
        LOOP2_ASSERT(&*wit1, &*wit2, &*wit1 == &*wit2);

        wit1->data = 13;
        LOOP_ASSERT(wit1->data,          13 == wit1->data );
        LOOP_ASSERT(wit2->data,          13 == wit2->data);
        LOOP2_ASSERT(&*wit1, &*wit2, &*wit1 == &*wit2);

        //  Confirm const_iterator can iterate, just like a non-const_iterator
        ++wit1;
        ++wit2;
        LOOP2_ASSERT(&*wit1, &*wit2, wit1 == wit2);

        // Tests for post-increment
        ASSERT(wit2++ == wit1);
        ASSERT(1 == (*(wit1++)).data);


        //  Test for default constructor, which constructs a singular iterator
        //      Can assign to a sigular value, and destroy it
        //      may not make a copy
        const_wrap_iterator x;

        //  Test assignment safely overwrites the singular value
        x = wit1;
        LOOP2_ASSERT(&*wit1, &*x, wit1 == x);

        //  Not yet validated constness of any APIs, e.g. operator*, operator->
        //  (are constness concerns appropriate for a breathing test, we just
        //   want to confirm that each individual API can be called.)

        // --------------------------------------------------------------------
        // (ORIGINAL) BREATHING/USAGE TEST
        //   We retain the original breathing test supplied by Pablo when the
        //   facility under test was a support class, rather than than a fully
        //   detailed component.  This section of the breathing test can be
        //   retired once the new testing facility has passed review.
        //
        // Concerns:
        //
        // Plan:
        //    Bring an iterator to life
        //    Make a copy, and assert is has the same value
        //    Increment the first iterator, assert no longer has the same value
        //    Increment the second iterator and assert that it again has the
        //    same value as the first
        //
        // Testing:
        //
        // --------------------------------------------------------------------

        if (verbose) cout << endl << "(ORIGINAL BREATHING TEST)" << endl
                                  << "=========================" << endl;

        static const int DATA[] = { 1, 2, 3, 4, 5 };
        static const int DATA_LEN = sizeof(DATA) / sizeof(DATA[0]);

        my_List<int> a; const my_List<int> &A = a;
        ASSERT(a.begin() == a.end());
        ASSERT(A.begin() == A.end());

        typedef my_List<int>::iterator iter_type;
        typedef my_List<int>::const_iterator const_iter_type;

        //  Assert iterator_traits instantiates for these iterators
        //  Assert iterator_traits finds the expected typedefs
        typedef bsl::iterator_traits<iter_type>  IterTraits;
        typedef bsl::iterator_traits<const_iter_type>  ConstIterTraits;
        ASSERT((bslmf_IsSame<IterTraits::difference_type,
                             std::ptrdiff_t>::VALUE));
        ASSERT((bslmf_IsSame<IterTraits::value_type, int>::VALUE));
        ASSERT((bslmf_IsSame<IterTraits::pointer, int *>::VALUE));
        ASSERT((bslmf_IsSame<IterTraits::reference, int &>::VALUE));
        ASSERT((bslmf_IsSame<IterTraits::iterator_category,
                             std::bidirectional_iterator_tag>::VALUE));

        ASSERT((bslmf_IsSame<ConstIterTraits::difference_type,
                             std::ptrdiff_t>::VALUE));
        ASSERT((bslmf_IsSame<ConstIterTraits::value_type, const int>::VALUE));
        ASSERT((bslmf_IsSame<ConstIterTraits::pointer, const int *>::VALUE));
        ASSERT((bslmf_IsSame<ConstIterTraits::reference, const int &>::VALUE));
        ASSERT((bslmf_IsSame<ConstIterTraits::iterator_category,
                             std::bidirectional_iterator_tag>::VALUE));

        if (verbose) cout << "\nPopulate the test list." << std::endl;
        int i;
        for (i = 0; i < DATA_LEN; ++i) {
            if(veryVerbose) { T_ P_(i) P(DATA[i]) }
            a.push(DATA[i]);
        }

        ASSERT(A.begin() == a.begin());
        ASSERT(A.end()   == a.end());

        if (verbose) cout << "\nCompare iterated values to original source."
                          << std::endl;
        i = 4;
        for (my_List<int>::const_iterator it = A.begin(); it != A.end(); ++it,
                                                                          --i){
            // Note that we assume we can derference without changing the value
            // of the iterator.  This is a fair assumption for a breathing test
            // but should be checked strictly in the main driver.
            if(veryVerbose) { T_ P_(i) P_(DATA[i]) P(*it) }
            LOOP3_ASSERT(i, DATA[i], *it, DATA[i] == *it);
        }

        if (verbose) cout << "\nCompare in reversed iteration order."
                          << std::endl;
        for (my_List<int>::const_iterator it = A.end(); it != A.begin();){
            // Note that we will be decremented inside the body of the loop.
            // Note that we assume we can derference without changing the value
            // of the iterator.  This is a fair assumption for a breathing test
            // but should be checked strictly in the main driver.
            --it;
            ++i;
            if(veryVerbose) { T_ P_(i) P_(DATA[i]) P(*it) }
            LOOP3_ASSERT(i, DATA[i], *it, DATA[i] == *it);
        }

        for (my_List<int>::iterator it = a.begin(); it != a.end(); ) {
            //  Each iteration of the loop shall advance 'it' exactly once.

            // Test that initialization from non-const to const iterator works.
            my_List<int>::const_iterator itc = it;
            ASSERT(itc == it);

            // Test assignment from non-const to const iterators
            ++itc;
            ASSERT(itc != it);
            itc = it;
            ASSERT(itc == it);

            // Test post-increment, including return value
            int val = *it;
            *it++ -= 2;
            ASSERT(*itc++ == (val-2));

            //  Test both post-increments were equivalent
            ASSERT(it == itc);
            if (it != a.end()) {
                ASSERT(*itc == *it);
            }

            // This shouldn't compile:
            // it = itc;
            // my_List<int>::iterator it2 = itc;
        }
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
//      Copyright (C) Bloomberg L.P., 2010
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ---------------------------------
