#include "DijkstraTransportationPlanner.h"
#include "DijkstraPathRouter.h"
#include <unordered_map>

struct CDijkstraTransportationPlanner::SImplementation {
    std::shared_ptr< CStreetMap > DStreetMap;
    std::shared_ptr <CBusSystem> DBusSystem;
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
        for (size_t Index = 0; Index< DStreetMap->WayCount(); Index++) {
            auto Way = DStreetMap->WayByIndex(Index);
            bool Bikable = Way->GetAttribute("bicycle") != "no";
            bool Bidirectional = Way->GetAttribute("oneway") != "yes";
            auto PreviousNodeID = Way->GetNodeID(0);
            for(size_t NodeIndex = 0; NodeIndex < Way->NodeCount(); NodeIndex++) {
                auto NextNodeID = Way->GetNodeID(NodeIndex);
                //Use haverstein distance function to get distance in miles between two nodes
                // fastest path you just divide by the speed you are moving with 
            }
        }
    }
    
    std::size_t NodeCount() const noexcept {
        return DStreetMap->NodeCount();
    }

    std::shared_ptr<CStreetMap::SNode> SortedNodeByIndex(std::size_t index) const noexcept {
        if (index <DStreetMap->NodeCount()) {
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
            path.push_back(std::any_cast<TNodeID>(DShortest))
        }
        return Distance;
    }

    // Returns the time in hours for the fastest path between the src and dest
    // nodes of the if one exists. NoPathExists is returned if no path exists.
    // The transportation mode and nodes of the fastest path are filled in the
    // path parameter.
    double FindFastestPath(TNodeID src, TNodeID dest, std::vector<TTripStep> &path) {

    }

    // Returns true if the path description is created. Takes the trip steps path
    // and converts it into a human readable set of steps.
    bool GetPathDescription(const std::vector<TTripStep> &path, std::vector<std::string> &desc) {

    }

};