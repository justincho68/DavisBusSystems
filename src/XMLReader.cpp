#include "XMLReader.h"
#include <expat.h>
#include <queue>
#include <cstdio>
#include <iostream>
#include "StringDataSource.h"

struct CXMLReader::SImplementation {
    std::shared_ptr< CDataSource > DDataSource;
    XML_Parser DXMLParser;
    std::queue<SXMLEntity> DEntityQueue;

    SImplementation(std::shared_ptr< CDataSource > src) {
        DDataSource = src;
        DXMLParser = XML_ParserCreate(NULL);
        XML_SetStartElementHandler(DXMLParser, StartElementHandlerCallback);
        XML_SetEndElementHandler(DXMLParser, EndElementHandlerCallback);
        XML_SetCharacterDataHandler(DXMLParser, CharacterDataHandlerCallback);
        XML_SetUserData(DXMLParser, this);
    };

    ~SImplementation()
    {
        XML_ParserFree(DXMLParser);
    }

    bool End() const {
        return DDataSource->End() && DEntityQueue.empty();
    };
    
    bool ReadEntity(SXMLEntity &entity, bool skipcdata) {
        std::vector<char> DataBuffer;
        while(DEntityQueue.empty()) {
            if (DDataSource->Read(DataBuffer,256)) {
                XML_Parse(DXMLParser,DataBuffer.data(),DataBuffer.size(),DataBuffer.size()<256);
            } else {
                XML_Parse(DXMLParser,DataBuffer.data(),0,true);
            }
        }
        while(!DEntityQueue.empty() && skipcdata && DEntityQueue.front().DType == SXMLEntity::EType::CharData) {
            DEntityQueue.pop();
        }

        if(DEntityQueue.empty()) {
            return false;
        }
        entity=DEntityQueue.front();
        DEntityQueue.pop();
        return true;
    }


    void StartElementHandler(const std::string &name, const std::vector<std::string> &attrs) {
        SXMLEntity TempEntity;
        TempEntity.DNameData = name;
        TempEntity.DType = SXMLEntity::EType::StartElement;
        //std::cout<<"Processing StartElement: " << name << std::endl;
        for (size_t Index = 0; Index < attrs.size(); Index += 2) {
            TempEntity.SetAttribute(attrs[Index], attrs[Index+1]);
            //std::cout << "Set Attr: " << attrs[Index] << " = '" << attrs[Index+1] << "'" << std::endl;
        }
        //std::cout << "Queueing StartElement: " << TempEntity.DNameData << std::endl;
        DEntityQueue.push(TempEntity);
    };

    void EndElementHandler(const std::string &name) {
        SXMLEntity temp;
        temp.DNameData = name;
        temp.DType = SXMLEntity::EType::EndElement;
        //std::cout << "Queueing EndElement: " << temp.DNameData << std::endl;
        DEntityQueue.push(temp);
        //std::cout << "Processing EndElement: " << name << std::endl;
    }

    void CharacterDataHandler(const std::string &data) {
        if (!data.empty()) {
            SXMLEntity temp;
            temp.DType = SXMLEntity::EType::CharData;
            temp.DNameData = data;
            DEntityQueue.push(temp);
        }
    }

    static void StartElementHandlerCallback(void *context, const XML_Char *name, const XML_Char **atts) {
        SImplementation *ReaderObject = static_cast<SImplementation *>(context);
        std::vector<std::string> Attributes;
        auto AttrPtr = atts;
        
        for (int i = 0; atts[i]; i+=2) {
            std::string attrName = atts[i];
            std::string attrValue = atts[i+1];
            Attributes.push_back(attrName);
            Attributes.push_back(attrValue);
        }
        ReaderObject->StartElementHandler(name,Attributes);
    };

    static void EndElementHandlerCallback(void *context, const XML_Char *name) {
        SImplementation *ReaderObject = static_cast<SImplementation *>(context);
        ReaderObject->EndElementHandler(name);
    };

    static void CharacterDataHandlerCallback(void *context, const XML_Char *s, int len) {
        SImplementation *ReaderObject = static_cast<SImplementation *>(context);
        ReaderObject->CharacterDataHandler(std::string(s,len));
    };

};

CXMLReader::CXMLReader(std::shared_ptr< CDataSource > src)
{
    DImplementation = std::make_unique<SImplementation>(src);
}

CXMLReader::~CXMLReader()
{}

bool CXMLReader::End() const 
{
    return DImplementation->End();
}

bool CXMLReader::ReadEntity(SXMLEntity &entity, bool skipcdata)
{
    return DImplementation->ReadEntity(entity, skipcdata);
}
