#pragma once
#include <stdint.h>
#pragma pack(push, 1)

struct PDBStreamHeader {
    uint32_t version;
    uint32_t signature;
    uint32_t age;
    uint8_t guid[16];
};

struct DBIStreamHeader {
    int32_t VersionSignature;
    uint32_t VersionHeader;
    uint32_t Age;
    uint16_t GlobalStreamIndex;
    uint16_t BuildNumber;
    uint16_t PublicStreamIndex;
    uint16_t PdbDllVersion;
    uint16_t SymRecordStream;
    uint16_t PdbDllRbld;
    int32_t ModInfoSize;
    int32_t SectionContributionSize;
    int32_t SectionMapSize;
    int32_t SourceInfoSize;
    int32_t TypeServerSize;
    uint32_t MFCTypeServerIndex;
    int32_t OptionalDbgHeaderSize;
    int32_t ECSubstreamSize;
    uint16_t Flags;
    uint16_t Machine;
    uint32_t Padding;
};

struct TPIStreamHeader {
    uint32_t Version;
    uint32_t HeaderSize;
    uint32_t TypeIndexBegin;
    uint32_t TypeIndexEnd;
    uint32_t TypeRecordBytes;

    uint16_t HashStreamIndex;
    uint16_t HashAuxStreamIndex;
    uint32_t HashKeySize;
    uint32_t NumHashBuckets;

    int32_t HashValueBufferOffset;
    uint32_t HashValueBufferLength;

    int32_t IndexOffsetBufferOffset;
    uint32_t IndexOffsetBufferLength;

    int32_t HashAdjBufferOffset;
    uint32_t HashAdjBufferLength;
};
#pragma pack(pop)


void load_pdb_file(const char* filename);
struct YACILModule* load_YACIL_from_pdb(const char* name);