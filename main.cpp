#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <map>
#include <random>

using namespace std;

///////////////////////Global variables
int nTrucks; //Number of trucks
int nNodes; //Number of nodes
vector<int> capacities; //Capacities of trucks
vector<int> nodes; //Position of nodes
vector<int> productions; //Production of nodes
vector<int> types; //Waste type of each node [A, B, C, D, E]
vector<int> emptyDistance; //Distance from depot to nodes when truck is empty
vector<vector<vector<int>>> distances; // Distances between nodes for each waste type [type][from][to]
vector<vector<vector<int>>> risks; // Risks between nodes for each waste type [type][from][to]

///////////////////////Random distribution variables
typedef std::mt19937 RNG;
uint32_t seed_val;
RNG rng;

///////////////////////Debug functions
void printVector(vector<int> vector){
    for(int i=0; i<vector.size(); ++i)
        std::cout << vector[i] << ' ';
    cout << "\n";
}


/////////////////////// Constants
map<string,int> typesToInt={
    {"A",0},
    {"B",1},
    {"C",2},
    {"D",3},
    {"E",4}, 
    {"-",5},
};
map<int,string> intToTypes={
    {0,"A"},
    {1,"B"},
    {2,"C"},
    {3,"D"},
    {4,"E"},
    {5,"-"}, 
};


///////////////////////helpers
vector<int> split(string str, char delimiter){
    string value="";
    vector<int> vector;

    
    for(char& c : str){
        if(c==delimiter){
            vector.push_back(stoi(value));
            value="";    
            continue;
        }
        value+=c;
    }
    if(value==""){
        return vector;
    }
    if(value=="-" || value=="A" || value=="B" || value=="C" || value=="D" || value=="E"){
        vector.push_back(typesToInt[value]);
    } else{
        vector.push_back(stoi(value));
    }
    return vector;
}

void readData(){
    ifstream infile("instancias/peligro-mezcla4-min-riesgo-zona1-2k-AE.2.hazmat");
    string line;

    //read number of trucks
    getline(infile, line);
    nTrucks=stoi(line);

    //read capacities of trucks
    getline(infile, line);
    capacities=split(line,' ');

    //read number of nodes
    getline(infile, line);
    nNodes=stoi(line);

    //read nodes positions, productions and types of materials
    for(int i=0; i<nNodes; i++){
        getline(infile, line);
        vector<int> values=split(line, ' ');
        nodes.push_back(values[0]);
        productions.push_back(values[1]);
        types.push_back(values[2]);
    }

    //read distance from depot to nodes with an empty truck
    getline(infile, line);
    emptyDistance=split(line,' ');
     for(int type=0; type<5; type++){
        vector<vector<int>> aux;
        for(int node=0; node<nNodes; node++){
            getline(infile, line);
            aux.push_back(split(line,' '));
        }
        distances.push_back(aux);
    }

     for(int type=0; type<5; type++){
        vector<vector<int>> aux;
        for(int node=0; node<nNodes; node++){
            getline(infile, line);
            aux.push_back(split(line,' '));
        }
        risks.push_back(aux);
    }
    return;
}

bool checkValidWaste(char from, char to){
    if(from==0 && (to==1 || to==4)){    //A => B/D
        return false;
    } else if(from==1 && to==0){        // B => A 
        return false;
    } else if(from==2 && to==3){        //C => D
        return false;
    } else if(from==3 && to==2){        //D => C
        return false;
    } else if(from==4 && to==0){        //E => A
        return false;
    } else {
        return true;
    }
}

int getRouteRisk(vector<int> route){
    
}

int getRouteDistance(vector<int> route){
    
}

void generateRandomSolution(){
    rng.seed(322);
    vector<int> unvisitedNodes;
    for(int i=1;i<nNodes;i++){
        unvisitedNodes.push_back(i);
    }
    shuffle(begin(unvisitedNodes), end(unvisitedNodes), rng);

    vector<vector<int>> solution;
    <vector<int>> solutionRisks;
    <vector<int>> solutionDistances;

    int totalDistance=0;
    int totalRisk=0;
    for(int truck=0;truck<nTrucks;truck++){
        vector<int> truckRoute;
        int routeDistance=0;
        int routeRisk=0;
        int currentType=-1;
        int previousNodeIndex=0;
        truckRoute.push_back(nodes[0]);
        for(vector<int>::iterator it=unvisitedNodes.begin(); it!=unvisitedNodes.end();){
            if(checkValidWaste(currentType, types[*it])){
                truckRoute.push_back(nodes[*it]);
                
                if(currentType==-1){//initialNode
                    routeDistance+=emptyDistance[*it];
                    previousNodeIndex=*it;

                } else{
                    routeDistance+=distances[currentType][previousNodeIndex][*it];
                    routeRisk+=risks[currentType][previousNodeIndex][*it];
                    previousNodeIndex=*it;
                }

                if(types[*it]>currentType){
                    currentType=types[*it];
                }
                unvisitedNodes.erase(it);
            } else{
                it++;
            }
        }

        routeDistance+=distances[currentType][previousNodeIndex][0];
        routeRisk+=risks[currentType][previousNodeIndex][0];

        solutionDistances.push_back(routeDistance);
        solutionRisks.push_back(routeRisk);

        truckRoute.push_back(nodes[0]);
        solution.push_back(truckRoute);

        /*cout << "route\n";
        printVector(truckRoute);
        cout << "distance: " << routeDistance << "\n";
        cout << "risk: " << routeRisk << "\n\n";
        totalDistance+=routeDistance;
        totalRisk+=routeRisk;*/

    }
    /*cout << "total distance: " << totalDistance << "\n";
    cout << "total risk: " << totalRisk << "\n";*/
    return 
}
//main
int main() 
{
    //read file
    readData();
    generateRandomSolution();
    /*vector<vector<int>> solution;
    for(int truck=0; truck<nTrucks; truck++){
        for(int type=0; type<5; type++){
            for(int node=0; node<nNodes; node++){

            }
        }
    }*/
    
    return 0;
}