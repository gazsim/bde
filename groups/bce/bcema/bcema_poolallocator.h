// bcema_poolallocator.h                                              -*-C++-*-
#ifndef INCLUDED_BCEMA_POOLALLOCATOR
#define INCLUDED_BCEMA_POOLALLOCATOR

#ifndef INCLUDED_BDES_IDENT
#include <bdes_ident.h>
#endif
BDES_IDENT("$Id: $")

//@PURPOSE: Provide thread-safe memory-pooling allocator of fixed-size blocks.
//
//@CLASSES:
//   bcema_PoolAllocator: thread-safe allocator of pooled, fixed-size blocks
//
//@AUTHOR: Ilougino Rocha (irocha)
//
//@DESCRIPTION: This component defines a class, 'bcema_PoolAllocator',
// that implements the 'bslma_Allocator' protocol and provides a thread-safe
// allocator of pooled memory blocks of uniform size (the "pooled size").  The
// pooled size is either (1) configured at construction, or (2) equal to the
// size of the first block allocated through the allocator.  All of the
// allocation requests of sizes up to the pooled size are satisfied with blocks
// from the underlying pool.  All requests of sizes larger than the pooled size
// will be satisfied through the external allocator (the allocator supplied at
// construction, or the default allocator if no allocator was provided).
//
///Protocol Hierarchy
///------------------
// The interface hierarchy (defined by direct public inheritance) of
// 'bcema_PoolAllocator' is as follows:
//..
//   ,-------------------.
//  ( bcema_PoolAllocator )
//   `-------------------'
//             |       ctor/dtor
//             |       objectSize
//             V
//     ,---------------.
//    ( bslma_Allocator )
//     `---------------'
//                     allocate
//                     deallocate
//..
// 'bcema_PoolAllocator' provides a concrete, thread-safe implementation of the
// 'bslma_Allocator' protocol.
//
///Usage
///-----
// The 'bcema_PoolAllocator' is intended to be used in either of the following
// two cases.
//
// The first case is where frequent allocation and deallocation of memory
// occurs through the 'bslma_Allocator' protocol and all of the allocated
// blocks have the same size.  In this case, the size of blocks to pool is
// determined the first time 'allocate' is called and need not be specified at
// construction.
//
// The second case is where frequent allocation and deallocation of memory
// occurs through the 'bslma_Allocator' protocol, most of the allocations have
// similar sizes, and a likely maximum for the largest allocation is known at
// the time of construction.
//
///Example 1 - Uniform Sized Allocations
///- - - - - - - - - - - - - - - - - - -
// The following example illustrates the use of 'bcema_PoolAllocator' when all
// allocations are of uniform size.  A 'bcema_PoolAllocator' is used in the
// implementation of a "work queue" where each "item" enqueued by a
// producer thread is of identical size.  Concurrently, a consumer dequeues
// each work item when it becomes available, verifies the content (a sequence
// number in ASCII), and deallocates the work item.  The concurrent allocations
// and deallocations are valid because 'bcema_PoolAllocator' is thread-safe.
//
// First, an abstract of the example will be given with focus and commentary on
// the relevant details of 'bcema_PoolAllocator'.  Details pertaining to queue
// management, thread creation, thread synchronization, etc., can be seen in
// the full listing at the end of this example.
//
// The parent thread creates the 'bcema_PoolAllocator' and work queue by the
// statements:
//..
//  bcema_PoolAllocator poolAlloc;
//  my1_WorkQueue queue(&poolAlloc);
//..
// Note that since the default constructor is used to create 'poolAlloc', the
// pooled size has not yet been fixed.
//
// The work queue is defined by the following data structures.
//..
//  struct my1_WorkItem {
//      // DATA
//      char *d_item;  // represents work to perform
//  };
//
//  struct my1_WorkQueue {
//      // DATA
//      bsl::list<my1_WorkItem>  d_queue;    // queue of work requests
//      bcemt_Mutex              d_mx;       // protects the shared queue
//      bcemt_Condition          d_cv;       // signals existence of new work
//      bslma_Allocator         *d_alloc_p;  // pooled allocator
//
//      // CREATORS
//      explicit my1_WorkQueue(bslma_Allocator *basicAllocator = 0)
//      : d_alloc_p(bslma_Default::allocator(basicAllocator))
//      {
//      }
//  };
//..
// The producer and consumer threads are given the address of the work queue as
// their sole argument.  Here, the producer allocates a work item, initializes
// it with a sequence number in ASCII, enqueues it, and signals its presence to
// the consumer thread.  This action is done 50 times, and then a 51st, empty
// work item is added to inform the consumer of the end of the queue.  The
// first allocation of a work item (100 bytes) fixes the pooled size.  Each
// subsequent allocation is that same size (100 bytes).  The producer's actions
// are shown below:
//..
//  extern "C"
//  void *my1_producer(void *arg)
//  {
//      my1_WorkQueue *queue = (my1_WorkQueue *)arg;
//
//      for (int i = 0; i < 50; ++i) {
//          char b[100];
//          bsl::sprintf(b, "%d", i);
//          int len = bsl::strlen(b);
//
//          my1_WorkItem request;
//
//          // Fixed allocation size sufficient for content.
//
//          request.d_item = (char *)queue->d_alloc_p->allocate(100);
//
//          bsl::memcpy(request.d_item, b, len+1);
//
//          // Enqueue item and signal any waiting threads.
//          // ...
//      }
//
//      // Add empty item.
//      // ...
//
//      return queue;
//  }
//..
// When the consumer thread finds that the queue is not empty it dequeues the
// item, verifies its content (a sequence number in ASCII), returns the work
// item to the pool, and checks for the next item.  If the queue is empty, the
// consumer blocks until signaled by the producer.  An empty work item
// indicates that the producer will send no more items, so the consumer exits.
// The consumer's actions are shown below:
//..
//  extern "C"
//  void *my1_consumer(void *arg)
//  {
//      my1_WorkQueue *queue = (my1_WorkQueue *)arg;
//
//      for (int i = 0; ; ++i) {
//
//          // Block until work item on queue.
//          // ...
//
//          // Dequeue item.
//          // ...
//
//          // Break when end-of-work item received.
//          // ...
//
//          char b[100];
//          bsl::sprintf(b, "%d", i);
//          assert(bsl::strcmp(b, item.d_item) == 0);   // check content
//
//          queue->d_alloc_p->deallocate(item.d_item);  // deallocate
//      }
//
//      return 0;
//  }
//..
// A complete listing of the example's structures and functions follows:
//..
//  struct my1_WorkItem {
//      // DATA
//      char *d_item;  // represents work to perform
//  };
//
//  struct my1_WorkQueue {
//      // DATA
//      bsl::list<my1_WorkItem>  d_queue;    // queue of work requests
//      bcemt_Mutex              d_mx;       // protects the shared queue
//      bcemt_Condition          d_cv;       // signals existence of new work
//      bslma_Allocator         *d_alloc_p;  // pooled allocator
//
//      // CREATORS
//      explicit my1_WorkQueue(bslma_Allocator *basicAllocator = 0)
//      : d_alloc_p(bslma_Default::allocator(basicAllocator))
//      {
//      }
//  };
//
//  extern "C" void *my1_producer(void *arg)
//  {
//      my1_WorkQueue *queue = (my1_WorkQueue *)arg;
//
//      for (int i = 0; i < 50; ++i) {
//
//          char b[100];
//          bsl::sprintf(b, "%d", i);
//          int len = bsl::strlen(b);
//
//          my1_WorkItem request;
//          request.d_item = (char *)queue->d_alloc_p->allocate(100);
//          bsl::memcpy(request.d_item, b, len+1);
//
//          if (veryVerbose) {
//              // Assume thread-safe implementations of 'cout' and 'endl'
//              // exist (named 'MTCOUT' and 'MTENDL', respectively).
//
//              MTCOUT << "Enqueuing " << request.d_item << MTENDL;
//          }
//
//          queue->d_mx.lock();
//          queue->d_queue.push_back(request);
//          queue->d_mx.unlock();
//          queue->d_cv.signal();
//      }
//
//      my1_WorkItem request;
//      request.d_item = 0;
//
//      queue->d_mx.lock();
//      queue->d_queue.push_back(request);
//      queue->d_mx.unlock();
//      queue->d_cv.signal();
//
//      return queue;
//  }
//
//  extern "C" void *my1_consumer(void *arg)
//  {
//      my1_WorkQueue *queue = (my1_WorkQueue *)arg;
//
//      for (int i = 0; ; ++i) {
//
//          queue->d_mx.lock();
//          while (0 == queue->d_queue.size()) {
//              queue->d_cv.wait(&queue->d_mx);
//          }
//
//          my1_WorkItem item = queue->d_queue.front();
//          queue->d_queue.pop_front();
//          queue->d_mx.unlock();
//
//          if (0 == item.d_item) {
//              break;
//          }
//
//          // Process the work requests.
//          if (veryVerbose) {
//              // Assume thread-safe implementations of 'cout' and 'endl'
//              // exist (named 'MTCOUT' and 'MTENDL', respectively).
//
//              MTCOUT << "Processing " << item.d_item << MTENDL;
//          }
//
//          char b[100];
//          bsl::sprintf(b, "%d", i);
//          assert(bsl::strcmp(b, item.d_item) == 0);
//
//          queue->d_alloc_p->deallocate(item.d_item);
//      }
//
//      return 0;
//  }
//
//  int main()
//  {
//      bcema_PoolAllocator poolAlloc;
//      my1_WorkQueue queue(&poolAlloc);
//
//      bcemt_Attribute attributes;
//
//      bcemt_ThreadUtil::Handle producerHandle;
//      int status = bcemt_ThreadUtil::create(&producerHandle,
//                                            attributes,
//                                            &my1_producer,
//                                            &queue);
//      assert(0 == status);
//
//      bcemt_ThreadUtil::Handle consumerHandle;
//      status = bcemt_ThreadUtil::create(&consumerHandle,
//                                        attributes,
//                                        &my1_consumer,
//                                        &queue);
//      assert(0 == status);
//      status = bcemt_ThreadUtil::join(consumerHandle);
//      assert(0 == status);
//      status = bcemt_ThreadUtil::join(producerHandle);
//      assert(0 == status);
//  }
//..
//
///Example 2 - Variable Allocation Size
/// - - - - - - - - - - - - - - - - - -
// The following example illustrates the use of 'bcema_PoolAllocator' when
// allocations are of varying size.  A 'bcema_PoolAllocator' is used in the
// implementation of a "work queue" where each "item" enqueued by a producer
// thread varies in size, but all items are smaller than a known maximum.
// Concurrently, a consumer thread dequeues each work item when it is
// available, verifies its content (a sequence number in ASCII), and
// deallocates the work item.  The concurrent allocations and deallocations are
// valid because 'bcema_PoolAllocator' is thread-safe.
//
// First, an abstract of the example will be given with focus and commentary on
// the relevant details of 'bcema_PoolAllocator'.  Details pertaining to queue
// management, thread creation, thread synchronization, etc., can be seen in
// the full listing at the end of this example.
//
// The parent thread creates the 'bcema_PoolAllocator' and work queue by the
// statements:
//..
//  bcema_PoolAllocator poolAlloc(100);
//  my1_WorkQueue queue(&poolAlloc);
//..
// Note that the pooled size (100) is specified in the construction of
// 'poolAlloc'.  Any requests in excess of that size will be satisfied by
// implicit calls to the default allocator, not from the underlying pool.
//
// The work queue is defined by the following data structures.
//..
//  struct my2_WorkItem {
//      // DATA
//      char *d_item;  // represents work to perform
//  };
//
//  struct my2_WorkQueue {
//      // DATA
//      bsl::list<my2_WorkItem>  d_queue;    // queue of work requests
//      bcemt_Mutex              d_mx;       // protects the shared queue
//      bcemt_Condition          d_cv;       // signals existence of new work
//      bslma_Allocator         *d_alloc_p;  // pooled allocator
//
//      // CREATORS
//      explicit my2_WorkQueue(bslma_Allocator *basicAllocator = 0)
//      : d_queue(basic_Allocator)
//      , d_alloc_p(bslma_Default::allocator(basic_Allocator))
//      {
//      }
//  };
//..
// In this example (unlike Example 1), the given allocator is used not only for
// the work items, but is also passed to the constructor of 'd_queue' so that
// it also serves memory for the operations of 'bsl::list<my2_WorkItem>'.
//
// The producer and consumer threads are given the address of the work queue as
// their sole argument.  Here, the producer allocates a work item, initializes
// it with a sequence number in ASCII, enqueues it, and signals its presence to
// the consumer thread.  The action is done 50 times, and then a 51st, empty
// work item is added to inform the consumer of the end of the queue.  In this
// example, each work item is sized to match the length of its contents, the
// sequence number in ASCII.  The producer's actions are shown below:
//..
//  extern "C" void *my2_producer(void *arg)
//  {
//      my2_WorkQueue *queue = (my2_WorkQueue *)arg;
//
//      for (int i = 0; i < 50; ++i) {
//
//          char b[100];
//          bsl::sprintf(b, "%d", i);
//          int len = bsl::strlen(b);
//
//          my2_WorkItem request;
//
//          // Allocate item to exactly match space needed for content.
//
//          request.d_item = (char *)queue->d_alloc_p->allocate(len+1);
//
//          bsl::memcpy(request.d_item, b, len+1);
//
//          // Enqueue item and signal any waiting threads.
//          // ...
//      }
//
//      // Add empty item.
//      // ...
//
//      return queue;
//  }
//..
// The actions of this consumer thread are essentially the same as those of the
// consumer thread in Example 1.
//
// When the consumer thread finds that the queue is not empty, it dequeues the
// item, verifies its content (a sequence number in ASCII), returns the
// work item to the pool, and checks for the next item.  If the queue is empty,
// the consumer blocks until signaled by the producer.  An empty work item
// indicates that the producer will send no more items, so the consumer exits.
// The consumer's actions are shown below.
//..
//  extern "C" void *my2_consumer(void *arg)
//  {
//      my2_WorkQueue *queue = (my2_WorkQueue *)arg;
//
//      while (int i = 0; ; ++i) {
//
//          // Block until work item on queue.
//          // ...
//
//          // Deque item.
//          // ...
//
//          // Break when end-of-work item received.
//          // ...
//
//          char b[100];
//          bsl::sprintf(b, "%d", i);
//          assert(bsl::strcmp(b, item.d_item) == 0);   // verify content
//
//          queue->d_alloc_p->deallocate(item.d_item);  // deallocate
//      }
//
//      return 0;
//  }
//..
// A complete listing of the example's structures and functions follows:
//..
//  struct my2_WorkItem {
//      // DATA
//      char *d_item;  // represents work to perform
//  };
//
//  struct my2_WorkQueue {
//      // DATA
//      bsl::list<my2_WorkItem>  d_queue;    // queue of work requests
//      bcemt_Mutex              d_mx;       // protects the shared queue
//      bcemt_Condition          d_cv;       // signals existence of new work
//      bslma_Allocator         *d_alloc_p;  // pooled allocator
//
//      // CREATORS
//      explicit my2_WorkQueue(bslma_Allocator *basicAllocator = 0)
//      : d_queue(basic_Allocator)
//      , d_alloc_p(basic_Allocator)
//      {
//      }
//  };
//
//  extern "C" void *my2_producer(void *arg)
//  {
//      my2_WorkQueue *queue = (my2_WorkQueue *)arg;
//
//      for (int i = 0; i < 50; ++i) {
//
//          char b[100];
//          bsl::sprintf(b, "%d", i);
//          int len = bsl::strlen(b);
//
//          my2_WorkItem request;
//          request.d_item = (char *)queue->d_alloc_p->allocate(len+1);
//          bsl::memcpy(request.d_item, b, len+1);
//
//          if (veryVerbose) {
//              // Assume thread-safe implementations of 'cout' and 'endl'
//              // exist (named 'MTCOUT' and 'MTENDL', respectively).
//
//              MTCOUT << "Enqueuing " << request.d_item << MTENDL;
//          }
//
//          queue->d_mx.lock();
//          queue->d_queue.push_back(request);
//          queue->d_mx.unlock();
//          queue->d_cv.signal();
//      }
//
//      my2_WorkItem request;
//      request.d_item = 0;
//
//      queue->d_mx.lock();
//      queue->d_queue.push_back(request);
//      queue->d_mx.unlock();
//      queue->d_cv.signal();
//
//      return queue;
//  }
//
//  extern "C" void *my2_consumer(void *arg)
//  {
//      my2_WorkQueue *queue = (my2_WorkQueue *)arg;
//
//      for (int i = 0; ; ++i) {
//
//          queue->d_mx.lock();
//          while (0 == queue->d_queue.size()) {
//              queue->d_cv.wait(&queue->d_mx);
//          }
//
//          my2_WorkItem item = queue->d_queue.front();
//          queue->d_queue.pop_front();
//          queue->d_mx.unlock();
//
//          if (0 == item.d_item) {
//              break;
//          }
//
//          // Process the work requests.
//          if (veryVerbose) {
//              // Assume thread-safe implementations of 'cout' and 'endl'
//              // exist (named 'MTCOUT' and 'MTENDL', respectively).
//
//              MTCOUT << "Processing " << item.d_item << MTENDL;
//          }
//
//          char b[100];
//          bsl::sprintf(b, "%d", i);
//          assert(bsl::strcmp(b, item.d_item) == 0);
//
//          queue->d_alloc_p->deallocate(item.d_item);
//      }
//
//      return 0;
//  }
//
//  int main()
//  {
//      bcema_PoolAllocator poolAlloc(100);
//      my2_WorkQueue queue(&poolAlloc);
//
//      bcemt_Attribute attributes;
//
//      bcemt_ThreadUtil::Handle producerHandle;
//      int status = bcemt_ThreadUtil::create(&producerHandle,
//                                            attributes,
//                                            &my2_producer,
//                                            &queue);
//      assert(0 == status);
//
//      bcemt_ThreadUtil::Handle consumerHandle;
//      status = bcemt_ThreadUtil::create(&consumerHandle,
//                                        attributes,
//                                        &my2_consumer,
//                                        &queue);
//      assert(0 == status);
//      status = bcemt_ThreadUtil::join(consumerHandle);
//      assert(0 == status);
//      status = bcemt_ThreadUtil::join(producerHandle);
//      assert(0 == status);
//  }
//..

