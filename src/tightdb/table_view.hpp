/*************************************************************************
 *
 * TIGHTDB CONFIDENTIAL
 * __________________
 *
 *  [2011] - [2012] TightDB Inc
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
 **************************************************************************/
#ifndef TIGHTDB_TABLE_VIEW_HPP
#define TIGHTDB_TABLE_VIEW_HPP

#include <iostream>

#include <tightdb/table.hpp>

namespace tightdb {


using std::size_t;


/// Common base class for TableView and ConstTableView.
class TableViewBase {
public:
    bool is_empty() const TIGHTDB_NOEXCEPT { return m_refs.is_empty(); }
    std::size_t size() const TIGHTDB_NOEXCEPT { return m_refs.size(); }

    // Column information
    size_t      get_column_count() const TIGHTDB_NOEXCEPT;
    StringData  get_column_name(size_t column_ndx) const TIGHTDB_NOEXCEPT;
    size_t      get_column_index(StringData name) const;
    DataType    get_column_type(size_t column_ndx) const TIGHTDB_NOEXCEPT;

    // Getting values
    int64_t     get_int(size_t column_ndx, size_t row_ndx) const TIGHTDB_NOEXCEPT;
    bool        get_bool(size_t column_ndx, size_t row_ndx) const TIGHTDB_NOEXCEPT;
    DateTime    get_datetime(size_t column_ndx, size_t row_ndx) const TIGHTDB_NOEXCEPT;
    float       get_float(size_t column_ndx, size_t row_ndx) const TIGHTDB_NOEXCEPT;
    double      get_double(size_t column_ndx, size_t row_ndx) const TIGHTDB_NOEXCEPT;
    StringData  get_string(size_t column_ndx, size_t row_ndx) const TIGHTDB_NOEXCEPT;
    BinaryData  get_binary(size_t column_ndx, size_t row_ndx) const TIGHTDB_NOEXCEPT;
    Mixed       get_mixed(size_t column_ndx, size_t row_ndx) const TIGHTDB_NOEXCEPT;
    DataType    get_mixed_type(size_t column_ndx, size_t row_ndx) const TIGHTDB_NOEXCEPT;

    // Subtables
    size_t      get_subtable_size(size_t column_ndx, size_t row_ndx) const TIGHTDB_NOEXCEPT;

    // Searching (Int and String)
    size_t find_first_int(size_t column_ndx, int64_t value) const;
    size_t find_first_bool(size_t column_ndx, bool value) const;
    size_t find_first_datetime(size_t column_ndx, DateTime value) const;
    size_t find_first_float(size_t column_ndx, float value) const;
    size_t find_first_double(size_t column_ndx, double value) const;
    size_t find_first_string(size_t column_ndx, StringData value) const;
    size_t find_first_binary(size_t column_ndx, BinaryData value) const;

    // Aggregate functions. count_target is ignored by all <int function> except Count. Hack because of bug in optional
    // arguments in clang and vs2010 (fixed in 2012)
    template <int function, typename T, typename R, class ColType>
    R aggregate(R (ColType::*aggregateMethod)(size_t, size_t, size_t) const, size_t column_ndx, T count_target) const;

    int64_t sum_int(size_t column_ndx) const;
    int64_t maximum_int(size_t column_ndx) const;
    int64_t minimum_int(size_t column_ndx) const;
    double average_int(size_t column_ndx) const;
    size_t count_int(size_t column_ndx, int64_t target) const;

    double sum_float(size_t column_ndx) const;
    float maximum_float(size_t column_ndx) const;
    float minimum_float(size_t column_ndx) const;
    double average_float(size_t column_ndx) const;
    size_t count_float(size_t column_ndx, float target) const;

    double sum_double(size_t column_ndx) const;
    double maximum_double(size_t column_ndx) const;
    double minimum_double(size_t column_ndx) const;
    double average_double(size_t column_ndx) const;
    size_t count_double(size_t column_ndx, double target) const;

    DateTime maximum_datetime(size_t column_ndx) const;
    DateTime minimum_datetime(size_t column_ndx) const;

