#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <map>
#include <random>

using namespace std;

int populationSize=10;
int generations=10;
///////////////////////Global variables
int _nTrucks; //Number of trucks
int _nNodes; //Number of nodes
vector<int> _capacities; //Capacities of trucks
vector<int> _nodes; //Position of nodes
vector<int> _productions; //Production of nodes
vector<int> _types; //Waste type of each node [A, B, C, D, E]
vector<int> _emptyDistance; //Distance from depot to nodes when truck is empty
vector<vector<vector<int>>> _distances; // Distances between nodes for each waste type [type][from][to]
vector<vector<vector<int>>> _risks; // Risks between nodes for each waste type [type][from][to]

///////////////////////Random distribution variables
typedef std::mt19937 RNG;
uint32_t seed_val;
RNG rng;

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
    _nTrucks=stoi(line);

    //read capacities of trucks
    getline(infile, line);
    _capacities=split(line,' ');

    //read number of nodes
    getline(infile, line);
    _nNodes=stoi(line);

    //read nodes positions, productions and types of materials
    for(int i=0; i<_nNodes; i++){
        getline(infile, line);
        vector<int> values=split(line, ' ');
        _nodes.push_back(values[0]);
        _productions.push_back(values[1]);
        _types.push_back(values[2]);
    }

    //read distance from depot to nodes with an empty truck
    getline(infile, line);
    _emptyDistance=split(line,' ');
     for(int type=0; type<5; type++){
        vector<vector<int>> aux;
        for(int node=0; node<_nNodes; node++){
            getline(infile, line);
            aux.push_back(split(line,' '));
        }
        _distances.push_back(aux);
    }

     for(int type=0; type<5; type++){
        vector<vector<int>> aux;
        for(int node=0; node<_nNodes; node++){
            getline(infile, line);
            aux.push_back(split(line,' '));
        }
        _risks.push_back(aux);
    }
    return;
}

/*bool checkValidNodeType(vector<bool> routeTypes, int nodeType){
    if(nodeType==0 && routeTypes[1] && routeTypes[4]){    // B/E =/> A
        return false;
    } else if(nodeType==1 && routeTypes[0]){        // A =/> B
        return false;
    } else if(from==2 && to==3){        // D =/> C
        return false;
    } else if(from==3 && to==2){        // C =/> D
        return false;
    } else if(from==4 && to==0){        // A =/> E
        return false;
    } else {
        return true;
    }
    [0] && [1] || [0] && [4] || 
}*/
bool checkValidWasteTypes(vector<bool> types){
    return(
        !(  
            (types[0] && (types[1] || types[4]))
            ||(types[2] && types[3])
        )
    );
}
bool checkValidWasteTypes(vector<bool> types, int newType){
    vector<bool> newTypes(types);
    newTypes[newType]=true;
    return(
        !(  
            (newTypes[0] && (newTypes[1] || newTypes[4]))
            ||(newTypes[2] && newTypes[3])
        )
    );
}

tuple<int,int> evaluateRoute(vector<int> route){
    int distance=_emptyDistance[route[1]];
    int risk=0;
    int currentType=_types[route[1]];
    for(int i=2;i<route.size();i++){
        int prevNode=route[i-1];
        int node=route[i];
        distance+=_distances[currentType][prevNode][node];    
        risk+=_risks[currentType][prevNode][node];
        if(i!=route.size()-1 && _types[node]>currentType){
            currentType=_types[node];
        }
    }
    return make_tuple(distance,risk);
}

int evaluateSolution(vector<vector<int>> solution){
    int totalDistance=0
    int totalRisk=0;
    for(int i=0;i<solution.size();i++){
        tuple<int,int> route=evaluateRoute(solution[i]);
        totalDistance+=get<0>(tuple);
        totalRisk+=get<0>(tuple);
    }
    return totalDistance+totalRisk;
}

void printSolution(vector<vector<int>> solution){
    vector<tuple<int,int>> evaluations;
    int totalDistance=0;
    int totalRisk=0;
    for(int i=0;i<solution.size();i++){
        evaluations.push_back(evaluateRoute(solution[i]));
        totalDistance+=get<0>(evaluations[i]);
        totalRisk+=get<1>(evaluations[i]);
    }
    cout << totalDistance << " " << totalRisk << "\n";
    for(int i=0;i<solution.size();i++){
        for(int j=0;j<solution[i].size();j++){
            cout << _nodes[solution[i][j]] << " ";
        }
        cout << get<0>(evaluations[i]) << " ";
        cout << get<1>(evaluations[i]) << "\n";
    }
}

void writeSolution(vector<vector<int>> solution){
    ofstream output;
    output.open ("output.txt");
    vector<tuple<int,int>> evaluations;
    int totalDistance=0;
    int totalRisk=0;
    for(int i=0;i<solution.size();i++){
        evaluations.push_back(evaluateRoute(solution[i]));
        totalDistance+=get<0>(evaluations[i]);
        totalRisk+=get<1>(evaluations[i]);
    }
    output << totalDistance << " " << totalRisk << "\n";
    for(int i=0;i<solution.size();i++){
        for(int j=0;j<solution[i].size();j++){
            output << _nodes[solution[i][j]] << " ";
        }
        output << get<0>(evaluations[i]) << " ";
        output << get<1>(evaluations[i]) << "\n";
    }
    output.close();
}

vector<vector<int>> generateRandomSolution(){
    vector<int> unvisitedNodes;
    for(int i=1;i<_nNodes;i++){
        unvisitedNodes.push_back(i);
    }
    shuffle(begin(unvisitedNodes), end(unvisitedNodes), rng);

    vector<vector<int>> solution;
    for(int truck=0;truck<_nTrucks;truck++){
        vector<int> truckRoute;
        vector<bool> routeTypes{false,false,false,false,false};
        int currentType=-1;
        truckRoute.push_back(0);
        int production=0;
        for(vector<int>::iterator it=unvisitedNodes.begin(); it!=unvisitedNodes.end();){
            int nodeType=_types[*it];
            if(checkValidWasteTypes(routeTypes, nodeType) && production+_productions[*it]<=_capacities[truck]){
                truckRoute.push_back(*it);
                production+=_productions[*it];
                routeTypes[nodeType]=true;
                if(nodeType>currentType){
                    currentType=nodeType;
                }
                unvisitedNodes.erase(it);
            } else{
                it++;
            }
        }
        truckRoute.push_back(0);
        solution.push_back(truckRoute);
        /*cout << "route\n";
        printVector(truckRoute);
        cout << "distance: " << routeDistance << "\n";
        cout << "risk: " << routeRisk << "\n\n";
        totalDistance+=routeDistance;
        totalRisk+=routeRisk;*/
    }
    cout << "unvisited\n";
    printVector(unvisitedNodes);
    cout << "end\n";
    /*cout << "total distance: " << totalDistance << "\n";
    cout << "total risk: " << totalRisk << "\n";*/
    return solution;
}

//main
int main() 
{
    rng.seed(123);
    //read file
    readData();
    vector<vector<vector<int>>> population;
    vector<int> scores;
    for(int i; i<populationSize; i++){
        population.push_back(generateRandomSolution());
        scores.push_back(evaluateSolution(population[i]));
        writeSolution(population[i]);
    }

    for(int i; i<generations;i++){

    }

    /*vector<vector<int>> solution;
    for(int truck=0; truck<_nTrucks; truck++){
        for(int type=0; type<5; type++){
            for(int node=0; node<_nNodes; node++){

            }
        }
    }*/
    
    return 0;
}