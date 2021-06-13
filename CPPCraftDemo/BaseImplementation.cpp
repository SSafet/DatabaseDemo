#include "BaseImplementation.h"
#include "stdafx.h"
#include <string>
#include <vector>
#include <algorithm>
#include <assert.h>
#include <chrono>
#include <iostream>
#include <ratio>

#include "Profiler.h"

/**
    Represents a Record Object
*/
struct QBRecord
{
    uint32_t column0; // unique id column
    std::string column1;
    long column2;
    std::string column3;
};

/**
Represents a Record Collections
*/
typedef std::vector<QBRecord> QBRecordCollection;

/**
    Return records that contains a string in the StringValue field
    records - the initial set of records to filter
    matchString - the string to search for
*/
QBRecordCollection QBFindMatchingRecords(const QBRecordCollection& records, const std::string& columnName, const std::string& matchString)
{
    QBRecordCollection result;
    std::copy_if(records.begin(), records.end(), std::back_inserter(result), [&](QBRecord rec)
        {
            if (columnName == "column0")
            {
                uint32_t matchValue = std::stoul(matchString);
                return matchValue == rec.column0;
            }
            else if (columnName == "column1")
            {
                return rec.column1.find(matchString) != std::string::npos;
            }
            else if (columnName == "column2")
            {
                long matchValue = std::stol(matchString);
                return matchValue == rec.column2;
            }
            else if (columnName == "column3")
            {
                return rec.column3.find(matchString) != std::string::npos;
            }
            else
            {
                return false;
            }
        });
    return result;
}

/**
    Utility to populate a record collection
    prefix - prefix for the string value for every record
    numRecords - number of records to populate in the collection
*/
QBRecordCollection populateDummyData(const std::string& prefix, int numRecords)
{
    QBRecordCollection data;
    data.reserve(numRecords);
    for (uint32_t i = 0; i < numRecords; i++)
    {
        QBRecord rec = { i, prefix + std::to_string(i), i % 100, std::to_string(i) + prefix };
        data.emplace_back(rec);
    }

    return data;
}

void RunBaseImplementation()
{
    using namespace std::chrono;
    // populate a bunch of data
    auto data = populateDummyData("testdata", 1000);
    // Find a record that contains and measure the perf
    auto startTimer = steady_clock::now();
    QBRecordCollection filteredSet;
    QBRecordCollection filteredSet2;

    {
        Profiler measure("Base QBFindMatchingRecords performance");
        filteredSet = QBFindMatchingRecords(data, "column1", "testdata500");
        filteredSet2 = QBFindMatchingRecords(data, "column2", "24");
    }

    std::cout << "profiler: " << double((steady_clock::now() - startTimer).count()) * steady_clock::period::num / steady_clock::period::den << std::endl;

    // make sure that the function is correct
    assert(filteredSet.size() == 1);
}