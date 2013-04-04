// bcemt_once.h                                                       -*-C++-*-
#ifndef INCLUDED_BCEMT_ONCE
#define INCLUDED_BCEMT_ONCE

#ifndef INCLUDED_BDES_IDENT
#include <bdes_ident.h>
#endif
BDES_IDENT("$Id: $")

//@PURPOSE: Provide a thread-safe way to execute code once per process.
//
//@CLASSES:
//        bcemt_Once: Gate-keeper for code executed only once per process
//   bcemt_OnceGuard: Guard class for safely using 'bcemt_Once'
//
//@SEE_ALSO: bcemt_qlock
//
//@AUTHOR: Pablo Halpern (phalpern)
//
//@DESCRIPTION: This component provides a pair of classes, 'bcemt_Once' and
// 'bcemt_OnceGuard', which give the caller a way to run a body of code
// exactly once within the current process, particularly in the presence of
// multiple threads.  This component also defines the macro 'BCEMT_ONCE_DO',
// which provides syntactic sugar to make one-time execution nearly fool-proof.
// A common use of one-time execution is the initialization of singletons on
// first use.
//
// The 'bcemt_Once' class is designed to be statically allocated and
// initialized using the 'BCEMT_ONCE_INITIALIZER' macro.  Client code may use
// the 'bcemt_Once' object in one of two ways: 1) it may use the 'callOnce'
// method to call a function or functor or 2) it may call the 'enter' and
// 'leave' methods just before and after the code that is intended to be
// executed only once.  That code must be executed conditionally on 'enter'
// returning 'true', indicating that the caller is the first thread to pass
// through this region of code.  The 'leave' method must be executed at the
// end of the code region, indicating that the one-time execution has completed
// and unblocking any threads waiting on 'enter'.
//
// A safer way to use the 'enter' and 'leave' methods of 'bcemt_Once' is to
// manage the 'bcemt_Once' object using a 'bcemt_OnceGuard' object constructed
// from the 'bcemt_Once' object.  Calling 'enter' on the 'bcemt_OnceGuard'
// object will call 'enter' on its associated 'bcemt_Once' object.
// If the call to 'enter' returns 'true', then the destructor for the guard
// will automatically call 'leave' on its associated 'bcemt_Once' object.
// The 'bcemt_OnceGuard' class is intended to be allocated on the stack (i.e.,
// as a local variable) so that it is automatically destroyed at the end of its
// enclosing block.  Thus, the to call 'leave' of the 'bcemt_Once' object is
// enforced by the compiler.
//
// An even easier way to use the facilities of this component is to use the
// 'BCEMT_ONCE_DO' macro.  This macro behaves like an 'if' statement --
// executing the following [compound] statement the first time the control
// passes through it in the course of a program's execution, and blocking
// other calling threads until the [compound] statement is executed the first
// time.  Thus, bracketing arbitrary code in a 'BCEMT_ONCE_DO' construct is the
// easiest way to ensure that code will be executed only once for a program.
// The 'BCEMT_ONCE_DO' behaves correctly even if there are 'return' statements
// within the one-time code block.
//
// The implementation of this component uses appropriate memory barriers so
// that changes made in the one-time execution code are immediately visible to
// all threads at the end of the one-time code block.
//
///Warning
///-------
// The 'BCEMT_ONCE_DO' macro consists of a declaration and a 'for' loop.
// Consequently, the following is syntactically incorrect:
//..
//  if (xyz) BCEMT_ONCE_DO { stuff() }
//..
// Also, a 'break' or 'continue' statement within a 'BCEMT_ONCE_DO' construct
// terminates the 'BCEMT_ONCE_DO', not a surrounding loop or 'switch'
// statement.  For example:
//..
//  switch (xyz) {
//    case 0: BCEMT_ONCE_DO { stuff(); break; /* does not break case */ }
//    case 1: // Oops! case 0 falls through to here.
//  }
//..
//
///Thread-Safety
///-------------
// Objects of the 'bcemt_Once' class are intended to be shared among threads
// and may be accessed and modified simultaneously in multiple threads by using
// the methods provided.  To allow static initialization, 'bcemt_Once' is a POD
// type with public member variables.  It is not safe to directly access or
// manipulate it's member variables (including object initialization)
// simultaneously from multiple threads.  (Note that static initialization
// takes place before multiple threading begins, and is thus safe.)
//
// The 'bcemt_OnceGuard' objects are designed to be used only by their creator
// threads and are typically created on the stack.  It is not safe to use a
// 'bcemt_OnceGuard' by a thread other than its creator.
//
///Usage
///-----
// Typically, the facilities in this component are used to implement a
// thread-safe singleton.  Below, we implement the a singleton four ways,
// illustrating the two ways to directly use 'bcemt_Once', the use of
// 'bcemt_OnceGuard', and the use of 'BCEMT_ONCE_DO'.  In each example, the
// singleton functions take a C-string ('const char*') argument and return a
// reference to a 'bsl::string' object constructed from the input string.  Only
// the first call to each singleton function affect the contents of the
// singleton string.  (The argument is ignored on subsequent calls.)
//
///First Implementation
/// - - - - - - - - - -
// Our first implementation uses the 'BCEMT_ONCE_DO' construct, the
// recommended way to use this component.  The function is a variation of the
// singleton pattern described by Scott Meyers, except that the 'BCEMT_ONCE_DO'
// macro is used to handle multiple entries to the function in a thread-safe
// manner:
//..
//  #include <bcemt_once.h>
//
//  const bsl::string& singleton0(const char *s)
//  {
//      static bsl::string *theSingletonPtr = 0;
//      BCEMT_ONCE_DO {
//          static bsl::string theSingleton(s);
//          theSingletonPtr = &theSingleton;
//      }
//      return *theSingletonPtr;
//  }
//..
// The 'BCEMT_ONCE_DO' mechanism suffices for most situations; however,
// if more flexibility is required, review the remaining examples in this
// series for more design choices.  The next example will use the lowest-level
// facilities of 'bcemt_Once'.  The two following examples use progressively
// higher-level facilities to produce simpler singleton implementations (though
// none as simple as the 'BCEMT_ONCE_DO' example above).
//
///Second Implementation
///- - - - - - - - - - -
// The next singleton function implementation directly uses the 'doOnce' method
// of 'bcemt_Once'.  We begin by declaring a simple function that does most of
// the work of the singleton, i.e., constructing the string and setting a
// (static) pointer to the string:
//..
//  static bsl::string *theSingletonPtr = 0;
//
//  void singletonImp(const char *s)
//  {
//      static bsl::string theSingleton(s);
//      theSingletonPtr = &theSingleton;
//  }
//..
// The above function is *not* thread-safe.  Firstly, many threads might
// attempt to simultaneously construct the 'theSingleton' object.  Secondly,
// once 'theSingletonPtr' is set by one thread, other threads still might not
// see the change (and try to initialize the singleton again).
//
// The 'singleton1' function, below, calls 'singletonImp' via the 'callOnce'
// method of 'bcemt_Once' to ensure that 'singletonImp' is called by only one
// thread and that the result is visible to all threads.  We start by creating
// and initializing a static object of type 'bcemt_Once':
//..
//  #include <bdef_bind.h>
//
//  const bsl::string& singleton1(const char *s)
//  {
//      static bcemt_Once once = BCEMT_ONCE_INITIALIZER;
//..
// Since the 'callOnce' method takes only a no-argument functor (or function),
// to call 'callOnce', we must bind our argument 's' to our function,
// 'singletonImp' using a binder method and then pass that functor to
// 'callOnce'.  The first thread (and only the first thread) entering this
// section of code we will set 'theSingleton'.
//..
//      once.callOnce(bdef_BindUtil::bind(singletonImp, s));
//      return *theSingletonPtr;
//  }
//..
// Once we return from 'callOnce', the appropriate memory barrier has been
// executed so that the change to 'theSingletonPtr' is visible to all threads.
// A thread calling 'callOnce' after the initialization has completed would
// immediately return from the call.  A thread calling 'callOnce' while
// initialization is still in progress would block until initialization
// completes and then return.
//
// *Implementation* *Note*: As an optimization, developers sometimes pre-check
// the value to be set, 'theSingletonPtr' in this case, to avoid (heavy)
// memory barrier operations; however, that practice is not recommended here.
// First, the value of the string may be cached by a different CPU, even
// though the pointer has already been updated on the common memory bus.
// Second,  The implementation of the 'callOnce' method is fast enough that a
// pre-check would not provide any performance benefit.
//
// The one advantage of this implementation over the previous one is that an
// exception thrown from within 'singletonImp' will cause the 'bcemt_Once'
// object to be restored to its original state, so that the next entry into
// the singleton will retry the operation.
//
///Third Implementation
/// - - - - - - - - - -
// Our next implementation, 'singleton2', eliminates the need for the
// 'singletonImp' function and thereby does away with the use of the
// 'bdef_BindUtil' method; however, it does require use of
// 'bcemt_Once::OnceLock', created on each thread's stack and passed to the
// methods of 'bcemt_Once'.  First, we declare a static 'bcemt_Once' object as
// before, and also declare a static pointer to 'bsl::string':
//..
//  const bsl::string& singleton2(const char *s)
//  {
//      static bcemt_Once   once            = BCEMT_ONCE_INITIALIZER;
//      static bsl::string *theSingletonPtr = 0;
//..
// Next, we define a local 'bcemt_Once::OnceLock' object and pass it to
// the 'enter' method:
//..
//      bcemt_Once::OnceLock onceLock;
//      if (once.enter(&onceLock)) {
//..
// If the 'enter' method returns 'true', we proceed with the initialization of
// the singleton, as before.
//..
//          static bsl::string theSingleton(s);
//          theSingletonPtr = &theSingleton;
//..
// When initialization is complete, the 'leave' method is called for the same
// context cookie previously used in the call to 'enter':
//..
//          once.leave(&onceLock);
//      }
//..
// When any thread reaches this point, initialization has been complete
// and initialized string is returned:
//..
//      return *theSingletonPtr;
//  }
//..
//
///Fourth Implementation
///- - - - - - - - - - -
// Our final implementation, 'singleton3', uses 'bcemt_OnceGuard' to simplify
// the previous implementation by using 'bcemt_OnceGuard' to hide (automate)
// the use of 'bcemt_Once::OnceLock'.  We begin as before, defining a static
// 'bcemt_Once' object and a static 'bsl::string' pointer:
//..
//  const bsl::string& singleton3(const char *s)
//  {
//      static bcemt_Once   once            = BCEMT_ONCE_INITIALIZER;
//      static bsl::string *theSingletonPtr = 0;
//..
// We then declare a local 'bcemt_OnceGuard' object and associate it with the
// 'bcemt_Once' object before entering the one-time initialization region:
//..
//      bcemt_OnceGuard onceGuard(&once);
//      if (onceGuard.enter()) {
//          static bsl::string theSingleton(s);
//          theSingletonPtr = &theSingleton;
//      }
//      return *theSingletonPtr;
//  }
//..
// Note that it is unnecessary to call 'onceGuard.leave()' because that is
// called automatically before the function returns.  This machinery makes the
// code more robust in the presence of, e.g., return statements in the
// initialization code.
//
// If there is significant code after the end of the one-time initialization,
// the guard and the initialization code should be enclosed in an extra block
// so that the guard is destroyed as soon as validly possible and allow other
// threads waiting on the initialization to continue.  Alternatively, one can
// call 'onceGuard.leave()' explicitly at the end of the initialization.
//
///Using the Semaphore Implementations
///- - - - - - - - - - - - - - - - - -
// The following pair of functions, 'thread1func' and 'thread2func' which will
// be run by different threads:
//..
//  void *thread1func(void *)
//  {
//      const bsl::string& s0 = singleton0("0 hello");
//      const bsl::string& s1 = singleton1("1 hello");
//      const bsl::string& s2 = singleton2("2 hello");
//      const bsl::string& s3 = singleton3("3 hello");
//
//      assert('0' == s0[0]);
//      assert('1' == s1[0]);
//      assert('2' == s2[0]);
//      assert('3' == s3[0]);
//
//      // ... lots more code goes here
//      return 0;
//  }
//
//  void *thread2func(void *)
//  {
//      const bsl::string& s0 = singleton0("0 goodbye");
//      const bsl::string& s1 = singleton1("1 goodbye");
//      const bsl::string& s2 = singleton2("2 goodbye");
//      const bsl::string& s3 = singleton3("3 goodbye");
//
//      assert('0' == s0[0]);
//      assert('1' == s1[0]);
//      assert('2' == s2[0]);
//      assert('3' == s3[0]);
//
//      // ... lots more code goes here
//      return 0;
//  }
//..
// Both threads attempt to initialize the four singletons.  In our example,
// each thread passes a distinct argument to the singleton, allowing us to
// identify the thread that initializes the singleton.  (In practice, the
// arguments passed to a specific singleton are almost always fixed and most
// singletons don't take arguments at all.)
//
// Assuming that the first thread function wins all of the races to initialize
// the singletons, the first singleton is set to "0 hello", the second
// singleton to "1 hello", etc.
//..
//  int main()
//  {
//      void startThread1();
//      void startThread2();
//
//      startThread1();
//      startThread2();
//
//      assert(singleton0("0") == "0 hello");
//      assert(singleton1("1") == "1 hello");
//      assert(singleton2("2") == "2 hello");
//      assert(singleton3("3") == "3 hello");
//
//      return 0;
//  }
//..