#ifndef INCLUDED_BCESCM_VERSION
#include <bcescm_version.h>
#endif

#ifndef INCLUDED_BCEMA_POOL
#include <bcema_pool.h>
#endif

#ifndef INCLUDED_BCES_ATOMICTYPES
#include <bces_atomictypes.h>
#endif

#ifndef INCLUDED_BSLMA_ALLOCATOR
#include <bslma_allocator.h>
#endif

#ifndef INCLUDED_BSLS_ALIGNMENTUTIL
#include <bsls_alignmentutil.h>
#endif

#ifndef INCLUDED_BSLS_BLOCKGROWTH
#include <bsls_blockgrowth.h>
#endif

#ifndef INCLUDED_BSLS_OBJECTBUFFER
#include <bsls_objectbuffer.h>
#endif

namespace BloombergLP {

                         // =========================
                         // class bcema_PoolAllocator
                         // =========================

class bcema_PoolAllocator : public bslma_Allocator {
    // This class implements the 'bslma_Allocator' protocol to provide an
    // allocator that manages pooled memory blocks of some uniform size,
    // specified either at construction, or at the first invocation of the
    // 'allocate' method.  This allocator maintains an internal linked list of
    // free memory blocks, and dispenses one block for each 'allocate' method
    // invocation.  When a memory block is deallocated, it is returned to the
    // free list for potential reuse.
    //
    // This class guarantees thread-safety while allocating or releasing
    // memory.

