        CTransportationPlannerCommandLine(std::shared_ptr<CDataSource> cmdsrc, std::shared_ptr<CDataSink> outsink, std::shared_ptr<CDataSink> errsink, std::shared_ptr<CDataFactory> results, std::shared_ptr<CTransportationPlanner> planner);

        This constructor is responsible for creating an instance of the CTransportationPlannerCommandLine class.
        arg1 - std::shared_ptr<CDataSource> cmdsrc is responsible for  implementing the CDataSource interface. The data source can be a file, user input through commands, or other options, but this is the source of data that the command line needs.
        arg2- std::shared_ptr<CDataSink> outsink - This is an object of CDataSink interface that is used as the destination for standard output from the command line. 
        arg3 - std::Shared_ptr<CDataSink> errsink - This is another object of the CDataSink interface that is used for sending error messages and diagnostic information back to the user if any mistakes are made.
        arg4 - std::shared_ptr<CDataFactory> results - This implementation of the CDataFactory interface is used to create instances of data-related objects or other services that the application could need.
        arg5- std::shared_ptr<CTransportationPlanner> planner - This argument is a pointer to the CTransportationPlanner interface. This argument is crucial as it allows the commands related to transportation planning such as managing the networks or getting the path descriptions.

        ~CTransportationPlannerCommandLine();
        This destructor is used for destroying the objects and freeing up memory.

        bool ProcessCommands();
        The purpose of this method is to process all of the possible commands that could come from the input source. These commands can be exit, help, count, node, fastest, shortest, save, print, or error. Exit simply exits the program from the command line. Help prints out a helptext that gives the user directions on how to use the program. Shortest finds shortest path and fastest finds fastest path. Count returns node count and node handles invalid nodes.

        Output - This function will return a boolean true if the commands were successfuly processed and false if they were not successfully processed.

        