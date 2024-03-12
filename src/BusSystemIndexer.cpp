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
    //Represent the roues in a sorted order
    std::vector<std::shared_ptr<SRoute>> DSortedRoutes;
    //Allows quickly identifying stops by their ID
    std::unordered_map< TNodeID, std::shared_ptr<SStop> > DNodeIDToStop;
    //Maps pairs of node IDsto a set of routes that connect those stops
    std::unordered_map<std::pair<TNodeID, TNodeID>, std::unordered_set<std::shared_ptr<SRoute>>, pair_hash, std::equal_to<std::pair<TNodeID, TNodeID>>> DSrcDestToRoutes;
    std::unordered_map<CBusSystem::TStopID, std::shared_ptr<CBusSystem::SStop>> DStopIDToStop;


    static bool StopIDCompare(std::shared_ptr<SStop> left, std::shared_ptr<SStop> right) {
        return left->ID() < right->ID();
    }

    static bool RouteNameCompare(const std::shared_ptr<SRoute> &left, const std::shared_ptr<SRoute> &right) {
        return left->Name() < right->Name();
    }


    //Constructor for SImplementation
    SImplementation(std::shared_ptr<CBusSystem> bussystem) {
        DBusSystem = bussystem;
        if (!DBusSystem) {
        throw std::runtime_error("BusSystem is null");
    }

    // Populate DStopIDToStop for direct access to stops by their ID
    for (size_t i = 0; i < DBusSystem->StopCount(); ++i) {
        //Retrieve each bus stop using its index
        auto stop = DBusSystem->StopByIndex(i);
        //Map each stop id to the stop object itself
        DStopIDToStop[stop->ID()] = stop;
        DSortedStops.push_back(stop); // add to the vector of stops
        DNodeIDToStop[stop->NodeID()] = stop; //map the stops nodeid to the stop object
    }

    // Populate DSortedRoutes with routes and create mappings
    for (size_t i = 0; i < DBusSystem->RouteCount(); ++i) {
        auto route = DBusSystem->RouteByIndex(i); // retrieve bus route using index
        DSortedRoutes.push_back(route); // add to the vector of routes
        //create mapping pairs from stop pairs to their routes
        for (size_t j = 1; j < route->StopCount(); ++j) {
            CBusSystem::TStopID prevStopID = route->GetStopID(j - 1);
            CBusSystem::TStopID currStopID = route->GetStopID(j);

            auto prevStop = DStopIDToStop[prevStopID]; // get prev stop object using ID
            auto currStop = DStopIDToStop[currStopID]; // get curr stop object using ID

            if (prevStop && currStop) {
                //create a key from node ids of the stop pair
                std::pair<TNodeID, TNodeID> key = {prevStop->NodeID(), currStop->NodeID()};
                DSrcDestToRoutes[key].insert(route); // map stop pair to the route
            }
        }
    }

    // Sorting DSortedStops by StopID
    std::sort(DSortedStops.begin(), DSortedStops.end(), [](const std::shared_ptr<SStop>& a, const std::shared_ptr<SStop>& b) {
        return a->ID() < b->ID();
    });

    // Sorting DSortedRoutes by RouteName
    std::sort(DSortedRoutes.begin(), DSortedRoutes.end(), [](const std::shared_ptr<SRoute>& a, const std::shared_ptr<SRoute>& b) {
        return a->Name() < b->Name();
    });
};

    
    std::size_t StopCount() const noexcept {
        return DBusSystem->StopCount();
    }

    std::size_t RouteCount() const noexcept {
        return DBusSystem->RouteCount();
    }
    
    std::shared_ptr<SStop> StopByNodeID(TNodeID id) const noexcept {
        auto Search = DNodeIDToStop.find(id);
        //Find the given stop using the id in parameter
        if(Search != DNodeIDToStop.end()) {
            return Search->second;
        }
        //return null pointer if nothing is found
        return nullptr;
    }

    bool RouteByNodeIDs(TNodeID src, TNodeID dest, std::unordered_set<std::shared_ptr<SRoute>> &routesOut) {
        auto searchKey = std::make_pair(src, dest);
        //use a pair of src and destination as a key in DSrcDestToRoutes
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

    //Returns the SStop specified by the index where the stops are sorted by their ID,
    //Returns null pointer if index is greater than or qual to StopCount
    std::shared_ptr<SStop> SortedStopByIndex(std::size_t index) const noexcept {
        if (index < DSortedStops.size()) {
            return DSortedStops[index];
        }
        return nullptr;
    }

    //Returns the SRoute specified by the index where the routes are sorted by their
    //Name, nullpointer is returned if index is greater than or equal to RouteCount
    std::shared_ptr<SRoute> SortedRouteByIndex(std::size_t index) const noexcept {
        if (index < DSortedRoutes.size()) {
            return DSortedRoutes[index];
        }
        return nullptr;
    }


};
//Constructor
CBusSystemIndexer::CBusSystemIndexer(std::shared_ptr<CBusSystem> bussystem) {
    DImplementation = std::make_unique<SImplementation>(bussystem);
}
//Destructor
CBusSystemIndexer::~CBusSystemIndexer() {

}
//Call all of the functions on DImplementation which is an instance of SImplementation
std::size_t CBusSystemIndexer::StopCount() const noexcept {
    return DImplementation->StopCount();
}

std::size_t CBusSystemIndexer::RouteCount() const noexcept {
    return DImplementation->RouteCount();
}

std::shared_ptr<CBusSystemIndexer::SStop> CBusSystemIndexer::SortedStopByIndex(std::size_t index) const noexcept {
    //check that it is in bounds
    if (index < DImplementation->DSortedStops.size()) {
        return DImplementation->DSortedStops[index];
    }
    return nullptr;
}

std::shared_ptr<CBusSystemIndexer::SRoute> CBusSystemIndexer::SortedRouteByIndex(std::size_t index) const noexcept {
    return DImplementation->SortedRouteByIndex(index);
}

std::shared_ptr<CBusSystemIndexer::SStop> CBusSystemIndexer::StopByNodeID(TNodeID id) const noexcept {
    auto it = DImplementation->DNodeIDToStop.find(id);
    //check for valid bounds
    if (it != DImplementation->DNodeIDToStop.end()) {
        return it->second;
    }
    return nullptr;
}

bool CBusSystemIndexer::RoutesByNodeIDs(TNodeID src, TNodeID dest,
std::unordered_set<std::shared_ptr<SRoute> > &routes) const noexcept {
    return DImplementation->RouteByNodeIDs(src, dest, routes);
}

bool CBusSystemIndexer::RouteBetweenNodeIDs(TNodeID src, TNodeID dest) const noexcept {
    return DImplementation->RouteBetweenNodeIDs(src, dest);
}