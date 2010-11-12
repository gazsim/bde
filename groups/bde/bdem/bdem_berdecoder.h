// bdem_berdecoder.h                  -*-C++-*-
#ifndef INCLUDED_BDEM_BERDECODER
#define INCLUDED_BDEM_BERDECODER

#ifndef INCLUDED_BDES_IDENT
#include <bdes_ident.h>
#endif
BDES_IDENT("$Id: $")

//@PURPOSE: Provide a BER decoder class.
//
//@CLASSES:
//  bdem_BerDecoder: BER decoder
//
//@SEE_ALSO: bdem_berencoder, bdem_bdemdecoder, baexml_decoder
//
//@AUTHOR: Rohan Bhindwale (rbhindwa),
//         Shezan Baig (sbaig),
//         Alexander Libman (alibman1@bloomberg.net)
//
//@DESCRIPTION:  The 'bdem_BerDecoder' class provided in this component
// contains a parameterized 'decode' function that decodes a specified
// value-semantic object from a specified stream.  There are two overloaded
// versions of this function:
//..
//    o reads from an 'bsl::streambuf'
//    o reads from an 'bsl::istream'
//..
// This component decodes objects based on the X.690 BER specification.  It can
// only be used with types supported by the 'bdeat' framework.  In particular,
// types generated by the 'bas_codegen.pl' tool can be used.  Since the types
// are generated from XSD, the X.694 mappings are applied.
//
///Usage
///-----
// Suppose we have an XML schema inside a file called 'employee.xsd':
//..
//  <?xml version='1.0' encoding='UTF-8'?>
//  <xs:schema xmlns:xs='http://www.w3.org/2001/XMLSchema'
//             xmlns:test='http://bloomberg.com/schemas/test'
//             targetNamespace='http://bloomberg.com/schemas/test'
//             elementFormDefault='unqualified'>
//
//      <xs:complexType name='Address'>
//          <xs:sequence>
//              <xs:element name='street' type='xs:string'/>
//              <xs:element name='city'   type='xs:string'/>
//              <xs:element name='state'  type='xs:string'/>
//          </xs:sequence>
//      </xs:complexType>
//
//      <xs:complexType name='Employee'>
//          <xs:sequence>
//              <xs:element name='name'        type='xs:string'/>
//              <xs:element name='homeAddress' type='test:Address'/>
//              <xs:element name='age'         type='xs:int'/>
//          </xs:sequence>
//      </xs:complexType>
//
//  </xs:schema>
//..
// Using the 'bas_codegen.pl' tool, we can generate C++ classes for this
// schema:
//..
//  $ bas_codegen.pl -g h -g cpp -p test employee.xsd
//..
// This tool will generate the header and implementation files for the
// 'test_address' and 'test_employee' components in the current directory.
//
// Now suppose we want to encode information about a particular employee using
// the BER encoding.  Note that we will use 'bdesb' stream buffers for in-core
// buffer management:
//..
//  #include <bdesb_memoutstreambuf.h>
//  #include <bdesb_fixedmeminstreambuf.h>
//
//  #include <test_employee.h>
//
//  #include <bdem_berencoder.h>
//  #include <bdem_berdecoder.h>
//
//  using namespace BloombergLP;
//
//  void usageExample()
//  {
//      bdesb_MemOutStreamBuf osb;
//
//      test::Employee bob;
//
//      bob.name()                 = "Bob";
//      bob.homeAddress().street() = "Some Street";
//      bob.homeAddress().city()   = "Some City";
//      bob.homeAddress().state()  = "Some State";
//      bob.age()                  = 21;
//
//      bdem_BerEncoder encoder;
//      int retCode = encoder.encode(&osb, bob);
//
//      assert(0 == retCode);
//..
// At this point, 'osb' contains a representation of 'bob' in BER format.  Now
// we will verify the contents of 'osb' using the 'bdem_berdecoder' component:
//..
//      bdesb_FixedMemInStreamBuf isb(osb.data(), osb.length());  // NO COPY
//      test::Employee            obj;
//
//      bdem_BerDecoderOptions options;
//      bdem_BerDecoder decoder(&options);
//      retCode = decoder.decode(&isb, &obj);
//
//      assert(0                          == retCode);
//      assert(bob.name()                 == obj.name());
//      assert(bob.homeAddress().street() == obj.homeAddress().street());
//      assert(bob.homeAddress().city()   == obj.homeAddress().city());
//      assert(bob.homeAddress().state()  == obj.homeAddress().state());
//      assert(bob.age()                  == obj.age());
//  }
//..

#ifndef INCLUDED_BDESCM_VERSION
#include <bdescm_version.h>
#endif

#ifndef INCLUDED_BDEM_BERCONSTANTS
#include <bdem_berconstants.h>
#endif

#ifndef INCLUDED_BDEM_BERDECODEROPTIONS
#include <bdem_berdecoderoptions.h>
#endif

#ifndef INCLUDED_BDEM_BERUNIVERSALTAGNUMBER
#include <bdem_beruniversaltagnumber.h>
#endif

#ifndef INCLUDED_BDEM_BERUTIL
#include <bdem_berutil.h>
#endif

