/* Name:Theodore Montalbano
 * CS login:montalbano
 * Section(s):001
 *
 * csim.c - A cache simulator that can replay traces from Valgrind
 *     and output statistics such as number of hits, misses, and
 *     evictions.  The replacement policy is LRU.
 *
 * Implementation and assumptions:
 *  1. Each load/store can cause at most one cache miss plus a possible eviction.
 *  2. Instruction loads (I) are ignored.
 *  3. Data modify (M) is treated as a load followed by a store to the same
 *  address. Hence, an M operation can result in two cache hits, or a miss and a
 *  hit plus a possible eviction.
 *
 * The function print_summary() is given to print output.
 * Please use this function to print the number of hits, misses and evictions.
 * This is crucial for the driver to evaluate your work. 
 */

#include <getopt.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <assert.h>
#include <math.h>
#include <limits.h>
#include <string.h>
#include <errno.h>
#include <stdbool.h>

/****************************************************************************/
/***** DO NOT MODIFY THESE VARIABLE NAMES ***********************************/

/* Globals set by command line args */
int s = 0; /* set index bits */
int E = 0; /* associativity */
int b = 0; /* block offset bits */
int verbosity = 0; /* print trace if set */
char* trace_file = NULL;

/* Derived from command line args */
int B; /* block size (bytes) B = 2^b */
int S; /* number of sets S = 2^s In C, you can use the left shift operator */

/* Counters used to record cache statistics */
int hit_cnt = 0;
int miss_cnt = 0;
int evict_cnt = 0;
/*****************************************************************************/


/* Type: Memory address 
 * Use this type whenever dealing with addresses or address masks
 */                    
typedef unsigned long long int mem_addr_t;

/* Type: Cache line
 * TODO 
 * 
 * NOTE: 
 * You might (not necessarily though) want to add an extra field to this struct
 * depending on your implementation
 * 
 * For example, to use a linked list based LRU,
 * you might want to have a field "struct cache_line * next" in the struct 
 */                    
typedef struct cache_line {                    
    char valid;
    mem_addr_t tag;
    int lastUsed;
    struct cache_line * next;
} cache_line_t;

typedef cache_line_t* cache_set_t;
typedef cache_set_t* cache_t;


/* The cache we are simulating */
cache_t cache;  

/* TODO - COMPLETE THIS FUNCTION
 * init_cache - 
 * Allocate data structures to hold info regrading the sets and cache lines
 * use struct "cache_line_t" here
 * Initialize valid and tag field with 0s.
 * use S (= 2^s) and E while allocating the data structures here
 */                    
void init_cache() {
  // First calculating S as 2^s  
  S = 1;
  for (int i = 0; i < s; i++){
    S = S + S;
  }
  // Allocating memory for the number of sets and checking
  // for correct allocation  
  cache = malloc(sizeof(cache_set_t) * S);
  if (cache == NULL){
    printf("%s, /n", "ERROR: Failure to dynamically allocate mem");
    exit(1);
  }
  // For each set allocat the memory to each line and check for correct
  // allocation. Then initalizes each lines fields to 0
  for (int i = 0; i < S; i++){
    cache[i] = malloc(sizeof(cache_line_t)*E);
    if (cache[i] == NULL){
      printf("%s, /n", "ERROR: Failure to dynamicly allocate mem");
      exit(1);
    }
    for (int j = 0; j < E; j++){
                        cache[i][j].valid ='0';
                        cache[i][j].tag = 0;
      // Counter to implement LRU
      cache[i][j].lastUsed = 0;
                }
  }
  // Calculating B as 2^b
  B = 1;
  for (int i = 0; i < b; i++){
                B = B + B;
        }
}


/* TODO - COMPLETE THIS FUNCTION 
 * free_cache - free each piece of memory you allocated using malloc 
 * inside init_cache() function
 */                    
void free_cache() {
  // Free all lines of every set
  for (int i = 0; i < S; i++){
    free(cache[i]);
    cache[i] = NULL;
  }
  // Free all sets from the cache
  free(cache);
  cache = NULL;
}

/* TODO - COMPLETE THIS FUNCTION 
 * access_data - Access data at memory address addr.
 *   If it is already in cache, increase hit_cnt
 *   If it is not in cache, bring it in cache, increase miss count.
 *   Also increase evict_cnt if a line is evicted.
 *   you will manipulate data structures allocated in init_cache() here
 */                    
