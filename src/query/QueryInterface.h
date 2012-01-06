
#ifndef Testing_Query_h
#define Testing_Query_h

#include <string>
#include "QueryEngine.h"

class Query {

public:
	Query& Equal(size_t column_id, int64_t value) {
		ParentNode *p = new NODE<int64_t, Column, EQUAL>(m_parent_node, value, column_id);
		m_parent_node = p;
		return *this;
	};
	Query& NotEqual(size_t column_id, int64_t value) {
		ParentNode *p = new NODE<int64_t, Column, NOTEQUAL>(m_parent_node, value, column_id);
		m_parent_node = p;
		return *this;
	};
	Query& Greater(size_t column_id, int64_t value) {
		ParentNode *p = new NODE<int64_t, Column, GREATER>(m_parent_node, value, column_id);
		m_parent_node = p;
		return *this;
	};
	Query& GreaterEqual(size_t column_id, int64_t value) {
		ParentNode *p = new NODE<int64_t, Column, GREATEREQUAL>(m_parent_node, value, column_id);
		m_parent_node = p;
		return *this;
	};
	Query& LessEqual(size_t column_id, int64_t value) {
		ParentNode *p = new NODE<int64_t, Column, LESSEQUAL>(m_parent_node, value, column_id);
		m_parent_node = p;
		return *this;
	};
	Query& Less(size_t column_id, int64_t value) {
		ParentNode *p = new NODE<int64_t, Column, LESS>(m_parent_node, value, column_id);
		m_parent_node = p;
		return *this;
	};
	Query& Equal(size_t column_id, const char *value) {
		ParentNode *p = new NODE<const char *, AdaptiveStringColumn, EQUAL>(m_parent_node, value, column_id);
		m_parent_node = p;
		return *this;
	};
	Query& NotEqual(size_t column_id, const char * value) {
		ParentNode *p = new NODE<const char *, AdaptiveStringColumn, NOTEQUAL>(m_parent_node, value, column_id);
		m_parent_node = p;
		return *this;
	};

	Query() : m_parent_node(0) {}

	TableView FindAll(Table& table, size_t start = 0, size_t end = -1) {
		TableView tv(table);
		size_t r = start - 1;
		if(end == -1)
			end = table.GetSize();
		for(;;) {
			r = m_parent_node->Find(r + 1, table.GetSize(), table);
			if(r == table.GetSize())
				break;
			tv.GetRefColumn().Add(r);
		}
		return tv;
	}

	size_t Find(Table& table, size_t start, size_t end = -1) {
		TableView tv(table);
		if(end == -1)
			end = table.GetSize();
		size_t r = m_parent_node->Find(start, end, table);
		if(r == table.GetSize())
			return (size_t)-1;
		else
			return r;
	}

protected:
	friend class XQueryAccessorInt;
	friend class XQueryAccessorString;
	ParentNode *m_parent_node;
};

class XQueryAccessorInt {
public:
	XQueryAccessorInt(size_t column_id) : m_column_id(column_id) {}
	Query& Equal(int64_t value) {return m_query->Equal(m_column_id, value);}
	Query& NotEqual(int64_t value) {return m_query->NotEqual(m_column_id, value);}
	Query& Greater(int64_t value) {return m_query->Greater(m_column_id, value);}
	Query& GreaterEqual(int64_t value) {return m_query->GreaterEqual(m_column_id, value);}
	Query& Less(int64_t value) {return m_query->Less(m_column_id, value);}
	Query& LessEqual(int64_t value) {return m_query->LessEqual(m_column_id, value);}
protected:
	Query* m_query;
	size_t m_column_id;
}; 
 
class XQueryAccessorString {
public:
	XQueryAccessorString(size_t column_id) : m_column_id(column_id) {}
	Query& Equal(const char *value) {return m_query->Equal(m_column_id, value);}
	Query& NotEqual(const char *value) {return m_query->NotEqual(m_column_id, value);}
protected:
	Query* m_query;
	size_t m_column_id;
};

#endif