#ifndef INCLUDED_BDEAT_ARRAYFUNCTIONS
#include <bdeat_arrayfunctions.h>
#endif

#ifndef INCLUDED_BDEAT_CHOICEFUNCTIONS
#include <bdeat_choicefunctions.h>
#endif

#ifndef INCLUDED_BDEAT_CUSTOMIZEDTYPEFUNCTIONS
#include <bdeat_customizedtypefunctions.h>
#endif

#ifndef INCLUDED_BDEAT_ENUMFUNCTIONS
#include <bdeat_enumfunctions.h>
#endif

#ifndef INCLUDED_BDEAT_FORMATTINGMODE
#include <bdeat_formattingmode.h>
#endif

#ifndef INCLUDED_BDEAT_NULLABLEVALUEFUNCTIONS
#include <bdeat_nullablevaluefunctions.h>
#endif

#ifndef INCLUDED_BDEAT_SEQUENCEFUNCTIONS
#include <bdeat_sequencefunctions.h>
#endif

#ifndef INCLUDED_BDEAT_TYPECATEGORY
#include <bdeat_typecategory.h>
#endif

#ifndef INCLUDED_BDEAT_VALUETYPEFUNCTIONS
#include <bdeat_valuetypefunctions.h>
#endif

#ifndef INCLUDED_BSLMA_ALLOCATOR
#include <bslma_allocator.h>
#endif

#ifndef INCLUDED_BDEUT_STRINGREF
#include <bdeut_stringref.h>
#endif

#ifndef INCLUDED_BDESB_MEMOUTSTREAMBUF
#include <bdesb_memoutstreambuf.h>
#endif

#ifndef INCLUDED_BSLS_ASSERT
#include <bsls_assert.h>
#endif

#ifndef INCLUDED_BSLS_OBJECTBUFFER
#include <bsls_objectbuffer.h>
#endif

#ifndef INCLUDED_BSL_ISTREAM
#include <bsl_istream.h>
#endif

#ifndef INCLUDED_BSL_OSTREAM
#include <bsl_ostream.h>
#endif

#ifndef INCLUDED_BSL_VECTOR
#include <bsl_vector.h>
#endif

namespace BloombergLP {

class bdem_BerDecoder_Node;
class bdem_BerDecoder_NodeVisitor;
class bdem_BerDecoder_UniversalElementVisitor;

                         // =====================
                         // class bdem_BerDecoder
                         // =====================

class bdem_BerDecoder {
    // This class contains the parameterized 'decode' functions that decode
    // data (in BER format) from an incoming stream into 'bdeat' types.

  private:
    // PRIVATE TYPES
    class MemOutStream : public bsl::ostream {
        // This class provides stream for logging using 'bdesb_MemOutStreamBuf'
        // as a streambuf.  The logging stream is created on demand, i.e.
        // during the first attempt to log message.
        bdesb_MemOutStreamBuf d_sb;

        // NOT IMPLEMENTED
        MemOutStream(const MemOutStream&);
        MemOutStream& operator=(const MemOutStream&);

      public:
        // CREATORS
        MemOutStream(bslma_Allocator *basicAllocator = 0);
            // Create a new stream using the specified 'basicAllocator'.
            // if 'basicAllocator' is 0, the default allocator is used.

        virtual ~MemOutStream();
            // Destroy this stream and release memory back to the allocator.
            //
            // Although the compiler should generate this destructor
            // implicitly, xlC 8 breaks when the destructor is called by name
            // unless it is explicitly declared.

        // MANIPULATORS
        void reset();
            // Reset the internal streambuf to empty.

        // ACCESSORS
        const char *data() const;
            // Return a pointer to the memory containing the formatted values
            // formatted to this stream.  The data is not null-terminated
            // unless a null character was appended onto this stream.

        int length() const;
            // Return the length of of the formatted data, including null
            // characters appended to the stream, if any.
    };

  public:
    // PUBLIC TYPES
    enum ErrorSeverity {
        BDEM_BER_SUCCESS = 0x00,
        BDEM_BER_ERROR   = 0x02
    };

  private:
    // DATA
    const bdem_BerDecoderOptions    *d_options;      // held, not owned
    bslma_Allocator                 *d_allocator;    // held, not owned

    bsls_ObjectBuffer<MemOutStream>  d_logArea;      // placeholder for
                                                     // MemOutStream

    MemOutStream                    *d_logStream;    // if not zero,
                                                     // log stream was
                                                     // created at the
                                                     // moment of first
                                                     // logging and must
                                                     // be destroyed

    ErrorSeverity                    d_severity;     // error severity level
    bsl::streambuf                  *d_streamBuf;    // held, not owned
    int                              d_currentDepth; // current Depth
    bdem_BerDecoder_Node            *d_topNode;      // last node

    // NOT IMPLEMENTED
    bdem_BerDecoder(const bdem_BerDecoder&);
    bdem_BerDecoder& operator=(const bdem_BerDecoder&);

    // FRIENDS
    friend class bdem_BerDecoder_Node;

