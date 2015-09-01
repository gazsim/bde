// bdlsb_fixedmemoutstreambuf.h                                       -*-C++-*-
#ifndef INCLUDED_BDLSB_FIXEDMEMOUTSTREAMBUF
#define INCLUDED_BDLSB_FIXEDMEMOUTSTREAMBUF

#ifndef INCLUDED_BSLS_IDENT
#include <bsls_ident.h>
#endif
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide an output 'basic_streambuf' using a client buffer.
//
//@CLASSES:
//   bdlsb::FixedMemOutStreamBuf: output stream buffer using client memory
//
//@AUTHOR: Lea Fester (lfester)
//
//@SEE_ALSO: bdlsb_memoutstreambuf, bdlsb_fixedmeminstreambuf
//
//@DESCRIPTION: This component implements the output portion of the
// 'bsl::basic_streambuf' protocol using a client-supplied memory buffer.
// Method names necessarily correspond to the protocol-specified method names.
// Clients supply the character buffer at stream buffer construction, and can
// later reinitialize the stream buffer with a different character buffer by
// calling the 'pubsetbuf' method.
//
// This component provides none of the input-related functionality of
// 'basic_streambuf' (see Streaming Architecture, below), nor does it use
// locales in any way.
//
///Streaming Architecture
///----------------------
// Stream buffers are designed to decouple device handling from content
// formatting, providing the requisite device handling and possible buffering
// services, and leaving the formatting to the client stream.  The standard C++
// IOStreams library further partitions streaming into input streaming and
// output streaming, separating responsibilities for each at both the stream
// layer and the stream buffer layer.  The BDE streaming library for 'bdex',
// including all of 'bdesb', follows this model.
//
///Usage Example
///-------------
// This example demonstrates use of a stream buffer by a stream, in this case a
// stream with simple formatting requirements -- namely, capitalizing all
// character data that passes through its management.  (To simplify the
// example, we do not include the functions for streaming non-character data.)
//
// The stream uses a client-supplied 'char'-array-based stream buffer, which is
// inherently a fixed-size buffer.  In order to minimize data loss, this stream
// (simplistically) flushes itself whenever utilization of the underlying
// buffer exceeds 90 percent.  (In this simplified example, 'flush' merely
// writes the buffer's contents to the screen.)
//..
// // my_capitalizingstream.h
//
// class my_CapitalizingStream {
//     // This class capitalizes character data....
//
//   private:
//     static const float NINETY_PERCENT;
//
//     bdlsb::FixedMemOutStreamBuf d_streamBuf;  // stream buffer to write to
//     int d_capacity;                          // size of byte storage
//     int d_utilization;                       // amount of storage in use
//
//     friend my_CapitalizingStream& operator<<(my_CapitalizingStream& stream,
//                                              const string&          data);
//     friend my_CapitalizingStream& operator<<(my_CapitalizingStream& stream,
//                                              char                   data);
//     // PRIVATE MANIPULATORS
//     void flush();
//         // Write the contents of this stream to....
//
//   public:
//     // CREATORS
//     my_CapitalizingStream(char *buffer, int   length);
//         // Create a stream that....
//
//     ~my_CapitalizingStream() {}
//         // Destroy this object.
//
//     // ACCESSORS
//     const bdlsb::FixedMemOutStreamBuf& streamBuf() { return d_streamBuf; }
//         // Return the stream buffer used by this stream.  Note that this
//         // method is for debugging purposes only.
// };
//
// // FREE OPERATORS
// my_CapitalizingStream& operator<<(my_CapitalizingStream& stream,
//                                   const string&          data);
// my_CapitalizingStream& operator<<(my_CapitalizingStream& stream,
//                                   char                   data);
//     // Write the contents of....
//..
// As is typical, the streaming operators are made friends of the class.
//..
// // my_capitalizingstream.cpp
//
// #include <algorithm>
//
// const float my_CapitalizingStream::NINETY_PERCENT = 0.9;
//
// // CREATORS
// my_CapitalizingStream::my_CapitalizingStream(char *buffer,
//                                              int   length)
//   : d_streamBuf(buffer, length)
//   , d_capacity(length)
//   , d_utilization(0)
// {
// }
//
// // PRIVATE MANIPULATORS
// void my_CapitalizingStream::flush()
// {
//     bsl::cout << d_streamBuf.data() << bsl::endl;
//     d_streamBuf.pubseekpos(0);
// }
//
// // FREE OPERATORS
// my_CapitalizingStream& operator<<(my_CapitalizingStream& stream,
//                                   const string&          data)
// {
//     if (stream.d_utilization > my_CapitalizingStream::NINETY_PERCENT *
//                                                         stream.d_capacity) {
//         stream.flush();
//     }
//
//     string tmp(data);
//     transform(tmp.begin(), tmp.end(), tmp.begin(), bsl::toupper);
//     stream.d_streamBuf.sputn(tmp.c_str(), tmp.length());
//     stream.d_utilization += tmp.length();
//     return stream;
// }
//
// my_CapitalizingStream& operator<<(my_CapitalizingStream& stream,
//                                   char                   data)
// {
//     if (stream.d_utilization > my_CapitalizingStream::NINETY_PERCENT *
//                                                         stream.d_capacity) {
//         stream.flush();
//     }
//
//     stream.d_streamBuf.sputc(
//                             bsl::toupper(static_cast<unsigned char>(data)));
//     ++stream.d_utilization;
//     return stream;
// }
//..
// Given the above two functions, we can now write 'main', as follows:
//..
// // my_app.m.cpp
//
// int main(int argc, char **argv)
// {
//     char localBuffer[20];
//     my_CapitalizingStream cs(localBuffer, sizeof localBuffer);
//     cs << "If concrete objects reside in such a library";
//
//     // Write again; since we do not have 10% capacity left, it will
//     // force a flush.
//     cs << "Hello!";
// }
//..
// Because the first write used more than 90% of the capacity (in fact, it
// tried to write more than 100% of the capacity, causing a loss of all
// characters beyond the twentieth), the second write forces a flush of the
// stream buffer contents before copying the new contents.  The output from
// this program therefore is:
//..
// IF CONCRETE OBJECTS
//..

