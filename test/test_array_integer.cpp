#include "testsettings.hpp"

#include <tightdb/array_integer.hpp>

#include "test.hpp"

using namespace std;
using namespace tightdb;
using namespace tightdb::test_util;

TEST(ArrayInteger_Sum0)
{
    ArrayInteger a(Allocator::get_default());
    a.create(Array::type_Normal);

    for (int i = 0; i < 64 + 7; ++i) {
        a.add(0);
    }
    CHECK_EQUAL(0, a.sum(0, a.size()));
    a.destroy();
}

TEST(ArrayInteger_Sum1)
{
    ArrayInteger a(Allocator::get_default());
    a.create(Array::type_Normal);

    int64_t s1 = 0;
    for (int i = 0; i < 256 + 7; ++i)
        a.add(i % 2);

    s1 = 0;
    for (int i = 0; i < 256 + 7; ++i)
        s1 += a.get(i);
    CHECK_EQUAL(s1, a.sum(0, a.size()));

    s1 = 0;
    for (int i = 3; i < 100; ++i)
        s1 += a.get(i);
    CHECK_EQUAL(s1, a.sum(3, 100));

    a.destroy();
}

TEST(ArrayInteger_Sum2)
{
    ArrayInteger a(Allocator::get_default());
    a.create(Array::type_Normal);

    int64_t s1 = 0;
    for (int i = 0; i < 256 + 7; ++i)
        a.add(i % 4);

    s1 = 0;
    for (int i = 0; i < 256 + 7; ++i)
        s1 += a.get(i);
    CHECK_EQUAL(s1, a.sum(0, a.size()));

    s1 = 0;
    for (int i = 3; i < 100; ++i)
        s1 += a.get(i);
    CHECK_EQUAL(s1, a.sum(3, 100));

    a.destroy();
}


TEST(ArrayInteger_Sum4)
{
    ArrayInteger a(Allocator::get_default());
    a.create(Array::type_Normal);

    int64_t s1 = 0;
    for (int i = 0; i < 256 + 7; ++i)
        a.add(i % 16);

    s1 = 0;
    for (int i = 0; i < 256 + 7; ++i)
        s1 += a.get(i);
    CHECK_EQUAL(s1, a.sum(0, a.size()));

    s1 = 0;
    for (int i = 3; i < 100; ++i)
        s1 += a.get(i);
    CHECK_EQUAL(s1, a.sum(3, 100));

    a.destroy();
}

TEST(ArrayInteger_Sum16)
{
    ArrayInteger a(Allocator::get_default());
    a.create(Array::type_Normal);

    int64_t s1 = 0;
    for (int i = 0; i < 256 + 7; ++i)
        a.add(i % 30000);

    s1 = 0;
    for (int i = 0; i < 256 + 7; ++i)
        s1 += a.get(i);
    CHECK_EQUAL(s1, a.sum(0, a.size()));

    s1 = 0;
    for (int i = 3; i < 100; ++i)
        s1 += a.get(i);
    CHECK_EQUAL(s1, a.sum(3, 100));

    a.destroy();
}

TEST(ArrayIntNull_SetNull) {
    ArrayIntNull a(Allocator::get_default());
    a.create(Array::type_Normal);

    a.add(0);
    a.set_null(0);
    CHECK(a.is_null(0));

    a.add(128);
    CHECK(a.is_null(0));

    a.add(120000);
    CHECK(a.is_null(0));

    a.destroy();
}

TEST(ArrayIntNull_SetIntegerToPreviousNullValueChoosesNewNull) {
    ArrayIntNull a(Allocator::get_default());
    a.create(Array::type_Normal);

    a.add(126);
    // NULL value should be 127
    a.add(0);
    a.set_null(1);
    a.set(0, 127);
    // array should be upgraded now
    CHECK(a.is_null(1));

    a.add(1000000000000LL); // upgrade to 64-bit, null should now be a "random" value
    CHECK(a.is_null(1));
    int64_t old_null = a.null_value();
    a.add(old_null);
    CHECK(a.is_null(1));
    CHECK_NOT_EQUAL(a.null_value(), old_null);

    a.destroy();
}

TEST(ArrayIntNull_Boundaries) {
    ArrayIntNull a(Allocator::get_default());
    a.create(Array::type_Normal);
    a.add(0);
    a.set_null(0);
    a.add(0);
    CHECK(a.is_null(0));
    CHECK(!a.is_null(1));
    CHECK_EQUAL(a.get_width(), 1); // not sure if this should stay. Makes assumtions about implementation details.


    // consider turning this into a array + loop
    a.add(0);
    CHECK_EQUAL(0, a.back());
    CHECK(a.is_null(0));

    a.add(1);
    CHECK_EQUAL(1, a.back());
    CHECK(a.is_null(0));

    a.add(3);
    CHECK_EQUAL(3, a.back());
    CHECK(a.is_null(0));

    a.add(15);
    CHECK_EQUAL(15, a.back());
    CHECK(a.is_null(0));


    a.add(INT8_MAX);
    CHECK_EQUAL(INT8_MAX, a.back());
    CHECK(a.is_null(0));

    a.add(INT8_MIN);
    CHECK_EQUAL(INT8_MIN, a.back());
    CHECK(a.is_null(0));

    a.add(UINT8_MAX);
    CHECK_EQUAL(UINT8_MAX, a.back());
    CHECK(a.is_null(0));


    a.add(INT16_MAX);
    CHECK_EQUAL(INT16_MAX, a.back());
    CHECK(a.is_null(0));
    a.add(INT16_MIN);
    CHECK_EQUAL(INT16_MIN, a.back());
    CHECK(a.is_null(0));
    a.add(UINT16_MAX);
    CHECK_EQUAL(UINT16_MAX, a.back());
    CHECK(a.is_null(0));


    a.add(INT32_MAX);
    CHECK_EQUAL(INT32_MAX, a.back());
    CHECK(a.is_null(0));
    a.add(INT32_MIN);
    CHECK_EQUAL(INT32_MIN, a.back());
    CHECK(a.is_null(0));
    a.add(UINT32_MAX);
    CHECK_EQUAL(UINT32_MAX, a.back());
    CHECK(a.is_null(0));


    a.add(INT64_MAX);
    CHECK_EQUAL(INT64_MAX, a.back());
    CHECK(a.is_null(0));
    a.add(INT64_MIN);
    CHECK_EQUAL(INT64_MIN, a.back());
    CHECK(a.is_null(0));
    a.add(UINT64_MAX);
    CHECK_EQUAL(UINT64_MAX, a.get_uint(a.size()-1));
    CHECK(a.is_null(0));


    a.destroy();
}

TEST(ArrayIntNull_SetUint0) {
    ArrayIntNull a(Allocator::get_default());
    a.create(Array::type_Normal);
    a.add(0);
    a.add(0);

    a.set_uint(0, 0);
    a.set_null(1);

    CHECK(!a.is_null(0));
    CHECK(a.is_null(1));

    a.destroy();
}