  private:
    // PRIVATE MANIPULATORS
    ErrorSeverity logError(const char *msg);
        // log 'msg' and upgrade the severity level.
        // Return the 'this->ErrorSeverity()'.

    ErrorSeverity logMsg(const char *prefix, const char *msg);
        // Log 'prefix' and 'msg'.  Return the 'this->ErrorSeverity()'.

    bsl::ostream& logStream();
        // Return the stream for logging.  Note the if stream has not
        // been created yet, it will be created during this call.

  public:
    // CREATORS
    bdem_BerDecoder(const bdem_BerDecoderOptions *options = 0,
                    bslma_Allocator              *basicAllocator = 0);
        // Construct a decoder object using the optionally specified 'options',
        // and 'basicAllocator'.  If 'basicAllocator' is 0, the default
        // allocator is used.

    ~bdem_BerDecoder();
        // Destroy this object.  This destruction has no effect on objects
        // pointed-to by the pointers provided at construction.

    // MANIPULATORS
    template <typename TYPE>
    int decode(bsl::streambuf *streamBuf, TYPE *variable);
        // Decode an object of parameterized 'TYPE' from the specified
        // 'streamBuf' and load the result into the specified modifiable
        // 'variable'.  Return 0 on success, and a non-zero value otherwise.

    template <typename TYPE>
    int decode(bsl::istream& stream, TYPE *variable);
        // Decode an object of parameterized 'TYPE' from the specified 'stream'
        // and load the result into the specified modifiable 'variable'.
        // Return 0 on success, and a non-zero value otherwise.  Note that
        // 'stream' will be invalidated if the decoding fails.

    // ACCESSORS
    const bdem_BerDecoderOptions *decoderOptions() const;
        // Return pointer to the BER decoder options.

    bool maxDepthExceeded() const;
       // Return 'true' if the maximum depth level is exceeded and 'false'
       // otherwise.

    ErrorSeverity  errorSeverity() const;
        // Return the severity of the most severe log or error message
        // encountered during the last call to the 'decode' method.  The
        // severity is reset each time 'decode' is called.

    bdeut_StringRef loggedMessages() const;
        // Return a string containing any error or trace messages that were
        // logged during the last call to the 'decode' method.  The log is
        // reset each time 'decode' is called.
};

// ---  Anything below this line is implementation specific.  Do not use.  ----

                    // ==================================
                    // private class bdem_BerDecoder_Node
                    // ==================================

class bdem_BerDecoder_Node {
    // This class provides current context for BER decoding process and
    // represents a node for BER element.  The BER element consists of  element
    // tag, length field, body field and optional end of tag.  The class also
    // provides various methods to read the different parts of BER element
    // such as tag header (tag itself and length fields), body for any type of
    // data, and optional tag trailer.

    // DATA
    bdem_BerDecoder             *d_decoder;              // decoder,
                                                         // held, not owned
    bdem_BerDecoder_Node        *d_parent;               // parent node
                                                         // held, not owned
    bdem_BerConstants::TagClass  d_tagClass;             // tag class
    bdem_BerConstants::TagType   d_tagType;              // tag type
    int                          d_tagNumber;            // tag id or number
    int                          d_expectedLength;       // body length
    int                          d_consumedHeaderBytes;  // header bytes read
    int                          d_consumedBodyBytes;    // body bytes read
    int                          d_consumedTailBytes;    // trailer bytes read
    int                          d_formattingMode;       // formatting mode
    const char                  *d_fieldName;            // name of the field
//     const char                  *d_typeName;             // C++ type name of
//                                                          // receiving object

    // NOT IMPLEMENTED
    bdem_BerDecoder_Node(bdem_BerDecoder_Node&);
    bdem_BerDecoder_Node& operator=(bdem_BerDecoder_Node&);

  private:
    // PRIVATE MANIPULATORS
    int decode(bsl::vector<char> *variable, bdeat_TypeCategory::Array);

    template <typename TYPE>
    int decode(TYPE *variable, bdeat_TypeCategory::Array);

    template <typename TYPE>
    int decode(TYPE *variable, bdeat_TypeCategory::Choice);

    template <typename TYPE>
    int decode(TYPE *variable, bdeat_TypeCategory::NullableValue);

    template <typename TYPE>
    int decode(TYPE *variable, bdeat_TypeCategory::CustomizedType);

    template <typename TYPE>
    int decode(TYPE *variable, bdeat_TypeCategory::Enumeration);

    template <typename TYPE>
    int decode(TYPE *variable, bdeat_TypeCategory::Sequence);

    template <typename TYPE>
    int decode(TYPE *variable, bdeat_TypeCategory::Simple);

    template <typename TYPE>
    int decode(TYPE *variable, bdeat_TypeCategory::DynamicType);
        // Family of methods to decode current element into specified
        // 'variable' of category 'bdeat_TypeCategory'.  Return zero on
        // success and non zero otherwise.  Note that the tag header is
        // already read at the moment of call and input stream is positioned
        // at the first byte of the body field.

    template <typename TYPE>
    int decodeArray(TYPE *variable);
        // Decode the current element, which is an array, into specified
        // 'variable'.  Return zero on success and non-zero otherwise.

