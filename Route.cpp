//Author: Trevor Huval

#include <iostream>
#include <fstream>
#include <iomanip>
#include <string>
#include <sstream>

using namespace std;

typedef struct Edge;
typedef struct Vertex;
const int MAXIMUM_INT = 2147483647;

int myhash(string c);
Edge* appendToAdjList(Edge* root, Edge* newEdge);
Vertex* appendToChain(Vertex* root, Vertex* newLink);
Vertex* chainTraverse(Vertex* root, Vertex* currentLink, Edge* currentEdge);
Vertex* getvertex(string code);

void dijkstra(Vertex* s);
void relax(Vertex* u, Vertex* w, int dep, int arr);

void insertHeap(Vertex* insertVertex);
Vertex* extractMin();
void decreaseKey(Vertex* newkey);
void floatUp(int index);
void sinkDown(int root);
void swap(int a, int b);

typedef struct Edge {
    string origin = "";
    string dest = "";
    string airlines = "";
    int fltno = 0;
    int deptime = 0;
    int arrtime = 0;
    Edge* next = NULL;
};

typedef struct Vertex {
    string code = "";
    int heap_pos = 0;
    int hash_pos = 0;
    Edge* adj_list = NULL;
    Vertex* chain = NULL;
    int dvalue = MAXIMUM_INT;
    Vertex* parent = NULL;
};

Vertex hasharray[1000];
Vertex* heaparray[1000] = {};
int realHeapSize = 0;
Vertex* cloud[1000];
int cloudCounter = 0;

int main(int argc, char* argv[])
{
    int counter = 1;
    ifstream inFile;
    inFile.open("airports.txt");
    int size = 0;
    string code;
    inFile >> size;
    for (int i = 0; i < size; i++) {      //adds all airports as vertexes into a chain hashtable
        inFile >> code;
        Vertex temp;
        temp.code = code;
        temp.hash_pos = myhash(temp.code);
        if (hasharray[myhash(temp.code)].code == "") {
            hasharray[myhash(temp.code)] = temp;
            counter++;
            Vertex* tempChain = new Vertex;
            tempChain->code = code;
            tempChain->hash_pos = myhash(tempChain->code);
            hasharray[myhash(temp.code)].chain = appendToChain(hasharray[myhash(temp.code)].chain, tempChain);
        }
        else {
            Vertex* tempChain = new Vertex;
            tempChain->code = code;
            tempChain->hash_pos = myhash(tempChain->code);
            hasharray[myhash(temp.code)].chain = appendToChain(hasharray[myhash(temp.code)].chain, tempChain);
            counter++;
        }
    }
    inFile.close();
    inFile.open("flights.txt");
    inFile >> size;
    string skip;
    inFile >> skip >> skip >> skip >> skip >> skip >> skip >> skip;
    for (int i = 0; i < size; i++) {    //adds all flights(edges) into adjacency list of airports(vertexes)
        Edge* temp = new Edge;
        inFile >> temp->airlines >> temp->fltno >> temp->origin >> temp->dest >> temp->deptime >> temp->arrtime >> skip;
        hasharray[myhash(temp->origin)].chain = chainTraverse(hasharray[myhash(temp->origin)].chain, hasharray[myhash(temp->origin)].chain, temp);
    }
    inFile.close();

    cout << "--==Quickest Flight Path Solver==--\n";
    menuHEAD:cout << "For a list of all airports, type 'help', otherwise type 'fly'\n";
    string menu;
    cin >> menu;

    if (menu == "help") {
        ifstream inFile;
        inFile.open("airports.txt");
        int size = 0;
        string code;
        inFile >> size;
        for (int i = 0; i < size; i++) {      //lists all airports
            inFile >> code;
            cout << code << endl;
        }
        inFile.close();
        goto menuHEAD;
    }
    else if (menu == "fly") {
        string sourceAirport, destinationAirport;

        bool validSource = false;   //valid source input flag
        do {
            cout << "Please input the 3 letter airport abbreviation for your starting airport: ";
            cin >> sourceAirport;

            ifstream inputSanitize;
            inputSanitize.open("airports.txt");     //this block checks if input is a valid airport
            int size = 0;
            string code;
            inputSanitize >> size;
            for (int i = 0; i < size; i++) {
                inputSanitize >> code;
                if (code == sourceAirport)
                    validSource = true;
            }
            inFile.close();
        } while (!validSource);

        bool validDestination = false;
        do {
            cout << "Please input the 3 letter airport abbreviation for your destination airport ";
            cin >> destinationAirport;

            ifstream inputSanitize;
            inputSanitize.open("airports.txt");
            int size = 0;
            string code;
            inputSanitize >> size;
            for (int i = 0; i < size; i++) {      //this block checks if input is a valid airport
                inputSanitize >> code;
                if (code == destinationAirport)
                    validDestination = true;
            }
            inFile.close();
        } while (!validDestination);

        Vertex* source;
        source = getvertex(sourceAirport);
        source->dvalue = 0;
        Vertex* destination;
        destination = getvertex(destinationAirport);

        dijkstra(source);

        string path = "";
        for (int i = 0; i < cloudCounter; i++) {        //puts path from the destination in reverse with dvalue at the end
            if (cloud[i]->code == destination->code) {
                path.insert(0, to_string(destination->dvalue));
                path.insert(0, destination->code + "-");
                while (destination->parent != nullptr) {
                    path.insert(0, destination->parent->code + "-");
                    destination->parent = destination->parent->parent;
                }
                break;
            }
        }
        cout << endl << "The quickest flight path is " << path << endl << endl;
        goto menuHEAD;
    }
    else
        goto menuHEAD;


    return 0;
}

