#include <iostream>
#include <string>
#include <queue>
#include <vector>
#include <algorithm>
#include <chrono>
#include <stdexcept>

using namespace std;
using namespace std::chrono;

// ==========================================
// SECTOR 1: THE FOUNDATION (Task 1)
// ==========================================
enum class CargoType { STANDARD, FRAGILE, HAZARDOUS };

string cargoTypeToString(CargoType type) {
    switch(type) {
        case CargoType::STANDARD: return "STANDARD";
        case CargoType::FRAGILE: return "FRAGILE";
        case CargoType::HAZARDOUS: return "HAZARDOUS";
        default: return "UNKNOWN";
    }
}

class Good {
private:
    string name;
    CargoType type;
    double weight;

public:
    Good() : name(""), type(CargoType::STANDARD), weight(0.1) {} 
    
    Good(string n, CargoType t, double w) {
        name = n;
        type = t;
        if (w > 0) {
            weight = w;
        } else {
            throw invalid_argument("Invalid weight for " + n + ". Weight must be positive.");
        }
    }

    string getName() const { return name; }
    CargoType getType() const { return type; }
    string getTypeString() const { return cargoTypeToString(type); }
    double getWeight() const { return weight; }
    
    void print() const {
        cout << "- " << name << " [" << cargoTypeToString(type) << "] : " << weight << " kg" << endl;
    }
};

// ==========================================
// SECTOR 2: SORTING ALGORITHMS (Task 1)
// ==========================================
void bubbleSortDescending(vector<Good>& arr) {
    int n = arr.size();
    for (int i = 0; i < n - 1; i++) {
        for (int j = 0; j < n - i - 1; j++) {
            if (arr[j].getWeight() < arr[j + 1].getWeight()) {
                swap(arr[j], arr[j + 1]);
            }
        }
    }
}

int partition(vector<Good>& arr, int low, int high) {
    double pivot = arr[high].getWeight();
    int i = (low - 1);
    for (int j = low; j <= high - 1; j++) {
        if (arr[j].getWeight() > pivot) {
            i++;
            swap(arr[i], arr[j]);
        }
    }
    swap(arr[i + 1], arr[high]);
    return (i + 1);
}

void quickSortDescending(vector<Good>& arr, int low, int high) {
    if (low < high) {
        int pi = partition(arr, low, high);
        quickSortDescending(arr, low, pi - 1);
        quickSortDescending(arr, pi + 1, high);
    }
}

// ==========================================
// SECTOR 3: THE LOADING BAY (Task 2)
// ==========================================
template <typename T>
class LoadingStack {
private:
    vector<T> elements;
public:
    void push(T const& item) {
        elements.push_back(item);
    }
    void pop() {
        if (!elements.empty()) {
            elements.pop_back();
        } else {
            throw out_of_range("Stack is empty, cannot pop.");
        }
    }
    T top() const {
        if (elements.empty()) {
            throw out_of_range("Stack is empty.");
        }
        return elements.back();
    }
    bool isEmpty() const { return elements.empty(); }
};

// ==========================================
// [REQUIREMENT M1] FIFO QUEUE FOR DELIVERY
// ==========================================
template <typename T>
class DeliveryQueue {
private:
    vector<T> elements;
public:
    void enqueue(T const& item) {
        elements.push_back(item);
    }
    void dequeue() {
        if (!elements.empty()) {
            elements.erase(elements.begin()); // Removes from front (FIFO)
        } else {
            throw out_of_range("Queue is empty, cannot dequeue.");
        }
    }
    T front() const {
        if (elements.empty()) {
            throw out_of_range("Queue is empty.");
        }
        return elements.front();
    }
    bool isEmpty() const { return elements.empty(); }
};

// ==========================================
// SECTOR 4: THE GPS ROUTING (Task 2)
// ==========================================
struct Edge {
    int targetCity;
    double distance;
};

