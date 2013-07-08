// bdet_calendarcache.cpp                                             -*-C++-*-
#include <bdet_calendarcache.h>

#include <bdes_ident.h>
BDES_IDENT_RCSID(bdet_calendarcache_cpp,"$Id$ $CSID$")

#include <bdet_calendarloader.h>
#include <bdet_datetime.h>
#include <bdet_packedcalendar.h>
#include <bdet_systemtimeutil.h>
#include <bdet_timeinterval.h>

#include <bslma_allocator.h>
#include <bslma_default.h>
#include <bslma_rawdeleterproctor.h>

#include <bsls_assert.h>
#include <bsls_types.h>

#include <bsl_functional.h>
#include <bsl_string.h>

namespace BloombergLP {

                        // =============================
                        // class bdet_CalendarCacheEntry
                        // =============================

class bdet_CalendarCacheEntry {
    // This class implements an entry in the calendar cache.  It contains the
    // calendar object, the time when it is loaded into the cache, and a flag
    // which indicates a forced reload of the calendar object if set to true.

    bdet_Calendar     d_calendar;         // the calendar object
    bdet_Datetime     d_loadTime;         // the time when this entry is loaded
    bool              d_forceReloadFlag;  // set it to 'true' to force a reload
    bool              d_firstLoadFlag;    // 'true' if 'd_calendar' has never
                                          // been loaded and 'false' otherwise
    bslma::Allocator *d_allocator_p;      // memory allocator(held, not owned)

  private:
    // NOT IMPLEMENTED
    bdet_CalendarCacheEntry(const bdet_CalendarCacheEntry&);
    bdet_CalendarCacheEntry& operator=(const bdet_CalendarCacheEntry&);

  public:
    // CREATORS
    explicit bdet_CalendarCacheEntry(bslma::Allocator *basicAllocator = 0);
        // Create an empty calendar cache entry object.  Optionally specify a
        // 'basicAllocator' used to supply memory.  If 'basicAllocator' is 0,
        // the currently installed default allocator is used.

    ~bdet_CalendarCacheEntry();
        // Destroy this object.

    // MANIPULATORS
    void invalidate();
        // Mark this entry expired.

    const bdet_Calendar *calendar(bdet_CalendarLoader      *loader,
                                  const char               *name,
                                  const bdet_TimeInterval&  timeout);
        // Return the calendar from the cache if the cache entry is valid, or
        // load it into the cache using the calendar loader otherwise.

    const bdet_Calendar *calendar(bdet_CalendarLoader *loader,
                                  const char          *name);
        // Reload the calendar from the calendar loader if 'd_forceReloadFlag'
        // is set; return the calendar from the cache otherwise.
};

// CREATORS
inline
bdet_CalendarCacheEntry::bdet_CalendarCacheEntry(
                                              bslma::Allocator *basicAllocator)
: d_calendar(basicAllocator)
, d_forceReloadFlag(true)
, d_firstLoadFlag(true)
, d_allocator_p(bslma::Default::allocator(basicAllocator))
{
}

bdet_CalendarCacheEntry::~bdet_CalendarCacheEntry()
{
}

// MANIPULATORS
const bdet_Calendar *bdet_CalendarCacheEntry::calendar(
                                                   bdet_CalendarLoader *loader,
                                                   const char          *name)
{
    BSLS_ASSERT(loader);
    BSLS_ASSERT(name);

    if (d_forceReloadFlag) {
        bdet_PackedCalendar packedCalendar(d_allocator_p);
        int ret = loader->load(&packedCalendar, name);
        if (ret != 0) {
            if (d_firstLoadFlag) {
                return 0;
            }
            else {
                return &d_calendar;
            }
        }
        d_loadTime = bdet_SystemTimeUtil::nowAsDatetimeUtc();
        bdet_Calendar calendar(packedCalendar, d_allocator_p);
        d_calendar.swap(calendar);
        d_forceReloadFlag = false;
        d_firstLoadFlag = false;
    }
    return &d_calendar;
}

const bdet_Calendar *bdet_CalendarCacheEntry::calendar(
                                             bdet_CalendarLoader      *loader,
                                             const char               *name,
                                             const bdet_TimeInterval&  timeout)
{
    BSLS_ASSERT(loader);
    BSLS_ASSERT(name);

    // Determine whether calendar should be reloaded by checking whether the
    // elapsed time is greater than the time out time.  Note that the elapsed
    // time has milliseconds precision while time out time has nanoseconds
    // precision, and that the behavior is undefined unless 'timeout' is small
    // enough to fit in a 64-bit integer value in milliseconds.

    bsls::Types::Int64 elapsedTime =
    (bdet_SystemTimeUtil::nowAsDatetimeUtc() - d_loadTime).totalMilliseconds();

    if (elapsedTime >= timeout.totalMilliseconds()) {

        // This entry has expired.

        d_forceReloadFlag = true;
    }

    return calendar(loader, name);
}

inline void bdet_CalendarCacheEntry::invalidate()
{
    d_forceReloadFlag = true;
}

