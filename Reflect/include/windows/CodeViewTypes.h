#pragma once
#include <stdint.h>

#define LF_ARRAY        0x00001503
#define LF_BITFIELD     0x00001205
#define LF_CLASS        0x00001504
#define LF_STRUCTURE    0x00001505
#define LF_UNION        0x00001506
#define LF_ENUM         0x00001507
#define LF_POINTER      0x00001002
#define LF_PROCEDURE    0x00001008
#define LF_MFUNCTION    0x00001009
#define LF_ARGLIST      0x00001201
#define LF_VTSHAPE      0x0000000A
#define LF_FIELDLIST    0x00001203

// ID leaf records. Subsequent leaf types may be referenced from .debug$S.
#define LF_FUNC_ID 0x1601
#define LF_MFUNC_ID 0x1602
#define LF_BUILDINFO 0x1603
#define LF_SUBSTR_LIST 0x1604
#define LF_STRING_ID 0x1605
#define LF_UDT_SRC_LINE 0x1606
#define LF_UDT_MOD_SRC_LINE 0x1607

#define S_PUB32         0x0000110e

#define LF_BCLASS       0x00001400
#define LF_BINTERFACE   0x0000151a
#define LF_VBCLASS      0x00001401
#define LF_IVBCLASS     0x00001402
#define LF_VFUNCTAB     0x00001409
#define LF_STMEMBER     0x0000150e
#define LF_METHOD       0x0000150f
#define LF_MEMBER       0x0000150d
#define LF_NESTTYPE     0x00001510
#define LF_ONEMETHOD    0x00001511
#define LF_ENUMERATE    0x00001502
#define LF_INDEX        0x00001404

#define LF_NUMERIC      0x00008000
#define LF_CHAR         0x00008000
#define LF_SHORT        0x00008001
#define LF_USHORT       0x00008002
#define LF_LONG         0x00008003
#define LF_ULONG        0x00008004
#define LF_REAL32       0x00008005
#define LF_REAL64       0x00008006
#define LF_REAL80       0x00008007
#define LF_REAL128      0x00008008
#define LF_QUADWORD     0x00008009
#define LF_UQUADWORD    0x0000800a
#define LF_REAL48       0x0000800b
#define LF_COMPLEX32    0x0000800c
#define LF_COMPLEX64    0x0000800d
#define LF_COMPLEX80    0x0000800e
#define LF_COMPLEX128   0x0000800f
#define LF_VARSTRING    0x00008010
#define LF_OCTWORD      0x00008017
#define LF_UOCTWORD     0x00008018
#define LF_DECIMAL      0x00008019
#define LF_DATE         0x0000801a
#define LF_UTF8STRING   0x0000801b
#define LF_REAL16       0x0000801c

typedef uint16_t CodeViewMemberAttributes;
typedef uint32_t CodeViewTypeIndex;
typedef uint16_t LeafType;
typedef uint16_t LeafLength;

uint16_t getMethodKind(CodeViewMemberAttributes attributes) {
    return (attributes & 0x1c) >> 2;
}

// Might need expanding when things start to fail
size_t size_of_numerical_leaf(void* numericalLeaf) {
    uint16_t firstBytes = *(uint16_t*)numericalLeaf;
    if (firstBytes < 0x8000) return 2;

    switch (firstBytes) {
        case LF_CHAR:
            return 3;
            break;
        case LF_SHORT:
        case LF_USHORT:
            return 4;
            break;
        case LF_LONG:
        case LF_ULONG:
            return 6;
            break;
        default:
            return 2;
            break;
    }
}

#pragma pack(push,1)
struct CodeViewRecordHeader {
    LeafLength recordLength;
    LeafType recordKind;
};

struct CodeViewPublicSymbol {
    struct CodeViewRecordHeader record;
    uint32_t pubSymFlags;
    uint32_t offset;
    uint16_t seg;
};

// LF_STRUCTURE
struct CodeViewStruct {
    struct CodeViewRecordHeader record;
    uint16_t count;
    uint16_t properties; // Classoptions bitset
    CodeViewTypeIndex field; // LF_FIELD
    CodeViewTypeIndex dlist; // LF_DERIVEDD
    CodeViewTypeIndex vshape; // LF_VTSHAPE
};

