#include "DijkstraPathRouter.h"
#include "DijkstraTransportationPlanner.h"
#include "TransportationPlannerConfig.h"
#include "GeographicUtils.h"
#include "BusSystemIndexer.h"
#include <unordered_map>

struct CDijkstraTransportationPlanner::SImplementation {
    std::shared_ptr< CStreetMap > DStreetMap;
    std::shared_ptr <CBusSystem> DBusSystem;
    std::shared_ptr <CBusSystemIndexer> DBusSystemIndexer;
    std::unordered_map< CStreetMap::TNodeID, CPathRouter::TVertexID > DNodeToVertexID;
    CDijkstraPathRouter DShortestPathRouter;
    CDijkstraPathRouter DFastestPathRouterBike;
    CDijkstraPathRouter DFastestPathRouterWalkBus;

    SImplementation(std::shared_ptr<SConfiguration> config) {
        DStreetMap = config->StreetMap();
        DBusSystem = config->BusSystem();

        for (size_t Index = 0; Index < DStreetMap->NodeCount(); Index++) {
            auto Node = DStreetMap->NodeByIndex(Index);
            auto VertexID = DShortestPathRouter.AddVertex(Node->ID());
            DFastestPathRouterBike.AddVertex(Node->ID());
            DFastestPathRouterWalkBus.AddVertex(Node->ID());
            DNodeToVertexID[Node->ID()] = VertexID;
        }
        for (size_t Index = 0; Index < DStreetMap->WayCount(); Index++) {
            auto Way = DStreetMap->WayByIndex(Index);
            bool Bikable = Way->GetAttribute("bicycle") != "no";
            bool Bidirectional = Way->GetAttribute("oneway") != "yes";
            auto PreviousNodeID = Way->GetNodeID(0);
            for(size_t NodeIndex = 0; NodeIndex < Way->NodeCount(); NodeIndex++) {
                auto NextNodeID = Way->GetNodeID(NodeIndex);
                auto Distance = SGeographicUtils::HaversineDistanceInMiles(DStreetMap->NodeByID(PreviousNodeID)->Location(), DStreetMap->NodeByID(NextNodeID)->Location());
                if (Bikable == true)
                {
                    auto BikeTimeHours = Distance / config->BikeSpeed();
                    DFastestPathRouterBike.AddEdge(DNodeToVertexID[PreviousNodeID], DNodeToVertexID[NextNodeID], BikeTimeHours);
                }
                if (Bidirectional == false || Bikable == false)
                {
                    DFastestPathRouterWalkBus.AddEdge(DNodeToVertexID[PreviousNodeID], DNodeToVertexID[NextNodeID], Distance);
                }
                PreviousNodeID = NextNodeID;
                //Use haverstein distance function to get distance in miles between two nodes
                // fastest path you just divide by the speed you are moving with 
            }
        }
        auto previousStopID = DBusSystem->StopByIndex(0)->NodeID();
        for (size_t busIndex = 1; busIndex < DBusSystem->StopCount(); busIndex++)
        {
            auto currentStopID = DBusSystem->StopByIndex(busIndex)->NodeID();
            auto DistanceBetweenStops = SGeographicUtils::HaversineDistanceInMiles(DStreetMap->NodeByID(previousStopID)->Location(), DStreetMap->NodeByID(currentStopID)->Location());
            auto TimeHours = 0.0;
            if (DBusSystemIndexer->RouteBetweenNodeIDs(previousStopID, currentStopID))
            {
                std::vector <CPathRouter::TVertexID> ShortestPath;
                TimeHours = (DistanceBetweenStops / config->DefaultSpeedLimit()) + config->BusStopTime()/3600;
                //auto distance = DShortestPathRouter.FindShortestPath(DNodeToVertexID[previousStopID],DNodeToVertexID[currentStopID], ShortestPath);
            }
            else
            {
                TimeHours = DistanceBetweenStops / config->WalkSpeed();
            }
            DFastestPathRouterWalkBus.AddEdge(DNodeToVertexID[previousStopID], DNodeToVertexID[currentStopID], TimeHours);
            previousStopID = currentStopID;
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
        std::vector <CPathRouter::TVertexID> ShortestPath;
        auto SourceVertexID = DNodeToVertexID[src];
        auto DestinationNodeID = DNodeToVertexID[dest];
        auto Distance = DShortestPathRouter.FindShortestPath(SourceVertexID, DestinationNodeID, ShortestPath);
        path.clear();
        for (auto VertexID : ShortestPath) {
            path.push_back(std::any_cast<TNodeID>(DShortestPathRouter.GetVertexTag(VertexID)));
        }
        return Distance;
    }

    // Returns the time in hours for the fastest path between the src and dest
    // nodes of the if one exists. NoPathExists is returned if no path exists.
    // The transportation mode and nodes of the fastest path are filled in the
    // path parameter.
    double FindFastestPath(TNodeID src, TNodeID dest, std::vector<TTripStep> &path) {
        auto SourceVertexID = DNodeToVertexID[src];
        auto DestinationVertexID = DNodeToVertexID[dest];
        auto Bike = CTransportationPlanner::ETransportationMode::Bike;
        std::vector <CPathRouter::TVertexID> BikeFastestPath;
        auto BikeFastestPathTime = DFastestPathRouterBike.FindShortestPath(SourceVertexID, DestinationVertexID, BikeFastestPath);
        std::vector <CPathRouter::TVertexID> WalkBusFastestPath;
        auto WalkBusFastestPathTime = DFastestPathRouterWalkBus.FindShortestPath(SourceVertexID, DestinationVertexID, BikeFastestPath);
        path.clear();
        if (BikeFastestPathTime < WalkBusFastestPathTime)
        {
            for (auto VertexID : BikeFastestPath) 
            {
                path.push_back(std::make_pair(Bike, DNodeToVertexID.find(VertexID)->second));
            }
            return BikeFastestPathTime;
        }
        else if (WalkBusFastestPathTime < BikeFastestPathTime)
        {
            return WalkBusFastestPathTime;
        }

        //auto Distance = SGeographicUtils::HaversineDistanceInMiles(SourceNodeID->Location(), DestinationNodeID->Location());
    }

    // Returns true if the path description is created. Takes the trip steps path
    // and converts it into a human readable set of steps.
    bool GetPathDescription(const std::vector<TTripStep> &path, std::vector<std::string> &desc) {
        //First check to make sure that the path is not empty
        if (path.empty()) {
            return false;
        } // path can not be described

        //Iterate through every step in the path to describe it
        for (const auto &step : path) {
            //Each mode will have a mode of transportation and a node id
            auto mode = step.first;
            auto nodeID = step.second;
            //Create a string to hold the mode of transportation
            std::string modeDescription;
            //Switch statements for each mode of transportation
            switch (mode) {
                case CTransportationPlanner::ETransportationMode::Walk:
                    modeDescription = "Walk";
                    break;
                case CTransportationPlanner::ETransportationMode::Bike:
                    modeDescription = "Bike";
                    break;
                case CTransportationPlanner::ETransportationMode::Bus:
                    modeDescription = "Bus";
                    break;
                default:
                    modeDescription = "Uninitialized";
            } 
            //find the correspoinding node in the map because the node id is given
            auto nodeIt = DNodeToVertexID.find(nodeID);
            //check to make sure it is not at the end
            if (nodeIt != DNodeToVertexID.end()) {
                auto nodeName = "Node " + std::to_string(nodeID);
                //Create a description using the mode and node id
                std::string description = modeDescription + " to " + nodeName;
                desc.push_back(description);
            } else {
                //Error message if the node id can not be mapped to a node
                desc.push_back("Unknown ID: " + std::to_string(nodeID));
            }
        }
        //Return true if descriptions is non empty
        return !desc.empty();
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
