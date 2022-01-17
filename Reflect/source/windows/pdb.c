#include "windows/pdb.h"
#include "windows/msf.h"
#include "windows/CodeViewTypes.h"
#include "YACRLTypes.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define OLD_DIRECTORY 0
#define PDB_INDEX 1
#define TPI_INDEX 2
#define DBI_INDEX 3
#define IPI_INDEX 4

int isPointerToMember(struct CodeViewPointer pointer) {
    size_t pointerMode = pointer.attributes >> 5 & 0x7;
    return pointerMode == PointerToDataMember || pointerMode == PointerToMemberFunction;
}

uint8_t getPointerSize(struct CodeViewPointer pointer) {
    return pointer.attributes >> 13 & 0xff; 
}


void print_all_public_symbols(struct Msf msf) {
    struct MsfStream dbiStream = parse_msf_stream(&msf, DBI_INDEX);
    struct DBIStreamHeader* dbiStreamHeader = dbiStream.data;
    uint32_t pubsymStreamIndex = dbiStreamHeader->SymRecordStream;
    printf("pubstreamindex %d \n", pubsymStreamIndex);
    struct MsfStream publicSymbolStream = parse_msf_stream(&msf, pubsymStreamIndex);
    uint8_t* stream = publicSymbolStream.data;
    printf("size in bytes of stream: %d \n", publicSymbolStream.size);
    for (size_t i = 0; i < publicSymbolStream.size; ) {
        struct CodeViewRecordHeader* record = stream + i;
        switch (record->recordKind) {
            case S_PUB32:
                // struct CodeViewPublicSymbol* symbol = stream + i;
                printf("found symbol: %s\n", stream + i + sizeof (struct CodeViewPublicSymbol));
                break;
            default:
                break;
        }
        i += record->recordLength + 2;
    }
}

void print_field_member_names(uint8_t* stream, size_t size) {
    for (size_t i = 0; i < size; ) {
        uint8_t padding;
        int offset = 0;
        do {
            padding = *(stream + offset + i);
            offset += 1;
            
        } while ((padding >= 0xf0 && padding <= 0xff));
        i += offset - 1;
        if (i >= size) return;
      
        uint16_t leaf = *(uint16_t*)(stream + i);
        i += sizeof(uint16_t);
        switch (leaf) {
            case LF_MEMBER:
                i += sizeof(struct CodeViewDataMember);
                i += size_of_numerical_leaf(stream + i);
                printf("name of member %s\n", stream + i);
                i += strlen(stream + i) + 1;
                break;
            case LF_ENUMERATE:
                i += sizeof(struct CodeViewEnumerator);
                i += size_of_numerical_leaf(stream + i);
                printf("name of enum member %s\n", stream + i);
                i += strlen(stream + i) + 1;
                break;
            case LF_BCLASS:
                i += sizeof(struct CodeViewBaseClass);
                i += size_of_numerical_leaf(stream + i);
                break;
            case LF_VFUNCTAB:
                i += sizeof(struct CodeViewVirtualFunctionPointer);
                break;
            case LF_ONEMETHOD:
                struct CodeViewOneMethod* info = (struct CodeViewOneMethod*)(stream + i);
                i += sizeof(struct CodeViewOneMethod);
                uint8_t mask = getMethodKind(info->attributes);
                if (
                    mask == IntroducingVirtual ||
                    mask == PureIntroducingVirtual
                ) {
                    i += sizeof(int32_t);
                }
                printf("One method name %s \n", stream + i);
                i += strlen(stream + i) + 1;
                break;
            case LF_STMEMBER:
                i += sizeof(struct CodeViewStaticDataMember);
                printf("Static member name %s \n", stream + i);
                i += strlen(stream + i) + 1;
                break;
            case LF_METHOD:
                i += sizeof(struct CodeViewOverloadedMethod);
                printf("name of method %s \n", stream + i);
                i += strlen(stream + i) + 1;
                break;
            case LF_NESTTYPE:
                i += sizeof(struct CodeViewNestedType);
                printf("name of nested type %s\n", stream + i);
                i += strlen(stream + i) + 1;
                break;
            case LF_INDEX:
                struct CodeViewIndex* index = stream + i;
                printf("List continous at: 0x%x", index->continuationIndex);
                i += sizeof(struct CodeViewIndex);
                break;
            default:
                i += 1;
                break;
        }
    }
}