    // PRIVATE TYPES
    enum {
        BCEMA_MAGIC_NUMBER  = 0x111902,  // magic number that is inserted in
                                         // header of items that are allocated
                                         // from the underlying pool

        BCEMA_UNINITIALIZED =  0,        // pool not yet initialized

        BCEMA_INITIALIZED   =  1,        // pool is initialized

        BCEMA_INITIALIZING  = -1         // pool initialization in progress
    };

    union Header {
        // Leading header on each allocated memory block.  If the memory block
        // is allocated from the pool, 'd_magicNumber' is set to
        // 'BCEMA_MAGIC_NUMBER'.  Otherwise, memory is allocated from the
        // external allocator supplied at construction, and 'd_magicNumber' is
        // set to 0.

        int                                d_magicNumber;  // allocation source
                                                           // and sanity check

        bsls_AlignmentUtil::MaxAlignedType d_dummy;        // forces alignment
    };

    // DATA
    bces_AtomicInt   d_initialized;  // initialization state indicator

    bsls_ObjectBuffer<bcema_Pool>
                     d_pool;         // buffer used to hold the 'bcema_Pool'
                                     // (initialization occurs when the pooled
                                     // memory block size first becomes known)

    int              d_blockSize;    // block size

    bsls_BlockGrowth::Strategy
                     d_growthStrategy;
                                     // growth strategy of the chunk size