                        // ------------------------
                        // class bdet_CalendarCache
                        // ------------------------

// CREATORS
bdet_CalendarCache::bdet_CalendarCache(bdet_CalendarLoader *loader,
                                       bslma::Allocator    *basicAllocator)

// We have to pass 'bsl::less<key>()' because 'bsl::map' does not have
// a constructor which takes only an allocator.

: d_cache(bsl::less<bsl::string>(), basicAllocator)
, d_loader_p(loader)
, d_useTimeOutFlag(false)
, d_allocator_p(bslma::Default::allocator(basicAllocator))
{
    BSLS_ASSERT(loader);
}

bdet_CalendarCache::bdet_CalendarCache(
                                      bdet_CalendarLoader      *loader,
                                      const bdet_TimeInterval&  timeout,
                                      bslma::Allocator         *basicAllocator)

// We have to pass 'bsl::less<key>()' because 'bsl::map' does not have
// a constructor which takes only an allocator.

: d_cache(bsl::less<bsl::string>(), basicAllocator)
, d_loader_p(loader)
, d_timeOut(timeout)
, d_useTimeOutFlag(true)
, d_allocator_p(bslma::Default::allocator(basicAllocator))
{
    BSLS_ASSERT(loader);
}

bdet_CalendarCache::~bdet_CalendarCache()
{
    for (CacheIterator it = d_cache.begin(); it != d_cache.end(); ++it) {
        d_allocator_p->deleteObjectRaw(it->second);
    }
}

// MANIPULATORS
const bdet_Calendar *bdet_CalendarCache::calendar(const char *calendarName)
{
    if (!calendarName) {
        return 0;
    }
    CacheIterator it = d_cache.find(calendarName);
    if (it != d_cache.end()) {
        if (d_useTimeOutFlag) {
            return it->second->calendar(d_loader_p, calendarName, d_timeOut);
        }
        else {
            return it->second->calendar(d_loader_p, calendarName);
        }
    }

    // Insert a new entry.

    bdet_CalendarCacheEntry *entry_p;
    entry_p = new(*d_allocator_p) bdet_CalendarCacheEntry(d_allocator_p);
    if (0 == entry_p) {
        return 0;
    }

    // The 'bslma::RawDeleterProctor' object below is used to release the
    // memory pointed to by 'entry_p' in the case when an exception takes
    // place.  It will release the memory when it is destroyed.

    bslma::RawDeleterProctor<bdet_CalendarCacheEntry, bslma::Allocator>
                                        deleterProctor(entry_p, d_allocator_p);

    const bdet_Calendar *calendar_p = entry_p->calendar(d_loader_p,
                                                        calendarName);
    if (0 == calendar_p) {
        return 0;
    }

    d_cache[calendarName] = entry_p;

    // No exception is thrown.  Release the auto deleter object so it won't
    // free the memory pointer to by entry_p.

    deleterProctor.release();

    return calendar_p;
}

void bdet_CalendarCache::invalidate(const char *calendarName)
{
    if (calendarName) {
        CacheIterator it = d_cache.find(calendarName);
        if (it != d_cache.end()) {
            it->second->invalidate();
        }
    }
}

void bdet_CalendarCache::invalidateAll()
{
    for (CacheIterator it = d_cache.begin(); it != d_cache.end(); ++it) {
        it->second->invalidate();
    }
}

                        // ----------------------------
                        // class bdet_CalendarCacheIter
                        // ----------------------------

// CREATORS
bdet_CalendarCacheIter::bdet_CalendarCacheIter(
                                     const CacheConstIterator&  iter,
                                     bdet_CalendarLoader       *loader,
                                     bool                       useTimeOutFlag,
                                     const bdet_TimeInterval&   timeout)
: d_iterator(iter)
, d_loader_p(loader)
, d_useTimeOutFlag(useTimeOutFlag)
, d_timeOut_p(&timeout)
{
}

// MANIPULATORS
bdet_CalendarCacheIter&
bdet_CalendarCacheIter::operator=(const bdet_CalendarCacheIter& original)
{
    d_iterator       = original.d_iterator;
    d_loader_p       = original.d_loader_p;
    d_useTimeOutFlag = original.d_useTimeOutFlag;
    d_timeOut_p      = original.d_timeOut_p;
    return *this;
}

bdet_CalendarCacheIter operator++(bdet_CalendarCacheIter& iter, int)
{
    bdet_CalendarCacheIter imp(iter);
    ++iter;
    return imp;
}

bdet_CalendarCacheIter operator--(bdet_CalendarCacheIter& iter, int)
{
    bdet_CalendarCacheIter imp(iter);
    --iter;
    return imp;
}

// ACCESSORS
bdet_CalendarCache_PairRef bdet_CalendarCacheIter::operator*() const
{
    return bdet_CalendarCache_PairRef(bdet_CalendarCache_Pair
                                   (d_iterator->first,
                                    d_useTimeOutFlag
                                    ? *d_iterator->second->calendar(
                                                  d_loader_p,
                                                  d_iterator->first.c_str(),
                                                 *d_timeOut_p)
                                    : *d_iterator->second->calendar(
                                                  d_loader_p,
                                                  d_iterator->first.c_str())));
}

}  // close namespace BloombergLP

// ---------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2005
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ---------------------------------