#ifndef INCLUDED_BCESCM_VERSION
#include <bcescm_version.h>
#endif

#ifndef INCLUDED_BCEMT_QLOCK
#include <bcemt_qlock.h>
#endif

#ifndef INCLUDED_BCES_ATOMICUTIL
#include <bces_atomicutil.h>
#endif

#ifndef INCLUDED_BSLS_ASSERT
#include <bsls_assert.h>
#endif

namespace BloombergLP {

#if defined(BSLS_PLATFORM_CMP_MSVC)
#   define BCEMT_ONCE_UNIQNUM __COUNTER__
        // MSVC: The '__LINE__' macro breaks when '/ZI' is used (see Q199057 or
        // KB199057).  Fortunately the '__COUNTER__' extension provided by MSVC
        // is even better.
#else
#   define BCEMT_ONCE_UNIQNUM __LINE__
#endif

#define BCEMT_ONCE_DO \
    BCEMT_ONCE_DO_IMP(BCEMT_ONCE_CAT(bcemt_doOnceObj, BCEMT_ONCE_UNIQNUM))
    // This macro provides a simple control construct to bracket a piece of
    // code that should only be executed once during the course of a
    // multithreaded program.  Usage:
    //..
    //  BCEMT_ONCE_DO { /* one-time code goes here */ }
    //..
    // Leaving a 'BCEMT_ONCE_DO' construct via 'break', 'continue', or
    // 'return' will put the construct in a "done" state (unless
    // 'BCEMT_ONCE_CANCEL' has been called) and will unblock all threads
    // waiting to enter the one-time region.  Note that a 'break' or 'continue'
    // within the one-time code will terminate only the 'BCEMT_ONCE_DO'
    // construct, not any surrounding loop or switch statement.  Due to a bug
    // in the Microsoft Visual C++ 2003 compiler, the behavior is undefined if
    // an exception is thrown from within this construct and is not caught
    // within the same construct.  Only one call to 'BCEMT_ONCE_DO' may appear
    // on a single source-code line in any code block.

#define BCEMT_ONCE_CANCEL() bcemt_doOnceGuard.cancel()
    // This macro provides a way to cancel once processing within a
    // 'BCEMT_ONCE_DO' construct.  It will not compile outside of a
    // 'BCEMT_ONCE_DO' construct.  Executing this function-like macro will set
    // the state of the 'BCEMT_ONCE_DO' construct to "not entered", possibly
    // unblocking a thread waiting to enter the one-time code region.  Note
    // that this macro does not exit the 'BCEMT_ONCE_DO' construct (i.e.,
    // it does not have 'break' or 'return' semantics).

#define BCEMT_ONCE_INITIALIZER { BCEMT_QLOCK_INITIALIZER, { 0 } }
    // Use this macro to initialize an object of type 'bcemt_Once'.  E.g.:
    //..
    //  bcemt_Once once = BCEMT_ONCE_INITIALIZER;
    //..