    int              d_maxBlocksPerChunk;
                                     // max chunk size (in blocks-per-chunk)

    bslma_Allocator *d_allocator_p;  // basic allocator, held but not owned

  private:
    // NOT IMPLEMENTED
    bcema_PoolAllocator(const bcema_PoolAllocator&);
    bcema_PoolAllocator& operator=(const bcema_PoolAllocator&);

  public:
    // CREATORS
    explicit
    bcema_PoolAllocator(bslma_Allocator            *basicAllocator = 0);
    explicit
    bcema_PoolAllocator(bsls_BlockGrowth::Strategy  growthStrategy,
                        bslma_Allocator            *basicAllocator = 0);
    bcema_PoolAllocator(bsls_BlockGrowth::Strategy  growthStrategy,
                        int                         maxBlocksPerChunk,
                        bslma_Allocator            *basicAllocator = 0);
        // Create a pool allocator that returns blocks of contiguous memory of
        // uniform size for each 'allocate' method invocation, where the size
        // is determined by the first invocation of 'allocate'.  Optionally
        // specify a 'growthStrategy' used to control the growth of internal
        // memory chunks (from which memory blocks are dispensed).  If
        // 'growthStrategy' is not specified, geometric growth is used.  If
        // 'growthStrategy' is specified, optionally specify a
        // 'maxBlocksPerChunk', indicating the maximum number of blocks to be
        // allocated at once when the underlying pool must be replenished.  If
        // 'maxBlocksPerChunk' is not specified, an implementation-defined
        // value is used.  If geometric growth is used, the chunk size grows
        // starting at the value returned by 'blockSize', doubling in size
        // until the size is exactly 'blockSize() * maxBlocksPerChunk'.  If
        // constant growth is used, the chunk size is always
        // 'maxBlocksPerChunk'.  Optionally specify a 'basicAllocator' used to
        // supply memory.  If 'basicAllocator' is 0, the currently installed
        // default allocator is used.  The behavior is undefined unless
        // '1 <= maxBlocksPerChunk'.

