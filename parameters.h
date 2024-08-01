
// upper bound of the main loop
// the number evalulated will be this value times 64
// but only to the last multiple of CPU_COUNT * 16384
#define MAIN_LOOP_END 15625000 // 1 billion
// #define MAIN_LOOP_END 156250000 // 10 billion
// #define MAIN_LOOP_END 0xffffffffffffffffULL // maximum the program can handle

// do not change this unless you're running low on memory
// the size of this table is this value times 64
// each block of this table is 8 bytes
// it takes 8 GB at default
#define TAIL_TABLE_SIZE (uint64_t)1000000000
// note that there is also a prime table with 2^32/8 bytes
// after this table, the program will run in about nsqrt(n) time

// do not change this unless you have more than 32 virtual cores (nproc)
// this is the number of threads the program will spawn
#define CPU_COUNT 32
