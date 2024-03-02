The purpose of the BusSystem.h header file is to serve as a public abstract interface for functions that will be implemented in CSVBusSystem.cpp. All of the functions in this header file are virtual, meaning they are meant to be overriden in other files which encourages polymorphism and object oriented programming. It also defines two crucial structs that are used in the implementation of CSVBusSystem. It defines the SStop and SRoute structs. The SStop struct is meant to show a stop for a bus and has a unique id attached to it and SRoute is meant to show a route as a connection of stops.

#ifndef BUSROUTE_H
#define BUSROUTE_H

#include "StreetMap.h"

class CBusSystem{
    public:
        using TStopID = uint64_t;

        static const TStopID InvalidStopID = std::numeric_limits<TStopID>::max();

        struct SStop{
            virtual ~SStop(){};
            virtual TStopID ID() const noexcept = 0;
            virtual CStreetMap::TNodeID NodeID() const noexcept= 0;
        };

        struct SRoute{
            virtual ~SRoute(){};
            virtual std::string Name() const noexcept = 0;
            virtual std::size_t StopCount() const noexcept = 0;
            virtual TStopID GetStopID(std::size_t index) const noexcept = 0;
        };

        virtual ~CBusSystem(){};

        virtual std::size_t StopCount() const noexcept = 0;
        virtual std::size_t RouteCount() const noexcept = 0;
        virtual std::shared_ptr<SStop> StopByIndex(std::size_t index) const noexcept = 0;
        virtual std::shared_ptr<SStop> StopByID(TStopID id) const noexcept = 0;
        virtual std::shared_ptr<SRoute> RouteByIndex(std::size_t index) const noexcept = 0;
        virtual std::shared_ptr<SRoute> RouteByName(const std::string &name) const noexcept = 0;
};

#endif