    explicit
    bcema_PoolAllocator(size_type                   blockSize,
                        bslma_Allocator            *basicAllocator = 0);
    bcema_PoolAllocator(size_type                   blockSize,
                        bsls_BlockGrowth::Strategy  growthStrategy,
                        bslma_Allocator            *basicAllocator = 0);
    bcema_PoolAllocator(size_type                   blockSize,
                        bsls_BlockGrowth::Strategy  growthStrategy,
                        int                         maxBlocksPerChunk,
                        bslma_Allocator            *basicAllocator = 0);
        // Create a pool allocator that returns blocks of contiguous memory of
        // the specified 'blockSize' (in bytes) for each 'allocate' method
        // invocation.  Optionally specify a 'growthStrategy' used to control
        // the growth of internal memory chunks (from which memory blocks are
        // dispensed).  If 'growthStrategy' is not specified, geometric growth
        // is used.  If 'blockSize' and 'growthStrategy' are specified,
        // optionally specify a 'maxBlocksPerChunk', indicating the maximum
        // number of blocks to be allocated at once when the underlying pool
        // must be replenished.  If 'maxBlocksPerChunk' is not specified, an
        // implementation-defined value is used.  If geometric growth is used,
        // the chunk size grows starting at 'blockSize', doubling in size until
        // the size is exactly 'blockSize * maxBlocksPerChunk'.  If constant
        // growth is used, the chunk size is always 'maxBlocksPerChunk'.
        // Optionally specify a 'basicAllocator' used to supply memory.  If
        // 'basicAllocator' is 0, the currently installed default allocator is
        // used.  The behavior is undefined unless '0 <= blockSize' and
        // '1 <= maxBlocksPerChunk'.

