#pragma once
#include <stdint.h>
#include <memory>
#include <string>
#include <vector>

struct Record
{
    using Ptr = std::shared_ptr<Record>;
    using Collection = std::vector<Ptr>;

    using IdType = uint32_t;
    using Column1Type = std::string;
    using Column2Type = long;
    using Column3Type = std::string;

    Record(IdType id, const Column1Type& col1, Column2Type col2, const Column3Type& col3);

    //Helper function to create Record::Ptr
    static Ptr Create(IdType id, const Column1Type& col1, Column2Type col2, const Column3Type& col3);

    //Helper function to insert Record::Ptr sorted to Collection
    static void InsertSorted(Collection& collection, const Record::Ptr& record);

    //Helper function to delete all occurrances of a record from a collection
    static void DeleteRecord(Collection& collection, Record::IdType id);

    //Helper function to get iterator to a record from collection
    //return collection.end() if record doesn't exist
    template<typename T>
    static auto FindInSortedCollection(T& collection, Record::IdType id);

    IdType uniqueId = 0;
    Column1Type column1;
    Column2Type column2 = 0;
    Column3Type column3;
};

template<typename T>
auto Record::FindInSortedCollection(T& collection, Record::IdType id)
{
    auto it = std::lower_bound(collection.begin(), collection.end(), id,
        [](const Record::Ptr& lhs, uint32_t rhs)
        {
            return lhs->uniqueId < rhs;
        });

    if (it != collection.end())
    {
        if (it->get()->uniqueId == id)
        {
            return it;
        }
    }

    return collection.end();
}
