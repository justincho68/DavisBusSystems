virtual std::size_t VertexCount() const noexcept = 0;
parameters - none
purpose - return the number of vertices in a graph

virtual TVertexID AddVertex(std::any tag) noexcept = 0;
purpose - add a new vertex to the graph of std::any tag so that any data types can be added in.
return - returns the TVertexID of the new vertex
arg1 - std::any tag means that any form of data can be stored in the graph

virtual std::any GetVertexTag(TVertexID id) const noexcept = 0;
purpose - Retrieve the tag associated with a given vertex ID
arg1 - TVertexID id is the id of the vertex whose tag is going to be retrieved
return - returns the vertex tag at the given vertex id

virtual bool AddEdge(TVertexID src, TVertexID dest, double weight, bool bidir = false) noexcept = 0;
purpose - Adds an edge between two vertices in the graph, with the option of making it a bidirectional connection
arg1 - TVertexID src is the vertex id of the source vertex that will be connected to another one
arg2 - TVertexID dest is the vertex id of the destination vertex that will be connected to the src vertex
arg3 - bool bidir is an optional parameter with a default value set to false. Marking this parameter is true gives the option of having a bidirectional connection so src and dest are both pointing to each other
return - retunrs true if the operation was successful

 virtual bool Precompute(std::chrono::steady_clock::time_point deadline) noexcept = 0;
 //extra credit function not implemented

virtual double FindShortestPath(TVertexID src, TVertexID dest, 
        std::vector<TVertexID> &path) noexcept = 0;

purpose - The purpose of this function is to find the shortest path between two vertices and output the path.
arg1 - TVertexID src - the ID of the source vertex
arg2 - TVertexID dest - the ID of the destination vertex
arg3 - std::vector<TVertexID &path> - This is a reference to a vector that will soon be filled with a sequence of vertex IDs in the order of the shortest route between the src and dest vertex.
return - returns a double of the shortest possible path from thes src to destination vertex