void dijkstra(int startNode, int targetNode, const vector<vector<Edge>>& graph) {
    auto startTime = high_resolution_clock::now(); 

    int n = graph.size();
    vector<double> minDistance(n, 1e9); 
    vector<int> previousNode(n, -1); // Array to store the path
    minDistance[startNode] = 0;
    
    priority_queue<pair<double, int>, vector<pair<double, int>>, greater<pair<double, int>>> pq;
    pq.push({0, startNode});
    
    while (!pq.empty()) {
        double currentDist = pq.top().first;
        int currentNode = pq.top().second;
        pq.pop();
        
        if (currentDist > minDistance[currentNode]) continue;
        
        for (const Edge& edge : graph[currentNode]) {
            double newDist = currentDist + edge.distance;
            if (newDist < minDistance[edge.targetCity]) {
                minDistance[edge.targetCity] = newDist;
                previousNode[edge.targetCity] = currentNode; // Record path
                pq.push({newDist, edge.targetCity});
            }
        }
    }
    
    auto stopTime = high_resolution_clock::now(); 
    auto duration = duration_cast<microseconds>(stopTime - startTime);

    cout << "\n[Calculating Delivery Route]" << endl;
    if (minDistance[targetNode] == 1e9) {
        cout << "No valid route found." << endl;
    } else {
        cout << "Shortest Distance to Target: " << minDistance[targetNode] << " km (Found in " << duration.count() << " us)" << endl;
        // Reconstruct path
        vector<int> path;
        for (int at = targetNode; at != -1; at = previousNode[at]) {
            path.push_back(at);
        }
        reverse(path.begin(), path.end());
        
        cout << "Optimal Route: ";
        for (size_t i = 0; i < path.size(); i++) {
            cout << "City " << path[i];
            if (i < path.size() - 1) cout << " -> ";
        }
        cout << endl;
    }
}

// ==========================================
// [REQUIREMENT D1] BELLMAN-FORD ALGORITHM
// ==========================================
void bellmanFord(int startNode, int targetNode, const vector<vector<Edge>>& graph) {
    auto startTime = high_resolution_clock::now(); 

    int n = graph.size();
    vector<double> minDistance(n, 1e9); 
    vector<int> previousNode(n, -1);
    minDistance[startNode] = 0;
    
    // Relax all edges V-1 times
    for (int i = 0; i < n - 1; i++) {
        for (int u = 0; u < n; u++) {
            for (const Edge& edge : graph[u]) {
                if (minDistance[u] != 1e9 && minDistance[u] + edge.distance < minDistance[edge.targetCity]) {
                    minDistance[edge.targetCity] = minDistance[u] + edge.distance;
                    previousNode[edge.targetCity] = u;
                }
            }
        }
    }
    
    auto stopTime = high_resolution_clock::now(); 
    auto duration = duration_cast<microseconds>(stopTime - startTime);

    cout << "\n[Calculating Delivery Route with Bellman-Ford]" << endl;
    if (minDistance[targetNode] == 1e9) {
        cout << "No valid route found." << endl;
    } else {
        cout << "Shortest Distance to Target: " << minDistance[targetNode] << " km (Found in " << duration.count() << " us)" << endl;
        // Reconstruct path
        vector<int> path;
        for (int at = targetNode; at != -1; at = previousNode[at]) {
            path.push_back(at);
        }
        reverse(path.begin(), path.end());
        
        cout << "Optimal Route: ";
        for (size_t i = 0; i < path.size(); i++) {
            cout << "City " << path[i];
            if (i < path.size() - 1) cout << " -> ";
        }
        cout << endl;
    }
}

// ==========================================
// SECTOR 5: THE WAREHOUSE BRAIN (Task 3)
// ==========================================
struct AVLNode {
    int cargoID;
    Good payload;
    AVLNode* left;
    AVLNode* right;
    int height;
};

class AVLTree {
private:
    void destroyTree(AVLNode* node) {
        if (node) {
            destroyTree(node->left);
            destroyTree(node->right);
            delete node;
        }
    }

    int height(AVLNode* N) { return N ? N->height : 0; }
    int max(int a, int b) { return (a > b) ? a : b; }

