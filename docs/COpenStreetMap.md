The purpose of this class is to pares and represent OSM data in a structured format. It utilizes the PIMPL idiom to effectively create Nodes and Ways that can be parsed and gives functions that can be called on this data. 

class COpenStreetMap::SImplementation { //Implementation of the forward declaration
        struct SNode : public CStreetMap::SNode {
            //SNode represents a single node in the OSM and is a single Lat/Lon point
            TNodeID DID;
            TLocation DLocation;
            //Hold a map of attribute key-value pairs
            std::unordered_map<std::string, std::string> DAttributes;
            //Use vector of attribute keps to maintain order
            std::vector<std::string> DAttributeKeys;


            //Constructor for the SNode
            SNode(TNodeID id, TLocation loc) {
                DID = id;
                DLocation = loc;
            }

            ~SNode(){}

            //Simply returns the ID of a node
            TNodeID ID() const noexcept override {
                return DID;
            }

            //Simply returns associated Location with a node
            TLocation Location() const noexcept override {
                return DLocation;
            }

            //Simply returns the number of attributes or keys
            std::size_t AttributeCount() const noexcept override {
                return DAttributeKeys.size();
            }
            //Simply returns an attribute given at a specific index
            std::string GetAttributeKey(std::size_t index) const noexcept override {
                if (index < DAttributeKeys.size()) {
                    return DAttributeKeys[index];
                }
                return std::string();
            }

            //Check if there exists an attribute for a given key
            bool HasAttribute(const std::string &key) const noexcept override {
                auto Search = DAttributes.find(key);
                return DAttributes.end() != Search;
            }

            //Returns an attribute at a specific key index
            std::string GetAttribute(const std::string &key) const noexcept override {
                auto Search = DAttributes.find(key);
                if(DAttributes.end() != Search) {
                    return Search->second;
                }
                return std::string();
            }

            //Sets an attributeus to the keys vector and corresponding map
            void SetAttribute(const std::string &key, const std::string &value) {
                if (DAttributes.find(key) == DAttributes.end()) {
                    DAttributeKeys.push_back(key);
                }
                DAttributes[key] = value;
            }
         };

This code block first initalizes the COpenStreetMap class from the SImplementation struct.
The first thing we did was to define the struct for an SNode which simply refers to a single node on the map. We are implementing the SNode that was declared in the CStreetMap 
class and header file. The first thing we did was initialize a variable for the Node, Location, and a vector to hold the attributes and an unordered map to hold the key value pairs. Next is the constructor that initalizes the member variables and a destructor used for destroying the object when the work on it is complete. Next are the SNode functions. All of these functions are quite straightforward and their implementations are expalined using comments. 

struct SWay : public CStreetMap::SWay {
            TWayID DID;
            std::vector<TNodeID> DNodeIDs;
            std::unordered_map<std::string, std::string> DAttributes;

            //Constructor for SWay
            SWay(TWayID id) : DID(id) {

            }

            ~SWay() {

            }
            //Simply returns the ID of a way
            TWayID ID() const noexcept override {
                return DID;
            }
            //Simply returns the number of nodes in a way
            std::size_t NodeCount() const noexcept override {
                return DNodeIDs.size();
            }
            //Simply returns the nodeID at a specified index
            TNodeID GetNodeID(std::size_t index) const noexcept override {
                if (index < DNodeIDs.size()) {
                    return DNodeIDs[index];
                }
                return InvalidNodeID;
            }
            //REturns the number of attributes
            std::size_t AttributeCount() const noexcept override {
                return DAttributes.size();
            }
            //Returns an attribute key at a given index
            std::string GetAttributeKey(std::size_t index) const noexcept override {
                auto Search = std::next(DAttributes.begin(), index);
                if (index < DAttributes.size()) {
                    return Search->first;
                }
                return std::string();
            }
            //Returns true if an attribute exists at a given key
            bool HasAttribute(const std::string &key) const noexcept override {
                return DAttributes.find(key) != DAttributes.end();
            }
            //Returns an attribute at a specified key or an empty string otherwise
            std::string GetAttribute(const std::string &key) const noexcept override {
                auto Search = DAttributes.find(key);
                if (Search != DAttributes.end()) {
                    return Search->second;
                }
                return std::string();
            }
            //Sets attribute to a given value at a specified key
            void SetAttribute(const std::string &key, const std::string &value) {
                DAttributes[key] = value;
            }
         };

After the implementation of the SNode comes the implementation of the SWay. The SWay is also defined in the CStreetMap header file and is being implemented through a struct. The functions are similar to the functions used in the SNode struct that are documented through the comments. The SetAttributes function is an important addition that adds a value at a specific key. 

