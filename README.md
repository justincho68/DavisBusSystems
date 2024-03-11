# Project 4 README

Student 1: Justin Cho (920862823)
Student 2: Rachel Lin (919418893)

## Project Status 
The current status of this project is completed.

## Known Issues 
This project should be functioning correctly. There are not any issues we know of.

## Code References 
* [std::make_pair](https://cplusplus.com/reference/utility/make_pair/) We used this link when writing the FastestPath for Transportation Planner for the TTripStep vector.
* [What is Dijkstra’s Algorithm? | Introduction to Dijkstra’s Shortest Path Algorithm](https://www.geeksforgeeks.org/introduction-to-dijkstras-shortest-path-algorithm/) We used this resource to get a deeper understanding of Dijikstra's algorithm and how it works.
* [dijikstras algorithm for a node with no path](https://stackoverflow.com/questions/27364193/dijikstras-algorithm-for-a-node-with-no-path) We used this to understand what happens when a node has no path when using Dijkstra’s Algorithm.
* [std::unordered_map](https://en.cppreference.com/w/cpp/container/unordered_map) We used this in the Open Street Map. 
* [std::cerr](https://cplusplus.com/reference/iostream/cerr/) We used this to display errors. 
* [Makefile: Error1](https://stackoverflow.com/questions/4034392/makefile-error1) We ran into error 1 when compiling our code with the make file, so we used this to do some more research. 
* [C++ Program to Create an Interface](https://www.geeksforgeeks.org/cpp-program-to-create-an-interface/) We used this to understand what an interface is.
* Path Router and part of transportation planner provided in discussion by Professor Nitta. 

## Generative AI Use 
We used ChatGPT for prompts 1-5.

### Prompt 1
What does this error in makefile mean?:
mv ./testtmp/run_testfiledatass run_testfiledatass
./bin/testkml --gtest_output=xml:./testtmp/run_testkml
make: ./bin/testkml: No such file or directory
make: *** [Makefile:39: run_testkml] Error 127

### Response 1
This error typically means that the testkml binary file is not found in the ./bin/ directory.

Here's what each part of the error means:

make: ./bin/testkml: No such file or directory: This part of the error indicates that make is trying to execute ./bin/testkml (which is a binary file) but it doesn't exist in the specified directory.

make: *** [Makefile:39: run_testkml] Error 127: This line indicates that the error occurred in line 39 of your Makefile while executing the target run_testkml, and the error code 127 signifies that the command specified (in this case, trying to execute ./bin/testkml) was not found.

To resolve this issue, you need to make sure that the testkml binary is built and placed in the ./bin/ directory before running the make command. This might involve compiling the source code that generates testkml if it's not already built.

### Prompt 2
Explain Dijkstra’s Algorithm.

### Response 2
Dijkstra's Algorithm is a popular method used in computer science to find the shortest path between nodes in a graph, particularly when all edge weights are non-negative. It was developed by Dutch computer scientist Edsger W. Dijkstra in 1956.

Here's a step-by-step explanation of how Dijkstra's Algorithm works:

Initialization: Start with a source node and mark its distance as 0. Mark the distances of all other nodes as infinity (or a very large number). Keep track of the shortest distance from the source node to each node.

Selecting the Next Node: Choose the unvisited node with the smallest known distance from the source node. This is typically implemented using a priority queue or a min-heap data structure, which efficiently keeps track of the next node to explore.

Update Distances: For the selected node, examine all of its neighboring nodes (adjacent nodes). Calculate the distance to each neighboring node by summing the distance from the source node to the current node and the distance from the current node to the neighboring node. If this distance is shorter than the previously known distance, update the distance of the neighboring node.

Mark as Visited: Once all the neighbors have been examined, mark the current node as visited. This means that its shortest path from the source node has been found and it does not need to be revisited.

Repeat: Repeat steps 2 to 4 until all nodes have been visited. At each iteration, select the unvisited node with the smallest known distance from the source node and explore its neighbors.

Termination: The algorithm terminates when all nodes have been visited or when the destination node has been visited (if a destination node was specified). At this point, the shortest path from the source node to every other node (or the destination node) has been found.

Once Dijkstra's Algorithm terminates, you can reconstruct the shortest path from the source node to any other node by following the recorded shortest distances and predecessors from the source node.

It's worth noting that Dijkstra's Algorithm guarantees finding the shortest path in a graph with non-negative edge weights, but it does not work correctly for graphs with negative edge weights. For graphs with negative edge weights, algorithms like the Bellman-Ford Algorithm are more appropriate.

### Prompt 3
When would no path exist in Dijkstra's algorithm?

### Response 3

Dijkstra's algorithm is a method used to find the shortest path between nodes in a graph, given a starting node. It works for graphs with non-negative edge weights. If you're encountering a situation where Dijkstra's algorithm is indicating that no path exists between two nodes, there are several possible reasons:

Disconnected Nodes: The nodes you're trying to find a path between might be in different connected components of the graph. In this case, there truly is no path between them.

Negative Edge Weights: Dijkstra's algorithm assumes that all edge weights are non-negative. If your graph contains negative edge weights, Dijkstra's algorithm may not give correct results. In such cases, you might consider using algorithms like Bellman-Ford.

Incorrect Input: Ensure that the input to the algorithm is correct. If there's an error in specifying the nodes or edges, it could lead to unexpected behavior.

Node or Edge Missing: Check if the nodes or edges you're considering actually exist in the graph. It's possible that there's a typo or mistake in specifying the graph structure.

Algorithm Implementation Error: If you've implemented the algorithm yourself, double-check your implementation for bugs. Even small errors can lead to incorrect results.

Unreachable Node: One of the nodes might be unreachable from the other due to the directionality or structure of the graph.

To resolve the issue, carefully review your graph structure and the inputs to Dijkstra's algorithm to ensure they are correct. If you're still unable to identify the problem, consider providing more details about your specific scenario for further assistance.

### Prompt 4
What are different ways to loop through objects in a vector in a for loop?

### Response 4
In C++, if you have a vector of objects and you want to loop through each object using a for loop, there are several ways you can accomplish this:

Using a traditional for loop:
This is the most basic way to iterate through a vector using indices.
#include <vector>

// Assuming we have a vector of objects of type T
std::vector<T> objects;

// Loop through the vector using a traditional for loop
for (size_t i = 0; i < objects.size(); ++i) {
    // Access each object using the index i
    // objects[i].someMethod();
}
Using a range-based for loop:
This method simplifies the code and makes it more readable.
#include <vector>

// Assuming we have a vector of objects of type T
std::vector<T> objects;

// Loop through the vector using a range-based for loop
for (const auto& obj : objects) {
    // Access each object directly as obj
    // obj.someMethod();
}
Using iterators:
Iterators offer flexibility and can be used to traverse through the vector.
#include <vector>

// Assuming we have a vector of objects of type T
std::vector<T> objects;

// Loop through the vector using iterators
for (auto it = objects.begin(); it != objects.end(); ++it) {
    // Access each object using the iterator *it
    // (*it).someMethod();
}
Using reverse iterators:
If you need to traverse the vector in reverse order, you can use reverse iterators.
#include <vector>

// Assuming we have a vector of objects of type T
std::vector<T> objects;

// Loop through the vector in reverse using reverse iterators
for (auto it = objects.rbegin(); it != objects.rend(); ++it) {
    // Access each object in reverse order using the iterator *it
    // (*it).someMethod();
}
These are the common ways to loop through objects in a vector using a for loop in C++. Choose the one that best fits your requirements and coding style.

### Prompt 5
How do you create std::pair in C++?

### Response 5
In C++, you can create a std::pair using its constructor or using the std::make_pair function. Here's how to do it:

Using the constructor:
#include <iostream>
#include <utility> // for std::pair

int main() {
    // Creating a pair of int and double
    std::pair<int, double> myPair(10, 3.14);

    // Accessing elements of the pair
    std::cout << "First element: " << myPair.first << std::endl;
    std::cout << "Second element: " << myPair.second << std::endl;

    return 0;
}
Using std::make_pair:
#include <iostream>
#include <utility> // for std::pair

int main() {
    // Creating a pair of int and double using make_pair
    auto myPair = std::make_pair(10, 3.14);

    // Accessing elements of the pair
    std::cout << "First element: " << myPair.first << std::endl;
    std::cout << "Second element: " << myPair.second << std::endl;

    return 0;
}
Both methods achieve the same result. The std::make_pair function is often preferred as it deduces the types of the arguments automatically, reducing the need for explicit type specification.