#include "stdafx.h"
#include "Record.h"
#include <algorithm>
#include <iostream>

namespace
{

struct RecordCompare
{
    bool operator()(const Record::Ptr& lhs, const Record::Ptr& rhs)
    {
        return lhs->uniqueId < rhs->uniqueId;
    }
};

}//end of namespace

Record::Record(IdType id, const Column1Type& col1, Column2Type col2, const Column3Type& col3)
: uniqueId(id)
, column1(col1)
, column2(col2)
, column3(col3)
{
}

Record::Ptr Record::Create(IdType id, const Column1Type& col1, Column2Type col2, const Column3Type& col3)
{
    return std::make_shared<Record>(id, col1, col2, col3);
}

void Record::InsertSorted(Collection& collection, const Record::Ptr& record)
{
    collection.insert(std::lower_bound(collection.begin(), collection.end(), record, RecordCompare()), record);
}

void Record::DeleteRecord(Collection& collection, Record::IdType id)
{
    collection.erase(std::remove_if(collection.begin(), collection.end(), [id](Record::Ptr r)
        {
            return r->uniqueId == id;
        }), collection.end());
}
