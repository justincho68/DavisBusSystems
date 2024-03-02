#include <gtest/gtest.h>
#include "OpenStreetMap.h"
#include "StringDataSink.h"
#include "XMLReader.h"
#include "StringDataSource.h"

TEST(OpenStreetMap, SimpleExampleTest){
    auto InStream = std::make_shared<CStringDataSource>("<?xml version='1.0' encoding='UTF-8'?>"
                                                        "<osm version=\"0.6\" generator=\"osmconvert 0.8.5\">"
                                                        "<node id=\"1\" lat=\"1.0\" lon=\"-1.0\"/>"
                                                        "</osm>");
    auto Reader = std::make_shared<CXMLReader>(InStream);
    COpenStreetMap StreetMap(Reader);
    
    EXPECT_EQ(StreetMap.NodeCount(),1);
    EXPECT_EQ(StreetMap.WayCount(),0);
    EXPECT_EQ(StreetMap.NodeByIndex(0)->ID(),1);
}

TEST(OpenStreetMap, HandlesEmptyXML) {
    auto inStream = std::make_shared<CStringDataSource>("<?xml version='1.0' encoding='UTF-8'?><osm></osm>");
    auto reader = std::make_shared<CXMLReader>(inStream);
    COpenStreetMap streetMap(reader);

    EXPECT_EQ(streetMap.NodeCount(), 0);
    EXPECT_EQ(streetMap.WayCount(), 0);
}

TEST(OpenStreetMap, ParsesMultipleNodesAndWays) {
    auto inStream = std::make_shared<CStringDataSource>(
        "<?xml version='1.0' encoding='UTF-8'?>"
        "<osm>"
        "<node id='1' lat='10.0' lon='10.0'><tag k='amenity' v='cafe'/></node>"
        "<node id='2' lat='20.0' lon='20.0'></node>"
        "<way id='1'><nd ref='1'/><nd ref='2'/><tag k='highway' v='residential'/></way>"
        "</osm>");
    auto reader = std::make_shared<CXMLReader>(inStream);
    COpenStreetMap streetMap(reader);

    EXPECT_EQ(streetMap.NodeCount(), 2);
    EXPECT_EQ(streetMap.WayCount(), 1);
    auto firstNode = streetMap.NodeByIndex(0);
    EXPECT_EQ(firstNode->ID(), 1);
    EXPECT_TRUE(firstNode->HasAttribute("amenity"));
    EXPECT_EQ(firstNode->GetAttribute("amenity"), "cafe");

    auto way = streetMap.WayByIndex(0);
    EXPECT_EQ(way->NodeCount(), 2);
    EXPECT_TRUE(way->HasAttribute("highway"));
    EXPECT_EQ(way->GetAttribute("highway"), "residential");
}

TEST(OpenStreetMap, RetrievesNodeAndWayByID) {
    auto inStream = std::make_shared<CStringDataSource>(
        "<?xml version='1.0' encoding='UTF-8'?>"
        "<osm>"
        "<node id='100' lat='30.0' lon='30.0'></node>"
        "<way id='200'><nd ref='100'/></way>"
        "</osm>");
    auto reader = std::make_shared<CXMLReader>(inStream);
    COpenStreetMap streetMap(reader);

    auto node = streetMap.NodeByID(100);
    EXPECT_NE(node, nullptr);
    EXPECT_EQ(node->ID(), 100);

    auto way = streetMap.WayByID(200);
    EXPECT_NE(way, nullptr);
    EXPECT_EQ(way->ID(), 200);
}

TEST(OpenStreetMap, HandlesMissingAttributes) {
    auto inStream = std::make_shared<CStringDataSource>(
        "<?xml version='1.0' encoding='UTF-8'?>"
        "<osm>"
        "<node id='1' lat='40.0' lon='40.0'></node>"
        "</osm>");
    auto reader = std::make_shared<CXMLReader>(inStream);
    COpenStreetMap streetMap(reader);

    auto node = streetMap.NodeByIndex(0);
    EXPECT_FALSE(node->HasAttribute("nonexistent"));
    EXPECT_EQ(node->GetAttribute("nonexistent"), "");
}
