#include "DSVReader.h"
#include "DataSource.h"
#include "StringDataSource.h"
#include <vector>
#include <sstream>
#include <queue>


struct CDSVReader::SImplementation {
    std::shared_ptr<CDataSource> DataSource;
    char Delimiter;
    std::string Buffer;
    std::queue<std::vector<std::string>> Rows;

    SImplementation(std::shared_ptr< CDataSource > src, char delimiter) {
        DataSource = src;
        Delimiter = delimiter;
    }

    bool End() const {
        return DataSource->End();
    }
    
    bool ReadRow(std::vector< std::string > &row) {
        row.clear();
        char ch;
        std::string line;
        //Loop to read characters as long as they exist
        while(!DataSource->End() && DataSource->Get(ch)) {
            if (ch == '\n') {
                break;
            }
            line += ch; // Continue adding characters as long as it isnt newline
        }
        if (line.empty() && DataSource->End()) {
            return false;
        }

        std::stringstream ss(line);
        std::string betweenDelim;
        while(std::getline(ss, betweenDelim, Delimiter)) {
            row.push_back(betweenDelim); 
        }
        return true;//Only return true if a line was read succesfully
    }

};

CDSVReader::CDSVReader(std::shared_ptr<CDataSource> src, char delimiter)
: DImplementation(std::make_unique<SImplementation>(src, delimiter)) {
}

CDSVReader::~CDSVReader() {
}

bool CDSVReader::ReadRow(std::vector<std::string>& row) {
    return DImplementation->ReadRow(row);
}