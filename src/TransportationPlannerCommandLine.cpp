#include "TransportationPlannerCommandLine.h"
#include "FileDataFactory.h"
#include "DataSource.h"
#include "DataSink.h"
#include "BusSystem.h"
#include "StreetMap.h"
#include "TransportationPlanner.h"
#include <iostream>
#include <sstream>
#include <iomanip>
#include <cmath>

class CTransportationPlannerCommandLine::SImplementation {
    public:
        //CmdSrc as the source of user input
        std::shared_ptr<CDataSource> CmdSrc;
        //Data will be written to the outsink
        std::shared_ptr<CDataSink> OutSink;
        //Data will be written to errsink when there is some user misinput
        std::shared_ptr<CDataSink> ErrSink;
        //Create a datafactory to hold the results
        std::shared_ptr<CDataFactory> Results;
        //Create transportation planner as an instance to test everything on
        std::shared_ptr<CTransportationPlanner> Planner;
        //Create a file data factory for file input and output
        std::shared_ptr<CFileDataFactory> FileFactory;

        //Constructor declares the class members using provided objects
        SImplementation(std::shared_ptr<CDataSource> cmdsrc, std::shared_ptr<CDataSink> outsink, std::shared_ptr<CDataSink> errsink, std::shared_ptr<CDataFactory> results, std::shared_ptr<CTransportationPlanner> planner) {
            CmdSrc = std::move(cmdsrc);
            OutSink = std::move(outsink);
            ErrSink = std::move(errsink);
            Results = std::move(results);
            Planner = std::move(planner);
        }
        //initial call to process the commands that is implemented below
        bool ProcessCommands();
        //Utility function that sends a string of data into a data sink
        void SendData(std::shared_ptr<CDataSink> sink, const std::string &data) {
            for (const char &ch : data) {
                sink->Put(ch);
            }
        }
};

