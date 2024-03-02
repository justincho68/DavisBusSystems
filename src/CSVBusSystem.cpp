#include "CSVBusSystem.h"
#include <vector>
#include <limits>
#include "DSVReader.h"
#include <algorithm>
#include "StringDataSource.h"
#include <cstdio>
#include <iostream>
#include <map>

class CCSVBusSystem::CSVStop : public CBusSystem::SStop {
    //Declaration of concrete stop that will be instantiated when reading from CSV files
    TStopID stopID; //all stops in the bus system
    CStreetMap::TNodeID nodeID; //all routes in the bus system
    public:
        CSVStop(TStopID stopID, CStreetMap::TNodeID nodeID) : stopID(stopID), nodeID(nodeID) {}
        TStopID ID() const noexcept override {//returns unique ID
            return stopID;
        }
        CStreetMap::TNodeID NodeID() const noexcept override {//returns unique map node id
            return nodeID;
        }
};

class CCSVBusSystem::CSVRoute : public CBusSystem::SRoute {
    std::string name;
    std::vector<TStopID> stopIDs; // all of the stops along the route
    public:
        CSVRoute(const std::string &name, const std::vector<TStopID> &stopIDs) : name(name), stopIDs(stopIDs) {}
        std::string Name() const noexcept override {//returns name of route
            return name;
        }
        std::size_t StopCount() const noexcept override {//returns number of stops in route
            return stopIDs.size();
        }
        TStopID GetStopID(std::size_t index) const noexcept override {//retrieves ID of stop at specific route
            if (index >= stopIDs.size()) {
                return CBusSystem::InvalidStopID;
            }
            return stopIDs[index];
        };
};
struct CCSVBusSystem::SImplementation {
    public:
        std::vector<std::shared_ptr<CCSVBusSystem::CSVStop>> Stops;
        std::vector<std::shared_ptr<CCSVBusSystem::CSVRoute>> Routes;
        //Declare the stops and routes using shared pointer to manage both
};

CCSVBusSystem::CCSVBusSystem(std::shared_ptr<CDSVReader> stopsrc, std::shared_ptr<CDSVReader> routesrc) 
: DImplementation(std::make_unique<SImplementation>()) {
    //Load stops and routes from the DSVReader
    //Parse data from routesrc and stopsrc
    //Create instances of CSVStop and CSVRoute to add to stops and routes vector
    std::vector<std::string> row;
    //Skip the first line of header in the file
    if (!stopsrc->ReadRow(row)) {
        std::cerr<<"Could not read header."<<std::endl;
        return;
    }
    //Use the DSVReader to read through the stops
    while (stopsrc->ReadRow(row)) {
        //Ensure that there are two columns in the DSV File
        if (row.size() >= 2) {
            TStopID stopID = std::stoull(row[0]);
            CStreetMap::TNodeID nodeID = std::stoull(row[1]);
            auto stop = std::make_shared<CSVStop>(stopID, nodeID);
            DImplementation->Stops.push_back(stop);
        }
        else {
            std::cerr<<"Could not read all stops"<<std::endl;
        }
    }

    //Use DSVReader to read through routes CSV
    //First skip the header line of the file
    if (!routesrc->ReadRow(row)) {
        std::cerr<<"Could not read the header"<<std::endl;
        return;
    }

    std::map<std::string, std::vector<TStopID>> routeMap;
    while(routesrc->ReadRow(row)) {
        if (row.size() >= 2) {
            std::string routeName = row[0];
            TStopID stopID = std::stoull(row[1]);
            routeMap[routeName].push_back(stopID);
        }
        else {
            std::cerr<<"Could not read all rows"<<std::endl;
        }
    }
    //Convert the routeMap to CSVRoute objects
    for (const auto &route: routeMap) {
        auto routeObject = std::make_shared<CSVRoute>(route.first, route.second);
        DImplementation->Routes.push_back(routeObject);
    }
    
}

//Destructor
CCSVBusSystem::~CCSVBusSystem() = default;

std::size_t CCSVBusSystem::StopCount() const noexcept {
    //Return the size of the stops of the created instance DImplementation
    return DImplementation->Stops.size();
}

std::size_t CCSVBusSystem::RouteCount() const noexcept {
    //Return the number of routes in the system
    return DImplementation->Routes.size();
}

//Purpose of function is to return a shared pointer to a given stop given its index
std::shared_ptr<CBusSystem::SStop> CCSVBusSystem::StopByIndex(std::size_t index) const noexcept {
    if (index < StopCount()) { // make sure index is not out of bounds
        return DImplementation->Stops[index]; //return route at the correct index
    }
    return nullptr; //return null pointer if nothing is found
}

//Purpose is to return a stop given its ID
std::shared_ptr<CBusSystem::SStop> CCSVBusSystem::StopByID(TStopID ID) const noexcept {
    for (const auto stopID: DImplementation->Stops) { //to run through the objects in Stops
        if (stopID->ID() == ID) //if the stop id is present in the Stops
        {
            return stopID;
        }
    }
    return nullptr; // Return null pointer if no match is found
    
}

//Return a shared pointer to a route given its index
std::shared_ptr<CBusSystem::SRoute> CCSVBusSystem::RouteByIndex(std::size_t index) const noexcept {
    if (index < RouteCount()) { // make sure index is not out of bounds
        return DImplementation->Routes[index]; //return route at the correct index
    }
    return nullptr; //return null pointer if nothing is found
}

//Return a shared pointer to a route given its name identifier
std::shared_ptr<CBusSystem::SRoute> CCSVBusSystem::RouteByName(const std::string &name) const noexcept {
   for (const auto RouteName: DImplementation->Routes) { //to run through the objects in Routes
        if (RouteName->Name() == name) //if the route name is present in the Routes
        {
            return RouteName; 
        }
    }
    return nullptr;//return null if there is no match
    
}