    template <typename TYPE>
    int decodeChoice(TYPE *variable);
        // Decode the current element, which is a choice object, into specified
        // 'variable'.  Return zero on success and non-zero otherwise.

  public:
    // CREATORS
    bdem_BerDecoder_Node(bdem_BerDecoder *decoder);

    template <typename TYPE>
    bdem_BerDecoder_Node(bdem_BerDecoder *decoder, const TYPE *variable);

    ~bdem_BerDecoder_Node();

    // MANIPULATORS
    template <typename TYPE>
    int operator()(TYPE *object, bslmf_Nil);

    template <typename TYPE, typename ANY_CATEGORY>
    int operator()(TYPE *object, ANY_CATEGORY category);

    template <typename TYPE>
    int operator()(TYPE *object);

    void print(bsl::ostream&  out,
               int            depth,
               int            spacePerLevel = 0,
               const char    *prefixText = 0) const;
        // Print the content of node to the specified stream 'out'.  'depth'
        // is the value d_decoder->currentDepth assumed after node was created.

    void printStack(bsl::ostream& out) const;
        // Print the chain of nodes to the specified 'out' stream, starting
        // from this node and iterating to the parent node, then its parent,
        // etc.

    void setFormattingMode(int formattingMode);
        // Set formatting mode specified by 'formattingMode'.

    void setFieldName(const char *name);
        // Set object field name associated with this node to the specified
        // 'name'.

//     template <typename TYPE>
//     void setType(const TYPE *variable);
//         // Set object type name associated with this node to the name of
//         // type 'TYPE'.

    int logError(const char *msg);
        // Set the node severity to BDEM_BER_ERROR, print the error message
        // specified by 'msg' to the decoder's log, print the stack of nodes
        // to the decoder's log, and return a non-zero value.

    int readTagHeader();
        // Read the node tag field, containing  tag class, tag type and tag
        // number, and the node length field.  Return zero on success and non
        // zero value otherwise.

    int readTagTrailer();
        // Read the node end-of-octets field, if such exists, so the stream
        // will be positioned at the start of next node.  Return zero on
        // success  and non zero value otherwise.

    bool hasMore();
        // Return 'true' if current node has more embedded elements and
        // return 'false' otherwise.

    int skipField();
        // Skip field body.  The identifier octet and length have already been
        // extracted.  Return zero on success and non zero otherwise.  Note
        // that method must be called when input stream is positioned at the
        // first byte of the body field.

    int readVectorChar(bsl::vector<char> *variable);
        // Read the node body content into specified 'variable'.  Return zero
        // on success  and non zero otherwise.

    // ACCESSORS
    bdem_BerDecoder_Node *parent() const;
        // Return a pointer to the parent node.

    bdem_BerConstants::TagClass tagClass() const;
        // Return the BER tag class for this node.

    bdem_BerConstants::TagType tagType() const;
        // Return the BER tag type for this node.

    int tagNumber() const;
        // Return the BER tag number for this node.

    int formattingMode() const;
        // Return formatting mode for this node.

    const char *fieldName() const;
        // Return field name for this node.

//     const char *typeName() const;
//         // Return type name for this node.

    int length() const;
       // Return expected length of the body or -1 in case of indefinite
       // length.

    int startPos() const;
        // Return the position of node tag from the beginning of input stream.
};

                 // =========================================
                 // private class bdem_BerDecoder_NodeVisitor
                 // =========================================

class bdem_BerDecoder_NodeVisitor {
    // This class is used as a visitor for visiting contained objects during
    // decoding.

    // DATA
    bdem_BerDecoder_Node *d_node;  // current node, held, not owned

    // NOT IMPLEMENTED
    bdem_BerDecoder_NodeVisitor(const bdem_BerDecoder_NodeVisitor&);
    bdem_BerDecoder_NodeVisitor& operator=(const bdem_BerDecoder_NodeVisitor&);

  public:
    // CREATORS
    bdem_BerDecoder_NodeVisitor(bdem_BerDecoder_Node *node);

    // Generated by compiler.
    // ~bdem_BerDecoder_NodeVisitor();

    // MANIPULATORS
    template <typename TYPE, typename INFO>
    int operator()(TYPE *variable, const INFO& info);
};

         // =====================================================
         // private class bdem_BerDecoder_UniversalElementVisitor
         // =====================================================

class bdem_BerDecoder_UniversalElementVisitor {
    // This 'class' is used as a visitor for visiting the top-level element and
    // also array elements during decoding.  This class is required so that the
    // universal tag number of the element can be determined when the element
    // is visited.

    // DATA
    bdem_BerDecoder_Node d_node;  // a new node

    // NOT IMPLEMENTED
    bdem_BerDecoder_UniversalElementVisitor(
                               const bdem_BerDecoder_UniversalElementVisitor&);
    bdem_BerDecoder_UniversalElementVisitor& operator=(
                               const bdem_BerDecoder_UniversalElementVisitor&);

  public:
    // CREATORS
    bdem_BerDecoder_UniversalElementVisitor(bdem_BerDecoder *d_decoder);

