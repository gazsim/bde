// bdlt_datetimetz.cpp                                                -*-C++-*-
#include <bdlt_datetimetz.h>

#include <bsls_ident.h>
BSLS_IDENT_RCSID(bdlt_datetimetz_cpp,"$Id$ $CSID$")

#include <bdlt_date.h>  // for testing only
#include <bdlt_time.h>  // for testing only

#include <bdlsb_fixedmemoutstreambuf.h>

#include <bslim_printer.h>

#include <bslmf_assert.h>

#include <bsls_alignedbuffer.h>

#include <bsl_cstdio.h>    // 'sprintf'
#include <bsl_ios.h>       // 'bsl::streamsize'
#include <bsl_ostream.h>

namespace BloombergLP {
namespace bdlt {

// 'DatetimeTz' is trivially copyable only if 'Datetime' is also trivially
// copyable.  In the header we have stated unconditionally that 'DatetimeTz' is
// trivially copyable, so we assert our assumption about 'Datetime', as a
// sanity check.

BSLMF_ASSERT(bsl::is_trivially_copyable<Datetime>::value);

                             // ----------------
                             // class DatetimeTz
                             // ----------------

// ACCESSORS
bsl::ostream& DatetimeTz::print(bsl::ostream& stream,
                                int           level,
                                int           spacesPerLevel) const
{
    if (stream.bad()) {
        return stream;                                                // RETURN
    }

    // Write to a temporary stream having width 0 in case the caller has done
    // something like:
    //..
    //  os << bsl::setw(20) << myDatetimeTz;
    //..
    // The user-specified width will be effective when 'streamBuf.data()' is
    // written to 'stream' (below).

    const int SIZE = 32;  // must be > 30

    bsls::AlignedBuffer<SIZE> alignedBuffer;
    bdlsb::FixedMemOutStreamBuf streamBuf(alignedBuffer.buffer(),
                                          static_cast<bsl::streamsize>(SIZE));

    bsl::ostream os(&streamBuf);

    os << localDatetime();

    const char sign    = d_offset < 0 ? '-' : '+';
    const int  minutes = '-' == sign ? -d_offset : d_offset;
    const int  hours   = minutes / 60;

    //       space usage: +-  hh  mm  nil
    const int offsetSize = 1 + 2 + 2 + 1;
    char      offsetBuffer[offsetSize];

    // Use only 2 digits for 'hours' (DRQS 12693813).
    if (hours < 100) {
        bsl::sprintf(offsetBuffer, "%c%02d%02d", sign, hours, minutes % 60);
    }
    else {
        bsl::sprintf(offsetBuffer, "%cXX%02d", sign, minutes % 60);
    }

    os << offsetBuffer << bsl::ends;

    bslim::Printer printer(&stream, level, spacesPerLevel);
    printer.start(true);  // 'true' -> suppress '['
    stream << streamBuf.data();
    printer.end(true);    // 'true' -> suppress ']'

    return stream;
}

}  // close package namespace
}  // close enterprise namespace

// ----------------------------------------------------------------------------
// Copyright 2014 Bloomberg Finance L.P.
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
