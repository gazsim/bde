// bslfwd_bsls_alignmentimpmatch.h                                    -*-C++-*-
#ifndef INCLUDED_BSLFWD_BSLS_ALIGNMENTIMPMATCH
#define INCLUDED_BSLFWD_BSLS_ALIGNMENTIMPMATCH

#ifndef INCLUDED_BSLFWD_BUILDTARGET
#include <bslfwd_buildtarget.h>
#endif

//@PURPOSE: Provide a forward-declaration for the corresponding BDE type.
//
//@SEE_ALSO: bslfwd_buildtarget
//
//@AUTHOR: Alexei Zakharov (azakharov7)
//
//@DESCRIPTION: This header provides a forward-declaration for the
// corresponding BDE type in order to enable client code to safely use a
// forward-declaration for that type during the period where the BDE libraries
// transition to C++ namespaces.  If 'BSLFWD_BUILDTARGET_BSL_USING_NAMESPACES'
// is defined this header will forward declare the type in the appropriate C++
// namespace (e.g., 'bslma::Allocator') otherwise it will forward declare the
// type in the legacy-namespace style (e.g., 'bslma_Allocator').

#ifdef BSLFWD_BUILDTARGET_BSL_USING_NAMESPACES

namespace BloombergLP {
    namespace bsls {
        class AlignmentImpMatch;
    }

    typedef bsls::AlignmentImpMatch bsls_AlignmentImpMatch;
}

#else

namespace BloombergLP {
    class bsls_AlignmentImpMatch;
}

#endif

#endif

// ---------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2012
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ---------------------------------
