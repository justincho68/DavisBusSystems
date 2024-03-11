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
        auto stop = DBusSystem->StopByIndex(i);
        DStopIDToStop[stop->ID()] = stop;
        // Also add to DSortedStops and DNodeIDToStop as they are needed for direct lookup
        DSortedStops.push_back(stop);
        DNodeIDToStop[stop->NodeID()] = stop;
    }

    // Populate DSortedRoutes
    for (size_t i = 0; i < DBusSystem->RouteCount(); ++i) {
        auto route = DBusSystem->RouteByIndex(i);
        DSortedRoutes.push_back(route);

        for (size_t j = 1; j < route->StopCount(); ++j) {
            CBusSystem::TStopID prevStopID = route->GetStopID(j - 1);
            CBusSystem::TStopID currStopID = route->GetStopID(j);

            // Utilizing stops directly from DStopIDToStop ensures accuracy
            auto prevStop = DStopIDToStop[prevStopID];
            auto currStop = DStopIDToStop[currStopID];

            if (prevStop && currStop) {
                std::pair<TNodeID, TNodeID> key = {prevStop->NodeID(), currStop->NodeID()};
                DSrcDestToRoutes[key].insert(route);
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
        /*
        if (!DBusSystem) {
        throw std::runtime_error("BusSystem is null");
    }

    // Initialize DStopIDToStop for direct access to stops by their ID
    for (size_t i = 0; i < DBusSystem->StopCount(); ++i) {
        auto stop = DBusSystem->StopByIndex(i);
        DStopIDToStop[stop->ID()] = stop;
    }

    // Populate DSortedStops and DSortedRoutes
    std::unordered_set<TNodeID> uniqueNodeIDs;
    for (size_t i = 0; i < DBusSystem->RouteCount(); ++i) {
        auto route = DBusSystem->RouteByIndex(i);
        DSortedRoutes.push_back(route);
        for(size_t j = 0; j < route->StopCount(); ++j) {
            CBusSystem::TStopID stopID = route->GetStopID(j);
            if (uniqueNodeIDs.insert(stopID).second) { // If it's a new node ID
                auto stop = DStopIDToStop[stopID]; // Use the populated map
                if(stop) {
                    DSortedStops.push_back(stop);
                    DNodeIDToStop[stop->NodeID()] = stop;
                }
            }
            // For each pair of consecutive stops, link their route
            if (j > 0) {
                CBusSystem::TStopID prevStopID = route->GetStopID(j-1);
                auto prevStop = DStopIDToStop[prevStopID];
                auto currStop = DStopIDToStop[stopID];
                if(prevStop && currStop) {
                    std::pair<TNodeID, TNodeID> key = {prevStop->NodeID(), currStop->NodeID()};
                    DSrcDestToRoutes[key].insert(route);
                }
            }
        }
    }

    // Sort DSortedStops by StopID
    std::sort(DSortedStops.begin(), DSortedStops.end(), [](const std::shared_ptr<SStop>& a, const std::shared_ptr<SStop>& b) {
        return a->ID() < b->ID();
    });

    // Sort DSortedRoutes by RouteName
    std::sort(DSortedRoutes.begin(), DSortedRoutes.end(), [](const std::shared_ptr<SRoute>& a, const std::shared_ptr<SRoute>& b) {
        return a->Name() < b->Name();
    }); */
        /*if (!DBusSystem) {
            throw std::runtime_error("BusSystem is null");
        }
        for (size_t Index = 0; Index < DBusSystem->StopCount(); Index++) {
            auto CurrentStop = DBusSystem->StopByIndex(Index);
            DSortedStops.push_back(CurrentStop);
            DNodeIDToStop[CurrentStop->NodeID()] = CurrentStop;
        }
        std::sort(DSortedStops.begin(), DSortedStops.end(), StopIDCompare);
        for (size_t Index = 0; Index < DBusSystem->RouteCount(); Index++) {
            auto CurrentRoute = DBusSystem->RouteByIndex(Index);
            for(size_t StopIndex = 1; StopIndex < CurrentRoute->StopCount(); StopIndex++) {
                auto SourceNodeID = DBusSystem->StopByID(CurrentRoute->GetStopID(StopIndex-1));
                auto DestinationNodeID = DBusSystem->StopByID(CurrentRoute->GetStopID(StopIndex));
                if (!SourceNodeID || !DestinationNodeID) continue;
                auto SearchKey = std::make_pair(SourceNodeID->NodeID(), DestinationNodeID->NodeID());
                auto Search = DSrcDestToRoutes.find(SearchKey);
                if (Search != DSrcDestToRoutes.end()) {
                    Search->second.insert(CurrentRoute);
                }
                else {
                    DSrcDestToRoutes[SearchKey] = {CurrentRoute};
                }
            }
        }
        */
        /*
        DBusSystem = bussystem;
        if (!DBusSystem) {
            throw std::runtime_error("BusSystem is null");
        }
        std::unordered_set<TNodeID> uniqueStopIDs;
        for (size_t routeIndex = 0; routeIndex < DBusSystem->RouteCount(); ++routeIndex) {
            auto route = DBusSystem->RouteByIndex(routeIndex);
            std::cout << "Processing Route: " << route->Name() << " with Stop Count: " << route->StopCount() << std::endl;
            if (!route) {
                std::cerr << "Failed to access route at index: " << routeIndex << std::endl;
                continue;
            }
            DSortedRoutes.push_back(route);
            for(size_t stopIndex = 0; stopIndex < route->StopCount(); ++stopIndex) {
                TNodeID stopID = route->GetStopID(stopIndex);
                std::cout << "Processing Stop ID: " << stopID << " for Route: " << route->Name() << std::endl;
                if (uniqueStopIDs.insert(stopID).second) {
                    auto stop = DBusSystem->StopByIndex(stopID);
                    if (!stop) {
                        std::cerr << "Stop with ID " << stopID << " could not be found withing the system." << std::endl;
                        continue;
                    }
                    DSortedStops.push_back(stop);
                    DNodeIDToStop[stopID] = stop;
                    std::cout << "Added Stop ID: " << stop->ID() << " Node ID: " << stop->NodeID() << std::endl;
                } 
                if (stopIndex > 0) {
                    TNodeID previousStopID = route->GetStopID(stopIndex-1);
                    DSrcDestToRoutes[{previousStopID, stopID}].insert(route);
                    std::cout << "Added Route between Stop IDs: " << previousStopID << " and " << stopID << std::endl;
                }
            }
        }
        std::sort(DSortedStops.begin(), DSortedStops.end(), [](const std::shared_ptr<SStop> &a, const std::shared_ptr<SStop> &b) {
            return  a->ID() < b->ID();
        });
        std::cout << "Sorted " << DSortedStops.size() << " Stops.\n";
        std::sort(DSortedRoutes.begin(), DSortedRoutes.end(), [](const std::shared_ptr<SRoute> &a, const std::shared_ptr<SRoute> &b) {
            return a->Name() < b->Name();
        });
        std::cout << "Sorted " << DSortedRoutes.size() << " Routes.\n";
    */
};

    
    std::size_t StopCount() const noexcept {
        return DBusSystem->StopCount();
    }

    std::size_t RouteCount() const noexcept {
        return DBusSystem->RouteCount();
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

CBusSystemIndexer::CBusSystemIndexer(std::shared_ptr<CBusSystem> bussystem) {
    DImplementation = std::make_unique<SImplementation>(bussystem);
}

CBusSystemIndexer::~CBusSystemIndexer() {

}

std::size_t CBusSystemIndexer::StopCount() const noexcept {
    return DImplementation->StopCount();
}

std::size_t CBusSystemIndexer::RouteCount() const noexcept {
    return DImplementation->RouteCount();
}

std::shared_ptr<CBusSystemIndexer::SStop> CBusSystemIndexer::SortedStopByIndex(std::size_t index) const noexcept {
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