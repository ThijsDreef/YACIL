#pragma once
#include <stdint.h>

struct SuperBlock {
    char fileMagic[32];
    // block size of the internal file structure.
    uint32_t blockSize;
    // The freeblock map which is currently in use, 1 or 2.
    uint32_t freeBlockMapBlock;
    // Total number of blocks used in the file.
    uint32_t numBlocks;
    // THe size of the stream directory.
    uint32_t numDirectoryBytes;
    uint32_t unknown;
    // The index of a block which contains the list of blocks the directory file resides in.
    uint32_t blockMapAddress;
};

struct StreamDirectory {
    char* buffer;
    // The number of streams stored
    uint32_t numStreams;
    // sizes of the stream.
    uint32_t* streamSizes;
    // 2D array of blocks indices
    uint32_t** streamBlocks;
};

struct Msf {
    struct SuperBlock superblock;
    struct StreamDirectory streamDirectory;
    void* fileBuffer;
};

struct MsfStream {
    void* data;
    size_t size;
};

struct Msf load_msf_file(const char* file);
struct MsfStream parse_msf_stream(struct Msf* msf, uint32_t streamIndex);
void free_msf_stream(struct MsfStream stream);
void free_msf(struct Msf* msf);