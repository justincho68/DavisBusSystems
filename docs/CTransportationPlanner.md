
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

bool GetPathDescription(const std::vector< TTripStep > &path, std::vector< std::string > &desc) const override;

arg1 - std::vector<TTripStep> &path is a reference to a vector of TTripStep objects that each represent a step in a single route.

arg 2- std::vector<std::string> &desc is  reference to a vector of strings. This vector is used to store the descriptions of the trip steps in a readable way.

Return - returns a boolean value of true if the path description is successfully given and false otherwise.

Purpose - The purpose of this function is to provide clear and understandable descriptions of the required steps in a given path or route.