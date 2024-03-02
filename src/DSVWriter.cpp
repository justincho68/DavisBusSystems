#include "DSVWriter.h"

struct CDSVWriter::SImplementation {
    std::shared_ptr<CDataSink> Sink;
    char Delimiter;
    bool Quoteall;
    SImplementation(std::shared_ptr< CDataSink > sink, char delimiter, bool quoteall) {
        Sink = sink;
        Delimiter = delimiter;
        Quoteall = quoteall;
    }

    bool WriteRow(const std::vector<std::string> &row) {
        std::string line;
        //Loop through every value in row to check for quoteall
        for(const auto &field : row) {
            if (Quoteall) {
                line += '"' + field + '"' + Delimiter;
            }
            else {
                line += field + Delimiter;
            }
        }
        if (!line.empty()) {
            line.pop_back();
        }
        std::vector<char> buffer(line.begin(), line.end());
        return Sink->Write(buffer);
    }
};

CDSVWriter::CDSVWriter(std::shared_ptr< CDataSink> sink, char delimiter, bool quoteall) {
    DImplementation = std::make_unique<SImplementation>(sink, delimiter, quoteall);
}

CDSVWriter::~CDSVWriter() {

}

bool CDSVWriter::WriteRow(const std::vector<std::string> &row) {
    return DImplementation->WriteRow(row);
}