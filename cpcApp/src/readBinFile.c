#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <epicsExport.h>
#include <aSubRecord.h>
#include <registryFunction.h>
#include <arpa/inet.h>  // for htonl

long readBinFile(aSubRecord *prec) {
    const char *path      = (const char *)prec->a;  // directory path
    const char *filename  = (const char *)prec->b;  // file name
    int maxElements       = *(int *)prec->c;        // max number of int32_t to read
    long offsetBytes      = *(long *)prec->d;    // offset in bytes

    int32_t *outBuffer    = (int32_t *)prec->vala;  // output array
    long *numElementsRead = (long *)prec->valb;     // number actually read

    if (!path || !filename || !outBuffer) {
        printf("Error: Missing input(s)\n");
        return -1;
    }

    if (maxElements <= 0) {
        printf("Error: Invalid maxElements %d\n", maxElements);
        return -1;
    }

    char fullPath[512];
    snprintf(fullPath, sizeof(fullPath), "%s/%s", path, filename);
    printf("Reading binary file: %s\n", fullPath);

    FILE *file = fopen(fullPath, "rb");
    if (!file) {
        printf("Error: cannot open file %s\n", fullPath);
        return -1;
    }

    // Get file size
    fseek(file, 0, SEEK_END);
    long fileSize = ftell(file);
    printf("File size: %ld bytes\n", fileSize);
    printf("Offset Bytes: %ld bytes\n", offsetBytes);
    // Seek to offset
    if (offsetBytes >= fileSize) {
        printf("End of file reached.\n");
        fclose(file);
        *numElementsRead = 0;
        prec->neva = 0;
        return 0; // no more data
    }
    fseek(file, offsetBytes, SEEK_SET);

    // Read block
    printf("prec->nova = %d\n",prec->nova);
    size_t count = fread(outBuffer, sizeof(int32_t), prec->nova, file);
 
    // After reading:
    for (size_t i = 0; i < count; i++) {
       outBuffer[i] = (int32_t)htonl((uint32_t)outBuffer[i]);
    }


    //size_t count = fread(outBuffer, sizeof(int32_t), maxElements, file);

    // Print first 100 bytes as hex (8 bytes per line)
    {
        uint8_t *bytePtr = (uint8_t *)outBuffer;
        size_t bytesToPrint = (count * sizeof(int32_t) < 100) ? count * sizeof(int32_t) : 100;
        printf("First %zu bytes in hex:\n", bytesToPrint);
        for (size_t i = 0; i < bytesToPrint; i++) {
            printf("%02X ", bytePtr[i]);
            if ((i + 1) % 8 == 0) {
                printf("\n");
            }
        }
        if (bytesToPrint % 8 != 0) {
            printf("\n");
        }
    }


    if (count == 0) {
        printf("Warning: no data read from %s\n", fullPath);
        *numElementsRead = 0;
        prec->neva = 0;
        return -1;
    }

    *numElementsRead = (long)count;
    prec->neva = (unsigned long)count;

    printf("Read %zu int32_t values (%zu bytes) from %s\n",
           count, count * sizeof(int32_t), fullPath);

    return 0;
}

epicsRegisterFunction(readBinFile);


