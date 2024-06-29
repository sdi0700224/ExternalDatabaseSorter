#include "merge.h"
#include <stdio.h>
#include <stdbool.h>

void merge(int input_FileDesc, int chunkSize, int bWay, int output_FileDesc)
{
    CHUNK_Iterator chunkIterator = CHUNK_CreateIterator(input_FileDesc, chunkSize);
    CHUNK_RecordIterator recordIterators[bWay];
    Record currentRecords[bWay];
    bool hasMoreRecords[bWay];
    int activeChunks;

    while (true)
    {
        activeChunks = 0;

        // Fill the chunk and record iterator arrays
        for (int i = 0; i < bWay; ++i)
        {
            CHUNK chunk;
            if (CHUNK_GetNext(&chunkIterator, &chunk) == 0)
            {
                recordIterators[i] = CHUNK_CreateRecordIterator(&chunk);
                if (CHUNK_GetNextRecord(&recordIterators[i], &currentRecords[i]) == 0)
                {
                    hasMoreRecords[i] = true;
                    activeChunks++;
                }
            }
            else
            {
                break; // No more chunks
            }
        }

        if (activeChunks == 0)
        {
            break; // No more chunks to process
        }

        // Merge loop for the current set of chunks
        while (true)
        {
            int minRecordIdx = -1;
            Record minRecord;
            bool foundRecord = false;

            // Find the smallest record among the current records
            for (int i = 0; i < activeChunks; ++i)
            {
                if (hasMoreRecords[i] && (!foundRecord || shouldSwap(&minRecord, &currentRecords[i])))
                {
                    minRecordIdx = i;
                    minRecord = currentRecords[i];
                    foundRecord = true;
                }
            }

            if (!foundRecord)
            {
                break; // No more records to process in this set
            }

            // Insert the smallest record into the output file
            HP_InsertEntry(output_FileDesc, minRecord);

            // Move to the next record in the chunk from which the smallest record was taken
            if (CHUNK_GetNextRecord(&recordIterators[minRecordIdx], &currentRecords[minRecordIdx]) != 0)
            {
                hasMoreRecords[minRecordIdx] = false;
            }
        }
    }
}