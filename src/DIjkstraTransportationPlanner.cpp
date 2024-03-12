#include "DijkstraPathRouter.h"
#include "DijkstraTransportationPlanner.h"
#include "TransportationPlannerConfig.h"
#include "GeographicUtils.h"
#include "BusSystemIndexer.h"
#include <unordered_map>
#include <iostream>

struct CDijkstraTransportationPlanner::SImplementation {
    std::shared_ptr< CStreetMap > DStreetMap;
    std::shared_ptr <CBusSystem> DBusSystem;
    std::shared_ptr <CBusSystemIndexer> DBusSystemIndexer;
    std::unordered_map< CStreetMap::TNodeID, CPathRouter::TVertexID > DNodeToVertexID;
    CDijkstraPathRouter DShortestPathRouter;
    CDijkstraPathRouter DFastestPathRouterBike;
    CDijkstraPathRouter DFastestPathRouterWalkBus;
    std::unordered_map< CStreetMap::TNodeID, CStreetMap::TNodeID> speedLimits;

    SImplementation(std::shared_ptr<SConfiguration> config) {
        //set map and bus system as the ones from config
        DStreetMap = config->StreetMap(); 
        DBusSystem = config->BusSystem();

        //add vertices to the path routers by running through nodes in street map
        for (size_t Index = 0; Index < DStreetMap->NodeCount(); Index++) {
            auto Node = DStreetMap->NodeByIndex(Index); 
            auto VertexID = DShortestPathRouter.AddVertex(Node->ID()); //add vertex to shortestpathrouter
            DFastestPathRouterBike.AddVertex(Node->ID()); //add vertex to fastestpathrouterbike
            DFastestPathRouterWalkBus.AddVertex(Node->ID()); //add vertex to fastestpathrouterwalkbus
            DNodeToVertexID[Node->ID()] = VertexID; //set vertex id for the node id 
        }
        //run through the different ways (streets) in DStreetMap
        for (size_t Index = 0; Index < DStreetMap->WayCount(); Index++) {
            auto Way = DStreetMap->WayByIndex(Index); //set way at the specific index
            bool Bikable = Way->GetAttribute("bicycle") != "no"; //see if it is bikable
            bool Bidirectional = Way->GetAttribute("oneway") != "yes"; //see if it is bidirectional
            auto speedLimit = 0.0; //to collect speed limit
            if (Way->GetAttribute("speed").empty() == false) //if there is an attribute for speed
            {
                speedLimit = stod(Way->GetAttribute("speed")); //convert from string to double and set it as speedlimit
            }
            else if (Way->GetAttribute("speed").empty() == true) //if there is no speed attribute
            {
                speedLimit = config->DefaultSpeedLimit(); //set speedlimit as default speed limit
            }
            auto PreviousNodeID = Way->GetNodeID(0); //get first node in way
            //to run through the nodes in the different ways
            for(size_t NodeIndex = 1; NodeIndex < Way->NodeCount(); NodeIndex++) {
                auto NextNodeID = Way->GetNodeID(NodeIndex); //node at the node index 
                speedLimits[PreviousNodeID, NextNodeID] = speedLimit; //for map of speed limits 
                //calculate distance using HaversineDistance
                auto Distance = SGeographicUtils::HaversineDistanceInMiles(DStreetMap->NodeByID(PreviousNodeID)->Location(), DStreetMap->NodeByID(NextNodeID)->Location()); 
                //add edge to shortestpathrouter 
                DShortestPathRouter.AddEdge(DNodeToVertexID[PreviousNodeID], DNodeToVertexID[NextNodeID], Distance, Bidirectional);
                if (Bikable == true) //if bikable is true
                {
                    auto BikeTimeHours = Distance / config->BikeSpeed(); //calculate the biking time in hours by dividing distance by bike speed
                    DFastestPathRouterBike.AddEdge(DNodeToVertexID[PreviousNodeID], DNodeToVertexID[NextNodeID], BikeTimeHours); //add edge to bikepathrouter
                }
                
                PreviousNodeID = NextNodeID;
                //Use haverstein distance function to get distance in miles between two nodes
                // fastest path you just divide by the speed you are moving with 
            }
        }
        //to run through the different routes for the bus stops
        for (size_t routeIndex = 0; routeIndex < DBusSystem->RouteCount(); routeIndex++)
        {
            auto route = DBusSystem->RouteByIndex(routeIndex); //set the route that is currently being used
            auto previousStopID = route->GetStopID(0); //set the first stop id and use as previous 
            auto previousStop = DBusSystem->StopByID(previousStopID); //to use as reference pointer
            //to run through the stops in the route
            for (size_t stopIndex = 1; stopIndex < route->StopCount(); stopIndex++)
            {
                auto currentStopID = route->GetStopID(stopIndex); //get currentStopID by getting the stop id at the index
                auto currentStop = DBusSystem->StopByID(currentStopID); //to use as reference pointer
                 
                std::vector <TNodeID> ShortestPath; //to collect the shortest path node IDs
                auto dist = FindShortestPath(previousStop->NodeID(), currentStop->NodeID(), ShortestPath); //find the shortest path between 2 nodes
                
                auto DriveTimeHours = config->BusStopTime()/3600; //variable to collect the hours of drive time with the bus stop time added
                auto WalkTimeHours = 0.0; //variable to collect the hours of walking
          
             
                if (dist != CPathRouter::NoPathExists)
                {
                    auto previousNodeID = ShortestPath[0]; //variable to use as first NodeID starting with first in path 
                    //to run through shortest path and get times
                    for (size_t ShortestPathIndex = 1; ShortestPathIndex < ShortestPath.size(); ShortestPathIndex++)
                    {
                        auto currentNodeID = ShortestPath[ShortestPathIndex];  //to find the node id for node at index shortest path
                        auto Distance = SGeographicUtils::HaversineDistanceInMiles(DStreetMap->NodeByID(previousNodeID)->Location(), DStreetMap->NodeByID(currentNodeID)->Location()); //calculate distance
                        DriveTimeHours += Distance/speedLimits[previousNodeID, currentNodeID]; //calculate the drive time in hours by dividing distance by speed limits
                        WalkTimeHours = Distance/config->WalkSpeed(); //calculate walk time in hours
                        previousNodeID = currentNodeID;
                    }
            
                    DFastestPathRouterWalkBus.AddEdge(DNodeToVertexID[previousStopID], DNodeToVertexID[currentStopID], DriveTimeHours); //add edge
                    DFastestPathRouterWalkBus.AddEdge(DNodeToVertexID[previousStopID], DNodeToVertexID[currentStopID], WalkTimeHours);  //add edge          
                
                }
                previousStopID = currentStopID; //set previous stop id to current
                previousStop = currentStop; //set previous stop to current stop
                
            }
        }
    }
    
