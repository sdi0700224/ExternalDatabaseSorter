#include "chunk.h"
#include <stdio.h>

int CalculateBlockIdAndCursor(CHUNK* chunk, int i, int* blockId, int* cursor);

CHUNK_Iterator CHUNK_CreateIterator(int fileDesc, int blocksInChunk)
{
    CHUNK_Iterator iterator;
    iterator.file_desc = fileDesc;
    iterator.current = 1;  // Start from the first block
    iterator.lastBlocksID = HP_GetIdOfLastBlock(fileDesc);
    iterator.blocksInChunk = blocksInChunk;

    return iterator;
}

int CHUNK_GetNext(CHUNK_Iterator *iterator, CHUNK* chunk)
{
    // No more chunks available, return -1 to signal the end
    if (iterator->current > iterator->lastBlocksID)
    {
        return -1;
    }

    chunk->file_desc = iterator->file_desc;
    chunk->from_BlockId = iterator->current;

    // Calculate the 'to_BlockId' based on the specified 'blocksInChunk'
    chunk->to_BlockId = iterator->current + iterator->blocksInChunk - 1;

    // Ensure 'to_BlockId' does not exceed 'lastBlocksID'
    if (chunk->to_BlockId > iterator->lastBlocksID)
    {
        chunk->to_BlockId = iterator->lastBlocksID;
    }

    chunk->blocksInChunk = chunk->to_BlockId - chunk->from_BlockId + 1;

    // Calculate records in chunk
    int maxRecordsPerBlock = HP_GetMaxRecordsInBlock(iterator->file_desc);
    chunk->recordsInChunk = (chunk->blocksInChunk - 1) * maxRecordsPerBlock;
    if (chunk->to_BlockId == iterator->lastBlocksID)
    {
        // Add records from the last block
        int recordsInLastBlock = HP_GetRecordCounter(iterator->file_desc, iterator->lastBlocksID);
        if (recordsInLastBlock < 0)
        {
            printf("Error getting record counter for block\n");
            return -1;
        }
        chunk->recordsInChunk += recordsInLastBlock;
    }
    else
    {
        // All other blocks are full
        chunk->recordsInChunk += maxRecordsPerBlock;
    }

    // Update the iterator for the next chunk
    iterator->current = chunk->to_BlockId + 1;

    return 0; // Successfully retrieved the next chunk
}

int CHUNK_GetIthRecordInChunk(CHUNK* chunk, int i, Record* record)
{
    if (i < 0 || i >= chunk->recordsInChunk)
    {
        // Invalid index, return -1 to indicate an error
        return -1;
    }

    int blockId, cursor;
    if (CalculateBlockIdAndCursor(chunk, i, &blockId, &cursor) != 0)
    {
        return -1; // Error occurred in calculating block ID and cursor
    }

    int result = HP_GetRecord(chunk->file_desc, blockId, cursor, record);
    HP_Unpin(chunk->file_desc, blockId);

    return result;
}

int CHUNK_UpdateIthRecord(CHUNK* chunk, int i, Record record)
{
    if (i < 0 || i >= chunk->recordsInChunk)
    {
        // Invalid index, return -1 to indicate an error
        return -1;
    }

    int blockId, cursor;
    if (CalculateBlockIdAndCursor(chunk, i, &blockId, &cursor) != 0)
    {
        return -1; // Error occurred in calculating block ID and cursor
    }

    int result = HP_UpdateRecord(chunk->file_desc, blockId, cursor, record);
    HP_Unpin(chunk->file_desc, blockId);

    return result;
}

void CHUNK_Print(CHUNK chunk)
{
    Record record;
    CHUNK_RecordIterator iterator = CHUNK_CreateRecordIterator(&chunk);

    // Iterate through the records in the chunk
    while (CHUNK_GetNextRecord(&iterator, &record) == 0)
    {
        printRecord(record);
    }
    printf("\n");
}

CHUNK_RecordIterator CHUNK_CreateRecordIterator(CHUNK *chunk)
{
    CHUNK_RecordIterator iterator;

    iterator.chunk = *chunk;
    iterator.currentBlockId = chunk->from_BlockId;  // Start iterating from the first block of the chunk
    iterator.cursor = 0;                            // Initialize cursor to the start of the block

    return iterator;
}

int CHUNK_GetNextRecord(CHUNK_RecordIterator *iterator, Record* record)
{
    // No more records in the chunk, return -1 to signal the end
    if (iterator->currentBlockId > iterator->chunk.to_BlockId)
    {
        return -1;
    }

    // Attempt to get the next record from the current block using HP_GetRecord
    while (HP_GetRecord(iterator->chunk.file_desc, iterator->currentBlockId, iterator->cursor, record) != 0)
    {
        HP_Unpin(iterator->chunk.file_desc, iterator->currentBlockId);
        
        // Move to the next block if HP_GetRecord fails (assuming it fails when no more records in the current block)
        iterator->currentBlockId++;
        iterator->cursor = 0; // Reset cursor for the new block

        // Check if the end of the chunk is reached
        if (iterator->currentBlockId > iterator->chunk.to_BlockId)
        {
            return -1; // No more records in the chunk
        }
    }
    HP_Unpin(iterator->chunk.file_desc, iterator->currentBlockId);

    // If record is successfully retrieved, move cursor to the next record
    iterator->cursor++;
    return 0; // Record successfully retrieved
}

int CalculateBlockIdAndCursor(CHUNK* chunk, int i, int* blockId, int* cursor)
{
    int maxRecordsPerBlock = HP_GetMaxRecordsInBlock(chunk->file_desc);

    // Calculate block offset and cursor for the ith record
    *blockId = chunk->from_BlockId + (i / maxRecordsPerBlock);
    *cursor = i % maxRecordsPerBlock;

    // Check if the blockId is within the chunk bounds
    if (*blockId > chunk->to_BlockId || *blockId < chunk->from_BlockId)
    {
        printf("Error, ith record: %d is outside the chunk bounds\n", i);
        return -1;
    }

    // Special handling for the last block in the file
    if (*blockId == HP_GetIdOfLastBlock(chunk->file_desc))
    {
        int recordsInLastBlock = HP_GetRecordCounter(chunk->file_desc, *blockId);
        if (recordsInLastBlock < 0)
        {
            printf("Error getting record counter for block\n");
            return -1;
        }
        if (*cursor >= recordsInLastBlock)
        {
            printf("Error, ith record is outside file last block bounds\n");
            return -1; // Cursor is out of range in the last block
        }
    }

    return 0;
}