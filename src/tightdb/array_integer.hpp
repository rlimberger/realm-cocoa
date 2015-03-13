/*************************************************************************
 *
 * TIGHTDB CONFIDENTIAL
 * __________________
 *
 *  [2011] - [2014] TightDB Inc
 *  All Rights Reserved.
 *
 * NOTICE:  All information contained herein is, and remains
 * the property of TightDB Incorporated and its suppliers,
 * if any.  The intellectual and technical concepts contained
 * herein are proprietary to TightDB Incorporated
 * and its suppliers and may be covered by U.S. and Foreign Patents,
 * patents in process, and are protected by trade secret or copyright law.
 * Dissemination of this information or reproduction of this material
 * is strictly forbidden unless prior written permission is obtained
 * from TightDB Incorporated.
 *
***************************************************************************/

#ifndef TIGHTDB_ARRAY_INTEGER_HPP
#define TIGHTDB_ARRAY_INTEGER_HPP

#include <tightdb/array.hpp>
#include <tightdb/util/safe_int_ops.hpp>

namespace tightdb {

class ArrayInteger: public Array {
public:
    typedef int64_t value_type;

    explicit ArrayInteger(no_prealloc_tag) TIGHTDB_NOEXCEPT;
    explicit ArrayInteger(Allocator&) TIGHTDB_NOEXCEPT;
    ~ArrayInteger() TIGHTDB_NOEXCEPT TIGHTDB_OVERRIDE {}

    /// Construct an array of the specified type and size, and return just the
    /// reference to the underlying memory. All elements will be initialized to
    /// the specified value.
    static MemRef create_array(Type, bool context_flag, std::size_t size, int_fast64_t value,
                               Allocator&);

    void add(int64_t value);
    void set(std::size_t ndx, int64_t value);
    void set_uint(std::size_t ndx, uint64_t value);
    int64_t get(std::size_t ndx) const TIGHTDB_NOEXCEPT;
    uint64_t get_uint(std::size_t ndx) const TIGHTDB_NOEXCEPT;
    static int64_t get(const char* header, std::size_t ndx) TIGHTDB_NOEXCEPT;

    /// Add \a diff to the element at the specified index.
    void adjust(std::size_t ndx, int_fast64_t diff);

    /// Add \a diff to all the elements in the specified index range.
    void adjust(std::size_t begin, std::size_t end, int_fast64_t diff);

    /// Add signed \a diff to all elements that are greater than, or equal to \a
    /// limit.
    void adjust_ge(int_fast64_t limit, int_fast64_t diff);

    int64_t operator[](std::size_t ndx) const TIGHTDB_NOEXCEPT { return get(ndx); }
    int64_t front() const TIGHTDB_NOEXCEPT;
    int64_t back() const TIGHTDB_NOEXCEPT;

    std::size_t lower_bound(int64_t value) const TIGHTDB_NOEXCEPT;
    std::size_t upper_bound(int64_t value) const TIGHTDB_NOEXCEPT;

    std::vector<int64_t> ToVector() const;

private:
    template<std::size_t w> bool minmax(std::size_t from, std::size_t to, uint64_t maxdiff,
                                   int64_t* min, int64_t* max) const;
};

class ArrayIntNull: public Array {
public:
    typedef int64_t value_type;

    explicit ArrayIntNull(no_prealloc_tag) TIGHTDB_NOEXCEPT;
    explicit ArrayIntNull(Allocator&) TIGHTDB_NOEXCEPT;
    ~ArrayIntNull() TIGHTDB_NOEXCEPT TIGHTDB_OVERRIDE;

    /// Construct an array of the specified type and size, and return just the
    /// reference to the underlying memory. All elements will be initialized to
    /// the specified value.
    static MemRef create_array(Type, bool context_flag, std::size_t size, int_fast64_t value,
                               Allocator&);
    void create(Type, bool context_flag = false);