Vertex* chainTraverse(Vertex* root, Vertex* currentLink, Edge* currentEdge) {   //finds corresponding chain link, and adds new edge to its adjacency list
    while (currentLink->code != currentEdge->origin) {
        currentLink = currentLink->chain;
    }
    currentLink->adj_list = appendToAdjList(currentLink->adj_list, currentEdge);
    return root;
}

Edge* appendToAdjList(Edge* root, Edge* newEdge) {     //returns a head pointer with a new edge added to the adjacency list
    if (root == nullptr)
    {
        return newEdge;
    }
    else
        root->next = appendToAdjList(root->next, newEdge);

    return root;
}

Vertex* appendToChain(Vertex* root, Vertex *newLink) {  //returns a head pointer with a new chain added to the hash chain

    if (root == nullptr)
    {
        Vertex* returnVertex = new Vertex;
        returnVertex = newLink;
        return returnVertex;
    }
    else
        root->chain = appendToChain(root->chain, newLink);
    return root;
}


void dijkstra(Vertex* s) {
    int i = 0;
    while (i < 1000) {      //put every vertex in the hashtable into the heap
        Vertex* temp;
        temp = hasharray[i].chain;
        while (temp != nullptr) {
            insertHeap(temp);
            temp = temp->chain;
        }
        i++;
    }

    Vertex* destination;

    while (realHeapSize > 0) {
        Vertex* min = extractMin();     //extractMin
        while (min->adj_list != nullptr) {  //Relax all outgoing edges of min
            destination = getvertex(min->adj_list->dest);
            relax(min, destination, min->adj_list->deptime, min->adj_list->arrtime);
            min->adj_list = min->adj_list->next;
        }
        cloud[cloudCounter] = min;  //Add to cloud
        cloudCounter++;
    }
}

int myhash(string c) {
    int p0 = (int)c.at(0) - 'A' + 1;
    int p1 = (int)c.at(1) - 'A' + 1;
    int p2 = (int)c.at(2) - 'A' + 1;
    int p3 = p0 * 467 * 467 + p1 * 467 + p2;
    int p4 = p3 % 7193;
    
    return p4 % 1000;
}

Vertex* getvertex(string code) {
    int p = myhash(code);
    Vertex* temp;
    temp = hasharray[myhash(code)].chain;
    while (true) {
        if (temp->code == code)
            return temp;
        temp = temp->chain;
    }
    for (int i = p + 1; (i != p); i++) {
        if (i >= 1000)
            i = i % 1000;   //rolling over
        if (hasharray[p].code == code)
            break;
        if (hasharray[p].code == "")
            return NULL;
    }
}

void relax(Vertex* u, Vertex* w, int dep, int arr) {
    if (u->dvalue <= dep) {
        if (arr <= w->dvalue) {
            w->dvalue = arr;
            w->parent = u;
        }
    }
}

void insertHeap(Vertex* insertVertex)     //insert function adds new number to end of heap and calls floatUp to correctly place new node
{
    insertVertex->heap_pos = realHeapSize;
    heaparray[realHeapSize] = insertVertex;
    realHeapSize++;
    floatUp(realHeapSize - 1);
}

Vertex* extractMin()           //extractMin returns root node, places last node at the root, and calls sinkDown to correctly place root
{
    Vertex* min = heaparray[0];

    heaparray[0] = heaparray[realHeapSize - 1];

    realHeapSize--;

    sinkDown(0);

    return min;
}

void decreaseKey(Vertex* newkey)      //decreaseKey in this implementation just calls on floatUp with the vertex's position in the heap
{
    floatUp(newkey->heap_pos);
}

void floatUp(int index)          //floatUp algorithm to correctly place inserted and key-decreased items in the heap
{                                           //compares current node with parent, if it is smaller it swaps the two nodes
    int parent = (index - 1) / 2;

    while (parent >= 0)
    {
        if (heaparray[index]->dvalue < heaparray[parent]->dvalue)
        {
            swap(index, parent);
            index = parent;
            parent = (index - 1) / 2;
        }

        else
            break;
    }
}

void sinkDown(int root)      //sinkdown function to correctly place node after an extract min, since last node overwrote first node it needs to be
{                                           //placed further down the tree
    Vertex* temp;
    int minChild, otherChild;
    int bottom = realHeapSize;

    minChild = root * 2 + 1;
    if (minChild > bottom) return;
    if (minChild < bottom) {
        otherChild = minChild + 1;
        minChild = (heaparray[otherChild]->dvalue < heaparray[minChild]->dvalue) ? otherChild : minChild;
    }

    if (heaparray[root]->dvalue <= heaparray[minChild]->dvalue) return;

    temp = heaparray[root];
    heaparray[root] = heaparray[minChild];
    heaparray[minChild] = temp;

    sinkDown(minChild);
}

void swap(int a, int b)        //swap function used in floatUp function
{
    Vertex* temp = heaparray[a];
    heaparray[a] = heaparray[b];
    heaparray[b] = temp;
}
