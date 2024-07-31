#include <stdio.h>
#include <stdint.h>

void manageBuffer(uint64_t* buffer, uint64_t position, uint32_t blocks);
void printStart();
void printTailTable();
void printPrimesTable(uint64_t bigPi, uint64_t expectedPi);
void printStarting();
void printBlocksFulfilled(uint64_t blockCount, uint32_t chunkSize, uint8_t multithreaded);
void printEnd();