        std::unordered_map<TNodeID, std::shared_ptr<CStreetMap::SNode> > DNodeIDToNode;
        std::vector< std::shared_ptr<CStreetMap::SNode> > DNodesByIndex;
        std::unordered_map<TWayID, std::shared_ptr<SWay>> DWayIDToWay;
        std::vector<std::shared_ptr<SWay>> DWaysByIndex;

This is the declaration of 4 variables that will be used for parsing nodes and ways in the CSV and OSM files. The DNodeIDToNote map uses key value pairs to locate nodes whereas the DNodesByIndex vector uses an index because the nodes will be in order. On the other hand DWayIDToWay does the same thing for ways but uses a map and key value pairs. DWaysByIndex uses a vector so that the Ways are sorted in incremental order.

        public:
        //Initialize an instance of SImplementation
         SImplementation(std::shared_ptr<CXMLReader> src) {
            SXMLEntity TempEntity; //Temp SXMLEntity because will be reading from an XML file
            //Loops through all XML Entities
            while (src->ReadEntity(TempEntity, true)) {
                if((TempEntity.DNameData == "osm") && (SXMLEntity::EType::EndElement == TempEntity.DType)) {
                    //Reached end
                    break;
                }
                //Conditional block checks if it a start element, for a node, where the DNameData is node
                else if((TempEntity.DNameData == "node") && (SXMLEntity::EType::StartElement == TempEntity.DType)) {
                    //Parse node
                    TNodeID NewNodeID = std::stoull(TempEntity.AttributeValue("id"));
                    double Lat = std::stod(TempEntity.AttributeValue("lat"));
                    double Lon = std::stod(TempEntity.AttributeValue("lon"));
                    //When new node is encountered, it creates an instance of SNode and initalizes the Lat and Lon values
                    TLocation NewNodeLocation = std::make_pair(Lat,Lon);
                    //Create a shared pointer using the new node so it can be stored
                    auto NewNode = std::make_shared<SNode>(NewNodeID, NewNodeLocation);
                    //Store the node in both a map DNodeIDToNode for unordered storage
                    //And the vector DNodesByIndex for order
                    DNodesByIndex.push_back(NewNode);
                    DNodeIDToNode[NewNodeID] = NewNode;
                    while(src->ReadEntity(TempEntity, true)) {
                        //Continue parsing through and checking for "tag" elements
                        if((TempEntity.DNameData == "node") && (SXMLEntity::EType::EndElement == TempEntity.DType)) {
                            break;
                        }
                        else if((TempEntity.DNameData == "tag")&&(SXMLEntity::EType::StartElement == TempEntity.DType)) {
                            //If a tag is found, its corresponding key "k" and value "v" pair will be added to the nodes attributes
                            NewNode->SetAttribute(TempEntity.AttributeValue("k"), TempEntity.AttributeValue("v"));
                        }
                    }
                }
This code block is used to initialize the SImplementation and add the code for parsing both nodes and ways. A CXMLReader instance is created as it will be used to read the OSM file and parse the nodes. The parsing loop reads every SXMLEntity using the loop conditional while (src->ReadEntity(TempEntity, true)). For each entity, the loop extracts the Latitude and Longitude values and puts them into a new SNode instance. It stores the SNode in both DNodeIdToNode and DNodesByIndex. After creating this node object, the loop continues reading more entities and searches for "tag" elements associated with the node. The tags define key value pairs. For every key value pair, it is added to the nodes attributes. 


                //Same concept but for ways. If a Way start tag is encountered, it begins the extraction process
                else if((TempEntity.DNameData == "way") && (SXMLEntity::EType::StartElement == TempEntity.DType)) {
                    //Parse way
                    TWayID NewWayID = std::stoull(TempEntity.AttributeValue("id"));
                    auto NewWay = std::make_shared<SWay>(NewWayID);
                    //Create a new SWay instance for this ID
                    //Continue reading through elements
                    //The ways are added to DWaysByIndex for ordered access and 
                    //DWayIDToWay for an ID based Search system
                    DWaysByIndex.push_back(NewWay);
                    DWayIDToWay[NewWayID] = NewWay;
                    while (src->ReadEntity(TempEntity,true)) {

                        if ((TempEntity.DNameData == "way") && (TempEntity.DType == SXMLEntity::EType::EndElement)) {
                            break; //reached the end of the way element
                        }
                        //nd references to nodes and ref is a node id. These IDs will be added to the way's list of IDs
                        else if ((TempEntity.DNameData == "nd") && (TempEntity.DType != SXMLEntity::EType::EndElement)) {
                            TNodeID NodeID = std::stoull(TempEntity.AttributeValue("ref"));
                            NewWay->DNodeIDs.push_back(NodeID);
                        }
                        //Parses through tag elements like the node parsing, adding additional attributes as it continues to parse
                        else if ((TempEntity.DNameData == "tag") && (TempEntity.DType == SXMLEntity::EType::StartElement)) {
                            std::string key = TempEntity.AttributeValue("k");
                            std::string value = TempEntity.AttributeValue("v");
                            NewWay->SetAttribute(key,value);
                        }
                    }
                }
            }
         }
This next loop uses similar logic as the node parsing but it is utilized for parsing through ways. This loop works by converting the string ID into a TWayID using stoull which is string to unsigned long long. This ID will be crucial in identifying the way in the map.
A new SWay instance is created for the extracted ID and will be stored in a shared pointer. Once this new SWay is created, it will be stored in two containser, DWaysByIndex and DWayIDToWay. Like the node parsing, this is used for key value pairs or for ordered vector arrangement. Then, the code enters another while loop that will read the way element until it reaches the ending tag. It also searched for ref or k tags which are present in the OSM file. If they are found, theyare added to the objects attributes.