#ifndef INCLUDED_BDLSCM_VERSION
#include <bdlscm_version.h>
#endif

#ifndef INCLUDED_BSLS_ASSERT
#include <bsls_assert.h>
#endif

#ifndef INCLUDED_BSLS_PLATFORM
#include <bsls_platform.h>
#endif

#ifndef INCLUDED_BSL_CSTDLIB
#include <bsl_cstdlib.h>
#endif

#ifndef INCLUDED_BSL_CSTRING
#include <bsl_cstring.h>
#endif

#ifndef INCLUDED_BSL_IOS
#include <bsl_ios.h>
#endif

#ifndef INCLUDED_BSL_STREAMBUF
#include <bsl_streambuf.h>
#endif

#if defined(BSLS_PLATFORM_CMP_MSVC) && defined(min)
    // Note: on Windows -> WinDef.h:#define min(a,b) ...
#undef min
#endif

namespace BloombergLP {
namespace bdlsb {

                         // ==========================
                         // class FixedMemOutStreamBuf
                         // ==========================

class FixedMemOutStreamBuf : public bsl::streambuf {
    // This class implements the output functionality of the 'basic_streambuf'
    // protocol for client-supplied memory.

  private:
    // NOT IMPLEMENTED
    FixedMemOutStreamBuf(const FixedMemOutStreamBuf&);
    FixedMemOutStreamBuf& operator=(const FixedMemOutStreamBuf&);

  protected:
    // PROTECTED MANIPULATORS
    virtual pos_type seekoff(
                           off_type                offset,
                           bsl::ios_base::seekdir  fixedPosition,
                           bsl::ios_base::openmode which = bsl::ios_base::out);
        // Set the location at which the next output will be written to the
        // specified 'offset' from the location indicated by the specified
        // 'fixedPosition' and return the new position on success or
        // pos_type(-1) on failure.  The seek operation will fail if 'which'
        // does not include the flag 'bsl::ios_base::out' or if the new
        // position is less than zero or greater than the value returned by
        // 'length'.

