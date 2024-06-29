#include "sort.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "hp_file.h"
#include "record.h"
#include "chunk.h"

bool shouldSwap(Record* rec1, Record* rec2)
{
    // Compare names
    int nameComparison = strcmp(rec1->name, rec2->name);
    if (nameComparison != 0)
    {
        return nameComparison > 0;
    }

    // Compare surnames
    int surnameComparison = strcmp(rec1->surname, rec2->surname);
    if (surnameComparison != 0)
    {
        return surnameComparison > 0;
    }

    // Compare ids
    return rec1->id > rec2->id;
}

void sort_FileInChunks(int file_desc, int numBlocksInChunk)
{
    CHUNK_Iterator chunkIterator = CHUNK_CreateIterator(file_desc, numBlocksInChunk);
    CHUNK chunk;

    // Iterate over the file using CHUNK_Iterator
    while (CHUNK_GetNext(&chunkIterator, &chunk) == 0)
    {
        // Sort the records in the chunk
        sort_Chunk(&chunk);
        printf("Chunk was sorted: \n");
        CHUNK_Print(chunk);
    }
}

void sort_Chunk(CHUNK* chunk)
{
    Record record1, record2;

    // Sort the records using bubble sort and shouldSwap for comparison
    for (int i = 0; i < chunk->recordsInChunk - 1; i++)
    {
        for (int j = 0; j < chunk->recordsInChunk - i - 1; j++)
        {
            if (CHUNK_GetIthRecordInChunk(chunk, j, &record1) != 0 || CHUNK_GetIthRecordInChunk(chunk, j + 1, &record2) != 0)
            {
                printf("Error, could not get record from chunk!\n");
                return;
            }

            // Compare and potentially swap
            if (shouldSwap(&record1, &record2))
            {
                if (CHUNK_UpdateIthRecord(chunk, j, record2) != 0 || CHUNK_UpdateIthRecord(chunk, j + 1, record1) != 0)
                {
                    printf("Error, could not update record from chunk!\n");
                    return;
                }
            }
        }
    }
}