// LF_UNION
struct CodeViewUnionType {
    struct CodeViewRecordHeader record;
    uint16_t memberCount;
    uint16_t properties; // Classoptions bitset
    CodeViewTypeIndex fieldList;
    // LF_NUMERIC size in bytes
    // char name[];
};

// LF_POINTER
struct CodeViewPointerType {
    struct CodeViewRecordHeader record;
    CodeViewTypeIndex pointerType;
    uint32_t attributes;
};

// LF_PROCEDURE
struct CodeViewProcedure {
    struct CodeViewRecordHeader record;
    CodeViewTypeIndex returnType;
    uint8_t callingConvection;
    uint8_t functionOptions;
    uint16_t numParameters;
    CodeViewTypeIndex argListType;
};

// LF_MFUNCTION
struct CodeViewMemberFunction {
    struct CodeViewRecordHeader record;
    CodeViewTypeIndex returnType;
    CodeViewTypeIndex classType;
    CodeViewTypeIndex thisType;
    uint8_t callingConvection;
    uint8_t functionOptions;
    uint16_t numParameters;
    CodeViewTypeIndex argListType;
    uint32_t thisAdjustment;
};

// LF_ARRAY
struct CodeViewArrayType {
    struct CodeViewRecordHeader record;
    CodeViewTypeIndex elementType;
    CodeViewTypeIndex indexType;
    // LF_NUMERIC encoded size in bytes
    // char name[];
};

// LF_ENUM
struct CodeViewEnumType {
    struct CodeViewRecordHeader record;
    uint16_t NumEnumerators; // Number of enumerators
    uint16_t Properties;
    CodeViewTypeIndex UnderlyingType;
    CodeViewTypeIndex FieldListType;
    // char name[];
};

struct CodeViewPointerToMember {
    CodeViewTypeIndex classType;
    uint16_t representation;
};


// LF_POINTER
struct CodeViewPointer {
    struct CodeViewRecordHeader record;
    CodeViewTypeIndex pointeeType;
    uint32_t attributes;
    // If pointer to member add CodeViewPointerToMember
};

struct CodeViewFuncID {
    struct CodeViewRecordHeader record;
    CodeViewTypeIndex ParentScope;
    CodeViewTypeIndex FunctionType;
    // Name: The null-terminated name follows.
};

// LF_ARGLIST
struct CodeViewArgList {
    struct CodeViewRecordHeader record;
    uint32_t numArguments;
    // CodeViewTypeIndex indices[];
};

// LF_NESTED_TYPE
struct CodeViewNestedType {
    CodeViewMemberAttributes attributes;
    CodeViewTypeIndex typeIndex;
    // char name[];
};

// LF_ONEMETHOD
struct CodeViewOneMethod {
    CodeViewMemberAttributes attributes;
    CodeViewTypeIndex type;
    // is introduced virtual method
    // int32_t offset;
    // char name[];
};
// LF_METHOD
struct CodeViewOverloadedMethod {
    uint16_t methodCount;
    CodeViewTypeIndex type;
    // char name[];
};

// LF_VFFUNCTAB
struct CodeViewVirtualFunctionPointer {
    uint16_t padding;
    CodeViewTypeIndex type;
};

// LF_MEMBER
struct CodeViewDataMember {
    CodeViewMemberAttributes attributes;
    CodeViewTypeIndex type;
    // LF_NUMERIC byte offset
    // char name[];
};

// LF_STATIC_MEMBER
struct CodeViewStaticDataMember {
    CodeViewMemberAttributes attributes;
    CodeViewTypeIndex type;
    // char name[]
};

// LF_ENUMERATE
struct CodeViewEnumerator {
    CodeViewMemberAttributes attributes;
    // LF_NUMERIC value
    // char name[];
};

// LF_BFCLASS, LF_BINTERFACE
struct CodeViewBaseClass {
    CodeViewMemberAttributes attributes;
    CodeViewTypeIndex type;
    // LF_NUMERIC baseOffset offset of base from derived
};

// LF_VBCLASS | LV_IVBCLASS
struct CodeViewVirtualBaseClass {
    CodeViewMemberAttributes attributes;
    CodeViewTypeIndex baseType;
    CodeViewTypeIndex virtualBasePointerType;
    // LF_NUMERIC virtual base Pointer Offset
    // LF_NUMERIC Virtual base table index
};

struct CodeViewIndex {
    uint16_t padding;
    CodeViewTypeIndex continuationIndex;
};
#pragma pack(pop)