    virtual pos_type seekpos(
                           pos_type                position,
                           bsl::ios_base::openmode which = bsl::ios_base::out);
        // Set the location at which the next output will be written to the
        // specified 'position' and return the 'position' on success or
        // pos_type(-1) on failure.  The seek operation will fail if 'which'
        // does not include the flag 'bsl::ios_base::out' or if
        // 'position < 0 || length() < position'.

    virtual FixedMemOutStreamBuf *setbuf(char_type       *buffer,
                                         bsl::streamsize  length);
        // Reinitialize this stream buffer to use the specified character
        // 'buffer' having the specified 'length'.  Return the address of this
        // modifiable stream buffer.  Upon reinitialization for use of the new
        // buffer, the length and next output location are reset to zero.  Note
        // that 'buffer' is held but not owned.

  public:
    // CREATORS
    FixedMemOutStreamBuf(char            *buffer,
                               bsl::streamsize  length);
        // Create an empty stream buffer that uses the specified character
        // 'buffer' of the specified 'length'.  The behavior is undefined
        // unless '0 < length'.  Note that 'buffer' is held but not owned.

    ~FixedMemOutStreamBuf();
        // Destroy this stream buffer.

    // MANIPULATORS
    char *data();
        // Return the address of the modifiable character buffer held by this
        // stream buffer.

    // ACCESSORS
    bsl::streamsize capacity() const;
        // Return the number of characters in the buffer held by this stream
        // buffer.  See 'length', below, for the span of bytes actually
        // written.

    const char *data() const;
        // Return the address of the non-modifiable character buffer held by
        // this stream buffer.

    bsl::streamsize length() const;
        // Return the number of characters from the beginning of the buffer to
        // the current write position.  This function returns the same value as
        // 'seekoff(0, bsl::ios_base::end)'.  The length is modified by a call
        // to 'seekpos' or 'seekoff' and reset to zero by a call to
        // 'pubsetbuf'.

#ifndef BDE_OMIT_INTERNAL_DEPRECATED

    bsl::streamsize bufSize() const;
        // Return the number of characters in the buffer held by this stream
        // buffer.  See 'length', below, for the span of bytes actually
        // written.
        //
        // DEPRECATED: Use 'capacity()' instead.

#endif // BDE_OMIT_INTERNAL_DEPRECATED

};

// ============================================================================
//                            INLINE DEFINITIONS
// ============================================================================

                         // --------------------------
                         // class FixedMemOutStreamBuf
                         // --------------------------

// PROTECTED MANIPULATORS
inline
FixedMemOutStreamBuf *
FixedMemOutStreamBuf::setbuf(char_type *buffer, bsl::streamsize length)
{
    BSLS_ASSERT_SAFE(buffer || 0 == length);
    BSLS_ASSERT_SAFE(0 <= length);

    // Reset pointers and length.
    setp(buffer, buffer + length);
    return this;
}

// CREATORS
inline
FixedMemOutStreamBuf::FixedMemOutStreamBuf(char            *buffer,
                                                       bsl::streamsize  length)
{
    BSLS_ASSERT_SAFE(buffer || 0 == length);
    BSLS_ASSERT_SAFE(0 <= length);

    setp(buffer, buffer + length);
}

inline
FixedMemOutStreamBuf::~FixedMemOutStreamBuf()
{
}

// MANIPULATORS
inline
char *FixedMemOutStreamBuf::data()
{
    return pbase();
}

// ACCESSORS
inline
bsl::streamsize FixedMemOutStreamBuf::capacity() const
{
    return epptr() - pbase();
}

inline
const char *FixedMemOutStreamBuf::data() const
{
    return pbase();
}

inline
bsl::streamsize FixedMemOutStreamBuf::length() const
{
    return pptr() - pbase();
}
}  // close package namespace

#ifndef BDE_OMIT_INTERNAL_DEPRECATED

namespace bdlsb {

inline
bsl::streamsize FixedMemOutStreamBuf::bufSize() const
{
    return capacity();
}
}  // close package namespace

#endif // BDE_OMIT_INTERNAL_DEPRECATED

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
