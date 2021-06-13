#include "stdafx.h"
#include "Database.h"

#include <stdexcept>
#include <algorithm>
#include <iostream>
#include <assert.h>

Database::Database(const RecordsCollection& records)
{
    for (const auto& record : records)
    {
        AddRecord(record);
    }
}

void Database::AddRecord(const Record::Ptr& record)
{
    if (!AddUniqueRecordId(record->uniqueId))
    {
        return;//Don't add record with duplicate id
    }

    Record::InsertSorted(m_records, record);
    m_recordsColumn1Indexer.Add(record->column1, record);
    m_recordsColumn3Indexer.Add(record->column3, record);
    auto& collection = m_column2Indexer[record->column2];
    Record::InsertSorted(collection, record);
}

void Database::DeleteRecord(Record::IdType uniqueId)
{
    auto it = Record::FindInSortedCollection(m_records, uniqueId);
    if (it != m_records.end())
    {
        Record::Ptr tmp(*it);//Get it as shared ptr to check for ref count after we remove record from indexed data

        Record::DeleteRecord(m_records, tmp->uniqueId);

        auto col2It = m_column2Indexer.find(tmp->column2);
        if (col2It != m_column2Indexer.end())
        {
            Record::DeleteRecord(col2It->second, tmp->uniqueId);
        }

        m_recordsColumn1Indexer.Delete(tmp->column1, tmp->uniqueId);
        m_recordsColumn3Indexer.Delete(tmp->column3, tmp->uniqueId);
        m_uniqueIds.erase(tmp->uniqueId);
    }//after this scope the record will be deleted from Database, only Record::Ptr outside of Database will exist
}

Database::RecordsCollection Database::FindMatchingRecords(const Database& db, const std::string& columnName, const std::string& matcher)
{
    if ("column0" == columnName)
    {
        try
        {
            const Record::IdType id = std::stoul(matcher);
            auto record = db.FindRecordById(id);
            if (record)
            {
                return { record };
            }
            return {};
        }
        catch (std::exception e)
        {
            std::cout << "Converting failed: " << matcher << " with: " << e.what() << std::endl;
            return {};
        }
    }
    else if ("column1" == columnName)
    {
        return db.FindColumn1Records(matcher);
    }
    else if ("column2" == columnName)
    {
        try
        {
            const Record::Column2Type value = std::stol(matcher);
            return db.FindColumn2Records(value);
        }
        catch (std::exception e)
        {
            std::cout << "Converting failed: " << matcher << " with: " << e.what() << std::endl;
            return {};
        }
    }
    else if ("column3" == columnName)
    {
        return db.FindColumn3Records(matcher);
    }

    std::cout << "No matching column!" << std::endl;
    return {};
}

Record::Ptr Database::FindRecordById(Record::IdType id) const
{
    auto it = Record::FindInSortedCollection(m_records, id);
    if (it != m_records.end())
    {
        return *it;
    }

    return nullptr;
}

Database::RecordsCollection Database::FindColumn1Records(const Record::Column1Type& value) const
{
    auto node = m_recordsColumn1Indexer.Find(value);
    if (node)
    {
        return node->m_links;
    }

    return {};
}

Database::RecordsCollection Database::FindColumn2Records(Record::Column2Type value) const
{
    auto it = m_column2Indexer.find(value);
    if (it != m_column2Indexer.end())
    {
        return it->second;
    }
    return {};
}

Database::RecordsCollection Database::FindColumn3Records(const Record::Column3Type& value) const
{
    auto node = m_recordsColumn3Indexer.Find(value);
    if (node)
    {
        return node->m_links;
    }

    return {};
}

bool Database::AddUniqueRecordId(Record::IdType id)
{
    auto it = m_uniqueIds.find(id);
    if (it == m_uniqueIds.end())
    {
        m_uniqueIds.insert(id);
        return true;
    }
    else
    {
        std::cout << "Trying to add duplicate Id: " << id << " !" << std::endl;
        return false;
    }
}
