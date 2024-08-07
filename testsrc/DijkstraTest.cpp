#include <gesth/gtest.h>
#include <"DijkstraPathRouter.h"

TEST(DijkstraPathRouter, RouteTest) {
    CDijkstraPathRouter PathRouter();
    //No requirement to increase in order, only that they are unique when added
    std::vector<CPathRouter::TVertexID> Vertices;
    for (std::size_t Index = 0; Index < 6; Index++) {
        Vertices.push_back(PathRouter.AddVertex(Index));
        //Returns standard any, must cast it to specific type
        EXPECT_EQ(Index,PathRouter.GetVertexTag(Vertices.back()));
    }
    EXPECT_EQ(6, PathRouter.VertexCount());
    PathRouter.AddEdge(Vertices[0], Vertices[4],3);
    PathRouter.AddEdge(Vertices[4], Vertices[5],90);
    PathRouter.AddEdge(Vertices[5], Vertices[3],6);
    PathRouter.AddEdge(Vertices[3], Vertices[2],8);
    PathRouter.AddEdge(Vertices[2], Vertices[0],1);
    PathRouter.AddEdge(Vertices[2], Vertices[1],3);
    PathRouter.AddEdge(Vertices[1], Vertices[3],9);
    std::vector<CPathRouter::TVertexID> Route;
    std::vector<CPathRouter, TVertexID> ExpectedRoute {Vertices[2],Vertices[1],Vertices[3]}
    EXPECT_EQ(12.0,PathRouter.FindShortestPath(Vertices[2], Vertices[3],Route))
    EXPET_EQ(Route, ExpectedRoute);
}