    // Generated by compiler:
    // ~bdem_BerDecoder_UniversalElementVisitor();

    // MANIPULATORS
    template <typename TYPE>
    int operator()(TYPE *variable);
};

                         // ============================
                         // class bdem_BerDecoder_Zeroer
                         // ============================

class bdem_BerDecoder_Zeroer {
    // This class is a deleter that just zeroes out a given pointer upon
    // destruction, for making code exception-safe.

    // DATA
    const bdem_BerDecoderOptions **d_options_p;  // address of pointer to zero
                                                 // out upon destruction

  public:
    // CREATORS
    bdem_BerDecoder_Zeroer(const bdem_BerDecoderOptions **options)
    : d_options_p(options)
    {
    }

    ~bdem_BerDecoder_Zeroer()
    {
        *d_options_p = 0;
    }
};

// ===========================================================================
//                      INLINE FUNCTION DEFINITIONS
// ===========================================================================

                         // -----------------------------------
                         // class bdem_BerDecoder::MemOutStream
                         // -----------------------------------

// CREATORS
inline
bdem_BerDecoder::MemOutStream::MemOutStream(bslma_Allocator *basicAllocator)
: bsl::ostream(0)
, d_sb(bslma_Default::allocator(basicAllocator))
{
    rdbuf(&d_sb);
}

// MANIPULATORS
inline
void bdem_BerDecoder::MemOutStream::reset()
{
    d_sb.reset();
}

// ACCESSORS
inline
const char *bdem_BerDecoder::MemOutStream::data() const
{
    return d_sb.data();
}

inline
int bdem_BerDecoder::MemOutStream::length() const
{
    return (int)d_sb.length();
}

                         // ---------------------
                         // class bdem_BerDecoder
                         // ---------------------

// ACCESSORS
inline
const bdem_BerDecoderOptions *bdem_BerDecoder::decoderOptions() const
{
    return d_options;
}

inline
bdem_BerDecoder::ErrorSeverity
bdem_BerDecoder::errorSeverity() const
{
    return d_severity;
}

inline
bdeut_StringRef bdem_BerDecoder::loggedMessages() const
{
    if (d_logStream) {
        return bdeut_StringRef(d_logStream->data(), d_logStream->length());
    }

    return bdeut_StringRef();
}

inline
bool bdem_BerDecoder::maxDepthExceeded() const
{
    return d_currentDepth > d_options->maxDepth();
}

// MANIPULATORS
inline
bsl::ostream& bdem_BerDecoder::logStream()
{
    if (0 == d_logStream) {
        d_logStream = new(d_logArea.buffer()) MemOutStream(d_allocator);
    }
    return *d_logStream;
}

template <typename TYPE>
inline
int bdem_BerDecoder::decode(bsl::istream& stream, TYPE *variable)
{
    if (!stream.good()) {
        return -1;
    }

    if (0 != this->decode(stream.rdbuf(), variable)) {
        stream.setstate(bsl::ios_base::failbit);
        return -1;
    }

    return 0;
}

template <typename TYPE>
int bdem_BerDecoder::decode(bsl::streambuf *streamBuf, TYPE *variable)
{
    BSLS_ASSERT(0 == d_streamBuf);
    d_streamBuf = streamBuf;

    d_currentDepth = 0;
    d_severity = BDEM_BER_SUCCESS;

    if (d_logStream != 0) {
        d_logStream->reset();
    }

    d_topNode = 0;

    bdeat_ValueTypeFunctions::reset(variable);

    int rc = d_severity;

    if (! d_options) {
        // Create temporary options object
        bdem_BerDecoderOptions options;
        d_options = &options;
        bdem_BerDecoder_Zeroer zeroer(&d_options);
        bdem_BerDecoder_UniversalElementVisitor visitor(this);
        rc = visitor(variable);
    }
    else {
        bdem_BerDecoder_UniversalElementVisitor visitor(this);
        rc = visitor(variable);
    }

    d_streamBuf = 0;
    return rc;
}