 //Functions outside of SImplementation that must be implemented as a part of OpenStreetMap
        //Simply return the total number of nodes
        std::size_t NodeCount() const {
            return DNodesByIndex.size();
         }
        //Simply return the total number of ways
         std::size_t WayCount() const {
            return DWaysByIndex.size();
         }
        //Return a shared pointer to a SNode given its index in the DNodesByINdex vector
        //Return null pointer if out of bounds
        std::shared_ptr<CStreetMap::SNode> NodeByIndex(std::size_t index) const {
            if (index < DNodesByIndex.size()) {
                return DNodesByIndex[index];
            }
            return nullptr;
         }
        //Look up a node by its ID in the DNodeIDToNode map. Return a shared pointer to the 
        //node with the ID. If desn't exist return null pointer
        std::shared_ptr<CStreetMap::SNode> NodeByID(TNodeID id) const {
            auto Search = DNodeIDToNode.find(id);
            if(DNodeIDToNode.end() != Search) {
                return Search->second;
            }
            return nullptr;
         }
        // Returns a shared pointer to a way given its Index in DWayIDByIndex vector
        // Returning null pointer if out out bounds
        std::shared_ptr<CStreetMap::SWay> WayByIndex(std::size_t index) const {
            if (index < DWaysByIndex.size()) {
                return DWaysByIndex[index];
             }
            return nullptr;
         }
        //Looks up a way given a specified ID in the DWayIDToWay map. Returns a shared pointer
        //To the specified way if found. Returns null pointer if nothing is found.
        std::shared_ptr<CStreetMap::SWay> WayByID(TWayID id) const {
            auto Search = DWayIDToWay.find(id);
            if (Search != DWayIDToWay.end()) {
                return Search->second;
            }
            return nullptr;
         }

    };
//Constructor initialized DImplementation unique pointer as an instance of SImplementation
//Passes in CXML reader so it can parse through OSM data
COpenStreetMap::COpenStreetMap(std::shared_ptr<CXMLReader> src) {
    DImplementation = std::make_unique<SImplementation>(src);
}
COpenStreetMap::~COpenStreetMap() {
}

//PUBLIC INTERFACE METHODS
//All methods forward the calls defined in SImplementation onto an instance DImplementation
std::size_t COpenStreetMap::NodeCount() const noexcept {
    return DImplementation->NodeCount();
}

std::size_t COpenStreetMap::WayCount() const noexcept {
    return DImplementation->WayCount();
}

std::shared_ptr<CStreetMap::SNode> COpenStreetMap::NodeByIndex(std::size_t index) const noexcept {
    return DImplementation->NodeByIndex(index);
}

std::shared_ptr<CStreetMap::SNode> COpenStreetMap::NodeByID(TNodeID id) const noexcept {
    return DImplementation->NodeByID(id);
}

std::shared_ptr<CStreetMap::SWay> COpenStreetMap::WayByIndex(std::size_t index) const noexcept {
    return DImplementation->WayByIndex(index);
}

std::shared_ptr<CStreetMap::SWay> COpenStreetMap::WayByID(TWayID id) const noexcept {
    return DImplementation->WayByID(id);
}

The final block of code is implementing the member functions of CSVBusSystem header file. These are not from the public interface and are explained through the comments documenting the purpose of each function. NodeCount function returns the number of nodes and WayCount function returns the number of ways. NodeByIndex returns a shared pointer to a SNode given an index and NodeByID returns a node given a unique ID. The WayByIndex and WayByID Does similar things but with ways. After that, the public interface functions are essentially forwarding the calls onto an instance of SImplementation called DImplementation.