#include "tightdb/array_integer.hpp"

#include <vector>

using namespace tightdb;

// Find max and min value, but break search if difference exceeds 'maxdiff' (in which case *min and *max is set to 0)
// Useful for counting-sort functions
template <size_t w>
bool ArrayInteger::minmax(size_t from, size_t to, uint64_t maxdiff, int64_t *min, int64_t *max) const
{
    int64_t min2;
    int64_t max2;
    size_t t;

    max2 = Array::get<w>(from);
    min2 = max2;

    for (t = from + 1; t < to; t++) {
        int64_t v = Array::get<w>(t);
        // Utilizes that range test is only needed if max2 or min2 were changed
        if (v < min2) {
            min2 = v;
            if (uint64_t(max2 - min2) > maxdiff)
                break;
        }
        else if (v > max2) {
            max2 = v;
            if (uint64_t(max2 - min2) > maxdiff)
                break;
        }
    }

    if (t < to) {
        *max = 0;
        *min = 0;
        return false;
    }
    else {
        *max = max2;
        *min = min2;
        return true;
    }
}


std::vector<int64_t> ArrayInteger::ToVector() const
{
    std::vector<int64_t> v;
    const size_t count = size();
    for (size_t t = 0; t < count; ++t)
        v.push_back(Array::get(t));
    return v;
}

MemRef ArrayIntNull::create_array(Type type, bool context_flag, std::size_t size, int_fast64_t value, Allocator& alloc)
{
    MemRef r = Array::create(type, context_flag, wtype_Bits, size + 1, value, alloc);
    ArrayIntNull arr(alloc);
    arr.init_from_mem(r);
    if (arr.m_width == 64) {
        int_fast64_t null_value = value ^ 1; // Just anything different from value.
        arr.Array::set(0, null_value);
    }
    else {
        arr.Array::set(0, arr.m_ubound);
    }
    return r;
}

void ArrayIntNull::choose_random_null(int64_t incoming)
{
    while (true) {
        // FIXME: Use a better rand() function.
        int64_t candidate = rand() * rand() * rand();
        if (candidate == incoming) {
            continue;
        }
        if (can_use_as_null(candidate)) {
            replace_nulls_with(candidate);
            break;
        }
    }
}

bool ArrayIntNull::can_use_as_null(int64_t candidate)
{
    // FIXME: Optimize!
    for (size_t i = 0; i < size(); ++i) {
        if (get(i) == candidate) {
            return false;
        }
    }
    return true;
}

void ArrayIntNull::replace_nulls_with(int64_t new_null)
{
    // FIXME: Optimize!
    int64_t old_null = Array::get(0);
    Array::set(0, new_null);
    for (size_t i = 0; i < size(); ++i) {
        if (Array::get(i+1) == old_null) {
            Array::set(i+1, new_null);
        }
    }
}


void ArrayIntNull::ensure_not_null(int64_t value)
{
    if (m_width == 64) {
        if (value == null_value()) {
            choose_random_null(value);
        }
    }
    else {
        if (value >= m_ubound) {
            size_t new_width = bit_width(value + 1); // +1 because we need room for ubound too
            if (new_width == 64) {
                // Width will be upgraded to 64, so we need to pick a random NULL.
                choose_random_null(value);
            }
            else {
                int64_t new_null = (1UL << (new_width - 1)) - 1; // == m_ubound after upgrade
                replace_nulls_with(new_null); // Expands array
            }
        }
    }
}
