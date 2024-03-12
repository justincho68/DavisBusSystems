CDijkstraTranportationPlanner is a class for computing transportation routes using Dijkstra's algorithm for different transportation methods of biking or walking/bussing. It finds and provides the fastest route and the shortest routes available. It also states the fastest transportation mode.


In the SImplementation, it takes the street map and bus system and adds vertices to the different Path Routers for shortest path, biking fastest path, and walking/bussing fastest path. It finds the different speed limits for the different streets and uses them to calculate the time it takes to travel. It also uses Haversine distance formula to calculate the distance in miles for edges in the fastest path routers. Then, it calculates the time it takes for the different transportation methods in hours based on the walking speed, biking speed, and speed limits for the buses. 
Parameter - std::shared_ptr<SConfiguration> config
struct CDijkstraTransportationPlanner::SImplementation {

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
    
    This function finds the amount of nodes present in the street map. 
    Return - number of nodes present 
    Parameter - None
    std::size_t NodeCount() const noexcept {
        return DStreetMap->NodeCount();
    }

    Parameter - Index Number
    Return - Returns the node at the index in the street map or none if it does not exist
    Purpose - To find the node at the index in Street Map
    std::shared_ptr<CStreetMap::SNode> SortedNodeByIndex(std::size_t index) const noexcept {
        if (index < DStreetMap->NodeCount()) {
            return DStreetMap->NodeByIndex(index);
        }
        return nullptr;
    }

    Parameter - src and dest nodes and path vector where the node IDs are filled in after running the function
    Return - Returns the distance in miles between the src and dest nodes of the shortest path if one exists or NoPathExists
    Purpose - The purpose of this function is to find the shortest path between the source and destination nodes and get the vector with the node IDs for the shortest path.
    double FindShortestPath(TNodeID src, TNodeID dest, std::vector<TNodeID> &path) {
        path.clear();
        std::vector <CPathRouter::TVertexID> ShortestPath; //create vector to collect vertexIDs for shortest path
        auto SourceVertexID = DNodeToVertexID[src]; //find the vertexID for the src node
        auto DestinationNodeID = DNodeToVertexID[dest]; //find the vertexID for the dest node
        //call the path router function of FindShortestPath
        auto Distance = DShortestPathRouter.FindShortestPath(SourceVertexID, DestinationNodeID, ShortestPath);
        for (auto VertexID : ShortestPath) {
            //convert the vertexIDs to NodeIDs and fill the path parameter
            if (VertexID)
            {
                path.push_back(std::any_cast<TNodeID>(DShortestPathRouter.GetVertexTag(VertexID))); 
            }
            
        }
        return Distance;

    }

    Parameter - source and destination nodes and vector TTripStep to fill in transporation mode and vertex ids
    Return - Returns the time in hours for the fastest path between the src and dest nodes if path exists, else return NoPathExists
    Purpose - The purpose of this function is to find the fastest path and transportation mode based on the source node and destination and return the time it takes for the specific fastest path. 
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
    bool GetPathDescription(const std::vector< TTripStep > &path, std::vector< std::string > &desc) const override;

    arg1 - std::vector<TTripStep> &path is a reference to a vector of TTripStep objects that each represent a step in a single route.

    arg 2- std::vector<std::string> &desc is  reference to a vector of strings. This vector is used to store the descriptions of the trip steps in a readable way.

    Return - returns a boolean value of true if the path description is successfully given and false otherwise.

    Purpose - The purpose of this function is to provide clear and understandable descriptions of the required steps in a given path or route.

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