    AVLNode* rightRotate(AVLNode* y) {
        AVLNode* x = y->left;
        AVLNode* T2 = x->right;
        x->right = y;
        y->left = T2;
        y->height = max(height(y->left), height(y->right)) + 1;
        x->height = max(height(x->left), height(x->right)) + 1;
        return x;
    }

    AVLNode* leftRotate(AVLNode* x) {
        AVLNode* y = x->right;
        AVLNode* T2 = y->left;
        y->left = x;
        x->right = T2;
        x->height = max(height(x->left), height(x->right)) + 1;
        y->height = max(height(y->left), height(y->right)) + 1;
        return y;
    }

    int getBalance(AVLNode* N) { return N ? height(N->left) - height(N->right) : 0; }

    AVLNode* insertNode(AVLNode* node, int key, Good item) {
        if (!node) {
            AVLNode* newNode = new AVLNode();
            newNode->cargoID = key;
            newNode->payload = item;
            newNode->left = newNode->right = nullptr;
            newNode->height = 1;
            return newNode;
        }

        if (key < node->cargoID) node->left = insertNode(node->left, key, item);
        else if (key > node->cargoID) node->right = insertNode(node->right, key, item);
        else throw invalid_argument("Cargo ID " + to_string(key) + " already exists.");

        node->height = 1 + max(height(node->left), height(node->right));
        int balance = getBalance(node);

        if (balance > 1 && key < node->left->cargoID) return rightRotate(node);
        if (balance < -1 && key > node->right->cargoID) return leftRotate(node);
        if (balance > 1 && key > node->left->cargoID) {
            node->left = leftRotate(node->left);
            return rightRotate(node);
        }
        if (balance < -1 && key < node->right->cargoID) {
            node->right = rightRotate(node->right);
            return leftRotate(node);
        }
        return node;
    }

    AVLNode* searchNode(AVLNode* root, int key) {
        if (root == nullptr) throw out_of_range("Cargo ID " + to_string(key) + " not found.");
        if (root->cargoID == key) return root;
        if (root->cargoID < key) return searchNode(root->right, key);
        return searchNode(root->left, key);
    }

public:
    AVLNode* root = nullptr;

    ~AVLTree() { destroyTree(root); }

    void insert(int key, Good item) { root = insertNode(root, key, item); }
    Good search(int key) { return searchNode(root, key)->payload; }
};

