// baexml_configschema.cpp   -*-C++-*-   GENERATED FILE -- DO NOT EDIT

#include <bdes_ident.h>
BDES_IDENT_RCSID(baexml_configschema_cpp,"$Id$ $CSID$")

#include <baexml_configschema.h>

namespace BloombergLP {

// CONSTANTS

const char baexml_ConfigSchema::TEXT[] =
"<?xml version='1.0' encoding='UTF-8'?>\n"
"<xs:schema xmlns:xs='http://www.w3.org/2001/XMLSchema'\n"
"           xmlns:bdem='http://bloomberg.com/schemas/bdem'\n"
"           bdem:package='baexml'\n"
"           elementFormDefault='qualified'>\n"
"\n"
"  <!-- NOTE: Do not run bas_codegen.pl directly on this file.\n"
"       Instead, run baexml_generateoptions.pl.  The latter script will\n"
"       massage the output so that the classes are prefixed with 'baexml_'\n"
"       instead of being put into a 'baexml' namespace.\n"
"  -->\n"
"\n"
"  <xs:annotation>\n"
"    <xs:documentation>\n"
"      Structures that control the XML encoding and decoding process\n"
"    </xs:documentation>\n"
"  </xs:annotation>\n"
"\n"
"  <xs:simpleType name='EncodingStyle' bdem:preserveEnumOrder='1'>\n"
"    <xs:annotation>\n"
"      <xs:documentation>\n"
"        Enumeration of encoding style (COMPACT or PRETTY).\n"
"\n"
"        This struct is generated using baexml_generateoptions.pl\n"
"\n"
"      </xs:documentation>\n"
"    </xs:annotation>\n"
"    <xs:restriction base='xs:string'>\n"
"      <xs:enumeration value='BAEXML_COMPACT'/>\n"
"      <xs:enumeration value='BAEXML_PRETTY'/>\n"
"    </xs:restriction>\n"
"  </xs:simpleType>\n"
"\n"
"  <xs:complexType name='EncoderOptions'>\n"
"    <xs:annotation>\n"
"      <xs:documentation>\n"
"        Options for performing XML encodings.\n"
"        Encoding style is either COMPACT or PRETTY.  If encoding style is\n"
"        COMPACT, no whitespace will be added between elements.  If encoding\n"
"        style is 'PRETTY', then the 'InitialIndentLevel', 'SpacesPerLevel',\n"
"        and 'WrapColumn' parameters are used to specify the formatting of\n"
"        the output.  Note that 'InitialIndentLevel', 'SpacesPerLevel', and\n"
"        'WrapColumn' are ignored when 'EncodingStyle' is COMPACT (this is\n"
"        the default).\n"
"\n"
"        This class is generated using baexml_generateoptions.pl\n"
"\n"
"      </xs:documentation>\n"
"    </xs:annotation>\n"
"    <xs:sequence>\n"
"      <xs:element name='ObjectNamespace' type='xs:string'\n"
"            minOccurs='0' maxOccurs='1'\n"
"            bdem:isNullable='0'\n"
"            bdem:allowsDirectManipulation='0'>\n"
"        <xs:annotation>\n"
"          <xs:documentation>\n"
"            namespace where object is defined\n"
"          </xs:documentation>\n"
"        </xs:annotation>\n"
"      </xs:element>\n"
"      <xs:element name='SchemaLocation' type='xs:string'\n"
"            minOccurs='0' maxOccurs='1'\n"
"            bdem:isNullable='0'\n"
"            bdem:allowsDirectManipulation='0'>\n"
"        <xs:annotation>\n"
"          <xs:documentation>location of the schema</xs:documentation>\n"
"\n"
"        </xs:annotation>\n"
"      </xs:element>\n"
"      <xs:element name='Tag' type='xs:string'\n"
"            minOccurs='0' maxOccurs='1'\n"
"            bdem:isNullable='0'\n"
"            bdem:allowsDirectManipulation='0'>\n"
"        <xs:annotation>\n"
"          <xs:documentation>tag for top level</xs:documentation>\n"
"        </xs:annotation>\n"
"      </xs:element>\n"
"      <xs:element name='FormattingMode' type='xs:int'\n"
"            minOccurs='0' maxOccurs='1'\n"
"            default='0'\n"
"            bdem:cppdefault='bdeat_FormattingMode::DEFAULT'\n"
"            bdem:cpptype='int'\n"
"            bdem:allocatesMemory='0'\n"
"            bdem:allowsDirectManipulation='0'>\n"
"        <xs:annotation>\n"
"          <xs:documentation>Formatting mode</xs:documentation>\n"
"        </xs:annotation>\n"
"      </xs:element>\n"
"      <xs:element name='InitialIndentLevel' type='xs:int'\n"
"            minOccurs='0' maxOccurs='1'\n"
"            default='0'\n"
"            bdem:allowsDirectManipulation='0'>\n"
"        <xs:annotation>\n"
"          <xs:documentation>initial indentation level</xs:documentation>\n"
"        </xs:annotation>\n"
"      </xs:element>\n"
"      <xs:element name='SpacesPerLevel' type='xs:int'\n"
"            minOccurs='0' maxOccurs='1'\n"
"            default='4'\n"
"            bdem:allowsDirectManipulation='0'>\n"
"        <xs:annotation>\n"
"          <xs:documentation>\n"
"            spaces per level of indentation\n"
"          </xs:documentation>\n"
"        </xs:annotation>\n"
"      </xs:element>\n"
"      <xs:element name='WrapColumn' type='xs:int'\n"
"            minOccurs='0' maxOccurs='1'\n"
"            default='80'\n"
"            bdem:allowsDirectManipulation='0'>\n"
"        <xs:annotation>\n"
"          <xs:documentation>\n"
"            number of characters to wrap text\n"
"          </xs:documentation>\n"
"        </xs:annotation>\n"
"      </xs:element>\n"
"      <xs:element name='MaxDecimalTotalDigits' type='xs:int'\n"
"            minOccurs='0' maxOccurs='1'\n"
"            bdem:allowsDirectManipulation='0'>\n"
"        <xs:annotation>\n"
"          <xs:documentation>\n"
"            Maximum total digits of the decimal value that should be\n"
"            displayed\n"
"          </xs:documentation>\n"
"        </xs:annotation>\n"
"      </xs:element>\n"
"      <xs:element name='MaxDecimalFractionDigits' type='xs:int'\n"
"            minOccurs='0' maxOccurs='1'\n"
"            bdem:allowsDirectManipulation='0'>\n"
"        <xs:annotation>\n"
"          <xs:documentation>\n"
"            Maximum fractional digits of the decimal value that should be\n"
"            displayed\n"
"          </xs:documentation>\n"
"        </xs:annotation>\n"
"      </xs:element>\n"
"      <xs:element name='SignificantDoubleDigits' type='xs:int'\n"
"            minOccurs='0' maxOccurs='1'\n"
"            bdem:allowsDirectManipulation='0'>\n"
"        <xs:annotation>\n"
"          <xs:documentation>\n"
"            The number of significant digits that must be displayed for the\n"
"            double value.\n"
"          </xs:documentation>\n"
"        </xs:annotation>\n"
"      </xs:element>\n"
"      <xs:element name='EncodingStyle'\n"
"                  type='EncodingStyle'\n"
"                  minOccurs='0'\n"
"                  default='BAEXML_COMPACT'\n"
"                  bdem:allowsDirectManipulation='0'>\n"
"        <xs:annotation>\n"
"          <xs:documentation>\n"
"            encoding style (see component-level doc)\n"
"          </xs:documentation>\n"
"        </xs:annotation>\n"
"      </xs:element>\n"
"      <xs:element name='AllowControlCharacters' type='xs:boolean'\n"
"            minOccurs='0' maxOccurs='1'\n"
"            default='false'\n"
"            bdem:allocatesMemory='0'\n"
"            bdem:allowsDirectManipulation='0'>\n"
"        <xs:annotation>\n"
"          <xs:documentation>\n"
"            Allow control characters to be encoded.\n"
"          </xs:documentation>\n"
"        </xs:annotation>\n"
"      </xs:element>\n"
"      <xs:element name='OutputXMLHeader' type='xs:boolean'\n"
"                  minOccurs='0' maxOccurs='1' default='true'>\n"
"        <xs:annotation>\n"
"          <xs:documentation>\n"
"            This option controls if the baexml encoder should output the XML\n"
"            header.\n"
"          </xs:documentation>\n"
"        </xs:annotation>\n"
"      </xs:element>\n"
"      <xs:element name='OutputXSIAlias' type='xs:boolean'\n"
"                  minOccurs='0' maxOccurs='1' default='true'>\n"
"        <xs:annotation>\n"
"          <xs:documentation>\n"
"            This option controls if the baexml encoder should output the XSI\n"
"            alias with the top-level element.\n"
"          </xs:documentation>\n"
"        </xs:annotation>\n"
"      </xs:element>\n"
"    </xs:sequence>\n"
"  </xs:complexType>\n"
"\n"
"  <xs:complexType name='DecoderOptions'>\n"
"    <xs:annotation>\n"
"      <xs:documentation>\n"
"        Options for controlling the XML decoding process.\n"
"      </xs:documentation>\n"
"    </xs:annotation>\n"
"    <xs:sequence>\n"
"      <xs:element name='MaxDepth' type='xs:int'\n"
"            minOccurs='0' maxOccurs='1'\n"
"            default='32'\n"
"            bdem:allowsDirectManipulation='0'>\n"
"        <xs:annotation>\n"
"          <xs:documentation>maximum recursion depth</xs:documentation>\n"
"        </xs:annotation>\n"
"      </xs:element>\n"
"      <xs:element name='FormattingMode' type='xs:int'\n"
"            minOccurs='0' maxOccurs='1'\n"
"            default='0'\n"
"            bdem:cppdefault='bdeat_FormattingMode::DEFAULT'\n"
"            bdem:cpptype='int'\n"
"            bdem:allocatesMemory='0'\n"
"            bdem:allowsDirectManipulation='0'>\n"
"        <xs:annotation>\n"
"          <xs:documentation>Formatting mode</xs:documentation>\n"
"        </xs:annotation>\n"
"      </xs:element>\n"
"      <xs:element name='SkipUnknownElements' type='xs:boolean'\n"
"            minOccurs='0' maxOccurs='1'\n"
"            default='true'\n"
"            bdem:allocatesMemory='0'\n"
"            bdem:allowsDirectManipulation='0'>\n"
"        <xs:annotation>\n"
"          <xs:documentation>\n"
"            Option to skip unknown elements\n"
"          </xs:documentation>\n"
"        </xs:annotation>\n"
"      </xs:element>\n"
"    </xs:sequence>\n"
"  </xs:complexType>\n"
"\n"
"</xs:schema>\n"
;

}  // close namespace BloombergLP

// GENERATED BY BLP_BAS_CODEGEN_3.6.9 Fri Jan 20 14:15:01 2012
// ----------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2012
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ------------------------------ END-OF-FILE ---------------------------------