                         // --------------------------
                         // class bdem_BerDecoder_Node
                         // --------------------------

// CREATORS
inline
bdem_BerDecoder_Node::bdem_BerDecoder_Node(bdem_BerDecoder *decoder)
: d_decoder              (decoder)
, d_parent               (d_decoder->d_topNode)
, d_tagClass             (bdem_BerConstants::BDEM_UNIVERSAL)
, d_tagType              (bdem_BerConstants::BDEM_PRIMITIVE)
, d_tagNumber            (0)
, d_expectedLength       (0)
, d_consumedHeaderBytes  (0)
, d_consumedBodyBytes    (0)
, d_consumedTailBytes    (0)
, d_formattingMode       (bdeat_FormattingMode::BDEAT_DEFAULT)
, d_fieldName            (0)
//, d_typeName             (0)
{
    ++d_decoder->d_currentDepth;
    if (d_parent) {
        d_formattingMode = d_parent->d_formattingMode;
    }
    d_decoder->d_topNode = this;
}

inline
bdem_BerDecoder_Node::~bdem_BerDecoder_Node()
{
    if (d_parent != 0) {
        d_parent->d_consumedBodyBytes += d_consumedHeaderBytes
                                      +  d_consumedBodyBytes
                                      +  d_consumedTailBytes;
    }
    d_decoder->d_topNode = d_parent;
    --d_decoder->d_currentDepth;
}

// MANIPULATORS
inline
bool
bdem_BerDecoder_Node::hasMore()
{
    BSLS_ASSERT_SAFE(d_tagType == bdem_BerConstants::BDEM_CONSTRUCTED);

    if (bdem_BerUtil::BDEM_INDEFINITE_LENGTH == d_expectedLength) {
        return 0 != d_decoder->d_streamBuf->sgetc();
    }

    return d_expectedLength > d_consumedBodyBytes;
}

// ACCESSORS
inline
bdem_BerDecoder_Node*bdem_BerDecoder_Node::parent() const
{
    return d_parent;
}

inline
bdem_BerConstants::TagClass bdem_BerDecoder_Node::tagClass() const
{
    return d_tagClass;
}

inline
bdem_BerConstants::TagType bdem_BerDecoder_Node::tagType() const
{
    return d_tagType;
}

inline
int bdem_BerDecoder_Node::tagNumber() const
{
    return d_tagNumber;
}

inline
int bdem_BerDecoder_Node::formattingMode() const
{
    return d_formattingMode;
}

inline
const char *bdem_BerDecoder_Node::fieldName() const
{
    return d_fieldName;
}

// inline
// const char *bdem_BerDecoder_Node::typeName() const
// {
//     return d_typeName;
// }

inline
int bdem_BerDecoder_Node::length() const
{
    return d_expectedLength;
}

// MANIPULATORS
inline
void bdem_BerDecoder_Node::setFormattingMode(int formattingMode)
{
    d_formattingMode = formattingMode;
}

inline
void bdem_BerDecoder_Node::setFieldName(const char *name)
{
    d_fieldName = name;
}

// template <typename TYPE>
// inline
// void bdem_BerDecoder_Node::setType(const TYPE *variable)
// {
//     d_typeName = bdeat_TypeName::name(*variable);
// }

template <typename TYPE>
inline
int bdem_BerDecoder_Node::operator()(TYPE *, bslmf_Nil)
{
    BSLS_ASSERT(0 && "Should never execute this function");
    return -1;
}

template <typename TYPE, typename ANY_CATEGORY>
inline
int bdem_BerDecoder_Node::operator()(TYPE *object, ANY_CATEGORY category)
{
    return this->decode(object, category);
}

template <typename TYPE>
inline
int bdem_BerDecoder_Node::operator()(TYPE *object)
{
    typedef typename bdeat_TypeCategory::Select<TYPE>::Type Tag;
    return this->decode(object, Tag());
}

// PRIVATE MANIPULATORS
template <typename TYPE>
int
bdem_BerDecoder_Node::decode(TYPE *variable, bdeat_TypeCategory::Choice)
{
    // A misunderstanding of X.694 (clause 20.4), an XML choice (not anonymous)
    // element is encoded as a sequence (outer) with 1 element (inner).
    // However, if the element is anonymous (i.e., untagged), then there is no
    // inner tag.  This behavior is kept for backward compatibility.

    if (d_tagType != bdem_BerConstants::BDEM_CONSTRUCTED) {
        return logError("Expected CONSTRUCTED tag type for choice");
    }

    bool isUntagged = d_formattingMode & bdeat_FormattingMode::BDEAT_UNTAGGED;

    int rc = bdem_BerDecoder::BDEM_BER_SUCCESS;

    if (!isUntagged) {

        // typeName will be taken from predecessor node
        bdem_BerDecoder_Node innerNode(d_decoder);
//        innerNode.setType(variable);

        rc = innerNode.readTagHeader();
        if (rc != bdem_BerDecoder::BDEM_BER_SUCCESS) {
            return rc;  // error message is already logged
        }

        if (innerNode.tagClass() != bdem_BerConstants::BDEM_CONTEXT_SPECIFIC) {
            return innerNode.logError(
                "Expected CONTEXT tag class for tagged choice");
        }

        if (innerNode.tagType() != bdem_BerConstants::BDEM_CONSTRUCTED) {
            return innerNode.logError(
                "Expected CONSTRUCTED tag type for tagged choice");
        }

        if (innerNode.tagNumber() != 0) {
            return innerNode.logError(
                "Expected 0 as a tag number for tagged choice");
        }

        if (innerNode.hasMore()) {
            // if shouldContinue returns false, then there is no selection
            rc  = innerNode.decodeChoice(variable);
            if (rc != bdem_BerDecoder::BDEM_BER_SUCCESS) {
                return rc;  // error message is already logged
            }
        }

        rc = innerNode.readTagTrailer();
    }
    else if (this->hasMore()) {

       // if shouldContinue returns false, then there is no selection
       rc = this->decodeChoice(variable);
    }

    return rc;
}

template <typename TYPE>
int
bdem_BerDecoder_Node::decode(TYPE *variable, bdeat_TypeCategory::NullableValue)
{
    int rc = bdem_BerDecoder::BDEM_BER_SUCCESS;

    if (d_formattingMode & bdeat_FormattingMode::BDEAT_NILLABLE) {
        // nillable is encoded in BER as a sequence with one optional element

        if (d_tagType != bdem_BerConstants::BDEM_CONSTRUCTED) {
            return logError("Expected CONSTRUCTED tag type for nullable");
        }

        if (hasMore()) {

            // If 'hasMore' returns false, then the nullable value is null.
            bdem_BerDecoder_Node innerNode(d_decoder);
//            innerNode.setType(variable);

            rc = innerNode.readTagHeader();
            if (rc != bdem_BerDecoder::BDEM_BER_SUCCESS) {
                return rc;  // error message is already logged
            }

            if (innerNode.tagClass()
                                 != bdem_BerConstants::BDEM_CONTEXT_SPECIFIC) {
                return innerNode.logError(
                    "Expected CONTEXT tag class for inner nillable");
            }

            if (innerNode.tagNumber() != 0) {
                return innerNode.logError(
                    "Expected 0 as tag number for inner nillable");
            }

            bdeat_NullableValueFunctions::makeValue(variable);

            rc = bdeat_NullableValueFunctions::manipulateValue(variable,
                                                               innerNode);
            if (rc != bdem_BerDecoder::BDEM_BER_SUCCESS) {
                return rc;  // error message is already logged
            }

            rc = innerNode.readTagTrailer();

        }  // this->hasMore()
        else {
            bdeat_ValueTypeFunctions::reset(variable);
        }
    }
    else {  // not bdeat_FormattingMode::BDEAT_NILLABLE
        bdeat_NullableValueFunctions::makeValue(variable);
        rc = bdeat_NullableValueFunctions::manipulateValue(variable, *this);
    }

    return rc;
}

template <typename TYPE>
int
bdem_BerDecoder_Node::decode(TYPE *variable,
                             bdeat_TypeCategory::CustomizedType)
{
    typedef typename
    bdeat_CustomizedTypeFunctions::BaseType<TYPE>::Type BaseType;

    BaseType base;

    typedef typename bdeat_TypeCategory::Select<BaseType>::Type BaseTag;
    int rc = this->decode(&base, BaseTag());

    if (rc != bdem_BerDecoder::BDEM_BER_SUCCESS) {
        return rc;  // error message is already logged
    }

    if (bdeat_CustomizedTypeFunctions::convertFromBaseType(variable,
                                                           base) != 0) {
        return logError("Error converting from base type for customized");
    }

    return bdem_BerDecoder::BDEM_BER_SUCCESS;
}

template <typename TYPE>
int
bdem_BerDecoder_Node::decode(TYPE *variable, bdeat_TypeCategory::Enumeration)
{
    int value;
    int rc = this->decode(&value, bdeat_TypeCategory::Simple());

    if (rc != bdem_BerDecoder::BDEM_BER_SUCCESS) {
        return rc;  // error message is already logged
    }

    if (0 != bdeat_EnumFunctions::fromInt(variable, value)) {

        return logError("Error converting enumeration value");
    }

    return bdem_BerDecoder::BDEM_BER_SUCCESS;
}

template <typename TYPE>
inline
int bdem_BerDecoder_Node::decode(TYPE *variable, bdeat_TypeCategory::Simple)
{
    if (d_tagType != bdem_BerConstants::BDEM_PRIMITIVE) {
        return logError("Expected PRIMITIVE tag type for simple type");
    }

    if (bdem_BerUtil::getValue(d_decoder->d_streamBuf,
                               variable,
                               d_expectedLength) != 0) {
        return logError("Error reading value for simple type");
    }

    d_consumedBodyBytes = d_expectedLength;

    return bdem_BerDecoder::BDEM_BER_SUCCESS;
}

template <typename TYPE>
int
bdem_BerDecoder_Node::decode(TYPE *variable, bdeat_TypeCategory::Sequence)
{
    if (d_tagType != bdem_BerConstants::BDEM_CONSTRUCTED) {
        return logError("Expected CONSTRUCTED tag type for sequence");
    }

    while (this->hasMore()) {

        bdem_BerDecoder_Node innerNode(d_decoder);

        int rc = innerNode.readTagHeader();
        if (rc != bdem_BerDecoder::BDEM_BER_SUCCESS) {
            return rc;  // error message is already logged
        }

        if (innerNode.tagClass() != bdem_BerConstants::BDEM_CONTEXT_SPECIFIC) {
            return innerNode.logError(
                "Expected CONTEXT tag class inside sequence");
        }

        if (bdeat_SequenceFunctions::hasAttribute(*variable,
                                                  innerNode.tagNumber())) {

            bdem_BerDecoder_NodeVisitor visitor(&innerNode);

            rc = bdeat_SequenceFunctions::manipulateAttribute(
                                                      variable,
                                                      visitor,
                                                      innerNode.tagNumber());
        }
        else {
            rc = innerNode.skipField();
        }

        if (rc != bdem_BerDecoder::BDEM_BER_SUCCESS) {
            return rc;  // error message is already logged
        }

        rc = innerNode.readTagTrailer();
        if (rc != bdem_BerDecoder::BDEM_BER_SUCCESS) {
            return rc;  // error message is already logged
        }
    }

    return bdem_BerDecoder::BDEM_BER_SUCCESS;
}

template <typename TYPE>
inline
int bdem_BerDecoder_Node::decode(TYPE *variable, bdeat_TypeCategory::Array)
{
    // Note: bsl::vector<char> is handled as a special case in the CPP file.
    return this->decodeArray(variable);
}

template <typename TYPE>
inline
int bdem_BerDecoder_Node::decode(TYPE *variable,
                                 bdeat_TypeCategory::DynamicType)
{
    return bdeat_TypeCategoryUtil::manipulateByCategory(variable, *this);
}

template <typename TYPE>
int bdem_BerDecoder_Node::decodeChoice(TYPE *variable)
{
    bdem_BerDecoder_Node innerNode(d_decoder);
//    innerNode.setType(variable);

    int rc = innerNode.readTagHeader();
    if (rc != bdem_BerDecoder::BDEM_BER_SUCCESS) {
        return rc;  // error message is already logged
    }

    if (innerNode.tagClass() != bdem_BerConstants::BDEM_CONTEXT_SPECIFIC) {
        return innerNode.logError(
            "Expected CONTEXT tag class for internal choice");
    }

    if (bdeat_ChoiceFunctions::hasSelection(*variable,
                                            innerNode.tagNumber())) {

        if (0 != bdeat_ChoiceFunctions::makeSelection(variable,
                                                      innerNode.tagNumber())) {

            return innerNode.logError("Unable to make choice selection");
        }

        bdem_BerDecoder_NodeVisitor visitor(&innerNode);

        rc = bdeat_ChoiceFunctions::manipulateSelection(variable, visitor);
    }
    else {
        rc = innerNode.skipField();
    }

    if (rc != bdem_BerDecoder::BDEM_BER_SUCCESS) {
        return rc;  // error message is already logged
    }

    return innerNode.readTagTrailer();
}

template <typename TYPE>
int
bdem_BerDecoder_Node::decodeArray(TYPE *variable)
{
    if (d_tagType != bdem_BerConstants::BDEM_CONSTRUCTED) {
        return logError("Expected CONSTRUCTED tag class for array");
    }

    const int maxSize = d_decoder->decoderOptions()->maxSequenceSize();

    int i = (int) bdeat_ArrayFunctions::size(*variable);
    while (this->hasMore()) {
        int j = i + 1;

        if (j > maxSize) {
            return logError("Array size exceeds the limit");
        }

        bdeat_ArrayFunctions::resize(variable, j);

        bdem_BerDecoder_UniversalElementVisitor visitor(d_decoder);
        int rc = bdeat_ArrayFunctions::manipulateElement(variable, visitor, i);
        if (rc != bdem_BerDecoder::BDEM_BER_SUCCESS) {
            return logError("Error in decoding array element");
        }
        i = j;
    }

    return bdem_BerDecoder::BDEM_BER_SUCCESS;
}