void print_all_structure_names(struct Msf msf) {
    struct MsfStream tpiStream = parse_msf_stream(&msf, TPI_INDEX);
    struct TPIStreamHeader* tpiStreamHeader = tpiStream.data;

    uint8_t* stream = tpiStream.data;
    stream += sizeof(struct TPIStreamHeader);
    uint32_t typeSize = tpiStreamHeader->TypeIndexEnd - tpiStreamHeader->TypeIndexBegin;
    for (unsigned int i = 0; i < typeSize; i++) {
        struct CodeViewRecordHeader* record = stream;
        switch (record->recordKind) {
            case LF_CLASS:
            case LF_STRUCTURE:
                struct CodeViewStruct* foundStruct = stream;
                printf("Found struct: %s\nfieldlist id 0x%x\nitem count %d\nType index: 0x%x\n\n", stream + sizeof(struct CodeViewStruct) + 2, foundStruct->field, foundStruct->count, tpiStreamHeader->TypeIndexBegin + i);
                break;
            case LF_FIELDLIST:
                printf("found field list with ID 0x%x, with size: %d\n", tpiStreamHeader->TypeIndexBegin + i, record->recordLength);
                print_field_member_names(stream + sizeof(struct CodeViewRecordHeader), record->recordLength - 2);
                printf("\n");
                break;
            case LF_PROCEDURE:
                struct CodeViewProcedure* procedure = stream;
                printf("Found procedure\n");
                printf("ReturnType is: 0x%x\n", procedure->returnType);
                printf("Arg list type: 0x%x\n", procedure->argListType);
                printf("Number of parameters is: %d\n\n", procedure->numParameters);
                break;
            case LF_MFUNCTION:
                struct CodeViewMemberFunction* memberFunction = stream;
                printf("Found memberfunction\n");
                printf("ReturnType is: 0x%x\n", memberFunction->returnType);
                printf("Arg list type: 0x%x\n", memberFunction->argListType);
                printf("Number of parameters is: %d\n\n", memberFunction->numParameters);
                break;
            case LF_UNION:
                struct CodeViewUnionType* uinionInfo = stream;
                printf("Found union\n");
                printf("Name of union: %s\n", stream + sizeof(struct CodeViewUnionType) + size_of_numerical_leaf(stream + sizeof(struct CodeViewUnionType)));
                break;
            case LF_ARRAY:
                struct CodeViewArrayType* arrayTypeInfo = stream;
                printf("Found array type\n");
                printf("Element type: 0x%x\n", arrayTypeInfo->elementType);
                printf("Name of array: %s\n\n", stream + sizeof(struct CodeViewArrayType) + size_of_numerical_leaf(stream + sizeof(struct CodeViewArrayType)));
                break;
            case LF_ENUM:
                struct CodeViewEnumType* enumType = stream;
                printf("Found enum type\n");
                printf("Name of enum: %s\n\n", stream + sizeof(struct CodeViewEnumType));
                break;
            case LF_ARGLIST:
                struct CodeViewArgList* arglist = stream;
                printf("Found arglist 0x%x\n", tpiStreamHeader->TypeIndexBegin + i);
                printf("number of arguments %d", arglist->numArguments);
                uint32_t* indices = stream + sizeof(struct CodeViewArgList);
                if (arglist->numArguments * sizeof(uint32_t) > record->recordLength) break;
                for (size_t j = 0; j < arglist->numArguments; j++) {
                    printf("arg %d, type 0x%x\n", j, indices[j]);
                }
                printf("\n");
                break;
            case LF_POINTER:
                struct CodeViewPointer* pointerInfo = stream;
                size_t offset = sizeof(struct CodeViewPointerToMember) * isPointerToMember(*pointerInfo);
                printf("Found pointer to type 0x%x\n\n", pointerInfo->pointeeType);
                break;
            case LF_VTSHAPE:
            default:
                break;
        }
        stream += record->recordLength + 2;
    }


    struct MsfStream ipiStream = parse_msf_stream(&msf, IPI_INDEX);
    struct TPIStreamHeader* ipiStreamHeader = ipiStream.data;
    stream = ipiStream.data;
    stream += sizeof(struct TPIStreamHeader);
    typeSize = tpiStreamHeader->TypeIndexEnd - tpiStreamHeader->TypeIndexBegin;
    for (size_t i = 0; i < typeSize; i++) {
        struct CodeViewRecordHeader* record = stream;
        switch (record->recordKind) {
            case LF_MFUNC_ID:
            case LF_FUNC_ID:
                printf("found function with name: %s\n", stream + sizeof(struct CodeViewFuncID));
                break;
            case LF_BUILDINFO:
            case LF_SUBSTR_LIST:
            case LF_STRING_ID:
            case LF_UDT_MOD_SRC_LINE:
            case LF_UDT_SRC_LINE:
            default:
                break;
        }
        stream += record->recordLength + 2;
    }
    


    free_msf_stream(tpiStream);
    free_msf_stream(ipiStream);
}


