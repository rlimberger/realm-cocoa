#include <stdio.h>
#include "../../src/tightdb.hpp"
#include "../../test/UnitTest++/src/UnitTest++.h"
#include "../../test/UnitTest++/src/Win32/TimeHelpers.h"
#include "../Support/mem.hpp"
#include "../Support/number_names.hpp"
#include <assert.h>
#include <string>
#include <vector>
#include <algorithm>
#include "../../src/win32/stdint.h"
#include <map>

using namespace tightdb;


// Get and Set are too fast (50ms/M) for normal 64-bit rand*rand*rand*rand*rand (5-10ms/M)
uint64_t rand2()
{
    static int64_t seed = 2862933555777941757ULL;
    static int64_t seed2 = 0;
    seed = (2862933555777941757ULL * seed + 3037000493ULL);
    seed2++;
    return seed * seed2 + seed2;
}

TIGHTDB_TABLE_1(IntegerTable,
                first, Int)

UnitTest::Timer timer;
int ITEMS = 50000;
int RANGE = 50000;

//void tightdb(void);
void stl(void);

volatile uint64_t writethrough;


void tightdb2(void)
{
    IntegerTable integers;
    volatile uint64_t force;
    int overhead; // Time of computing 1 rand and 1 modulo and doing a loop (is ~0ms with new rand)

    uint64_t dummy = 0;

    for(int index = 0; index < 2; index++) {
        std::string indexed;
        integers.clear();
        if(index == 1) {
            integers.column().first.set_index();
            indexed = "Indexed ";
        }

        overhead = 0;

        timer.Start();
        for (size_t i = 0; i < ITEMS; ++i) {
            size_t p = rand2() % (i + 1);
            integers.add((int64_t)rand2() % RANGE);
        }
//      printf((indexed + "Memory usage: %lld bytes\n").c_str(), (int64_t)GetMemUsage()); // %zu doesn't work in vc
        printf((indexed + "Add: %dms\n").c_str(), timer.GetTimeInMs() - overhead);


        //integers.clear();
        timer.Start();
        for (size_t i = 0; i < ITEMS; ++i) {
            size_t p = rand2() % (i + 1);
            integers.insert(p, (int64_t)rand2() % RANGE);
        }
        printf((indexed + "Insert: %dms\n").c_str(), timer.GetTimeInMs() - overhead);

        timer.Start();
        for (size_t i = 0; i < ITEMS; ++i) {
            size_t p = rand2() % ITEMS;
            dummy += integers[p].first;
        }
        force = dummy;
        printf((indexed + "Get: %dms\n").c_str(), timer.GetTimeInMs() - overhead);


        timer.Start();
        for (size_t i = 0; i < ITEMS; ++i) {
            size_t p = rand2() % ITEMS;
            integers[p].first = rand2() % RANGE;
        }
        force = dummy;
        printf((indexed + "Set: %dms\n").c_str(), timer.GetTimeInMs() - overhead);

        uint64_t distance_sum = 0;
        timer.Start();
        for (size_t i = 0; i < ITEMS; ++i) {
            uint64_t f = rand2() % RANGE;
            integers.column().first.find_first(f);

            // Sanity test to ensure that average distance between matches is the same as in the STL tests
/*
            int j;
            for(j = 0; j < integers.size(); j++)
                if(integers.Get64(0, j) == f)
                    break;
            distance_sum += j;
*/
        }
//      printf("avg dist=%llu in ", distance_sum / ITEMS);
        printf((indexed + "Find: %dms\n").c_str(), timer.GetTimeInMs() - overhead);


        timer.Start();
        for (size_t i = 0; i < ITEMS; ++i) {
            integers.column().first.find_all(rand2() % RANGE);
        }
        printf((indexed + "FindAll: %dms\n").c_str(), timer.GetTimeInMs() - overhead);


        timer.Start();
        for (size_t i = 0; i < ITEMS; ++i) {
            size_t p = rand2() % (ITEMS - i);
            integers.remove(p);
        }
        printf((indexed + "Delete: %dms\n").c_str(), timer.GetTimeInMs() - overhead);
        printf("\n");
    }
}



void main(void)
{
    tightdb2();
#ifdef _MSC_VER
    getchar();
#endif
}