// ==========================================
// SECTOR 6: MAIN EXECUTION
// ==========================================
int main() {
    cout << "===============================================\n";
    cout << "   SWIFT-LOAD LOGISTICS SYSTEM INITIATED       \n";
    cout << "===============================================\n" << endl;

    // --- 1. Sorter Benchmark (Task 1 & 2 integration) ---
    cout << ">>> PHASE 1: SORTING BENCHMARK (Comparing Algorithms)" << endl;
    vector<Good> massiveCargoBubble;
    vector<Good> massiveCargoQuick;
    for(int i = 0; i < 5000; i++) { // Using Good objects now
        massiveCargoBubble.push_back(Good("Item " + to_string(i), CargoType::STANDARD, i * 1.5 + 1));
        massiveCargoQuick.push_back(Good("Item " + to_string(i), CargoType::STANDARD, i * 1.5 + 1));
    }

    auto startB = high_resolution_clock::now();
    bubbleSortDescending(massiveCargoBubble);
    auto stopB = high_resolution_clock::now();
    cout << "Bubble Sort Time (5000 Objects): " << duration_cast<microseconds>(stopB - startB).count() << " us" << endl;

    auto startQ = high_resolution_clock::now();
    quickSortDescending(massiveCargoQuick, 0, massiveCargoQuick.size() - 1);
    auto stopQ = high_resolution_clock::now();
    cout << "Quick Sort Time  (5000 Objects): " << duration_cast<microseconds>(stopQ - startQ).count() << " us" << endl;
    cout << "(Quick Sort is significantly faster for large imports)\n" << endl;

    // --- 2. Warehouse Brain & Safety Protocols (Task 3) ---
    cout << ">>> PHASE 2: IMPORT & WAREHOUSE STORAGE" << endl;
    AVLTree inventory;
    try {
        cout << "[Importing Cargo into AVL Database...]" << endl;
        inventory.insert(101, Good("Medical Supplies", CargoType::FRAGILE, 120.5));
        inventory.insert(105, Good("Steel Beams", CargoType::STANDARD, 850.0));
        inventory.insert(102, Good("Chemicals", CargoType::HAZARDOUS, 45.0));
        cout << "Successfully registered 3 shipments.\n" << endl;
        
        cout << "[Error Handling Test: Invalid Weight]" << endl;
        Good invalidCargo("Ghost Cargo", CargoType::STANDARD, -5.0); // Should trigger error
    } catch (const exception& e) {
        cout << "SYSTEM ALERT (Import Blocked): " << e.what() << "\n" << endl;
    }

    try {
        cout << "[Error Handling Test: Duplicate ID]" << endl;
        inventory.insert(101, Good("Duplicate Try", CargoType::STANDARD, 10.0)); // Should trigger error
    } catch (const exception& e) {
        cout << "SYSTEM ALERT (Duplicate Blocked): " << e.what() << "\n" << endl;
    }

    // --- 3. Retrieval & Truck Loading (Task 2 & 3) ---
    cout << ">>> PHASE 3: EXPORT & TRUCK LOADING" << endl;
    LoadingStack<Good> truckA;
    try {
        cout << "Searching for Cargo ID 105 to export..." << endl;
        Good foundItem = inventory.search(105);
        cout << "Cargo Located! Details: ";
        foundItem.print();
        
        cout << "Loading onto Truck Stack..." << endl;
        truckA.push(foundItem);
        cout << "Truck A current loaded items: 1\n" << endl;

        cout << "Searching for unknown Cargo ID 999..." << endl;
        inventory.search(999); // Should trigger error
    } catch (const exception& e) {
        cout << "SYSTEM ALERT (Export Failed): " << e.what() << "\n" << endl;
    }

    // --- [REQUIREMENT M1 Demonstration] ---
    cout << "[REQUIREMENT M1: Testing FIFO Delivery Queue]" << endl;
    DeliveryQueue<string> truckQueue;
    truckQueue.enqueue("Truck 1 (License: ABC-123)");
    truckQueue.enqueue("Truck 2 (License: XYZ-987)");
    cout << "Queue Head (First to be loaded): " << truckQueue.front() << endl;
    truckQueue.dequeue(); // Truck 1 leaves
    cout << "Queue Head after dequeue: " << truckQueue.front() << "\n" << endl;

    // --- 4. GPS Routing (Task 2) ---
    cout << ">>> PHASE 4: GPS ROUTING & DISPATCH" << endl;
    vector<vector<Edge>> distGraph(5);
    // Building Map: City 0 is Warehouse, City 4 is Destination
    distGraph[0].push_back({1, 10.0}); distGraph[0].push_back({2, 15.0});
    distGraph[1].push_back({0, 10.0}); distGraph[1].push_back({3, 12.0});
    distGraph[2].push_back({0, 15.0}); distGraph[2].push_back({3, 4.0}); distGraph[2].push_back({4, 6.0});
    distGraph[3].push_back({1, 12.0}); distGraph[3].push_back({2, 4.0}); distGraph[3].push_back({4, 8.0});
    
    cout << "\n--- [REQUIREMENT D1: Algorithm 1 - Dijkstra] ---";
    dijkstra(0, 4, distGraph);

    cout << "\n--- [REQUIREMENT D1: Algorithm 2 - Bellman-Ford] ---";
    bellmanFord(0, 4, distGraph);

    cout << "\n===============================================\n";
    cout << "   SYSTEM SHUTDOWN SEQUENCE COMPLETED          \n";
    cout << "===============================================\n";

    return 0;
}