                        // ================
                        // class bcemt_Once
                        // ================

class bcemt_Once {
    // Gate-keeper class for code that should only execute once per process.
    // This class is a POD-type and can be statically initialized to the value
    // of the 'BCEMT_ONCE_INITIALIZE' macro.  For this reason, it does not
    // have any explicitly-declared constructors or destructor.

    // PRIVATE TYPES
    enum { BCEMT_NOT_ENTERED, BCEMT_IN_PROGRESS, BCEMT_DONE };

  private:
    // NOT IMPLEMENTED
    bcemt_Once& operator=(const bcemt_Once&);
        // Copy-assignment is not allowed.  We cannot declare a private copy
        // constructor because that would make this class a non-POD.

  public:
    // These variables are public so that this (POD) type can be statically
    // initialized.  Do not access these variables directly.

    // DATA
    bcemt_QLock          d_mutex; // public, but do *not* access directly
    bces_AtomicUtil::Int d_state; // public, but do *not* access directly

  public:
    // PUBLIC TYPES
    typedef bcemt_QLockGuard OnceLock;
        // Special token created by a single thread to pass to the 'enter',
        // 'leave', and 'cancel' methods.

    // MANIPULATORS
    bool enter(OnceLock *onceLock);
        // Lock the internal mutex using the specified 'onceLock' (possibly
        // blocking if another thread has already locked the mutex).  If no
        // other thread has yet called 'enter' or 'callOnce' on this object,
        // return 'true'.  Otherwise, unlock the mutex and return 'false'.
        // The mutex lock may be skipped if it can be determined that it will
        // not be needed.  The behavior is undefined if 'onceLock' is already
        // in a locked state on entry to this method.  Note that if 'enter'
        // returns 'true', the caller *must* eventually call 'leave', or else
        // other threads may block indefinitely.