                 // -----------------------------------------
                 // private class bdem_BerDecoder_NodeVisitor
                 // -----------------------------------------

// CREATORS
inline
bdem_BerDecoder_NodeVisitor::
bdem_BerDecoder_NodeVisitor(bdem_BerDecoder_Node *node)
: d_node(node)
{
}

// MANIPULATORS
template <typename TYPE, typename INFO>
inline
int bdem_BerDecoder_NodeVisitor::operator()(TYPE *variable, const INFO& info)
{
    d_node->setFormattingMode(info.formattingMode());
    d_node->setFieldName(info.name());
//    d_node->setType(variable);

    return d_node->operator()(variable);
}

         // -----------------------------------------------------
         // private class bdem_BerDecoder_UniversalElementVisitor
         // -----------------------------------------------------

// CREATORS
inline
bdem_BerDecoder_UniversalElementVisitor::
bdem_BerDecoder_UniversalElementVisitor(bdem_BerDecoder *decoder)
: d_node(decoder)
{
}

// MANIPULATORS
template <typename TYPE>
int bdem_BerDecoder_UniversalElementVisitor::operator()(TYPE *variable)
{
    bdem_BerUniversalTagNumber::Value expectedTagNumber =
            bdem_BerUniversalTagNumber::select(*variable,
                                               d_node.formattingMode());

//    d_node.setType(variable);

    int rc = d_node.readTagHeader();
    if (rc != bdem_BerDecoder::BDEM_BER_SUCCESS) {
        return rc;  // error message is already logged
    }

    if (d_node.tagClass() != bdem_BerConstants::BDEM_UNIVERSAL) {
        return d_node.logError("Expected UNIVERSAL tag class");
    }

    if (d_node.tagNumber() != static_cast<int>(expectedTagNumber)) {
        return d_node.logError("Unexpected tag number");
    }

    rc = d_node(variable);

    if (rc != bdem_BerDecoder::BDEM_BER_SUCCESS) {
        return rc;
    }

    rc = d_node.readTagTrailer();

    return rc;
}

}  // close namespace BloombergLP

#endif

// ---------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2007
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ---------------------------------