    // Sort the view according to the specified column and the
    // specified direction.
    void sort(size_t column_ndx, bool ascending = true);

    // Get row index in the source table this view is "looking" at.
    size_t get_source_ndx(size_t row_ndx) const TIGHTDB_NOEXCEPT
    {
        return size_t(m_refs.get(row_ndx));
    }

    // Conversion
    void to_json(std::ostream& out) const;
    void to_string(std::ostream& out, size_t limit=500) const;
    void row_to_string(std::size_t row_ndx, std::ostream& out) const;

protected:
    TableRef m_table;
    Array m_refs;


/// Copy constructor.
    /// Construct null view (no memory allocated).
    TableViewBase(): m_refs(Allocator::get_default()) {}

    /// Construct empty view, ready for addition of row indices.
    TableViewBase(Table* parent): m_table(parent->get_table_ref()) {}

    /// Copy constructor.
    TableViewBase(const TableViewBase& tv):
        m_table(tv.m_table), m_refs(tv.m_refs, Allocator::get_default()) {}

    /// Moving constructor.
    TableViewBase(TableViewBase*) TIGHTDB_NOEXCEPT;

    ~TableViewBase() TIGHTDB_NOEXCEPT;

    void move_assign(TableViewBase*) TIGHTDB_NOEXCEPT;

    Array& get_ref_column() TIGHTDB_NOEXCEPT { return m_refs; }
    const Array& get_ref_column() const TIGHTDB_NOEXCEPT { return m_refs; }

    template <class R, class V> static R find_all_integer(V*, std::size_t, int64_t);
    template <class R, class V> static R find_all_float(V*, std::size_t, float);
    template <class R, class V> static R find_all_double(V*, std::size_t, double);
    template <class R, class V> static R find_all_string(V*, std::size_t, StringData);

private:
    std::size_t find_first_integer(std::size_t column_ndx, int64_t value) const;

    friend class Table;
    friend class Query;
};



class ConstTableView;



/// A TableView gives read and write access to the parent table.
///
/// A 'const TableView' cannot be changed (e.g. sorted), nor can the
/// parent table be modified through it.
///
/// A TableView is both copyable and movable. Copying a TableView
/// makes a proper copy. Copying a temporary TableView is optimized
/// away on all modern compilers due to such things as 'return value
/// optimization'. Move semantics is accessed using the move()
/// function. 
///
/// You should use 'return tv' whenever the type of 'tv' matches the
/// return type in the function signature exactly, such as
/// �T fun() { return T(...); }� or �T fun() { T tv; return tv }� to
/// enable return-value-optimization and named-return-value-optimization
/// respectively.
///
/// You should use 'return move(tv)' whenever the type of 'tv' mismatch
/// the signature (where 'tv' needs conversion to return type), such as 
/// �ConstTableView fun() {TableView tv; return move(tv);}� to enable
/// move-semantics.
///
/// Avoid return(tv) whenever possible because it inhibits rvo and nrvo. 
/// �return tv� has been benchmarked to be slower than �return move(tv)� 
/// for both VC2012 and GCC 4.7 in many cases but never the opposite.
//
/// Note that move(tv) removes the contents from tv and leaves it
/// truncated.
///
/// FIXME: Add general documentation about move semantics, and refer
/// to it from here.
class TableView: public TableViewBase {
public:
    TableView() {}
    ~TableView() TIGHTDB_NOEXCEPT {}
    TableView& operator=(TableView tv) { move_assign(&tv); return *this; }
    friend TableView move(TableView& tv) { return TableView(&tv); }

    // Subtables
    TableRef      get_subtable(size_t column_ndx, size_t row_ndx);
    ConstTableRef get_subtable(size_t column_ndx, size_t row_ndx) const;
    void          clear_subtable(size_t column_ndx, size_t row_ndx);

