#include "BusSystemIndexer.h"
#include <vector>
#include <algorithm>
#include <unordered_map>
#include <memory>
#include "StreetMap.h"
#include <unordered_set>
#include <cstdio>
#include <iostream>


//Struct designed to create a hash pair out of two objects
//This is necessary so that the pair can be used as a key in a has based object such as unordered
//set and unordered map
struct pair_hash {
    template <class T1, class T2>
    std::size_t operator() (const std::pair<T1, T2> &pair) const {
        auto hash1 = std::hash<T1>{}(pair.first);
        auto hash2 = std::hash<T2>{}(pair.second);
        return hash1 ^ hash2;
    } 
};

struct CBusSystemIndexer::SImplementation{
    //Bus system to hold information of routes/stops
    std::shared_ptr<CBusSystem> DBusSystem;
    //Represents the stops in the system in sorted manner
    std::vector<std::shared_ptr<SStop>> DSortedStops;
    //Allows quickly identifying stops by their ID
    std::unordered_map< TNodeID, std::shared_ptr<SStop> > DNodeIDToStop;
    //Maps pairs of node IDsto a set of routes that connect those stops
    std::unordered_map<std::pair<TNodeID, TNodeID>, std::unordered_set<std::shared_ptr<SRoute>>, pair_hash, std::equal_to<std::pair<TNodeID, TNodeID>>> DSrcDestToRoutes;


    static bool StopIDCompare(std::shared_ptr<SStop> left, std::shared_ptr<SStop> right) {
        return left->ID() < right->ID();
    }
    //Constructor for SImplementation
    SImplementation(std::shared_ptr<CBusSystem> bussystem) {
        //Initialize DBusSystem to the the internal bussystem pointer
        DBusSystem = bussystem;
        //For loop will populate the DSortedStops and DNodeIDToStop
        //Loops through every route in the bus system
        for (size_t Index = 0; Index< DBusSystem->RouteCount(); Index++) {
            //Retrieves each stop and adds it to DSortedStops
            auto CurrentStop = DBusSystem->StopByIndex(Index);
            //Adds the stop and node ID for each route to DSortedStops and DNodeIDToStop
            DSortedStops.push_back(CurrentStop);
            DNodeIDToStop[CurrentStop->NodeID()] = CurrentStop;
        }
        //Once the vector and map are populated, it sorts the DSortedStops using built in sort function
        std::sort(DSortedStops.begin(), DSortedStops.end(),StopIDCompare);
        //For loop to build DSrcDestToRoutes and loops through every route
        for (size_t Index = 0; Index < DBusSystem->RouteCount(); Index++) {
            //Retrieve the current route using its index
            auto CurrentRoute = DBusSystem->RouteByIndex(Index);
            //Once inside route, iterate through every stop in the route
            //For loop starts at index 1 because must use the previous node
            for (size_t StopIndex = 1; StopIndex < CurrentRoute->StopCount(); StopIndex++) {
                //Use the previous node ID as the source node ID
                auto SourceNodeID = CurrentRoute->GetStopID(StopIndex-1);
                auto DestinationNodeID = DBusSystem->StopByID(CurrentRoute->GetStopID(StopIndex-1));
                //Create a pair of source and destination node IDS to use as a key for finding routes
                auto SearchKey = std::make_pair(SourceNodeID, DestinationNodeID);
                //Look for the key in the DSrcDestToRoutes map
                auto Search = DSrcDestToRoutes.find(SearchKey);
                if (Search != DSrcDestToRoutes.end()) {
                    Search->second.insert(CurrentRoute);
                }
                else {
                    DSrcDestToRoutes[SearchKey] = {CurrentRoute};
                }
            }
        }
    };
    
    std::size_t StopCount() const noexcept {
        return DBusSystem->StopCount();
    }

    std::shared_ptr<SRoute> SortedRouteByIndex(std::size_t index) const noexcept {

    }
    
    std::shared_ptr<SStop> StopByNodeID(TNodeID id) const noexcept {
        auto Search = DNodeIDToStop.find(id);
        if(Search != DNodeIDToStop.end()) {
            return Search->second;
        }
        return nullptr;
    }

    bool RouteByNodeIDs(TNodeID src, TNodeID dest, std::unordered_set<std::shared_ptr<SRoute>> &routesOut) {
        auto searchKey = std::make_pair(src, dest);
        auto search = DSrcDestToRoutes.find(searchKey);
        if (search != DSrcDestToRoutes.end()) {
            routesOut = search->second;
            return true;
        }
        return false;
    }

    bool RouteBetweenNodeIDs(TNodeID src, TNodeID dest) const noexcept {
        auto searchKey = std::make_pair(src, dest);
        return DSrcDestToRoutes.find(searchKey) != DSrcDestToRoutes.end();
    }
};

CBusSystemIndexer::CBusSystemIndexer(std::shared_ptr<CBusSystem> bussystem) {
    DImplementation = std::make_unique<SImplementation>(bussystem);
}

std::size_t CBusSystemIndexer::StopCount() const noexcept {
    return DImplementation->StopCount();
}