
virtual std::shared_ptr<CStreetMap> StreetMap() const noexcept = 0;
Parameter - none
Return - Returns a shared pointer to a CStreetMap object
Purpose - This function allows the program to access the street map being used by the transportation planner. This map data is important for computing paths and other operations.

virtual std::shared_ptr<CBusSystem> BusSystem() const noexcept = 0;
Parameter - None
Return - Returns a pointer to a CBusSystem object. 
Purpose - The purpose of this function is to allow the program to access the bus system. This is crucial as the bus system holds all he information for the routes and stops which is essential.

virtual double WalkSpeed() const noexcept = 0;
Parameter - none
Return - Returns a double representing the walk speed.
Purpose - specifies the average walking speed of a person that will be used in calculations for fastest walking paths.

virtual double BikeSpeed() const noexcept = 0;
Parameter - none
Return - Returns a double representing the bike speed.
Purpose - specifies the average biking speed of a person that will be used in calculations for fastest biking speeds.

virtual double DefaultSpeedLimit() const noexcept = 0;
Parameter - none
Return - returns a a double of the default speed limit
Purpose - the purpose of this method is to provide the default speed limit that is required for calculating the speed of the busses on average. 

virtual double BusStopTime() const noexcept = 0;
Parameter - none
return - returns a double as the average time spent at a bus stop.
purpose - the purpose of this function is to provide the average time a bus stays at a stop which is crucial for calculating its overall speed and fastest routes.

virtual int PrecomputeTime() const noexcept = 0;
extra credit

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


bool GetPathDescription(const std::vector< TTripStep > &path, std::vector< std::string > &desc) const override;

arg1 - std::vector<TTripStep> &path is a reference to a vector of TTripStep objects that each represent a step in a single route.

arg 2- std::vector<std::string> &desc is  reference to a vector of strings. This vector is used to store the descriptions of the trip steps in a readable way.

Return - returns a boolean value of true if the path description is successfully given and false otherwise.

Purpose - The purpose of this function is to provide clear and understandable descriptions of the required steps in a given path or route.