    // Setting values
    void set_int(size_t column_ndx, size_t row_ndx, int64_t value);
    void set_bool(size_t column_ndx, size_t row_ndx, bool value);
    void set_datetime(size_t column_ndx, size_t row_ndx, DateTime value);
    template<class E> void set_enum(size_t column_ndx, size_t row_ndx, E value);
    void set_float(size_t column_ndx, size_t row_ndx, float value);
    void set_double(size_t column_ndx, size_t row_ndx, double value);
    void set_string(size_t column_ndx, size_t row_ndx, StringData value);
    void set_binary(size_t column_ndx, size_t row_ndx, BinaryData value);
    void set_mixed(size_t column_ndx, size_t row_ndx, Mixed value);
    void set_subtable(size_t column_ndx,size_t row_ndx, const Table* table);
    void add_int(size_t column_ndx, int64_t value);

    // Deleting
    void clear();
    void remove(size_t row_ndx);
    void remove_last() { if (!is_empty()) remove(size()-1); }

    // Searching (Int and String)
    TableView       find_all_int(size_t column_ndx, int64_t value);
    ConstTableView  find_all_int(size_t column_ndx, int64_t value) const;
    TableView       find_all_bool(size_t column_ndx, bool value);
    ConstTableView  find_all_bool(size_t column_ndx, bool value) const;
    TableView       find_all_datetime(size_t column_ndx, DateTime value);
    ConstTableView  find_all_datetime(size_t column_ndx, DateTime value) const;
    TableView       find_all_float(size_t column_ndx, float value);
    ConstTableView  find_all_float(size_t column_ndx, float value) const;
    TableView       find_all_double(size_t column_ndx, double value);
    ConstTableView  find_all_double(size_t column_ndx, double value) const;
    TableView       find_all_string(size_t column_ndx, StringData value);
    ConstTableView  find_all_string(size_t column_ndx, StringData value) const;
    // FIXME: Need: TableView find_all_binary(size_t column_ndx, BinaryData value);
    // FIXME: Need: ConstTableView find_all_binary(size_t column_ndx, BinaryData value) const;

    Table& get_parent() TIGHTDB_NOEXCEPT { return *m_table; }
    const Table& get_parent() const TIGHTDB_NOEXCEPT { return *m_table; }

private:
    TableView(Table& parent): TableViewBase(&parent) {}
    TableView(TableView* tv) TIGHTDB_NOEXCEPT: TableViewBase(tv) {}
    TableView(ConstTableView tv);

    TableView find_all_integer(size_t column_ndx, int64_t value);
    ConstTableView find_all_integer(size_t column_ndx, int64_t value) const;

    friend class ConstTableView;
    friend class Table;
    friend class Query;
    friend class TableViewBase;
};




/// A ConstTableView gives read access to the parent table, but no
/// write access. The view itself, though, can be changed, for
/// example, it can be sorted.
///
/// Note that methods are declared 'const' if, and only if they leave
/// the view unmodified, and this is irrespective of whether they
/// modify the parent table.
///
/// A ConstTableView has both copy and move semantics. See TableView
/// for more on this.
class ConstTableView: public TableViewBase {
public:
    ConstTableView() {}
    ~ConstTableView() TIGHTDB_NOEXCEPT {}
    ConstTableView& operator=(ConstTableView tv) { move_assign(&tv); return *this; }
    friend ConstTableView move(ConstTableView& tv) { return ConstTableView(&tv); }

    ConstTableView(TableView tv): TableViewBase(&tv) {}
    ConstTableView& operator=(TableView tv) { move_assign(&tv); return *this; }

    // Getting values
    ConstTableRef get_subtable(size_t column_ndx, size_t row_ndx) const;

    // Searching (Int and String)
    ConstTableView find_all_int(size_t column_ndx, int64_t value) const;
    ConstTableView find_all_bool(size_t column_ndx, bool value) const;
    ConstTableView find_all_datetime(size_t column_ndx, DateTime value) const;
    ConstTableView find_all_float(size_t column_ndx, float value) const;
    ConstTableView find_all_double(size_t column_ndx, double value) const;
    ConstTableView find_all_string(size_t column_ndx, StringData value) const;