void load_pdb_file(const char* filename) {
    struct Msf msf = load_msf_file(filename);

    print_all_structure_names(msf);
    free_msf(&msf);
}

enum YACRLKindNumeric parse_numeric(void* numericalLeaf) {
    uint16_t firstBytes = *(uint16_t*)numericalLeaf;
    if (firstBytes < 0x8000) return (enum YACRLKindNumeric)USHORT;

    switch (firstBytes) {
        case LF_CHAR:
            return CHAR;
        case LF_SHORT:
            return SHORT;
        case LF_USHORT:
            return USHORT;
        case LF_LONG:
            return LONG;
        case LF_ULONG:
            return ULONG;
        default:
            return 0;
    }
}

uint32_t getNumericAsUint32(void* numericalLeaf) {
    uint16_t firstBytes = *(uint16_t*)numericalLeaf;
    if (firstBytes < 0x8000) return firstBytes;
    void* afterID = ((uint8_t*)numericalLeaf) + 2;
    uint32_t value;
    switch (firstBytes) {
        case LF_CHAR:
            value = *(char*)afterID;
            break;
        case LF_SHORT:
            value = *(int16_t*)afterID;
            break;
        case LF_USHORT:
            value = *(uint16_t*)afterID;
            break;
        case LF_LONG:
            value = *(int32_t*)afterID;
            break;
        case LF_ULONG:
            value = *(uint32_t*)afterID;
            break;
        default:
            break;
    }
    return value;
}

GrowableStreamHandle parse_enumerator(struct CodeViewEnumType* enumData, struct CodeViewRecordHeader** lookUpTable, struct YACRLModule* module) {
    char* typeName = (char*)enumData + sizeof(struct CodeViewEnumType);
    // setup for parsed data
    size_t recordSize = 0;
    recordSize += sizeof(struct YACRLEnum);
    recordSize += strlen(typeName) + 1;
    struct YACRLEnum* entry = commit_to_growable_stream(&module->data, recordSize);
    GrowableStreamHandle entryHandle = get_imperative_handle(&module->data, entry);

    entry->numEnumerators = enumData->NumEnumerators;
    entry->typeValue = enumData->UnderlyingType;
    strcpy(entry->name, typeName);

    struct CodeViewRecordHeader* header = lookUpTable[enumData->FieldListType - 0x1000];
    uint8_t* stream = header;
    struct YACRLEnumMember* enumerate;
    size_t offset = 4;

    while (offset <= header->recordLength - 2) {

        uint16_t leaf = *((uint16_t*)(stream + offset));
        offset += sizeof(uint16_t);

        switch (leaf) {
            case LF_ENUMERATE:
                // create YACRL enumeration member
                enumerate = commit_to_growable_stream(&module->data, sizeof(struct YACRLEnumMember));

                // skip over code view enumerator
                offset += sizeof(struct CodeViewEnumerator);

                // parse numeric values
                enumerate->value.numeric = parse_numeric(stream + offset);
                enumerate->value.data = getNumericAsUint32(stream + offset);
                
                // Offset by size of numerical leaf
                offset += size_of_numerical_leaf(stream + offset);

                // Append typename to end of buffer
                strcpy(commit_to_growable_stream(&module->data, strlen(stream + offset) + 1), stream + offset);
                recordSize += strlen(stream + offset) + 1 + sizeof (struct YACRLEnumMember);

                // move offset along by size of typename
                offset += strlen(stream + offset) + 1;
                break;
            case LF_INDEX:
                header = lookUpTable[((struct CodeViewIndex*)((uint8_t*)header + offset))->continuationIndex - 0x1000];
                stream = header;
                size_t offset = sizeof(struct CodeViewRecordHeader);
                break;
            default:
                break;
        }
        uint8_t padding = *(stream + offset);
        size_t iterators = 0;
        while (padding >= 0xf0 && padding <= 0xff) {
            padding = *(stream + offset + iterators);
            iterators += 1;
        }
        offset += iterators - 1;
    }

    entry = imperative_handle_to_pointer(&module->data, entryHandle);
    entry->header.kind = ENUM;
    entry->header.length = recordSize;
    return entryHandle;
}

GrowableStreamHandle parse_pointer(struct CodeViewPointer* cvPointer, YACRLTypeIndex** yacrlLookupTable, struct YACRLModule* module) {
    struct YACRLPointer* parsed = commit_to_growable_stream(&module->data, sizeof(struct YACRLPointer));

    parsed->attributes = cvPointer->attributes;
    parsed->pointeeTypeIndex = cvPointer->pointeeType > 0x8000 ? yacrlLookupTable[cvPointer->pointeeType - 0x1000] : cvPointer->pointeeType;
    parsed->header.kind = POINTER;
    parsed->header.length = sizeof(struct YACRLPointer);

    return get_imperative_handle(&module->data, parsed);
}

