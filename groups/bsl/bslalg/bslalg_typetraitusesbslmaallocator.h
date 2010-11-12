// bslalg_typetraitusesbslmaallocator.h                               -*-C++-*-
#ifndef INCLUDED_BSLALG_TYPETRAITUSESBSLMAALLOCATOR
#define INCLUDED_BSLALG_TYPETRAITUSESBSLMAALLOCATOR

#ifndef INCLUDED_BSLS_IDENT
#include <bsls_ident.h>
#endif
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide a primitive trait for types that use 'bslma' allocators.
//
//@CLASSES:
//   bslalg_TypeTraitUsesBslmaAllocator: uses 'bslma' allocators
//
//@SEE_ALSO: bslmf_typetraits, bslalg_constructorproxy
//
//@AUTHOR: Herve Bronnimann (hbronnim)
//
//@DESCRIPTION: This component provides a single traits class,
// 'bslalg_TypeTraitUsesBslmaAllocator'.  A type 'T' with this trait uses an
// allocator derived from 'bslma_Allocator' to allocate memory.  Such a type
// *must* have a "copy" constructor with prototype
// 'T(const T&, bslma_Allocator*)'.  It usually also has a "default"
// constructor with prototype 'T(bslma_Allocator*)', and it may also have
// additional constructors with various numbers of arguments, that take an
// optional 'bslma_Allocator*' last argument.
//
// This component is used by virtually all 'bslalg' components for providing
// primitives that ensure that a 'bslma' allocator is always passed through
// properly to a class creator if that class has the 'bslma' allocator trait.
//
///Usage
///-----
// TBD

#ifndef INCLUDED_BSLSCM_VERSION
#include <bslscm_version.h>
#endif

namespace BloombergLP {

                 //==========================================
                 // struct bslalg_TypeTraitUsesBslmaAllocator
                 //==========================================

struct bslalg_TypeTraitUsesBslmaAllocator {
    // A type 'T' with this trait uses an allocator derived from
    // 'bslma_Allocator' to allocate memory.  Such a type *must* have a "copy"
    // constructor with prototype 'T(const T&, bslma_Allocator*)'.  It usually
    // also has a "default" constructor with prototype 'T(bslma_Allocator*)',
    // and it may also have additional constructors with various numbers of
    // arguments, that take an optional 'bslma_Allocator*' last argument.  The
    // allocator argument is usually defaulted, so that these two constructors
    // operate as true copy and default constructors.  The default constructor
    // is usually declared 'explicit' to avoid implicit conversion from
    // 'bslma_Allocator*'.  Generic containers use this trait to determine if
    // their contained elements use allocators.  Types that use
    // 'bslma_Allocator' should always have this trait defined.  (Note that
    // container classes instantiated with 'std::allocator' should also be
    // assigned this trait, since 'std::allocator' is built on
    // 'bslma_Allocator'.  )
};


#if !defined(BSL_LEGACY) || 1 == BSL_LEGACY

                 //==========================================
                 // struct bdealg_TypeTraitUsesBdemaAllocator
                 //==========================================

typedef bslalg_TypeTraitUsesBslmaAllocator bdealg_TypeTraitUsesBdemaAllocator;
    // This alias is defined for backward compatibility.

#endif

}  // close namespace BloombergLP

#endif

// ---------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2008
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ---------------------------------