    std::size_t size() const TIGHTDB_NOEXCEPT;
    bool is_empty() const TIGHTDB_NOEXCEPT;

    void insert(std::size_t ndx, int_fast64_t value);
    void add(int64_t value);
    void set(std::size_t ndx, int64_t value);
    void set_uint(std::size_t ndx, uint64_t value);
    int64_t get(std::size_t ndx) const TIGHTDB_NOEXCEPT;
    uint64_t get_uint(std::size_t ndx) const TIGHTDB_NOEXCEPT;
    static int64_t get(const char* header, std::size_t ndx) TIGHTDB_NOEXCEPT;

    void set_null(std::size_t ndx) TIGHTDB_NOEXCEPT;
    bool is_null(std::size_t ndx) TIGHTDB_NOEXCEPT;
    int64_t null_value() const TIGHTDB_NOEXCEPT;

    int64_t operator[](std::size_t ndx) const TIGHTDB_NOEXCEPT;
    int64_t front() const TIGHTDB_NOEXCEPT;
    int64_t back() const TIGHTDB_NOEXCEPT;
    void erase(std::size_t ndx);
    void erase(std::size_t begin, std::size_t end);
    void truncate(std::size_t size);
    void clear();
    void set_all_to_zero();
    
    void move(std::size_t begin, std::size_t end, std::size_t dest_begin);
    void move_backward(std::size_t begin, std::size_t end, std::size_t dest_end);

    std::size_t lower_bound(int64_t value) const TIGHTDB_NOEXCEPT;
    std::size_t upper_bound(int64_t value) const TIGHTDB_NOEXCEPT;

    int64_t sum(std::size_t start = 0, std::size_t end = std::size_t(-1)) const;
    std::size_t count(int64_t value) const TIGHTDB_NOEXCEPT;
    bool maximum(int64_t& result, std::size_t start = 0, std::size_t end = std::size_t(-1),
        std::size_t* return_ndx = null_ptr) const;
    bool minimum(int64_t& result, std::size_t start = 0, std::size_t end = std::size_t(-1),
                 std::size_t* return_ndx = null_ptr) const;

    bool find(int cond, Action action, int64_t value, std::size_t start, std::size_t end, std::size_t baseindex,
              QueryState<int64_t>* state) const;

    template<class cond, Action action, std::size_t bitwidth, class Callback>
    bool find(int64_t value, std::size_t start, std::size_t end, std::size_t baseindex,
              QueryState<int64_t>* state, Callback callback) const;

    // This is the one installed into the m_finder slots.
    template<class cond, Action action, std::size_t bitwidth>
    bool find(int64_t value, std::size_t start, std::size_t end, std::size_t baseindex,
              QueryState<int64_t>* state) const;

    template<class cond, Action action, class Callback>
    bool find(int64_t value, std::size_t start, std::size_t end, std::size_t baseindex,
              QueryState<int64_t>* state, Callback callback) const;

    // Optimized implementation for release mode
    template<class cond2, Action action, std::size_t bitwidth, class Callback>
    bool find_optimized(int64_t value, std::size_t start, std::size_t end, std::size_t baseindex,
                        QueryState<int64_t>* state, Callback callback) const;

    // Called for each search result
    template<Action action, class Callback>
    bool find_action(std::size_t index, int64_t value,
                     QueryState<int64_t>* state, Callback callback) const;

    template<Action action, class Callback>
    bool find_action_pattern(std::size_t index, uint64_t pattern,
                             QueryState<int64_t>* state, Callback callback) const;

    // Wrappers for backwards compatibility and for simple use without
    // setting up state initialization etc
    template<class cond>
    std::size_t find_first(int64_t value, std::size_t start = 0,
                           std::size_t end = std::size_t(-1)) const;

    void find_all(Column* result, int64_t value, std::size_t col_offset = 0,
                  std::size_t begin = 0, std::size_t end = std::size_t(-1)) const;