   const Table& get_parent() const TIGHTDB_NOEXCEPT { return *m_table; }

private:
    ConstTableView(const Table& parent): TableViewBase(const_cast<Table*>(&parent)) {}
    ConstTableView(ConstTableView* tv) TIGHTDB_NOEXCEPT: TableViewBase(tv) {}

    ConstTableView find_all_integer(size_t column_ndx, int64_t value) const;

    friend class TableView;
    friend class Table;
    friend class Query;
    friend class TableViewBase;
};




// ================================================================================================
// TableViewBase Implementation:


inline TableViewBase::~TableViewBase() TIGHTDB_NOEXCEPT
{
    m_refs.destroy();
}
 
inline TableViewBase::TableViewBase(TableViewBase* tv) TIGHTDB_NOEXCEPT:
    m_table(tv->m_table),
    m_refs(tv->m_refs) // Note: This is a moving copy
{
    tv->m_table = TableRef();
    tv->m_refs.detach();
}

inline void TableViewBase::move_assign(TableViewBase* tv) TIGHTDB_NOEXCEPT
{
    m_table = tv->m_table;
    tv->m_table = TableRef();
    m_refs.move_assign(tv->m_refs);
}



#define TIGHTDB_ASSERT_COLUMN(column_ndx)                                   \
    TIGHTDB_ASSERT(m_table);                                                \
    TIGHTDB_ASSERT(column_ndx < m_table->get_column_count());

#define TIGHTDB_ASSERT_COLUMN_AND_TYPE(column_ndx, column_type)             \
    TIGHTDB_ASSERT_COLUMN(column_ndx)                                       \
    TIGHTDB_ASSERT(m_table->get_column_type(column_ndx) == column_type ||   \
                  (m_table->get_column_type(column_ndx) == type_DateTime && column_type == type_Int));

#define TIGHTDB_ASSERT_INDEX(column_ndx, row_ndx)                           \
    TIGHTDB_ASSERT_COLUMN(column_ndx)                                       \
    TIGHTDB_ASSERT(row_ndx < m_refs.size());

#define TIGHTDB_ASSERT_INDEX_AND_TYPE(column_ndx, row_ndx, column_type)     \
    TIGHTDB_ASSERT_COLUMN_AND_TYPE(column_ndx, column_type)                 \
    TIGHTDB_ASSERT(row_ndx < m_refs.size());

#define TIGHTDB_ASSERT_INDEX_AND_TYPE_TABLE_OR_MIXED(column_ndx, row_ndx)   \
    TIGHTDB_ASSERT_COLUMN(column_ndx)                                       \
    TIGHTDB_ASSERT(m_table->get_column_type(column_ndx) == type_Table ||    \
                   (m_table->get_column_type(column_ndx) == type_Mixed));   \
    TIGHTDB_ASSERT(row_ndx < m_refs.size());

// Column information


inline size_t TableViewBase::get_column_count() const TIGHTDB_NOEXCEPT
{
    TIGHTDB_ASSERT(m_table);
    return m_table->get_column_count();
}

inline StringData TableViewBase::get_column_name(size_t column_ndx) const TIGHTDB_NOEXCEPT
{
    TIGHTDB_ASSERT(m_table);
    return m_table->get_column_name(column_ndx);
}

inline size_t TableViewBase::get_column_index(StringData name) const
{
    TIGHTDB_ASSERT(m_table);
    return m_table->get_column_index(name);
}

inline DataType TableViewBase::get_column_type(size_t column_ndx) const TIGHTDB_NOEXCEPT
{
    TIGHTDB_ASSERT(m_table);
    return m_table->get_column_type(column_ndx);
}


// Getters


inline int64_t TableViewBase::get_int(size_t column_ndx, size_t row_ndx) const
    TIGHTDB_NOEXCEPT
{
    TIGHTDB_ASSERT_INDEX(column_ndx, row_ndx);

    const size_t real_ndx = size_t(m_refs.get(row_ndx));
    return m_table->get_int(column_ndx, real_ndx);
}

inline bool TableViewBase::get_bool(size_t column_ndx, size_t row_ndx) const
    TIGHTDB_NOEXCEPT
{
    TIGHTDB_ASSERT_INDEX_AND_TYPE(column_ndx, row_ndx, type_Bool);

    const size_t real_ndx = size_t(m_refs.get(row_ndx));
    return m_table->get_bool(column_ndx, real_ndx);
}

inline DateTime TableViewBase::get_datetime(size_t column_ndx, size_t row_ndx) const
    TIGHTDB_NOEXCEPT
{
    TIGHTDB_ASSERT_INDEX_AND_TYPE(column_ndx, row_ndx, type_DateTime);

    const size_t real_ndx = size_t(m_refs.get(row_ndx));
    return m_table->get_datetime(column_ndx, real_ndx);
}

inline float TableViewBase::get_float(size_t column_ndx, size_t row_ndx) const
    TIGHTDB_NOEXCEPT
{
    TIGHTDB_ASSERT_INDEX_AND_TYPE(column_ndx, row_ndx, type_Float);

    const size_t real_ndx = size_t(m_refs.get(row_ndx));
    return m_table->get_float(column_ndx, real_ndx);
}

inline double TableViewBase::get_double(size_t column_ndx, size_t row_ndx) const
    TIGHTDB_NOEXCEPT
{
    TIGHTDB_ASSERT_INDEX_AND_TYPE(column_ndx, row_ndx, type_Double);

    const size_t real_ndx = size_t(m_refs.get(row_ndx));
    return m_table->get_double(column_ndx, real_ndx);
}

inline StringData TableViewBase::get_string(size_t column_ndx, size_t row_ndx) const
    TIGHTDB_NOEXCEPT
{
    TIGHTDB_ASSERT_INDEX_AND_TYPE(column_ndx, row_ndx, type_String);

    const size_t real_ndx = size_t(m_refs.get(row_ndx));
    return m_table->get_string(column_ndx, real_ndx);
}

inline BinaryData TableViewBase::get_binary(size_t column_ndx, size_t row_ndx) const
    TIGHTDB_NOEXCEPT
{
    TIGHTDB_ASSERT_INDEX_AND_TYPE(column_ndx, row_ndx, type_Binary);

    const size_t real_ndx = size_t(m_refs.get(row_ndx));
    return m_table->get_binary(column_ndx, real_ndx); // Throws
}

inline Mixed TableViewBase::get_mixed(size_t column_ndx, size_t row_ndx) const
    TIGHTDB_NOEXCEPT
{
    TIGHTDB_ASSERT_INDEX_AND_TYPE(column_ndx, row_ndx, type_Mixed);

    const size_t real_ndx = size_t(m_refs.get(row_ndx));
    return m_table->get_mixed(column_ndx, real_ndx); // Throws
}

inline DataType TableViewBase::get_mixed_type(size_t column_ndx, size_t row_ndx) const
    TIGHTDB_NOEXCEPT
{
    TIGHTDB_ASSERT_INDEX_AND_TYPE(column_ndx, row_ndx, type_Mixed);

    const size_t real_ndx = size_t(m_refs.get(row_ndx));
    return m_table->get_mixed_type(column_ndx, real_ndx);
}

inline size_t TableViewBase::get_subtable_size(size_t column_ndx, size_t row_ndx) const
    TIGHTDB_NOEXCEPT
{
    TIGHTDB_ASSERT_INDEX_AND_TYPE_TABLE_OR_MIXED(column_ndx, row_ndx);

    const size_t real_ndx = size_t(m_refs.get(row_ndx));
    return m_table->get_subtable_size(column_ndx, real_ndx);
}


// Searching


inline size_t TableViewBase::find_first_int(size_t column_ndx, int64_t value) const
{
    TIGHTDB_ASSERT_COLUMN_AND_TYPE(column_ndx, type_Int);
    return find_first_integer(column_ndx, value);
}

inline size_t TableViewBase::find_first_bool(size_t column_ndx, bool value) const
{
    TIGHTDB_ASSERT_COLUMN_AND_TYPE(column_ndx, type_Bool);
    return find_first_integer(column_ndx, value ? 1 : 0);
}

inline size_t TableViewBase::find_first_datetime(size_t column_ndx, DateTime value) const
{
    TIGHTDB_ASSERT_COLUMN_AND_TYPE(column_ndx, type_DateTime);
    return find_first_integer(column_ndx, int64_t(value.get_datetime()));
}


template <class R, class V>
R TableViewBase::find_all_integer(V* view, size_t column_ndx, int64_t value)
{
    R tv(*view->m_table);
    for (size_t i = 0; i < view->m_refs.size(); i++)
        if (view->get_int(column_ndx, i) == value)
            tv.get_ref_column().add(view->get_source_ndx(i));
    return tv;
}

template <class R, class V>
R TableViewBase::find_all_float(V* view, size_t column_ndx, float value)
{
    R tv(*view->m_table);
    for (size_t i = 0; i < view->m_refs.size(); i++)
        if (view->get_float(column_ndx, i) == value)
            tv.get_ref_column().add(view->get_source_ndx(i));
    return tv;
}

template <class R, class V>
R TableViewBase::find_all_double(V* view, size_t column_ndx, double value)
{
    R tv(*view->m_table);
    for (size_t i = 0; i < view->m_refs.size(); i++)
        if (view->get_double(column_ndx, i) == value)
            tv.get_ref_column().add(view->get_source_ndx(i));
    return tv;
}

template <class R, class V>
R TableViewBase::find_all_string(V* view, size_t column_ndx, StringData value)
{
    TIGHTDB_ASSERT(view->m_table);
    TIGHTDB_ASSERT(column_ndx < view->m_table->get_column_count());
    TIGHTDB_ASSERT(view->m_table->get_column_type(column_ndx) == type_String);

    R tv(*view->m_table);
    for (size_t i = 0; i < view->m_refs.size(); i++) {
        if (view->get_string(column_ndx, i) == value)
            tv.get_ref_column().add(view->get_source_ndx(i));
    }
    return tv;
}


//-------------------------- TableView, ConstTableView implementation:

// - string
inline TableView TableView::find_all_string(size_t column_ndx, StringData value)
{
    return TableViewBase::find_all_string<TableView>(this, column_ndx, value);
}

inline ConstTableView TableView::find_all_string(size_t column_ndx, StringData value) const
{
    return TableViewBase::find_all_string<ConstTableView>(this, column_ndx, value);
}

inline ConstTableView ConstTableView::find_all_string(size_t column_ndx, StringData value) const
{
    return TableViewBase::find_all_string<ConstTableView>(this, column_ndx, value);
}

// - float
inline TableView TableView::find_all_float(size_t column_ndx, float value)
{
    return TableViewBase::find_all_float<TableView>(this, column_ndx, value);
}

inline ConstTableView TableView::find_all_float(size_t column_ndx, float value) const
{
    return TableViewBase::find_all_float<ConstTableView>(this, column_ndx, value);
}

inline ConstTableView ConstTableView::find_all_float(size_t column_ndx, float value) const
{
    return TableViewBase::find_all_float<ConstTableView>(this, column_ndx, value);
}


// - double
inline TableView TableView::find_all_double(size_t column_ndx, double value)
{
    return TableViewBase::find_all_double<TableView>(this, column_ndx, value);
}

inline ConstTableView TableView::find_all_double(size_t column_ndx, double value) const
{
    return TableViewBase::find_all_double<ConstTableView>(this, column_ndx, value);
}

inline ConstTableView ConstTableView::find_all_double(size_t column_ndx, double value) const
{
    return TableViewBase::find_all_double<ConstTableView>(this, column_ndx, value);
}



// -- 3 variants of the 3 find_all_{int, bool, date} all based on integer

inline TableView TableView::find_all_integer(size_t column_ndx, int64_t value)
{
    return TableViewBase::find_all_integer<TableView>(this, column_ndx, value);
}

inline ConstTableView TableView::find_all_integer(size_t column_ndx, int64_t value) const
{
    return TableViewBase::find_all_integer<ConstTableView>(this, column_ndx, value);
}

inline ConstTableView ConstTableView::find_all_integer(size_t column_ndx, int64_t value) const
{
    return TableViewBase::find_all_integer<ConstTableView>(this, column_ndx, value);
}


inline TableView TableView::find_all_int(size_t column_ndx, int64_t value)
{
    TIGHTDB_ASSERT_COLUMN_AND_TYPE(column_ndx, type_Int);
    return find_all_integer(column_ndx, value);
}

inline TableView TableView::find_all_bool(size_t column_ndx, bool value)
{
    TIGHTDB_ASSERT_COLUMN_AND_TYPE(column_ndx, type_Bool);
    return find_all_integer(column_ndx, value ? 1 : 0);
}

inline TableView TableView::find_all_datetime(size_t column_ndx, DateTime value)
{
    TIGHTDB_ASSERT_COLUMN_AND_TYPE(column_ndx, type_DateTime);
    return find_all_integer(column_ndx, int64_t(value.get_datetime()));
}


inline ConstTableView TableView::find_all_int(size_t column_ndx, int64_t value) const
{
    TIGHTDB_ASSERT_COLUMN_AND_TYPE(column_ndx, type_Int);
    return find_all_integer(column_ndx, value);
}

inline ConstTableView TableView::find_all_bool(size_t column_ndx, bool value) const
{
    TIGHTDB_ASSERT_COLUMN_AND_TYPE(column_ndx, type_Bool);
    return find_all_integer(column_ndx, value ? 1 : 0);
}

inline ConstTableView TableView::find_all_datetime(size_t column_ndx, DateTime value) const
{
    TIGHTDB_ASSERT_COLUMN_AND_TYPE(column_ndx, type_DateTime);
    return find_all_integer(column_ndx, int64_t(value.get_datetime()));
}


inline ConstTableView ConstTableView::find_all_int(size_t column_ndx, int64_t value) const
{
    TIGHTDB_ASSERT_COLUMN_AND_TYPE(column_ndx, type_Int);
    return find_all_integer(column_ndx, value);
}

inline ConstTableView ConstTableView::find_all_bool(size_t column_ndx, bool value) const
{
    TIGHTDB_ASSERT_COLUMN_AND_TYPE(column_ndx, type_Bool);
    return find_all_integer(column_ndx, value ? 1 : 0);
}

inline ConstTableView ConstTableView::find_all_datetime(size_t column_ndx, DateTime value) const
{
    TIGHTDB_ASSERT_COLUMN_AND_TYPE(column_ndx, type_DateTime);
    return find_all_integer(column_ndx, int64_t(value.get_datetime()));
}


// Subtables


inline TableRef TableView::get_subtable(size_t column_ndx, size_t row_ndx)
{
    TIGHTDB_ASSERT_INDEX_AND_TYPE_TABLE_OR_MIXED(column_ndx, row_ndx);

    const size_t real_ndx = size_t(m_refs.get(row_ndx));
    return m_table->get_subtable(column_ndx, real_ndx);
}

inline ConstTableRef TableView::get_subtable(size_t column_ndx, size_t row_ndx) const
{
    TIGHTDB_ASSERT_INDEX_AND_TYPE_TABLE_OR_MIXED(column_ndx, row_ndx);

    const size_t real_ndx = size_t(m_refs.get(row_ndx));
    return m_table->get_subtable(column_ndx, real_ndx);
}

inline ConstTableRef ConstTableView::get_subtable(size_t column_ndx, size_t row_ndx) const
{
    TIGHTDB_ASSERT_INDEX_AND_TYPE_TABLE_OR_MIXED(column_ndx, row_ndx);

    const size_t real_ndx = size_t(m_refs.get(row_ndx));
    return m_table->get_subtable(column_ndx, real_ndx);
}

inline void TableView::clear_subtable(size_t column_ndx, size_t row_ndx)
{
    TIGHTDB_ASSERT_INDEX_AND_TYPE_TABLE_OR_MIXED(column_ndx, row_ndx);

    const size_t real_ndx = size_t(m_refs.get(row_ndx));
    return m_table->clear_subtable(column_ndx, real_ndx);
}


// Setters


inline void TableView::set_int(size_t column_ndx, size_t row_ndx, int64_t value)
{
    TIGHTDB_ASSERT_INDEX_AND_TYPE(column_ndx, row_ndx, type_Int);

    const size_t real_ndx = size_t(m_refs.get(row_ndx));
    m_table->set_int(column_ndx, real_ndx, value);
}

inline void TableView::set_bool(size_t column_ndx, size_t row_ndx, bool value)
{
    TIGHTDB_ASSERT_INDEX_AND_TYPE(column_ndx, row_ndx, type_Bool);

    const size_t real_ndx = size_t(m_refs.get(row_ndx));
    m_table->set_bool(column_ndx, real_ndx, value);
}

inline void TableView::set_datetime(size_t column_ndx, size_t row_ndx, DateTime value)
{
    TIGHTDB_ASSERT_INDEX_AND_TYPE(column_ndx, row_ndx, type_DateTime);

    const size_t real_ndx = size_t(m_refs.get(row_ndx));
    m_table->set_datetime(column_ndx, real_ndx, value);
}

inline void TableView::set_float(size_t column_ndx, size_t row_ndx, float value)
{
    TIGHTDB_ASSERT_INDEX_AND_TYPE(column_ndx, row_ndx, type_Float);

    const size_t real_ndx = size_t(m_refs.get(row_ndx));
    m_table->set_float(column_ndx, real_ndx, value);
}

inline void TableView::set_double(size_t column_ndx, size_t row_ndx, double value)
{
    TIGHTDB_ASSERT_INDEX_AND_TYPE(column_ndx, row_ndx, type_Double);

    const size_t real_ndx = size_t(m_refs.get(row_ndx));
    m_table->set_double(column_ndx, real_ndx, value);
}

template<class E> inline void TableView::set_enum(size_t column_ndx, size_t row_ndx, E value)
{
    const size_t real_ndx = size_t(m_refs.get(row_ndx));
    m_table->set_int(column_ndx, real_ndx, value);
}

inline void TableView::set_string(size_t column_ndx, size_t row_ndx, StringData value)
{
    TIGHTDB_ASSERT_INDEX_AND_TYPE(column_ndx, row_ndx, type_String);

    const size_t real_ndx = size_t(m_refs.get(row_ndx));
    m_table->set_string(column_ndx, real_ndx, value);
}

inline void TableView::set_binary(size_t column_ndx, size_t row_ndx, BinaryData value)
{
    TIGHTDB_ASSERT_INDEX_AND_TYPE(column_ndx, row_ndx, type_Binary);

    const size_t real_ndx = size_t(m_refs.get(row_ndx));
    m_table->set_binary(column_ndx, real_ndx, value);
}

inline void TableView::set_mixed(size_t column_ndx, size_t row_ndx, Mixed value)
{
    TIGHTDB_ASSERT_INDEX_AND_TYPE(column_ndx, row_ndx, type_Mixed);

    const size_t real_ndx = size_t(m_refs.get(row_ndx));
    m_table->set_mixed(column_ndx, real_ndx, value);
}

inline void TableView::set_subtable(size_t column_ndx, size_t row_ndx, const Table* value)
{
    TIGHTDB_ASSERT_INDEX_AND_TYPE_TABLE_OR_MIXED(column_ndx, row_ndx);
    const size_t real_ndx = size_t(m_refs.get(row_ndx));
    m_table->set_subtable(column_ndx, real_ndx, value);
}


inline void TableView::add_int(size_t column_ndx, int64_t value)
{
    m_table->add_int(column_ndx, value);
}

} // namespace tightdb

#endif // TIGHTDB_TABLE_VIEW_HPP