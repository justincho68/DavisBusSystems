#include <queue>
#include "XMLWriter.h"
#include "DataSink.h"
#include "StringDataSink.h"
#include <iostream>
#include <cstdio>
#include <expat.h>

struct CXMLWriter::SImplementation {
    std::shared_ptr<CDataSink> DataSink;
    std::queue<SXMLEntity> ItemQueue;

    SImplementation(std::shared_ptr<CDataSink> sink) {
        DataSink = sink;
    }

    bool WriteEntity(const SXMLEntity &entity) {
        std::string output;
        if(entity.DType == SXMLEntity::EType::StartElement) {
            output += "<" + entity.DNameData;
            for (const auto &attr : entity.DAttributes) {
                output += " " + attr.first + "=\"" + attr.second + "\"";
            }
            output += ">";
        }
        else if (entity.DType == SXMLEntity::EType::EndElement) {
            output += "</" + entity.DNameData + ">";
        }
        else if (entity.DType == SXMLEntity::EType::CharData) {
            output += entity.DNameData;
        }
        std::vector<char> data(output.begin(),output.end());
        DataSink->Write(data);
        return true;
    }
};

CXMLWriter::CXMLWriter(std::shared_ptr<CDataSink> sink) {
    DImplementation = std::make_unique<SImplementation>(std::move(sink));
}

CXMLWriter::~CXMLWriter() {
    Flush();
}

bool CXMLWriter::WriteEntity(const SXMLEntity &entity) {
    DImplementation->ItemQueue.push(entity);
    return Flush();
}

bool CXMLWriter::Flush() {
    bool success = true;
    while (!DImplementation->ItemQueue.empty()) {
        const auto &entity = DImplementation->ItemQueue.front();
        if (!DImplementation->WriteEntity(entity)) {
            success = false;
        }
        DImplementation->ItemQueue.pop();
    }
    return success;
}