    std::size_t NodeCount() const noexcept {
        return DStreetMap->NodeCount();
    }

    std::shared_ptr<CStreetMap::SNode> SortedNodeByIndex(std::size_t index) const noexcept {
        if (index < DStreetMap->NodeCount()) {
            return DStreetMap->NodeByIndex(index);
        }
        return nullptr;
    }

    //Returns the distance in miles between the src and dest nodes of the 
    //shortest path if one exists. NoPathExists is returned if no path exists.
    //The nodes of the shortest path are filled in the path parameter
    double FindShortestPath(TNodeID src, TNodeID dest, std::vector<TNodeID> &path) {
        path.clear();
        std::vector <CPathRouter::TVertexID> ShortestPath; //create vector to collect vertexIDs for shortest path
        auto SourceVertexID = DNodeToVertexID[src]; //find the vertexID for the src node
        auto DestinationNodeID = DNodeToVertexID[dest]; //find the vertexID for the dest node
        //call the path router function of FindShortestPath
        auto Distance = DShortestPathRouter.FindShortestPath(SourceVertexID, DestinationNodeID, ShortestPath);
        for (auto VertexID : ShortestPath) {
            //convert the vertexIDs to NodeIDs and fill the path parameter
            path.push_back(std::any_cast<TNodeID>(DShortestPathRouter.GetVertexTag(VertexID))); 

        }
        return Distance;

    }

