#include "DijkstraPathRouter.h"
#include <algorithm>
#include "PathRouter.h"

struct CDijkstraPathRouter::SImplementation {
    //T for type
    using TEdge = std::pair<double, TVertexID>;

    struct SVertex {
        std::any DTag;
        //All edges need weight and destination
        std::vector<TEdge> DEdges;
    };

    std::vector <SVertex> DVertices;

    std::size_t VertexCount() const noexcept {
        return DVertices.size();
    }

    TVertexID AddVertex(std::any tag) noexcept {
        TVertexID NewVertexID = DVertices.size();
        DVertices.push_back({tag, {}});
        return NewVertexID;
    }

    std::any GetVertexTag(TVertexID id) const noexcept {
        if (id < DVertices.size()) {
            return DVertices[id].DTag;
        }
        return std::any();
    }

    bool AddEdge(TVertexID src, TVertexID dest, double weight, bool bidir = false) noexcept {
        if (weight <= 0.0) {
            return false;
        }
        //make sure source and destination are valid
        if ((src < DVertices.size()) && (dest < DVertices.size()) && (weight >= 0.0)) {
            DVertices[src].DEdges.push_back(std::make_pair(weight,dest));
            if (bidir) {
                DVertices[dest].DEdges.push_back(std::make_pair(weight,src));
            }
            return true;
        }
        return false;
    }

    bool Precompute(std::chrono::steady_clock::time_point deadline) noexcept {
        return true;
    }

    double FindShortestPath(TVertexID src, TVertexID dest, std::vector<TVertexID> &path) noexcept {
        if (src >= DVertices.size() || dest >= DVertices.size()) {
            return CPathRouter::NoPathExists;
        }
        if (DVertices.empty()) {
            path.clear();
            return CPathRouter::NoPathExists;
        }
        std::vector<TVertexID> PendingVertices;
        std::vector<TVertexID> Previous(DVertices.size(), CPathRouter::InvalidVertexID);
        std::vector<double> Distances(DVertices.size(), CPathRouter::NoPathExists);
        auto VertexCompare = [&Distances](TVertexID left, TVertexID right){return Distances[left] < Distances[right];};

        Distances[src] = 0.0;
        PendingVertices.push_back(src);
        while (!PendingVertices.empty()) {
            //Assume it is in heap format
            //Pop the heap then look at the back 
            auto CurrentID = PendingVertices.front();
            //sorts by ID number not distance
            std::pop_heap(PendingVertices.begin(), PendingVertices.end());
            //Get current ID off the top of the heap
            PendingVertices.pop_back();

            //Go through all edges to find new location
            for (auto Edge : DVertices[CurrentID].DEdges) {
                //Get the destination node and the weight 
                auto EdgeWeight = Edge.first;
                auto DestID = Edge.second;
                auto TotalDistance = Distances[CurrentID] + EdgeWeight;

                if (TotalDistance < Distances[DestID]) {
                    //Relax the edge
                    if (CPathRouter::NoPathExists == Distances[DestID]) {
                        PendingVertices.push_back(DestID);
                    }
                    Distances[DestID] = TotalDistance;
                    Previous[DestID] = CurrentID;
                }
            }
            // RE heapify after going through all edges
            std::make_heap(PendingVertices.begin(), PendingVertices.end(), VertexCompare);
        }
        //start at destination and work backwards towards source 
        if(CPathRouter::NoPathExists == Distances[dest]) {
            return CPathRouter::NoPathExists;
        }
        double PathDistance = Distances[dest];
        path.clear();
        path.push_back(dest);
        do {
            dest = Previous[dest];
            path.push_back(dest);
        }while (dest != src);
        std::reverse(path.begin(), path.end());
        return PathDistance;
        //while loop going from destination node back to source node 

    }
};

CDijkstraPathRouter::CDijkstraPathRouter() {
    DImplementation = std::make_unique<SImplementation>();
}

CDijkstraPathRouter::~CDijkstraPathRouter() {

}

std::size_t CDijkstraPathRouter::VertexCount() const noexcept {
    return DImplementation->VertexCount();
}

std::any CDijkstraPathRouter::GetVertexTag(TVertexID id) const noexcept {
    return DImplementation->GetVertexTag(id);
}

bool CDijkstraPathRouter::AddEdge(TVertexID src, TVertexID dest, double weight, bool bidir) noexcept {
    return DImplementation->AddEdge(src, dest, weight, bidir);
}

CPathRouter::TVertexID CDijkstraPathRouter::AddVertex(std::any tag) noexcept {
    return DImplementation->AddVertex(tag);
}

bool CDijkstraPathRouter::Precompute(std::chrono::steady_clock::time_point deadline) noexcept {
    return true;
}

double CDijkstraPathRouter::FindShortestPath(TVertexID src, TVertexID dest, std::vector<TVertexID> &path) noexcept {
    return DImplementation->FindShortestPath(src, dest, path);
}