    void leave(OnceLock *onceLock);
        // Set this object into a state such that pending and future calls to
        // 'enter' or 'callOnce' will return 'false' or do nothing,
        // respectively, then unlock the internal mutex using the specified
        // 'onceLock' (possibly unblocking pending calls to 'enter' or
        // 'callOnce').  The behavior is undefined unless 'onceLock' was locked
        // by a matching call to 'enter' on this object and has not been
        // tampered-with since.

    void cancel(OnceLock *onceLock);
        // Revert this object to the state it was in before 'enter' or
        // 'callOnce' was called, then unlock the internal mutex using the
        // specified 'onceLock' (possibly unblocking pending calls to 'enter'
        // or 'callOnce').  This method may only be used to cancel execution of
        // one-time code that has not yet completed.  The behavior is undefined
        // unless 'onceLock' was locked by a matching call to 'enter' on this
        // object and has not been tampered-with since (especially by calling
        // 'leave').

    template <typename FUNC>
    void callOnce(FUNC& function);
    template <typename FUNC>
    void callOnce(const FUNC& function);
        // If no other thread has yet called 'enter' or 'callOnce', then call
        // the specified 'function' and set this object to the state where
        // pending and future calls to 'enter' or 'callOnce' will return
        // 'false' or do nothing, respectively.  Otherwise, wait for the
        // one-time code to complete and return without calling 'function'
        // where 'function' is a function or functor that can be called with no
        // arguments.  Note that one-time code is considered not to have run if
        // 'function' terminates with an exception.
};

