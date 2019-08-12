#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <map>


using namespace std;


///////////////////////Debug functions
void printVector(vector<int> vector){
    for(int i=0; i<vector.size(); ++i)
        std::cout << vector[i] << ' ';
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

//main
int main() 
{
    //read file
    ifstream infile("instancias/peligro-mezcla4-min-riesgo-zona1-2k-AE.2.hazmat");
    string line;
    
    //read number of trucks
    getline(infile, line);
    int nTrucks=stoi(line);

    //read capacities of trucks
    getline(infile, line);
    vector<int> capacities=split(line,' ');

    //read number of nodes
    getline(infile, line);
    int nNodes=stoi(line);

    //read nodes positions, productions and types of materials
    vector<int> nodes;
    vector<int> productions;
    vector<int> types;
    for(int i=0; i<nNodes; i++){
        getline(infile, line);
        vector<int> values=split(line, ' ');
        nodes.push_back(values[0]);
        productions.push_back(values[1]);
        types.push_back(values[2]);
    }

    //read distance from depot to nodes with an empty truck
    getline(infile, line);
    vector<int> emptyDistance=split(line,' ');

    vector<vector<vector<int>>> distances;
     for(int type=0; type<5; type++){
        for(int node=0; node<nNodes; node++){
            getline(infile, line);
            distances[type][node]=split(line,' ');
        }
    }

    vector<vector<vector<int>>> risks;
     for(int type=0; type<5; type++){
        for(int node=0; node<nNodes; node++){
            getline(infile, line);
            risks[type][node]=split(line,' ');
        }
    }


    return 0;
}