    std::size_t find_first(int64_t value, std::size_t begin = 0,
                           std::size_t end = std::size_t(-1)) const;
protected:
    void ensure_not_null(int64_t value);
private:
    int_fast64_t choose_random_null(int64_t incoming);
    void replace_nulls_with(int64_t new_null);
    bool can_use_as_null(int64_t value);
};


// Implementation:

inline ArrayInteger::ArrayInteger(Array::no_prealloc_tag) TIGHTDB_NOEXCEPT:
    Array(Array::no_prealloc_tag())
{
}

inline ArrayInteger::ArrayInteger(Allocator& alloc) TIGHTDB_NOEXCEPT:
    Array(alloc)
{
}

inline MemRef ArrayInteger::create_array(Type type, bool context_flag, std::size_t size,
                                  int_fast64_t value, Allocator& alloc)
{
    return Array::create(type, context_flag, wtype_Bits, size, value, alloc); // Throws
}

inline void ArrayInteger::add(int64_t value)
{
    Array::add(value);
}

inline int64_t ArrayInteger::get(std::size_t ndx) const TIGHTDB_NOEXCEPT
{
    return Array::get(ndx);
}

inline uint64_t ArrayInteger::get_uint(std::size_t ndx) const TIGHTDB_NOEXCEPT
{
    return get(ndx);
}

inline int64_t ArrayInteger::get(const char* header, std::size_t ndx) TIGHTDB_NOEXCEPT
{
    return Array::get(header, ndx);
}

inline void ArrayInteger::set(std::size_t ndx, int64_t value)
{
    Array::set(ndx, value);
}

inline void ArrayInteger::set_uint(std::size_t ndx, uint_fast64_t value)
{
    // When a value of a signed type is converted to an unsigned type, the C++
    // standard guarantees that negative values are converted from the native
    // representation to 2's complement, but the effect of conversions in the
    // opposite direction is left unspecified by the
    // standard. `tightdb::util::from_twos_compl()` is used here to perform the
    // correct opposite unsigned-to-signed conversion, which reduces to a no-op
    // when 2's complement is the native representation of negative values.
    set(ndx, util::from_twos_compl<int_fast64_t>(value));
}


inline int64_t ArrayInteger::front() const TIGHTDB_NOEXCEPT
{
    return Array::front();
}

inline int64_t ArrayInteger::back() const TIGHTDB_NOEXCEPT
{
    return Array::back();
}

inline void ArrayInteger::adjust(std::size_t ndx, int_fast64_t diff)
{
    Array::adjust(ndx, diff);
}

inline void ArrayInteger::adjust(std::size_t begin, std::size_t end, int_fast64_t diff)
{
    Array::adjust(begin, end, diff);
}

inline void ArrayInteger::adjust_ge(int_fast64_t limit, int_fast64_t diff)
{
    Array::adjust_ge(limit, diff);
}

inline std::size_t ArrayInteger::lower_bound(int64_t value) const TIGHTDB_NOEXCEPT
{
    return lower_bound_int(value);
}

inline std::size_t ArrayInteger::upper_bound(int64_t value) const TIGHTDB_NOEXCEPT
{
    return upper_bound_int(value);
}


inline
ArrayIntNull::ArrayIntNull(no_prealloc_tag tag) TIGHTDB_NOEXCEPT: Array(tag)
{
}

inline
ArrayIntNull::ArrayIntNull(Allocator& alloc) TIGHTDB_NOEXCEPT: Array(alloc)
{
}

inline
ArrayIntNull::~ArrayIntNull() TIGHTDB_NOEXCEPT
{
}

inline
void ArrayIntNull::create(Type type, bool context_flag)
{
    MemRef r = create_array(type, context_flag, 0, 0, m_alloc);
    init_from_mem(r);
}

inline
std::size_t ArrayIntNull::size() const TIGHTDB_NOEXCEPT
{
    return Array::size() - 1;
}

inline
bool ArrayIntNull::is_empty() const TIGHTDB_NOEXCEPT
{
    return size() == 0;
}

inline
void ArrayIntNull::insert(std::size_t ndx, int_fast64_t value)
{
    ensure_not_null(value);
    Array::insert(ndx + 1, value);
}

inline
void ArrayIntNull::add(int64_t value)
{
    ensure_not_null(value);
    Array::add(value);
}

inline
void ArrayIntNull::set(std::size_t ndx, int64_t value)
{
    ensure_not_null(value);
    Array::set(ndx + 1, value);
}

inline
void ArrayIntNull::set_uint(std::size_t ndx, uint64_t value)
{
    ensure_not_null(value);
    Array::set(ndx + 1, value);
}

inline
int64_t ArrayIntNull::get(std::size_t ndx) const TIGHTDB_NOEXCEPT
{
    return Array::get(ndx + 1);
}

inline
uint64_t ArrayIntNull::get_uint(std::size_t ndx) const TIGHTDB_NOEXCEPT
{
    return Array::get(ndx + 1);
}

inline
int64_t ArrayIntNull::get(const char* header, std::size_t ndx) TIGHTDB_NOEXCEPT
{
    return Array::get(header, ndx + 1);
}

inline
void ArrayIntNull::set_null(std::size_t ndx) TIGHTDB_NOEXCEPT
{
    Array::set(ndx + 1, null_value());
}

inline
bool ArrayIntNull::is_null(std::size_t ndx) TIGHTDB_NOEXCEPT
{
    return Array::get(ndx + 1) == null_value();
}

inline
int64_t ArrayIntNull::null_value() const TIGHTDB_NOEXCEPT
{
    return Array::get(0);
}

inline
int64_t ArrayIntNull::operator[](std::size_t ndx) const TIGHTDB_NOEXCEPT
{
    return get(ndx);
}

inline    
int64_t ArrayIntNull::front() const TIGHTDB_NOEXCEPT
{
    return get(0);
}

inline
int64_t ArrayIntNull::back() const TIGHTDB_NOEXCEPT
{
    return Array::back();
}

inline
void ArrayIntNull::erase(std::size_t ndx)
{
    Array::erase(ndx + 1);
}

inline
void ArrayIntNull::erase(std::size_t begin, std::size_t end)
{
    Array::erase(begin + 1, end + 1);
}

inline
void ArrayIntNull::truncate(std::size_t size)
{
    Array::truncate(size + 1);
}

inline
void ArrayIntNull::clear()
{
    truncate(0);
}

inline
void ArrayIntNull::set_all_to_zero()
{
    for (size_t i = 0; i < size(); ++i) {
        set(i, 0);
    }
}

inline
void ArrayIntNull::move(std::size_t begin, std::size_t end, std::size_t dest_begin)
{
    Array::move(begin + 1, end + 1, dest_begin + 1);
}

inline
void ArrayIntNull::move_backward(std::size_t begin, std::size_t end, std::size_t dest_end)
{
    Array::move_backward(begin + 1, end + 1, dest_end + 1);
}

inline
std::size_t ArrayIntNull::lower_bound(int64_t value) const TIGHTDB_NOEXCEPT
{
    // FIXME: Consider this behaviour with NULLs.
    // Array::lower_bound_int assumes an already sorted array, but
    // this array could be sorted with nulls first or last.
    return Array::lower_bound_int(value);
}

inline
std::size_t ArrayIntNull::upper_bound(int64_t value) const TIGHTDB_NOEXCEPT
{
    // FIXME: see lower_bound
    return Array::upper_bound_int(value);
}

inline
int64_t ArrayIntNull::sum(std::size_t start, std::size_t end) const
{
    // FIXME: Optimize!
    int64_t null = null_value();
    int64_t sum = 0;
    if (end == size_t(-1))
        end = size();
    for (size_t i = start; i < end; ++i) {
        int64_t x = get(i);
        if (x != null) {
            sum += x;
        }
    }
    return sum;
}

inline
std::size_t ArrayIntNull::count(int64_t value) const TIGHTDB_NOEXCEPT
{
    // FIXME: Optimize!
    std::size_t count = 0;
    for (size_t i = 0; i < size(); ++i) {
        if (get(i) == value) {
            ++count;
        }
    }
    return count;
}

inline
bool ArrayIntNull::maximum(int64_t& result, std::size_t start, std::size_t end, std::size_t* return_ndx) const
{
    ++start;
    if (end != std::size_t(-1)) {
        ++end;
    }
    bool r = Array::maximum(result, start, end, return_ndx);
    if (return_ndx) {
        --(*return_ndx);
    }
    return r;
}

inline
bool ArrayIntNull::minimum(int64_t& result, std::size_t start, std::size_t end, std::size_t* return_ndx) const
{
    ++start;
    if (end != std::size_t(-1)) {
        ++end;
    }
    bool r = Array::minimum(result, start, end, return_ndx);
    if (return_ndx) {
        --(*return_ndx);
    }
    return r;
}

inline
bool ArrayIntNull::find(int cond, Action action, int64_t value, std::size_t start, std::size_t end, std::size_t baseindex, QueryState<int64_t>* state) const
{
    ++start;
    if (end != std::size_t(-1)) {
        ++end;
    }
    return Array::find(cond, action, value, start, end, baseindex, state);
}


template<class cond, Action action, std::size_t bitwidth, class Callback>
bool ArrayIntNull::find(int64_t value, std::size_t start, std::size_t end, std::size_t baseindex, QueryState<int64_t>* state, Callback callback) const
{
    ++start;
    if (end != std::size_t(-1)) {
        ++end;
    }
    return Array::find<cond, Action, bitwidth, Callback>(value, start, end, baseindex, state, callback);
}


template<class cond, Action action, std::size_t bitwidth>
bool ArrayIntNull::find(int64_t value, std::size_t start, std::size_t end, std::size_t baseindex, QueryState<int64_t>* state) const
{
    ++start;
    if (end != std::size_t(-1)) {
        ++end;
    }
    return Array::find<cond, Action, bitwidth>(value, start, end, baseindex, state);
}


template<class cond, Action action, class Callback>
bool ArrayIntNull::find(int64_t value, std::size_t start, std::size_t end, std::size_t baseindex, QueryState<int64_t>* state, Callback callback) const
{
    ++start;
    if (end != std::size_t(-1)) {
        ++end;
    }
    return Array::find<cond, Action, Callback>(value, start, end, baseindex, state, callback);
}


template<Action action, class Callback>
bool ArrayIntNull::find_action(std::size_t index, int64_t value, QueryState<int64_t>* state, Callback callback) const
{
    ++index;
    return Array::find_action<Action, Callback>(index, value, state, callback);
}


template<Action action, class Callback>
bool ArrayIntNull::find_action_pattern(std::size_t index, uint64_t pattern, QueryState<int64_t>* state, Callback callback) const
{
    ++index;
    return Array::find_action_pattern<Action, Callback>(index, pattern, state, callback);
}


template<class cond>
std::size_t ArrayIntNull::find_first(int64_t value, std::size_t start, std::size_t end) const
{
    ++start;
    if (end != std::size_t(-1)) {
        ++end;
    }
    return Array::find_first<cond>(value, start, end);
}

inline
void ArrayIntNull::find_all(Column* result, int64_t value, std::size_t col_offset, std::size_t begin, std::size_t end) const
{
    ++begin;
    if (end != std::size_t(-1)) {
        ++end;
    }
    Array::find_all(result, value, col_offset, begin, end);
}

inline
std::size_t ArrayIntNull::find_first(int64_t value, std::size_t begin, std::size_t end) const
{
    ++begin;
    if (end != std::size_t(-1)) {
        ++end;
    }
    return Array::find_first(value, begin, end);
}


}

#endif // TIGHTDB_ARRAY_INTEGER_HPP