                        // =====================
                        // class bcemt_OnceGuard
                        // =====================

class bcemt_OnceGuard {
    // Guard class for using 'bcemt_Once' safely.  Construct an object of this
    // class before conditionally entering one-time processing code.  Destroy
    // the object when the one-time code is complete.  When used this way, this
    // object will be in an "in-progress" state during the time that the
    // one-time code is being executed.

    // PRIVATE TYPES
    enum State { BCEMT_NOT_ENTERED, BCEMT_IN_PROGRESS, BCEMT_DONE };

    // DATA
    bcemt_Once::OnceLock  d_onceLock;
    bcemt_Once           *d_once;
    State                 d_state;

    // NOT IMPLEMENTED
    bcemt_OnceGuard(const bcemt_OnceGuard&);
    bcemt_OnceGuard& operator=(const bcemt_OnceGuard&);

  public:
    // CREATORS
    explicit bcemt_OnceGuard(bcemt_Once *once = 0);
        // Initialize this object to guard the (optionally) specified 'once'
        // object.  If 'once' is not specified, then it must be set later using
        // the 'setOnce' method before other methods may be called.

    ~bcemt_OnceGuard();
        // Destroy this object.  If this object is not in an "in-progress"
        // state, do nothing.  If this object is in an "in-progress" state and
        // is being destroyed in the course of normal processing, then call
        // 'leave' on the associated 'bcemt_Once' object.  Due to a bug in the
        // MS VC++ 2003 compiler, the behavior is undefined if this destructor
        // is called in the course of stack-unwinding during exception
        // processing (i.e., if an exception escapes from the one-time code
        // region.  [Eventually, we hope to call 'cancel' if this destructor
        // is called during exception-processing.]

