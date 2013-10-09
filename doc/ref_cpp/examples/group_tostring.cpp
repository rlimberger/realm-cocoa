// @@Example: ex_cpp_group_tostring @@
// @@Fold@@
#include <iostream>
#include <sstream>
#include <tightdb.hpp>
#include <tightdb/file.hpp>

using namespace std;
using namespace tightdb;

TIGHTDB_TABLE_2(PeopleTable,
                name, String,
                age, Int)

int main()
{
    Group group;

    PeopleTable::Ref table = group.get_table<PeopleTable>("people");
    table->add("Mary", 14);
    table->add("Joe", 17);
    table->add("Jack", 22);
// @@EndFold@@

    ostringstream ss;
    group.to_string(ss);
    cout << ss.str() << endl;

// @@Fold@@
}
// @@EndFold@@
// @@EndExample@@