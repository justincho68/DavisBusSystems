#include <gtest/gtest.h>
#include "CSVBusSystem.h"
#include "StringDataSource.h"
#include "DSVReader.h"

TEST(CSVBusSystem, SimpleExampleTest){
    auto InStreamStops = std::make_shared<CStringDataSource>("stop_id,node_id\n1,1001");
    auto InStreamRoutes = std::make_shared<CStringDataSource>("route,stop_id");
    auto CSVReaderStops = std::make_shared<CDSVReader>(InStreamStops,',');
    auto CSVReaderRoutes = std::make_shared<CDSVReader>(InStreamRoutes,',');
    CCSVBusSystem BusSystem(CSVReaderStops, CSVReaderRoutes);

    EXPECT_EQ(BusSystem.StopCount(),1);
    EXPECT_EQ(BusSystem.RouteCount(),0);
    EXPECT_EQ(BusSystem.StopByIndex(0)->ID(),1);
}

TEST(CSVBusSystem, StopsandRoutesTest) {
    auto InStreamStops = std::make_shared<CStringDataSource>("stop_id,node_id\n1,1001\n2,2001\n3, 3001");
    auto InStreamRoutes = std::make_shared<CStringDataSource>("route,stop_id\nA,1\nA,2\nB,3\nC,1");
    auto CSVReaderStops = std::make_shared<CDSVReader>(InStreamStops,',');
    auto CSVReaderRoutes = std::make_shared<CDSVReader>(InStreamRoutes,',');
    CCSVBusSystem BusSystem(CSVReaderStops, CSVReaderRoutes);

    EXPECT_EQ(BusSystem.StopCount(),3);
    EXPECT_EQ(BusSystem.RouteCount(),3);
    EXPECT_EQ(BusSystem.StopByIndex(2)->ID(),3);
    EXPECT_EQ(BusSystem.StopByID(1)->NodeID(),1001);
    EXPECT_EQ(BusSystem.RouteByIndex(2)->Name(),"C");
    EXPECT_EQ(BusSystem.RouteByName("A")->Name(),"A"); 
}

TEST(CSVBusSystem, EdgeTest){
    auto InStreamStops = std::make_shared<CStringDataSource>("stop_id,node_id\n1,1001\n2,2001\n3, 3001");
    auto InStreamRoutes = std::make_shared<CStringDataSource>("route,stop_id\nA,1\nA,2\nB,3\nC,1");
    auto CSVReaderStops = std::make_shared<CDSVReader>(InStreamStops,',');
    auto CSVReaderRoutes = std::make_shared<CDSVReader>(InStreamRoutes,',');
    CCSVBusSystem BusSystem(CSVReaderStops, CSVReaderRoutes);

    EXPECT_EQ(BusSystem.StopByIndex(3),nullptr);
    EXPECT_EQ(BusSystem.StopByID(5), nullptr);
    EXPECT_EQ(BusSystem.RouteByIndex(4), nullptr);
    EXPECT_EQ(BusSystem.RouteByName("D"), nullptr); 
}

TEST(CSVBusSystem, MissingIDTest){
    auto InStreamStops = std::make_shared<CStringDataSource>("stop_id,node_id\n1,1001\n2,2001\n3\n4,4001");
    auto InStreamRoutes = std::make_shared<CStringDataSource>("route,stop_id\nA,1\nA,2\nB,1\nC\nD,2");
    auto CSVReaderStops = std::make_shared<CDSVReader>(InStreamStops,',');
    auto CSVReaderRoutes = std::make_shared<CDSVReader>(InStreamRoutes,',');
    CCSVBusSystem BusSystem(CSVReaderStops, CSVReaderRoutes);

    EXPECT_EQ(BusSystem.StopByIndex(2)->NodeID(),4001);
    EXPECT_EQ(BusSystem.StopByID(4)->NodeID(),4001);
    EXPECT_EQ(BusSystem.StopByID(3),nullptr);
    EXPECT_EQ(BusSystem.RouteByIndex(2)->Name(),"D");
    EXPECT_EQ(BusSystem.RouteByName("C"), nullptr); 
}

TEST(CSVBusSystem, LongerRouteNameTest){
    auto InStreamStops = std::make_shared<CStringDataSource>("stop_id,node_id\n10,1001\n20,2001\n30,3001\n40,4001");
    auto InStreamRoutes = std::make_shared<CStringDataSource>("route,stop_id\nA,10\nAA,10\nA,40\nAA,20\nB,10\nBB,30");
    auto CSVReaderStops = std::make_shared<CDSVReader>(InStreamStops,',');
    auto CSVReaderRoutes = std::make_shared<CDSVReader>(InStreamRoutes,',');
    CCSVBusSystem BusSystem(CSVReaderStops, CSVReaderRoutes);

    EXPECT_EQ(BusSystem.StopCount(),4);
    EXPECT_EQ(BusSystem.RouteCount(),4);
    EXPECT_EQ(BusSystem.StopByIndex(5),nullptr);
    EXPECT_EQ(BusSystem.StopByID(30)->NodeID(),3001);
    EXPECT_EQ(BusSystem.StopByID(10)->ID(),10);
    EXPECT_EQ(BusSystem.RouteByIndex(1)->Name(),"AA");
    EXPECT_EQ(BusSystem.RouteByIndex(2)->Name(),"B");
    EXPECT_EQ(BusSystem.RouteByName("AA")->StopCount(),2); 
    EXPECT_EQ(BusSystem.RouteByIndex(1)->GetStopID(1),20);
}
