#pragma once
#include "Record.h"
#include "StringIndexerTree.h"

#include <string>
#include <vector>
#include <map>
#include <unordered_set>

class Database
{
public:
    using RecordsCollection = Record::Collection;

    Database() = default;
    Database(const RecordsCollection& records);

    void AddRecord(const Record::Ptr& record);
    void DeleteRecord(Record::IdType uniqueId);

    static RecordsCollection FindMatchingRecords(const Database& db, const std::string& columnName, const std::string& matcher);

    Record::Ptr FindRecordById(Record::IdType id) const;
    RecordsCollection FindColumn1Records(const Record::Column1Type& value) const;
    RecordsCollection FindColumn2Records(Record::Column2Type value) const;
    RecordsCollection FindColumn3Records(const Record::Column3Type& value) const;

private:
    bool AddUniqueRecordId(Record::IdType id);

    RecordsCollection m_records;
    StringIndexerTree m_recordsColumn1Indexer;
    StringIndexerTree m_recordsColumn3Indexer;
    std::map<Record::Column2Type, RecordsCollection> m_column2Indexer;
    std::unordered_set<Record::IdType> m_uniqueIds;
};
