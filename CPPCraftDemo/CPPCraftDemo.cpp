#include "stdafx.h"

#include "BaseImplementation.h"
#include "Database.h"
#include "StringIndexerTree.h"
#include "Profiler.h"

#include <assert.h>
#include <iostream>

Database::RecordsCollection PopulateDummyData(const std::string& prefix, int numRecords)
{
    Database::RecordsCollection data;
    data.reserve(numRecords);
    for (uint32_t i = 0; i < numRecords; i++)
    {
        data.push_back(Record::Create(i, prefix + std::to_string(i), i % 100, std::to_string(i) + prefix));
    }

    return data;
}

int main(int argc, _TCHAR* argv[])
{
	RunBaseImplementation();//Base QB Implementation

    std::cout << "Populating database..." << std::endl;
    Database db(PopulateDummyData("testdata", 1000));

    {//Searching matching records and testing for correct size of returned records
        Profiler measure("Optimized FindMatchingRecords performance");
        auto testRecords = Database::FindMatchingRecords(db, "column1", "testdata500");
        auto col2 = Database::FindMatchingRecords(db, "column2", "24");

        assert(testRecords.size() == 1);//QB check for correct records count
    }///////////

    {//Deleting existing record
        Profiler measure("Delete operation");
        db.DeleteRecord(500);
    }//////////

    {//Searching the deleted record by column1 value
        Profiler measure("FindMatchingRecords after delete");
        auto testRecords = Database::FindMatchingRecords(db, "column1", "testdata500");
        assert(testRecords.empty());//Should have 0 records after deleting record 500
    }//////////

    {//Searching for column2 value with invalid data
        auto nonExistingRecords = Database::FindMatchingRecords(db, "column2", "-1200");
        assert(nonExistingRecords.empty());
    }//////////

    {//Long string test
        {//Adding record with long string
            Profiler measure("AddRecord with long string");
            db.AddRecord(Record::Create(3434, "Lorem ipsum dolor sit amet, consectetur adipiscing elit, sed do eiusmod tempor incididunt ut labore et dolore magna aliqua.", 9000, "s"));
        }//////////

        Database::RecordsCollection longStringCollection;
        {//Searching for substring of the long string
            Profiler measure("FindMatchingRecords substring from long string");
            longStringCollection = Database::FindMatchingRecords(db, "column1", "nt u");
        }//////////

        std::cout << "Long string record: ";
        for (const auto& rec : longStringCollection)
        {
            std::cout << "id: " << rec->uniqueId;
            std::cout << " col1: " << rec->column1;
            std::cout << " col2: " << rec->column2;
            std::cout << " col3: " << rec->column3;
        }
        std::cout << std::endl;
    }//Long string test

    {//Adding record with no strings
        Profiler measure("AddRecord with no strings");
        db.AddRecord(Record::Create(9777, "", 9777, ""));
    }//////////

    {//Adding record with duplicate id
        db.AddRecord(Record::Create(9777, "asd", 98888, "asd"));//record won't be added
    }//////////

    {//Searching for huge number
        auto hugeNumberRecords = Database::FindMatchingRecords(db, "column2", "123524530987253478925347925347682354872534");
        assert(hugeNumberRecords.empty());//expected: converting failed argument out of range
    }//////////

    {//Searching for id with invalid input
        auto invalidIdCollection = Database::FindMatchingRecords(db, "column0", "a99");
        assert(invalidIdCollection.empty());//expected: converting failed invalid argument
    }//////////

    {//Adding previously deleted id
        Profiler measure("AddRecord previously deleted id");
        db.AddRecord(Record::Create(500, "a", 99999, "b"));
    }//////////

	return 0;
}

