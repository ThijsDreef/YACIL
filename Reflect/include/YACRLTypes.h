#pragma once
#include <stdint.h>
#include "GrowAbleStream.h"


typedef uint32_t YACRLTypeIndex;
// #define TYPE_OFFSET 0x1000

enum YACRLTypes {
    STRUCT,
    POINTER,
    ENUM,
    FUNCTION,
    UNION,
};

enum YACRLKindNumeric {
    CHAR,
    SHORT,
    USHORT,
    LONG,
    ULONG,
};
#define MAX_SIZE_NUMERIC sizeof (unsigned long)

/**
 * YACRL uses the same enums as codeview
*/

enum YACRLBaseTypeMode {
    Direct = 0x00000000,                // Not a pointer
    NearPointer = 0x00000100,           // Near pointer
    FarPointer = 0x00000200,            // Far pointer
    HugePointer = 0x00000300,           // Huge pointer
    NearPointer32 = 0x00000400,         // 32 bit near pointer
    FarPointer32 = 0x00000500,          // 32 bit far pointer
    NearPointer64 = 0x00000600,         // 64 bit near pointer
    NearPointer128 = 0x00000700         // 128 bit near pointer
};

enum YACRLPointerMode {
    Pointer = 0x00,                 // "normal" pointer
    LValueReference = 0x01,         // "old" reference
    PointerToDataMember = 0x02,     // pointer to data member
    PointerToMemberFunction = 0x03, // pointer to member function
    RValueReference = 0x04          // r-value reference
};

enum YACRLMethodKind {
    Vanilla = 0x00,
    Virtual = 0x01,
    Static = 0x02,
    Friend = 0x03,
    IntroducingVirtual = 0x04,
    PureVirtual = 0x05,
    PureIntroducingVirtual = 0x06
};

enum YACRLBaseTypeKind {
    None = 0x0000,                      // uncharacterized type (no type)
    Void = 0x0003,                      // void
    NotTranslated = 0x0007,             // type not translated by cvpack
    HResult = 0x0008,                   // OLE/COM HRESULT

    SignedCharacter = 0x0010,           // 8 bit signed
    UnsignedCharacter = 0x0020,         // 8 bit unsigned
    NarrowCharacter = 0x0070,           // really a char
    WideCharacter = 0x0071,             // wide char
    Character16 = 0x007a,               // char16_t
    Character32 = 0x007b,               // char32_t

    SByte = 0x0068,                     // 8 bit signed int
    Byte = 0x0069,                      // 8 bit unsigned int
    Int16Short = 0x0011,                // 16 bit signed
    UInt16Short = 0x0021,               // 16 bit unsigned
    Int16 = 0x0072,                     // 16 bit signed int
    UInt16 = 0x0073,                    // 16 bit unsigned int
    Int32Long = 0x0012,                 // 32 bit signed
    UInt32Long = 0x0022,                // 32 bit unsigned
    Int32 = 0x0074,                     // 32 bit signed int
    UInt32 = 0x0075,                    // 32 bit unsigned int
    Int64Quad = 0x0013,                 // 64 bit signed
    UInt64Quad = 0x0023,                // 64 bit unsigned
    Int64 = 0x0076,                     // 64 bit signed int
    UInt64 = 0x0077,                    // 64 bit unsigned int
    Int128Oct = 0x0014,                 // 128 bit signed int
    UInt128Oct = 0x0024,                // 128 bit unsigned int
    Int128 = 0x0078,                    // 128 bit signed int
    UInt128 = 0x0079,                   // 128 bit unsigned int

    Float16 = 0x0046,                   // 16 bit real
    Float32 = 0x0040,                   // 32 bit real
    Float32PartialPrecision = 0x0045,   // 32 bit PP real
    Float48 = 0x0044,                   // 48 bit real
    Float64 = 0x0041,                   // 64 bit real
    Float80 = 0x0042,                   // 80 bit real
    Float128 = 0x0043,                  // 128 bit real

    Complex16 = 0x0056,                 // 16 bit complex
    Complex32 = 0x0050,                 // 32 bit complex
    Complex32PartialPrecision = 0x0055, // 32 bit PP complex
    Complex48 = 0x0054,                 // 48 bit complex
    Complex64 = 0x0051,                 // 64 bit complex
    Complex80 = 0x0052,                 // 80 bit complex
    Complex128 = 0x0053,                // 128 bit complex

    Boolean8 = 0x0030,                  // 8 bit boolean
    Boolean16 = 0x0031,                 // 16 bit boolean
    Boolean32 = 0x0032,                 // 32 bit boolean
    Boolean64 = 0x0033,                 // 64 bit boolean
    Boolean128 = 0x0034,                // 128 bit boolean
};

/**
 * YACRL Numeric type
 * holds kind and value
*/
struct YACRLNumeric {
    enum YACRLKindNumeric numeric;
    uint32_t data;
};

/**
 * YACRL Header type
 * A header in the type stream to denote the length and kind of structure
*/
struct YACRLHeader {
    uint16_t length;
    uint16_t kind;
};

// /**
//  * YACRL Member type
//  * points to type of member using typeindex.
// */ 
// struct YACRLMemberType {
//     uint32_t typeIndex;
//     // enum YACRLKindNumeric offset;
//     // char name[];
// };
;

// /**
//  * YACRL Struct type
//  * Struct descriptor
// */ 
// struct YACRLStruct {
//     struct YACRLHeader header;
//     uint16_t count;
//     // char name[];
//     // struct YACRLMemberType members[];
// };

struct YACRLArray {
    struct YACRLHeader header;
    YACRLTypeIndex elementType;
    YACRLTypeIndex indexType;
    uint32_t size;
    char name[];
};

/**
 * YACRL Pointer type
 * Pointer descriptor
*/ 
struct YACRLPointer {
    struct YACRLHeader header;
    YACRLTypeIndex pointeeTypeIndex;
    uint32_t attributes;
};

// /**
//  * YACRL function type
//  * Function descriptor names of parameters are unavailable
// */
// struct YACRLFunction {
//     struct YACRLHeader header;
//     uint32_t returnType;
//     uint8_t callingConvention;
//     uint16_t numParameters;
//     // char name[];
//     // uint32_t parameterIndices[];
// };

struct YACRLEnumMember {
    struct YACRLNumeric value;
    char name[];
};

/**
 * YACRL Enum type
 * Holds all info for a enumerator
*/
struct YACRLEnum {
    struct YACRLHeader header;
    YACRLTypeIndex typeValue;
    uint32_t numEnumerators;
    char name[];
    // List of enum members follows.
};

// /**
//  * YACRL union type
//  * Describes a union
// */ah
// struct YACRLUnionType {
//     struct YACRLHeader header;
//     uint16_t memberCount;
//     // struct YACRLNumeric size
//     // char name[];
//     // struct YACRLMemberType members[];
// };

/**
 * YACRL Module
 * Hold all type information loaded from the module.
*/
struct YACRLModule {
    struct GrowAbleStream data;
    struct YACRLHeader** types;
    uint32_t count;
    char* name;
    char* loadedModule; 
    uint64_t timestamp;  
};