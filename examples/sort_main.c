#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "merge.h"

#define RECORDS_NUM 500 // you can change it if you want
#define FILE_NAME "data.db"
#define OUT_NAME "out"

int createAndPopulateHeapFile(char* filename);
void sortPhase(int file_desc, int chunkSize);
void mergePhases(int inputFileDesc, int chunkSize, int bWay, int* fileCounter);
int nextOutputFile(int* fileCounter);

int main()
{
    int chunkSize = 5;
    int bWay = 4;
    int fileIterator = 0;
    
    BF_Init(LRU);
    int file_desc = createAndPopulateHeapFile(FILE_NAME);
    printf("Initial file:\n");
    HP_PrintAllEntries(file_desc);
    printf("\n");

    sortPhase(file_desc, chunkSize);
    printf("Sorted per chunk file:\n");
    HP_PrintAllEntries(file_desc);

    mergePhases(file_desc, chunkSize, bWay, &fileIterator);
}

int createAndPopulateHeapFile(char* filename)
{
    HP_CreateFile(filename);
    
    int file_desc;
    HP_OpenFile(filename, &file_desc);

    Record record;
    srand(12569874);
    for (int id = 0; id < RECORDS_NUM; ++id)
    {
        record = randomRecord();
        HP_InsertEntry(file_desc, record);
    }

    return file_desc;
}

/*Performs the sorting phase of external merge sort algorithm on a file specified by 'file_desc', using chunks of size 'chunkSize'*/
void sortPhase(int file_desc,int chunkSize)
{ 
    sort_FileInChunks(file_desc, chunkSize);
}

/* Performs the merge phase of the external merge sort algorithm  using chunks of size 'chunkSize' and 'bWay' merging. The merge phase may be performed in more than one cycles.*/
void mergePhases(int inputFileDesc, int chunkSize, int bWay, int* fileCounter)
{
    int outputFileDesc;
    int counter = 0;

    while (chunkSize <= HP_GetIdOfLastBlock(inputFileDesc))
    {
        outputFileDesc = nextOutputFile(fileCounter);
        merge(inputFileDesc, chunkSize, bWay, outputFileDesc);
        HP_CloseFile(inputFileDesc);
        chunkSize *= bWay;
        inputFileDesc = outputFileDesc;

        printf("\n");
        printf("Merged file %d:\n", ++counter);
        HP_PrintAllEntries(outputFileDesc);
    }

    HP_CloseFile(outputFileDesc);
}

/*Creates a sequence of heap files: out0.db, out1.db, ... and returns for each heap file its corresponding file descriptor. */
int nextOutputFile(int* fileCounter)
{
    char mergedFile[50];
    char tmp[] = OUT_NAME;
    sprintf(mergedFile, "%s%d.db", tmp, (*fileCounter)++);
    int file_desc;
    HP_CreateFile(mergedFile);
    HP_OpenFile(mergedFile, &file_desc);

    return file_desc;
}