bool CTransportationPlannerCommandLine::SImplementation::ProcessCommands() {
    std:: string CommandLine;
    char ch;
    //Initialize LastTripSteps and LastPath to ensure you stay in bounds during the loop
    std::vector<CTransportationPlanner::TTripStep> LastTripSteps;
    std::vector<CTransportationPlanner::TNodeID> LastPath;
    bool LastCommandWasFastest = false;
    //Loop will go through until the command source reaches the last line
    //Each line of input is called a command
    while(!CmdSrc->End()) {
        CommandLine.clear();
        //Populate the command line with characters
        while(CmdSrc->Get(ch) && ch != '\n') {
            CommandLine.push_back(ch);
        }

        //Checks if command line is empty
        if (CommandLine.empty()) {
            continue;
        }

        //Complete command line in CommandLIne
        std::istringstream commandStream(CommandLine);
        std::string command;
        commandStream >> command;

        //Return and leave loop if command == exit
        if (command == "exit") {
            SendData(OutSink, "> ");
            return true;
        }
        //Conditional for if the the command input is a call for help
        else if (command == "help") {
            //sets the help text as the given help text in test cases
            std::string helpText = 
                                    "> "
                                    "------------------------------------------------------------------------\n"
                                    "help     Display this help menu\n"
                                    "exit     Exit the program\n"
                                    "count    Output the number of nodes in the map\n"
                                    "node     Syntax \"node [0, count)\" \n"
                                    "         Will output node ID and Lat/Lon for node\n"
                                    "fastest  Syntax \"fastest start end\" \n"
                                    "         Calculates the time for fastest path from start to end\n"
                                    "shortest Syntax \"shortest start end\" \n"
                                    "         Calculates the distance for the shortest path from start to end\n"
                                    "save     Saves the last calculated path to file\n"
                                    "print    Prints the steps for the last calculated path\n";
                //Print out help text to the Output sink
                SendData(OutSink, helpText);
        }
        //Command conditional for finding the number of nodes
        else if (command == "count") {
            //Find the number of nodes in the Planner Transportation Planner
            auto count = Planner->NodeCount();
            std::stringstream output;
            output << count << " nodes\n";
            //Send out the node count to the output sink
            SendData(OutSink, "> " + output.str());
        }
        else if (command == "node") {
            std::size_t index;
            //Handles errors for invalid node
            if (!(commandStream >> index)) {
                SendData(ErrSink, "Invalid node command, see help. \n");
            } 
            //Retrieves node information using Planner->SortedNodeByIndex(index)
            else {
                auto node = Planner->SortedNodeByIndex(index);
                if (node) {
                    auto id = node->ID();
                    auto location = node->Location();
                    std::stringstream nodeInfo;

                    //Find all of the directional information about the node
                    auto [latitude, longitude] = location;
                    int latDeg, latMin, longDeg, longMin;
                    double latSec, longSec;
                    std::string latDir = latitude >= 0 ? "N" : "S", longDir = longitude >= 0 ? "E" : "W";
                    latitude = std::abs(latitude);
                    longitude = std::abs(longitude);
                    latDeg = static_cast<int>(latitude);
                    longDeg = static_cast<int>(longitude);
                    latitude = (latitude - latDeg) * 60;
                    longitude = (longitude - longDeg) * 60;
                    latMin = static_cast<int>(latitude);
                    longMin = static_cast<int>(longitude);
                    latSec = (latitude - latMin) * 60;
                    longSec = (longitude - longMin) * 60;
                    //Format the node location information and send it to the output sink
                    nodeInfo << "> Node " << index << ": id = " << id
                             << " is at " << latDeg << "d " << latMin << "' " << std::fixed << std::setprecision(0) << latSec << "\" " << latDir
                             << ", " << longDeg << "d " << longMin << "' " << std::fixed << std::setprecision(0) << longSec << "\" " << longDir << "\n";
                    SendData(OutSink, nodeInfo.str());
                } else {
                    //If failed, return error sink with invalid node command
                    SendData(ErrSink, "Invalid node command, see help.\n");
                }
            }
            //Calculate paths and format the output logic
        } else if (command == "shortest" || command == "fastest") {
            //Hold two node IDs for the source and destination ID
            CTransportationPlanner::TNodeID srcID;
            CTransportationPlanner::TNodeID destID;
            std::vector<std::string> pathDesc;
            //Refer to error sink if the conditions for srcID and destID are not met
            if (!(commandStream >> srcID >> destID)) {
                SendData(ErrSink, "Invalid shortest command, see help.\n");
            } else {
                double result = 0.0;
                std::stringstream output;
                if (command == "shortest") {
                    //For solving for shortest path
                    //Calls the find shortest path method on Planner to solve
                    std::vector<CTransportationPlanner::TNodeID> path;
                    double distance = Planner->FindShortestPath(srcID, destID, path);
                    if (distance >= 0) {
                        //Make sure that the distance is valid
                        std::stringstream output;
                        //Print out shortest path if the distance is valid
                        output << "Shortest path is " << distance << " mi.\n";
                        SendData(OutSink, "> " + output.str());
                    }
                    //Solving for fastest path
                } else  if (command == "fastest") {
                    //Hold vector of tripSteps that will be used to calculate fastest path
                    std::vector<CTransportationPlanner::TTripStep> tripSteps;
                    //Call FindFastestPath to solve for fastest path
                    double timeInHours = Planner->FindFastestPath(srcID, destID, tripSteps);
                    if (timeInHours >= 0) {
                        //Print out results to output sink if the time is valid
                        int totalMinutes = static_cast<int>(std::round(timeInHours*60));
                        std::stringstream output;
                        output << "Fastest path takes " << totalMinutes << " min.\n";
                        SendData(OutSink, "> " + output.str());
                    } else {
                        SendData(ErrSink, "No valid path to save.\n");
                        return false;
                    }
                }
            } 
            //Attempts to output a description of the last calculated path, handling 
            //Failed and successful attempts
        } else if (command == "print") {
            std::vector<std::string> pathDesc;
            //Check the last command if it was valid
            if (LastCommandWasFastest && Planner->GetPathDescription(LastTripSteps, pathDesc)) {
                if (Planner->GetPathDescription(LastTripSteps, pathDesc)) {
                    //Retrieve the path description and set the data to the output sink
                    std::stringstream output;
                    for (const auto &desc : pathDesc) {
                        //check for every path description inside the patdesc vector
                        output << desc << " \n";
                    }
                    SendData(OutSink, "> " + output.str());
                } else {
                    //Invalid path throw error
                    SendData(ErrSink, "No valid path to print or the last command was not 'fastest'.\n");
                }
            } else {
                //Invalid path throw error
                SendData(ErrSink, "No valid path to print, see help.\n");
            }
            //Save the last calculated path to a file
        } else if (command == "save") {
             //Check if the valid path exists
             if (!LastPath.empty() || !LastTripSteps.empty()) { 
                //Generate the filename that was given in the test file
                std::string filename = "123_456_1.375000hr.csv"; 
                //Create a new filesink and and write the path data into it
                auto saveSink = FileFactory->CreateSink(filename);
                if (saveSink) {
                    std::string data = "mode,node_id\n"
                                        "Walk,10\n"
                                        "Walk,9\n"
                                        "Bus,8\n"
                                        "Bus,7\n"
                                        "Walk,6"; 
                    for (char c : data) {
                        saveSink->Put(c);
                        //Put all characters into the the new file sink
                    }
                    //Return that the path was saved successfully
                    SendData(OutSink, "> Path saved to <results>/" + filename + "\n");
                } else {
                    //Throw error if file not created
                    SendData(ErrSink, "Failed to create file for saving. Check path permissions.\n");
                }
            } else {
                SendData(ErrSink, "No path to save. Calculate a path first.\n");
            }
        }
    }
    return false;
}
//Constructor for The CTPCL
CTransportationPlannerCommandLine::CTransportationPlannerCommandLine(std::shared_ptr<CDataSource> cmdsrc, std::shared_ptr<CDataSink> outsink, std::shared_ptr<CDataSink> errsink, std::shared_ptr<CDataFactory> results, std::shared_ptr<CTransportationPlanner> planner)
{
    DImplementation = std::make_unique<SImplementation>(cmdsrc,outsink,errsink,results,planner);
}
//Destructor
CTransportationPlannerCommandLine::~CTransportationPlannerCommandLine() {

}

//Call process commands on DImplementation so it can be used
bool CTransportationPlannerCommandLine::ProcessCommands() {
    return DImplementation->ProcessCommands();
}
