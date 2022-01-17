#include "windows/msf.h"
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>

void* loadFileIntoBuffer(const char* file) {
    FILE* msfFile = fopen(file, "rb");
    fseek(msfFile, 0, SEEK_END);
    size_t fileSize = ftell(msfFile);
    fseek(msfFile, 0, SEEK_SET);
    
    void* buffer = malloc(fileSize + 1);
    fread(buffer, 1, fileSize, msfFile);
    fclose(msfFile);

    return buffer;
}

void* copyBlocksToContiguous(struct SuperBlock superblock, void* buffer, uint32_t* blocks, uint32_t size) {
    char* targetBuffer = malloc(size);
    size_t block_size = superblock.blockSize;
    uint32_t numBlocks = (size + block_size - 1) / block_size;
    size_t totalBytesToCopy = size;

    // Copy all the blocks from indices to buffer.
    for (size_t i = 0; i < numBlocks; i++) {
        // Do we copy the whole block? Or just bytes that are left behind.
        size_t bytesToCopy = totalBytesToCopy > superblock.blockSize ? superblock.blockSize : totalBytesToCopy;
        // Find pointer to the current part we want to copy.
        void* directoryPart = (char*)buffer + (superblock.blockSize * blocks[i]);
        // Copy the right amount of bytes.
        memcpy(targetBuffer + i * superblock.blockSize, directoryPart, bytesToCopy);
        // Substract the amount of bytes we copied.
        totalBytesToCopy -= bytesToCopy;
    }

    return targetBuffer;
}

struct Msf load_msf_file(const char* file) {
    struct Msf msf;
    msf.fileBuffer = loadFileIntoBuffer(file);
    // copy super block
    memcpy(&msf.superblock, msf.fileBuffer, sizeof(struct SuperBlock));
    size_t block_size = msf.superblock.blockSize;

    uint32_t* contiguousStreamDirectory = copyBlocksToContiguous(
        msf.superblock,
        msf.fileBuffer,
        (char*)msf.fileBuffer + msf.superblock.blockMapAddress * block_size,
        msf.superblock.numDirectoryBytes
    );

    // Setup the msf stream directory it is now in contigous memory.
    msf.streamDirectory.buffer = contiguousStreamDirectory;
    msf.streamDirectory.numStreams = *contiguousStreamDirectory;
    msf.streamDirectory.streamSizes = (contiguousStreamDirectory + 1);
    msf.streamDirectory.streamBlocks = malloc(sizeof(uint32_t*) * (msf.streamDirectory.numStreams));

    uint32_t* streamBlocksStartPoint = (contiguousStreamDirectory + (1 + msf.streamDirectory.numStreams));
    uint32_t passedBlocks = 0;
    for (size_t i = 0; i < msf.streamDirectory.numStreams; i++) {
        msf.streamDirectory.streamBlocks[i] = (streamBlocksStartPoint + passedBlocks);
        passedBlocks += (msf.streamDirectory.streamSizes[i] + block_size - 1) / block_size;
    }

    return msf;
}

struct MsfStream parse_msf_stream(struct Msf* msf, uint32_t streamIndex) {
    struct MsfStream stream;
    
    stream.size = msf->streamDirectory.streamSizes[streamIndex];
    stream.data = copyBlocksToContiguous(
        msf->superblock,
        msf->fileBuffer,
        msf->streamDirectory.streamBlocks[streamIndex],
        msf->streamDirectory.streamSizes[streamIndex]
    );
    
    return stream;
}

void free_msf_stream(struct MsfStream stream) {
    free(stream.data);
}

void free_msf(struct Msf* msf) {
    free(msf->streamDirectory.buffer);
    free(msf->streamDirectory.streamBlocks);
    free(msf->fileBuffer);
}