    ~bcema_PoolAllocator();
        // Destroy this pool allocator.

    // MANIPULATORS
    void *allocate(size_type size);
        // Return a newly allocated block of memory of (at least) the specified
        // positive 'size' (in bytes), and having alignment conforming to the
        // platform requirement for any object having 'size' bytes.  If 'size'
        // is 0, a null pointer is returned with no effect.  If the block size
        // is not supplied at construction, then the 'size' specified in the
        // first call to this method fixes the block size.  If 'size' is
        // greater than the value returned by 'blockSize', the allocation
        // request will be satisfied directly by the external allocator
        // supplied at construction (or the default allocator, if no allocator
        // was supplied).  The behavior is undefined unless '0 <= size'.

    void deallocate(void *address);
        // Return the memory at the specified 'address' back to this allocator.
        // If 'address' is 0, this method has no effect.  The behavior is
        // undefined unless 'address' was allocated using this allocator and
        // has not already been deallocated.

    // ACCESSORS
    int blockSize() const;
        // Return the size (in bytes) of the memory blocks allocated from this
        // allocator.  Note that all blocks dispensed by this allocator have
        // the same size.

#ifndef BDE_OMIT_INTERNAL_DEPRECATED

    int objectSize() const;
        // Return the size (in bytes) of the memory blocks allocated from this
        // allocator.  Note that all blocks dispensed by this allocator have
        // the same size.
        //
        // DEPRECATED: Use 'blockSize' instead.

#endif
};

// ============================================================================
//                      INLINE FUNCTION DEFINITIONS
// ============================================================================

                         // -------------------------
                         // class bcema_PoolAllocator
                         // -------------------------

// ACCESSORS
inline
int bcema_PoolAllocator::blockSize() const
{
    return d_blockSize;
}

#ifndef BDE_OMIT_INTERNAL_DEPRECATED

inline
int bcema_PoolAllocator::objectSize() const
{
    return blockSize();
}

#endif

}  // close namespace BloombergLP

#endif

// ---------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2010
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ---------------------------------
