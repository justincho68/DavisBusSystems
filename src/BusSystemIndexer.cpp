#include "BusSystemIndexer.h"
#include <vector>
#include <algorithm>
#include <unordered_map>

struct CBusSystemIndexer::SImplementation{
    std::shared_ptr<CBusSystem> DBusSystem;
    std::vector<std::shared_ptr<SStop>> DSortedStops;
    std::unordered_map< TNodeID, std::shared_ptr<SStop> NodeIDToStop;
    std::unordered_map< std::pair<TNodeID, TNodeID>, 
    SImplementation(std::shared_ptr<CBusSystem> bussystem) {
        DBusSystem = bussystem;
        for (size_t Index = 0; Index< DBusSystem.StopCount(); Index++) {
                DSortedStops.push_back(DBusSystem->StopByIndex(Index));
        }
        std::sort(DSortedStops.begin(), DSortedStops.end(),StopIDCompare);
    };
};

std::size_t Routecount() const noexcept {

        }

        std::size_t StopCount() const noexcept {
            return DBusSystem->StopCount();
        }


std::size_t CBusSystemIndexer::StopCount() const noexcept {

}