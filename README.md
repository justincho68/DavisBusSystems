# Project 2 README

Student 1: Justin Cho (920862823)
Student 2: Rachel Lin (919418893)

## Project Status 
The current status of this project is completed.

## Known Issues 
This project should be functioning correctly. There are not any issues we know of.

## Code References 
* [std::map](https://cplusplus.com/reference/map/map/) We used this link when writing CSV Bus System for the routes. 
* [How do you loop through a std::map?](https://stackoverflow.com/questions/26281979/how-do-you-loop-through-a-stdmap) We used this resource when looping through the map objects for route in CSV Bus System. 
* [std::stoull](https://cplusplus.com/reference/string/stoull/) We used this for the stop IDs in CSV Bus System. 
* [std::unordered_map](https://en.cppreference.com/w/cpp/container/unordered_map) We used this in the Open Street Map. 
* [std::cerr](https://cplusplus.com/reference/iostream/cerr/) We used this to display errors. 
* [Definitive List of Common Reasons for Segmentation Faults](https://stackoverflow.com/questions/33047452/definitive-list-of-common-reasons-for-segmentation-faults) We ran into a segfault while testing and used this as a reference to see the cause of the issue. 
* [C++ Program to Create an Interface](https://www.geeksforgeeks.org/cpp-program-to-create-an-interface/) We used this to understand what an interface is.
* Several OSM functions provided in discussion by Professor Nitta. 

## Generative AI Use 
We used ChatGPT for prompts 1-5.

### Prompt 1
How to return a null pointer in c++?

### Response 1
In C++, you can return a null pointer by simply using the nullptr keyword, which was introduced in C++11. nullptr is a keyword that represents a null pointer constant.

Here's an example of how to return a null pointer from a function:
#include <iostream>

int* returnNullPointer() {
    return nullptr;
}

int main() {
    int* ptr = returnNullPointer();
    
    if (ptr == nullptr) {
        std::cout << "Pointer is null." << std::endl;
    } else {
        std::cout << "Pointer is not null." << std::endl;
    }
    
    return 0;
}
In the returnNullPointer() function, we're simply returning nullptr, indicating that the function returns a null pointer. Then, in the main() function, we check if the pointer returned by returnNullPointer() is null using the nullptr keyword.

Before C++11, programmers often used NULL, which is a macro defined in <cstddef> or <stddef.h>, to represent null pointers. However, nullptr is preferred in modern C++ code because it provides better type safety and readability.

### Prompt 2
The Set Attribute print statement is not being printed becuse the conditional is never entered, please tell me where specifically the issue is:
    while (src->ReadEntity(TempEntity,true) && TempEntity.DType != SXMLEntity::EType::EndElement) {
    //nd references to nodes and ref is a node id. These IDs will be added to the way's list of IDs
        if (TempEntity.DNameData == "nd") {
            TNodeID NodeID = std::stoull(TempEntity.AttributeValue("ref"));
            NewWay->DNodeIDs.push_back(NodeID);
            std::cout<<"Added Node ID to way:" << NodeID << std::endl;
        }
        //Parses through tag elements like the node parsing, adding additional attributes as it continues to parse
        else if ((TempEntity.DType == SXMLEntity::EType::StartElement) && (TempEntity.DNameData == "tag")) {
            std::string key = TempEntity.AttributeValue("k");
            std::string value = TempEntity.AttributeValue("v");
            NewWay->SetAttribute(key,value);
            std::cout << "Set attribute for way: Key=" << key << ", Value=" << value << std::endl;
        }
    }

### Response 2
If the SetAttribute print statement is not being printed and the conditional for entering the block that processes <tag> elements is never met, it suggests that there might be an issue with how the conditions are evaluated or with the structure of the XML being parsed. Here are specific areas to review and fix:

EndElement Condition for <way>: The loop condition TempEntity.DType != SXMLEntity::EType::EndElement might prematurely terminate the loop before reaching the <tag> elements if an EndElement of <nd> is encountered. Ensure that your logic only exits the loop when the EndElement for the <way> itself is reached, not an EndElement for any nested elements like <nd>. You might need a more precise way to track when you've finished processing a <way> element, possibly by tracking nested levels or by setting a flag when entering and exiting <way> elements.

StartElement Condition for <tag>: Double-check that the condition TempEntity.DType == SXMLEntity::EType::StartElement && TempEntity.DNameData == "tag" is correct for your XML structure. Ensure that <tag> elements are direct children of <way> elements in your XML file and that they are properly formatted. If <tag> elements are correctly formatted and positioned as children of <way>, this condition should theoretically be met.

XML Structure: Verify the XML file structure to ensure that <tag> elements are indeed present within <way> elements and correctly formatted. If the <tag> elements are somehow nested differently than expected or if there are namespaces or prefixes involved that affect the DNameData, it could prevent the condition from being met.

Debugging: Temporarily remove the condition && TempEntity.DType != SXMLEntity::EType::EndElement and replace the else if for <tag> processing with just an if statement to force the execution into the block, then carefully monitor the output. This is not a solution but a debugging step to understand if the issue lies within the loop's exit condition or elsewhere.

Printing Entity Types and Names: Add debug print statements before the conditional checks within your loop to print out TempEntity.DType and TempEntity.DNameData for every entity read. This can help identify if and when <tag> elements are being read and what might be preventing the condition from being true.

By carefully reviewing these areas and adding strategic debug print statements, you should be able to pinpoint why the loop for processing <tag> elements is not being entered and adjust your conditions accordingly.

### Prompt 3
What does error 1 makefile mean?

### Response 3

Error 1 in a Makefile typically means that the make process encountered an error while trying to execute a specific rule or command. This error message is generated by the make utility when a command within the Makefile fails to execute properly.

Here's an example of how an error might be displayed:
make: *** [target_name] Error 1

In this message:
make indicates that the error occurred during the execution of the make command.
target_name refers to the target in the Makefile where the error occurred.
To diagnose the issue, you would need to examine the specific rule or command associated with the target mentioned in the error message. Check for typos, missing dependencies, incorrect file paths, or any other issues that might be causing the command to fail. Additionally, you can inspect any error messages or output generated by the failed command to understand the nature of the problem.

### Prompt 4
What is a segmentation fault?

### Response 4
A segmentation fault, often abbreviated as segfault, is a specific kind of error that occurs in a program when it tries to access a memory location that it doesn't have permission to access. This typically happens when a program tries to access memory that is outside of its address space, or when it tries to access memory that has been deallocated or otherwise invalidated.

Segmentation faults are a common type of bug in programming, especially in languages like C and C++ where programmers have direct control over memory management. They often occur due to errors such as dereferencing a null pointer, accessing an array element outside of its bounds, or attempting to write to read-only memory.

When a segmentation fault occurs, the operating system typically terminates the offending program and may generate a core dump or other diagnostic information to help programmers debug the issue. Fixing segmentation faults usually involves careful examination of the code to identify and correct the memory access errors that caused the fault.

### Prompt 5
What are the differences between a .h file and an abstract interface file in c++?

### Response 5
In C++, header files (typically with a ".h" extension) and abstract interfaces serve different purposes, though they can be related in certain contexts. Here are the key differences:

Purpose:

Header files (.h): Header files are used to declare the interface to functions, classes, variables, and other declarations that are defined in corresponding source files (usually ".cpp" files). They provide a way to share declarations between different source files and enable modular programming.
Abstract interface files: Abstract interfaces define a contract that classes must adhere to if they claim to implement the interface. They are used to achieve abstraction and polymorphism by allowing different classes to implement the same interface while providing their own implementations for the methods defined in the interface.
Content:

Header files (.h): Typically contain function prototypes, constant definitions, type definitions, and other declarations necessary for using the functionality provided by a library or module.
Abstract interface files: Define a set of methods or functions that a class must implement without specifying how those methods are implemented. They do not contain the actual implementation of the methods, only their declarations.
Implementation:

Header files (.h): Usually paired with corresponding source files (.cpp) that contain the actual implementation of the declarations in the header file.
Abstract interface files: Implementing classes provide their own implementations for the methods defined in the interface. These implementations are separate from the interface definition.
Usage:

Header files (.h): Included in other source files to make the declarations available for use. They facilitate code reuse and modular development.
Abstract interface files: Implemented by classes that intend to fulfill the contract defined by the interface. They provide a way to achieve polymorphism and abstraction in object-oriented programming.
In C++, header files and abstract interfaces are both important concepts, but they serve different roles in the design and implementation of software systems. Header files facilitate modular programming and code reuse, while abstract interfaces enable polymorphism and abstraction in object-oriented design.