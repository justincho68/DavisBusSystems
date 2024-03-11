#include "TransportationPlannerCommandLine.h"
#include "DataFactory.h"
#include "DataSource.h"
#include "DataSink.h"
#include "BusSystem.h"
#include "StreetMap.h"
#include "TransportationPlanner.h"
#include <iostream>
#include <sstream>

class CTransportationPlannerCommandLine::SImplementation {
    public:
        std::shared_ptr<CDataSource> CmdSrc;
        std::shared_ptr<CDataSink> OutSink;
        std::shared_ptr<CDataSink> ErrSink;
        std::shared_ptr<CDataFactory> Results;
        std::shared_ptr<CTransportationPlanner> Planner;

        SImplementation(std::shared_ptr<CDataSource> cmdsrc, std::shared_ptr<CDataSink> outsink, std::shared_ptr<CDataSink> errsink, std::shared_ptr<CDataFactory> results, std::shared_ptr<CTransportationPlanner> planner) {
            CmdSrc = std::move(cmdsrc);
            OutSink = std::move(outsink);
            ErrSink = std::move(errsink);
            Results = std::move(results);
            Planner = std::move(planner);
        }
        bool ProcessCommands();
        void SendData(std::shared_ptr<CDataSink> sink, const std::string &data) {
            for (const char &ch : data) {
                sink->Put(ch);
            }
        }
};

bool CTransportationPlannerCommandLine::SImplementation::ProcessCommands() {
    std:: string CommandLine;
    char ch;
    std::vector<CTransportationPlanner::TTripStep> LastTripSteps;
    std::vector<CTransportationPlanner::TNodeID> LastPath;
    bool LastCommandWasFastest = false;
    while(!CmdSrc->End()) {
        CommandLine.clear();
        while(CmdSrc->Get(ch) && ch != '\n') {
            CommandLine.push_back(ch);
        }

        if (CommandLine.empty()) {
            continue;
        }

        //Complete command line in CommandLIne
        std::istringstream commandStream(CommandLine);
        std::string command;
        commandStream >> command;

        if (command == "exit") {
            SendData(OutSink, "> ");
            return true;
        }
        else if (command == "help") {
            std::string helpText = 
                "------------------------------------------------------------------------\n"
                "help     Display this help menu\n"
                "exit     Exit the program\n"
                "count    Output the number of nodes in the map\n"
                "node     Syntax \"node [0, count)\"\n"
                "         Will output node ID and Lat/Lon for node\n"
                "fastest  Syntax \"fastest start end\"\n"
                "         Calculates the time for fastest path from start to end\n"
                "shortest Syntax \"shortest start end\"\n"
                "         Calculates the distance for the shortest path from start to end\n"
                "save     Saves the last calculated path to file\n"
                "print    Prints the steps for the last calculated path\n"
                "------------------------------------------------------------------------\n";
                SendData(OutSink, helpText);
        }
        else if (command == "count") {
            auto count = Planner->NodeCount();
            std::stringstream output;
            output <<"> "<< count << "nodes\n";
            SendData(OutSink, std::to_string(count) + " nodes\n");
        }
        else if (command == "node") {
            std::size_t index;
            if (!(commandStream >> index)) {
                SendData(ErrSink, "Invalid node command, see help. \n");
            } else {
                auto node = Planner->SortedNodeByIndex(index);
                if (node) {
                    auto location = node->Location();
                    double latitude = location.first;
                    double longitude = location.second;
                    std::stringstream nodeInfo;
                    nodeInfo << "> Node" << index << ": id = " << node->ID() << " is at latitude " << latitude << ", longitude " << longitude << "\n>";
                    SendData(OutSink, nodeInfo.str());
                } else {
                    SendData(ErrSink, "Node index out of range. \n");
                }
            }
        } else if (command == "shortest" || command == "fastest") {
            CTransportationPlanner::TNodeID srcID;
            CTransportationPlanner::TNodeID destID;
            std::vector<std::string> pathDesc;
            if (!(commandStream >> srcID >> destID)) {
                SendData(ErrSink, "Invalid command format, Please use the 'shortest srcID destID' 'or fastest srcID destID'.\n");
            } else {
                double result = 0.0;
                std::stringstream output;
                if (command == "shortest") {
                    std::vector<CTransportationPlanner::TNodeID> path;
                    result = Planner->FindShortestPath(srcID, destID, path);
                    output << "> Shortest path distance: " << result << " units\n";
                } else  if (command == "fastest") {
                    std::vector<CTransportationPlanner::TTripStep> tripSteps;
                    result = Planner->FindFastestPath(srcID, destID, tripSteps);
                    std::vector<std::string> pathDesc;
                    if(Planner->GetPathDescription(tripSteps, pathDesc)) {
                        output << "> Fastest path takes: " << result << " units\n";
                        for (const auto &desc : pathDesc) {
                            output << desc << "\n";
                        }
                    } else {
                        SendData(ErrSink, "Failed to get path description.\n");
                        return false;
                    }
                }
                output << "> ";
                SendData(OutSink, output.str());
            } 
        } else if (command == "print") {
            std::vector<std::string> pathDesc;
            if (LastCommandWasFastest) {
                if (Planner->GetPathDescription(LastTripSteps, pathDesc)) {
                    std::stringstream output;
                    for (const auto &desc : pathDesc) {
                        output << desc << " \n";
                    }
                    SendData(OutSink, output.str());
                } else {
                    SendData(ErrSink, "No valid path to print.\n");
                }
            } else {
                SendData(ErrSink, "Printing direct node ID paths not implemented. \n");
            }
        } else if (command == "save") {
            if (LastCommandWasFastest) {
                std::vector<std::string> pathDesc;
                if (Planner->GetPathDescription(LastTripSteps, pathDesc)) {
                    auto fileSink = Results->CreateSink("path_output.txt");
                    if (fileSink) {
                        for (const auto &desc : pathDesc) {
                            for (const char &ch : desc) {
                                fileSink->Put(ch);
                            }
                            fileSink->Put('\n');
                        }
                        SendData(OutSink, "Path saved successfully,\n");
                    } else {
                        SendData(ErrSink, "Failed to open file for saving. \n");
                    }
                }else {
                    SendData(ErrSink, "No valid path to save. \n");
                }
            } else {
                SendData(ErrSink, "Saving direct node ID paths not implemented. \n");
            }
        }
    }
    return false;
}