    // Returns the time in hours for the fastest path between the src and dest
    // nodes of the if one exists. NoPathExists is returned if no path exists.
    // The transportation mode and nodes of the fastest path are filled in the
    // path parameter.
    double FindFastestPath(TNodeID src, TNodeID dest, std::vector<TTripStep> &path) {
        path.clear();
        auto SourceVertexID = DNodeToVertexID[src]; //get vertex id at src node
        auto DestinationVertexID = DNodeToVertexID[dest]; //get vertex id at dest node
        auto Bike = CTransportationPlanner::ETransportationMode::Bike; //set transportation mode
        auto Walk = CTransportationPlanner::ETransportationMode::Walk; //set transportation mode
        std::vector <CPathRouter::TVertexID> BikeFastestPath; //vector to colect BikeFastestPath
        auto BikeFastestPathTime = DFastestPathRouterBike.FindShortestPath(SourceVertexID, DestinationVertexID, BikeFastestPath); //to find fastest path for bike
        std::vector <CPathRouter::TVertexID> WalkBusFastestPath; //vector to colect WalkBusFastestPath
        auto WalkBusFastestPathTime = DFastestPathRouterWalkBus.FindShortestPath(SourceVertexID, DestinationVertexID, WalkBusFastestPath); //to find fastest path for walk/bus
        
        if (BikeFastestPathTime < WalkBusFastestPathTime) //if biking is faster than walking/bussing
        {
            //to push into path parameter
            for (auto VertexID : BikeFastestPath) 
            {
                path.push_back(std::make_pair(Bike, DNodeToVertexID.find(VertexID)->second)); //make pair and add to TTripStep vector
            }
            return BikeFastestPathTime;
        }
        else if (WalkBusFastestPathTime < BikeFastestPathTime) //walking/bussing is faster
        {
            //to push into path parameter
            for (auto VertexID : WalkBusFastestPath) 
            {
                path.push_back(std::make_pair(Walk, DNodeToVertexID.find(VertexID)->second)); //make pair and add to TTripStep vector
            }
            return WalkBusFastestPathTime;
        }
        return CPathRouter::NoPathExists; //return if no path exists
    }

    // Returns true if the path description is created. Takes the trip steps path
    // and converts it into a human readable set of steps.
    bool GetPathDescription(const std::vector<TTripStep> &path, std::vector<std::string> &desc) {
        return true;
    }

};

CDijkstraTransportationPlanner::CDijkstraTransportationPlanner(std::shared_ptr<SConfiguration> config) {
    DImplementation = std::make_unique<SImplementation>(config);
}

CDijkstraTransportationPlanner::~CDijkstraTransportationPlanner() {

}

bool CDijkstraTransportationPlanner::GetPathDescription(const std::vector<TTripStep> &path, std::vector<std::string> &desc) const {
    return DImplementation->GetPathDescription(path, desc);
}

double CDijkstraTransportationPlanner::FindFastestPath(TNodeID src, TNodeID dest, std::vector<TTripStep> &path) {
    return DImplementation->FindFastestPath(src, dest, path);
}


double CDijkstraTransportationPlanner::FindShortestPath(TNodeID src, TNodeID dest, std::vector<TNodeID> &path) {
    return DImplementation->FindShortestPath(src, dest, path);
}

std::shared_ptr<CStreetMap::SNode> CDijkstraTransportationPlanner::SortedNodeByIndex(std::size_t index) const noexcept {
    return DImplementation->SortedNodeByIndex(index);
}

std::size_t CDijkstraTransportationPlanner::NodeCount() const noexcept {
    return DImplementation->NodeCount();
}