void access_data(mem_addr_t addr) {
  // Find the s bits of the cache
  mem_addr_t sAddress = (addr / B)%S;
  // Find the t bits of the cache
  mem_addr_t tAddress = addr /(S*B);
  // Used to test if cache is full
  int count = 0;
  // Used as an indicator of a hit or miss
  int hit = 0;
  // Used to keep track of the index of where
  // the address of where data is put outside of 
  // the for-loop
  int index = -1;
  // Used in data structure to find where least
  // recently used bit is.
  int max = -1;
  // Finds if this addr is a hit
  // If it is then the hit indicator is
  // set to true and its last used value
  // is set to 0 and its index is kept track
  // for when all other lastUsed values are updated
  for (int i = 0; i < E; i++){
    if (cache[sAddress][i].valid == '1'
      && cache[sAddress][i].tag == tAddress){
      hit_cnt = hit_cnt + 1;
      hit = 1;
      cache[sAddress][i].lastUsed = 0;
      index = i;
    }
    // Counts the number of taken bits in set
    // If it is equal to the number of lines
    // in the set then the set is full
    if (cache[sAddress][i].valid == '1'){
      count++;
    }
  }
  // If there is not a hit
  if (!hit){
    // First update the miss count then check if an
    // eviction is needed
    miss_cnt = miss_cnt + 1;
    if (count == E){
      // If an eviction is needed first
      // update the count
      evict_cnt = evict_cnt + 1;
      // Next find the correct spot to evict
      // which will be the line with max
      // LRU value
      for (int i = 0; i < E; i++){
         if (cache[sAddress][i].lastUsed > max){
          index = i;
          max = cache[sAddress][i].lastUsed;
         }
                        }
      // As this was just accessed set LRU to 0
      // and update the tAddress
      cache[sAddress][index].lastUsed = 0;
      cache[sAddress][index].tag = tAddress;
      // Now for everything else in the cache
      // update there LRU value to be 1 greater
      // as now they were all used 1 later.
      for (int i = 0; i < E; i++){
                                 if (index != i){
          cache[sAddress][i].lastUsed = 
            cache[sAddress][i].lastUsed + 1;
        }
                        }
    }else{
      // If the cache is not full find the first open space
      // and input the addr. While updating all other LRU
      // with vbit = 1 value to be 1 greater
      for (int i = 0; i < E; i++){
        if (cache[sAddress][i].valid == '0'){
          cache[sAddress][i].valid = '1';
          cache[sAddress][i].tag = tAddress;
          break;
        }else{
          cache[sAddress][i].lastUsed =
            cache[sAddress][i].lastUsed + 1;
        }
      }
    }
  }else{
    // If it is a hit then update all other
    // LRU values to be one greater
    for (int i = 0; i < E; i++){
      if (cache[sAddress][i].valid == '1' && index != i){
        cache[sAddress][i].lastUsed =
          cache[sAddress][i].lastUsed + 1;
      }
    }
  }
}

/* TODO - FILL IN THE MISSING CODE
 * replay_trace - replays the given trace file against the cache 
 * reads the input trace file line by line
 * extracts the type of each memory access : L/S/M
 * YOU MUST TRANSLATE one "L" as a load i.e. 1 memory access
 * YOU MUST TRANSLATE one "S" as a store i.e. 1 memory access
 * YOU MUST TRANSLATE one "M" as a load followed by a store i.e. 2 memory accesses 
 */                    
void replay_trace(char* trace_fn) {                      
    char buf[1000];
    mem_addr_t addr = 0;
    unsigned int len = 0;
    FILE* trace_fp = fopen(trace_fn, "r");

    if (!trace_fp) {
        fprintf(stderr, "%s: %s\n", trace_fn, strerror(errno));
        exit(1);
    }

    while (fgets(buf, 1000, trace_fp) != NULL) {
        if (buf[1] == 'S' || buf[1] == 'L' || buf[1] == 'M') {
            sscanf(buf+3, "%llx,%u", &addr, &len);
      
            if (verbosity)
                printf("%c %llx,%u ", buf[1], addr, len);

            // TODO - MISSING CODE
            // now you have: 
            // 1. address accessed in variable - addr
      access_data(addr); 
            // 2. type of acccess(S/L/M)  in variable - buf[1] 
            // call access_data function here depending on type of access
      if (buf[1] == 'M') {
        access_data(addr);
      } 

            if (verbosity)
                printf("\n");
        }
    }

    fclose(trace_fp);
}

/*
 * print_usage - Print usage info
 */                    
void print_usage(char* argv[]) {                 
    printf("Usage: %s [-hv] -s <num> -E <num> -b <num> -t <file>\n", argv[0]);
    printf("Options:\n");
    printf("  -h         Print this help message.\n");
    printf("  -v         Optional verbose flag.\n");
    printf("  -s <num>   Number of set index bits.\n");
    printf("  -E <num>   Number of lines per set.\n");
    printf("  -b <num>   Number of block offset bits.\n");
    printf("  -t <file>  Trace file.\n");
    printf("\nExamples:\n");
    printf("  linux>  %s -s 4 -E 1 -b 4 -t traces/yi.trace\n", argv[0]);
    printf("  linux>  %s -v -s 8 -E 2 -b 4 -t traces/yi.trace\n", argv[0]);
    exit(0);
}

/*
 * print_summary - Summarize the cache simulation statistics. Student cache simulators
 *                must call this function in order to be properly autograded.
 */                    
void print_summary(int hits, int misses, int evictions) {                
    printf("hits:%d misses:%d evictions:%d\n", hits, misses, evictions);
    FILE* output_fp = fopen(".csim_results", "w");
    assert(output_fp);
    fprintf(output_fp, "%d %d %d\n", hits, misses, evictions);
    fclose(output_fp);
}

/*
 * main - Main routine 
 */                    
int main(int argc, char* argv[]) {                      
    char c;
    
    // Parse the command line arguments: -h, -v, -s, -E, -b, -t 
    while ((c = getopt(argc, argv, "s:E:b:t:vh")) != -1) {
        switch (c) {
            case 'b':
                b = atoi(optarg);
                break;
            case 'E':
                E = atoi(optarg);
                break;
            case 'h':
                print_usage(argv);
                exit(0);
            case 's':
                s = atoi(optarg);
                break;
            case 't':
                trace_file = optarg;
                break;
            case 'v':
                verbosity = 1;
                break;
            default:
                print_usage(argv);
                exit(1);
        }
    }

    /* Make sure that all required command line args were specified */
    if (s == 0 || E == 0 || b == 0 || trace_file == NULL) {
        printf("%s: Missing required command line argument\n", argv[0]);
        print_usage(argv);
        exit(1);
    }

    /* Initialize cache */
    init_cache();

    replay_trace(trace_file);

    /* Free allocated memory */
    free_cache();

    /* Output the hit and miss statistics for the autograder */
    print_summary(hit_cnt, miss_cnt, evict_cnt);
    return 0;
}