GrowableStreamHandle parse_array(struct CodeViewArrayType* cvArray, YACRLTypeIndex** yacrlLookUpTable, struct YACRLModule* module) {
    uint32_t numericalValue = getNumericAsUint32((uint8_t*)cvArray + sizeof(struct CodeViewArrayType));
    char* name = (uint8_t*)cvArray + size_of_numerical_leaf((uint8_t*)cvArray + sizeof(struct CodeViewArrayType));
    struct YACRLArray* yacrlArray = commit_to_growable_stream(&module->data, sizeof(struct YACRLArray) + strlen(name) + 1);

    yacrlArray->elementType = cvArray->elementType > 0x8000 ? yacrlLookUpTable[cvArray->elementType - 0x1000] : cvArray->elementType;
    yacrlArray->indexType = cvArray->elementType > 0x8000 ? yacrlLookUpTable[cvArray->indexType - 0x1000] : cvArray->indexType;
    yacrlArray->size = numericalValue;
    strcpy(yacrlArray->name, name);

    return get_imperative_handle(&module->data, yacrlArray);
}

GrowableStreamHandle parse_union(struct CodeViewUnionType* cvUnion, YACRLTypeIndex** yacrlLookupTable, struct YACRLModule* module) {
  
}

struct YACRLModule* load_yacrl_from_pdb(const char* name) {
    struct YACRLModule* result = malloc(sizeof(struct YACRLModule));
    result->count = 0;
    result->loadedModule = malloc(strlen(name) + 5);
    strcpy(result->loadedModule, name);
    result->loadedModule = strcat(result->loadedModule, ".pdb");
    init_growable_stream(&result->data, 1);
    struct Msf msf = load_msf_file(result->loadedModule);
    struct MsfStream tpiStream = parse_msf_stream(&msf, TPI_INDEX);
    struct MsfStream ipiStream = parse_msf_stream(&msf, IPI_INDEX);
    struct TPIStreamHeader* tpiStreamHeader = tpiStream.data;
    struct TPIStreamHeader* ipiStreamHeader = ipiStream.data;

    size_t amountTypes = tpiStreamHeader->TypeIndexEnd - tpiStreamHeader->TypeIndexBegin;
    uint8_t* tpiStreamData = (uint8_t*)tpiStream.data + sizeof(struct TPIStreamHeader);
    uint8_t* ipiStreamData = (uint8_t*)ipiStream.data + sizeof(struct TPIStreamHeader);
    struct CodeViewRecordHeader** lookUpTable = malloc(sizeof(struct CodeViewRecordHeader*) * amountTypes);
    YACRLTypeIndex* yacrlLookUpTable = malloc(sizeof(YACRLTypeIndex) * amountTypes);

    for (size_t i = 0; i < amountTypes; i++) {
        struct CodeViewRecordHeader* record = tpiStreamData;
        lookUpTable[i] = record;

        switch (record->recordKind) {
            case LF_CLASS:
            case LF_STRUCTURE:
                struct CodeViewStruct* structureRecord = tpiStreamData;
                break;
            case LF_PROCEDURE:
                struct CodeViewProcedure* procedure = tpiStreamData;
                break;
            case LF_MFUNCTION:
                struct CodeViewMemberFunction* memberFunction = tpiStreamData;
            case LF_UNION:
                struct CodeViewUnionType* uinionInfo = tpiStreamData;
                break;
            case LF_ARRAY:
                parse_array(record, yacrlLookUpTable, result);
                result->count++;
                break;
            case LF_ENUM:
                parse_enumerator(record, lookUpTable, result);
                result->count++;
                break;
            case LF_POINTER:
                parse_pointer(record, yacrlLookUpTable, result);
                result->count++;
                break;
            default:
                break;
        }
        yacrlLookUpTable[i] = result->count;
        tpiStreamData += record->recordLength + 2;
    }

    uint8_t* data = result->data.data;
    size_t offset = 0;
    size_t size = result->data.used;
    size_t index = 0;

    result->types = malloc(sizeof(struct YACRLHeader*) * result->count);

    while (offset <= size && index != result->count) {
        struct YACRLHeader* header = data + offset;
        result->types[index] = header;
        index++;
        offset += header->length;
    }

    free(lookUpTable);
    free(yacrlLookUpTable);

    return result;
}