    // MANIPULATORS
    void setOnce(bcemt_Once *once);
        // Set this object to guard the specified 'once' object.  The behavior
        // is undefined if this object is currently in the "in-progress" state.

    bool enter();
        // Call 'enter' on the associated 'bcemt_Once' object and return the
        // result.  If 'bcemt_Once::enter' returns 'true', set this object into
        // the "in-progress" state.  The behavior is undefined unless this
        // object has been associated with a 'bcemt_Once' object, either in the
        // constructor or using 'setOnce', or if this object is already in the
        // "in-progress" state.

    void leave();
        // If this object is in the "in-progress" state, call 'leave' on the
        // associated 'bcemt_Once' object and exit the "in-progress" state.
        // Otherwise, do nothing.

    void cancel();
        // If this object is in the "in-progress" state, call 'cancel' on the
        // associated 'bcemt_Once' object and exit the "in-progress" state.
        // Otherwise, do nothing.

    // ACCESSORS
    bool isInProgress() const;
        // Return 'true' if this object is in the "in-progress" state.  The
        // object is in-progress if 'enter' has been called and returned 'true'
        // and neither 'leave' nor 'cancel' have been called.  The one-time
        // code controlled by this object should only be executing if this
        // object is in the "in-progress" state.
};

// ===========================================================================
//                      INLINE FUNCTION DEFINITIONS
// ===========================================================================

                        // ----------------------------------
                        // Token concatenation support macros
                        // ----------------------------------

// Second layer needed to ensure that arguments are expanded before
// concatenation.
#define BCEMT_ONCE_CAT(X, Y) BCEMT_ONCE_CAT_IMP(X, Y)
#define BCEMT_ONCE_CAT_IMP(X, Y) X##Y

                        // -------------------------------------
                        // Implementation of BCEMT_ONCE_DO Macro
                        // -------------------------------------

// Use a for-loop to initialize the guard, test if we can enter the
// once-region, then leave the once-region at the end.  Each invocation of this
// macro within a source file supplies a different 'doOnceObj' name.
#define BCEMT_ONCE_DO_IMP(doOnceObj)                                         \
    static BloombergLP::bcemt_Once doOnceObj = BCEMT_ONCE_INITIALIZER;       \
    for (BloombergLP::bcemt_OnceGuard bcemt_doOnceGuard(&doOnceObj);         \
         bcemt_doOnceGuard.enter(); bcemt_doOnceGuard.leave())

                        // ---------------------
                        // class bcemt_OnceGuard
                        // ---------------------

// CREATORS
inline
bcemt_OnceGuard::bcemt_OnceGuard(bcemt_Once *once)
: d_once(once)
, d_state(BCEMT_NOT_ENTERED)
{
}

// MANIPULATORS
inline
void bcemt_OnceGuard::setOnce(bcemt_Once *once)
{
    BSLS_ASSERT_SAFE(BCEMT_IN_PROGRESS != d_state);

    d_once = once;
    d_state = BCEMT_NOT_ENTERED;
}

// ACCESSORS
inline
bool bcemt_OnceGuard::isInProgress() const
{
    return BCEMT_IN_PROGRESS == d_state;
}

                        // ----------------
                        // class bcemt_Once
                        // ----------------

template <typename FUNC>
inline
void bcemt_Once::callOnce(FUNC& function)
{
    bcemt_OnceGuard guard(this);
    if (guard.enter()) {
#ifdef BDE_BUILD_TARGET_EXC
        try {
            function();
        }
        catch (...) {
            guard.cancel();
            throw;
        }
#else
        function();
#endif
    }
}

template <typename FUNC>
inline
void bcemt_Once::callOnce(const FUNC& function)
{
    bcemt_OnceGuard guard(this);
    if (guard.enter()) {
#ifdef BDE_BUILD_TARGET_EXC
        try {
            function();
        }
        catch (...) {
            guard.cancel();
            throw;
        }
#else
        function();
#endif
    }
}

}  // close namespace BloombergLP

#if !defined(BSL_DOUBLE_UNDERSCORE_XLAT) || 1 == BSL_DOUBLE_UNDERSCORE_XLAT
#define BCEMT_ONCE__CAT(X, Y) BCEMT_ONCE_CAT(X, Y)
#endif

#endif

// ---------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2007
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ---------------------------------
