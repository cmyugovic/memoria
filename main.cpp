#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <map>
#include <random>
#include <algorithm>
#include <iomanip>

using namespace std;

///////////////////////Configuration
string filePath="instancias/peligro-mezcla4-min-riesgo-zona1-2k-AE.2.hazmat";
int populationSize = 200;
int generations = 100;
int tournamentSize=20;
double mutationChance=0.5;
int seed=123;
int hillClimbingIterations=10;

///////////////////////Global variables
int _nTrucks;                           //Number of trucks
int _nNodes;                            //Number of nodes
vector<int> _capacities;                //Capacities of trucks
vector<int> _nodes;                     //Position of nodes
vector<int> _productions;               //Production of nodes
vector<int> _types;                     //Waste type of each node [A, B, C, D, E]
vector<int> _emptyDistance;             //Distance from depot to nodes when truck is empty
vector<vector<vector<int>>> _distances; // Distances between nodes for each waste type [type][from][to]
vector<vector<vector<int>>> _risks;     // Risks between nodes for each waste type [type][from][to]

///////////////////////Random distribution variables
typedef std::mt19937 RNG;
uint32_t seed_val;
RNG rng;


std::string GetCurrentTimeForFileName()
{
    auto time = std::time(nullptr);
    std::stringstream ss;
    ss << std::put_time(std::localtime(&time), "%F_%T"); // ISO 8601 without timezone information.
    auto s = ss.str();
    std::replace(s.begin(), s.end(), ':', '-');
    return s;
}

string currentTime=GetCurrentTimeForFileName();

void printVector(vector<int> vector)
{
    for (int i = 0; i < vector.size(); ++i)
        std::cout << vector[i] << ' ';
    std::cout << "\n";
}

/////////////////////// Constants
map<string, int> typesToInt = {
    {"A", 0},
    {"B", 1},
    {"C", 2},
    {"D", 3},
    {"E", 4},
    {"-", 5},
};
map<int, string> intToTypes = {
    {0, "A"},
    {1, "B"},
    {2, "C"},
    {3, "D"},
    {4, "E"},
    {5, "-"},
};

///////////////////////helpers
vector<int> split(string str, char delimiter)
{
    string value = "";
    vector<int> vector;

    for (char &c : str)
    {
        if (c == delimiter)
        {
            vector.push_back(stoi(value));
            value = "";
            continue;
        }
        value += c;
    }
    if (value == "")
    {
        return vector;
    }
    if (value == "-" || value == "A" || value == "B" || value == "C" || value == "D" || value == "E")
    {
        vector.push_back(typesToInt[value]);
    }
    else
    {
        vector.push_back(stoi(value));
    }
    return vector;
}

void readData()
{
    ifstream infile(filePath);
    string line;

    //read number of trucks
    getline(infile, line);
    _nTrucks = stoi(line);

    //read capacities of trucks
    getline(infile, line);
    _capacities = split(line, ' ');

    //read number of nodes
    getline(infile, line);
    _nNodes = stoi(line);

    //read nodes positions, productions and types of materials
    for (int i = 0; i < _nNodes; i++)
    {
        getline(infile, line);
        vector<int> values = split(line, ' ');
        _nodes.push_back(values[0]);
        _productions.push_back(values[1]);
        _types.push_back(values[2]);
    }

    //read distance from depot to nodes with an empty truck
    getline(infile, line);
    _emptyDistance = split(line, ' ');

    //read distance from node to node for each waste type
    for (int type = 0; type < 5; type++)
    {
        vector<vector<int>> aux;
        for (int node = 0; node < _nNodes; node++)
        {
            getline(infile, line);
            aux.push_back(split(line, ' '));
        }
        _distances.push_back(aux);
    }

    //read risk from node to node for each waste type
    for (int type = 0; type < 5; type++)
    {
        vector<vector<int>> aux;
        for (int node = 0; node < _nNodes; node++)
        {
            getline(infile, line);
            aux.push_back(split(line, ' '));
        }
        _risks.push_back(aux);
    }
    return;
}

bool checkValidWasteTypes(vector<bool> types)
{
    return (
        !(
            (types[0] && (types[1] || types[4])) || (types[2] && types[3])));
}
bool checkValidWasteTypes(vector<bool> types, int newType)
{
    vector<bool> newTypes(types);
    newTypes[newType] = true;
    return (
        !(
            (newTypes[0] && (newTypes[1] || newTypes[4])) || (newTypes[2] && newTypes[3])));
}
bool checkValidNodeIntoRoute(int node, vector<int> route)
{
    int nodeType = _types[node];
    if (nodeType == 0)
    {
        for (int i = 0; i < route.size(); i++)
        {
            if (_types[route[i]] == 1 || _types[route[i]] == 4)
            {
                return false;
            }
        }
    }
    else if (nodeType == 1)
    {
        for (int i = 0; i < route.size(); i++)
        {
            if (_types[route[i]] == 0)
            {
                return false;
            }
        }
    }
    else if (nodeType == 2)
    {
        for (int i = 0; i < route.size(); i++)
        {
            if (_types[route[i]] == 3)
            {
                return false;
            }
        }
    }
    else if (nodeType == 3)
    {
        for (int i = 0; i < route.size(); i++)
        {
            if (_types[route[i]] == 2)
            {
                return false;
            }
        }
    }
    else if (nodeType == 4)
    {
        for (int i = 0; i < route.size(); i++)
        {
            if (_types[route[i]] == 0)
            {
                return false;
            }
        }
    }
    return true;
}

tuple<int, int> evaluateRoute(vector<int> route)
{
    int distance = _emptyDistance[route[1]];
    int risk = 0;
    int currentType = _types[route[1]];
    for (int i = 2; i < route.size(); i++)
    {
        int prevNode = route[i - 1];
        int node = route[i];
        distance += _distances[currentType][prevNode][node];
        risk += _risks[currentType][prevNode][node];
        if (i != route.size() - 1 && _types[node] > currentType)
        {
            currentType = _types[node];
        }
    }
    return make_tuple(distance, risk);
}

tuple<int, int> evaluateSolution(vector<vector<int>> solution)
{
    int totalDistance = 0;
    int totalRisk = 0;
    for (int i = 0; i < solution.size(); i++)
    {
        tuple<int, int> route = evaluateRoute(solution[i]);
        totalDistance += get<0>(route);
        totalRisk += get<1>(route);
    }
    return make_tuple(totalDistance, totalRisk);
}

void printSolution(vector<vector<int>> solution)
{
    vector<tuple<int, int>> evaluations;
    int totalDistance = 0;
    int totalRisk = 0;
    for (int i = 0; i < solution.size(); i++)
    {
        evaluations.push_back(evaluateRoute(solution[i]));
        totalDistance += get<0>(evaluations[i]);
        totalRisk += get<1>(evaluations[i]);
    }
    std::cout << "Total Distance and Risk: " << totalDistance << " " << totalRisk << "\n";
    for (int i = 0; i < solution.size(); i++)
    {
        std::cout << "route: \n";
        for (int j = 0; j < solution[i].size(); j++)
        {
            std::cout << _nodes[solution[i][j]] << " ";
        }
        std::cout << get<0>(evaluations[i]) << " ";
        std::cout << get<1>(evaluations[i]) << "\n";
    }
}

void writeSolution(vector<vector<int>> solution)
{
    ofstream output;
    ofstream newOutput;
    output.open("full_output_"+currentTime+".txt", ios_base::app);
    newOutput.open("score_output_"+currentTime+".txt", ios_base::app);
    vector<tuple<int, int>> evaluations;
    int totalDistance = 0;
    int totalRisk = 0;
    for (int i = 0; i < solution.size(); i++)
    {
        evaluations.push_back(evaluateRoute(solution[i]));
        totalDistance += get<0>(evaluations[i]);
        totalRisk += get<1>(evaluations[i]);
    }
    output << totalDistance << " " << totalRisk << "\n";
    for (int i = 0; i < solution.size(); i++)
    {
        for (int j = 0; j < solution[i].size(); j++)
        {
            output << _nodes[solution[i][j]] << " ";
        }
        output << get<0>(evaluations[i]) << " ";
        output << get<1>(evaluations[i]) << "\n";
    }

    newOutput << totalDistance << " " << totalRisk << "\n";
    output << "\n";
    output.close();
    newOutput.close();

}

vector<vector<int>> generateRandomSolution()
{
    vector<int> unvisitedNodes;
    for (int i = 1; i < _nNodes; i++)
    {
        unvisitedNodes.push_back(i);
    }
    shuffle(begin(unvisitedNodes), end(unvisitedNodes), rng);
    vector<vector<int>> solution;
    for (int truck = 0; truck < _nTrucks; truck++)
    {
        vector<int> truckRoute;
        vector<bool> routeTypes{false, false, false, false, false};
        truckRoute.push_back(0);
        int production = 0;
        for (vector<int>::iterator it = unvisitedNodes.begin(); it != unvisitedNodes.end();)
        {
            int nodeType = _types[*it];
            if (checkValidWasteTypes(routeTypes, nodeType) && production + _productions[*it] <= _capacities[truck])
            {
                truckRoute.push_back(*it);
                production += _productions[*it];
                routeTypes[nodeType] = true;
                unvisitedNodes.erase(it);
            }
            else
            {
                it++;
            }
        }
        truckRoute.push_back(0);
        solution.push_back(truckRoute);
    }
    return solution;
}

//swap 2 nodes from the same truck route
void swapNodesFromRoute(vector<vector<int>> &solution)
{
    uniform_int_distribution<int> randomTruckDistribution(0, solution.size() - 1);
    int randomTruck = randomTruckDistribution(rng);
    int randomTruckIndex;
    int routeSize;
    for (int i = 0; i < solution.size(); i++)
    {
        randomTruckIndex = (i + randomTruck) % solution.size();
        routeSize = solution[randomTruckIndex].size();
        if (routeSize > 3)
        {
            uniform_int_distribution<int> randomNodeDistribution(0, routeSize - 3);
            int randomNode = randomNodeDistribution(rng);
            int randomNode2 = randomNodeDistribution(rng);
            if (randomNode == randomNode2)
            {
                randomNode2 = (randomNode2 + 1) % (routeSize - 2);
            }
            int aux = solution[randomTruckIndex][randomNode + 1];
            solution[randomTruckIndex][randomNode + 1] = solution[randomTruckIndex][randomNode2 + 1];
            solution[randomTruckIndex][randomNode2 + 1] = aux;
            return;
        }
    }
}

//Change a node to another truck route
/* void changeNodeRoute(vector<vector<int>> &solution)
{
    uniform_int_distribution<int> randomTruckDistribution(0, solution.size() - 1);
    int randomTruck = randomTruckDistribution(rng);
    int randomTruckIndex;
    int routeSize;
    for (int i = 0; i < solution.size(); i++)
    {
        randomTruckIndex = (i + randomTruck) % solution.size();
        routeSize = solution[randomTruckIndex].size();
        if (routeSize > 2)
        {
            int randomTruck2 = randomTruckDistribution(rng);
            uniform_int_distribution<int> randomNodeDistribution(0, routeSize - 3);
            int randomNode = randomNodeDistribution(rng);

            int aux = solution[randomTruckIndex][randomNode + 1];

            uniform_int_distribution<int> randomNodeDistribution2(0, solution[randomTruck2].size() - 3);
            int randomNode2 = randomNodeDistribution2(rng);
            solution[randomTruck2].insert(solution[randomTruck2].begin()+randomNode2+1, aux);

            solution[randomTruckIndex].erase(solution[randomTruckIndex].begin() + randomNode + 1);
            return;
        }
    }
} */
void changeNodeRoute(vector<vector<int>> &solution)
{
    uniform_int_distribution<int> randomTruckDistribution(0, solution.size() - 1);
    while(true){
        int fromTruck = randomTruckDistribution(rng);
        int toTruck = randomTruckDistribution(rng);
        if (solution[fromTruck].size() <= 2){
            continue;
        }
        vector<bool> routeTypes{false, false, false, false, false};
        int production = 0;
        for (vector<int>::iterator it = solution[toTruck].begin(); it != solution[toTruck].end();)
        {
            int nodeType = _types[*it];
            routeTypes[nodeType] = true;
            production += _productions[*it];
            it++;
        }
        uniform_int_distribution<int> randomNodeDistribution(1, solution[fromTruck].size() - 2);
        uniform_int_distribution<int> randomPositionDistribution(1, solution[toTruck].size() - 1);
        int randomNode = randomNodeDistribution(rng);
        if (!checkValidWasteTypes(routeTypes, _types[solution[fromTruck][randomNode]]) ||  production + _productions[solution[fromTruck][randomNode]] > _capacities[toTruck])
        {
            continue;
        }

        int randomPosition = randomPositionDistribution(rng);
        int aux = solution[fromTruck][randomNode];
        solution[fromTruck].erase(solution[fromTruck].begin() + randomNode);
        if(fromTruck == toTruck && randomNode < randomPosition){
            randomPosition--;
        }
        solution[toTruck].insert(solution[toTruck].begin()+randomPosition, aux);
        return;
    }
}

//do a random mutation
void mutate(vector<vector<int>> &solution)
{
    uniform_real_distribution<float> randomMutationDistribution(0, 1);
    float randomMutation = randomMutationDistribution(rng);
    if (randomMutation <= mutationChance)
    {
        swapNodesFromRoute(solution);
    }
}

int findNodeConnection(int node, vector<vector<int>> solution)
{
    for (int i = 0; i < solution.size(); i++)
    {
        for (int j = 0; j < solution[i].size(); j++)
        {
            if (solution[i][j] == node)
            {
                return solution[i][j + 1];
            }
        }
    }
    return -1;
}

bool find(vector<int> v, int value)
{
    for (vector<int>::iterator iter = v.begin(); iter != v.end(); ++iter)
    {
        if (*iter == value)
        {
            return true;
        }
    }
    return false;
}

bool erase(int value, vector<int> &v)
{
    for (vector<int>::iterator iter = v.begin(); iter != v.end(); ++iter)
    {
        if (*iter == value)
        {
            v.erase(iter);
            return true;
        }
    }
    return false;
}

int findRandomValidNode(vector<int> nodes, vector<bool> types, int availableCapacity)
{
    shuffle(begin(nodes), end(nodes), rng);
    for (int i = 0; i < nodes.size(); i++)
    {
        if (checkValidWasteTypes(types, _types[nodes[i]]) && availableCapacity >= _productions[nodes[i]])
        {
            return nodes[i];
        }
    }
    return -1;
}

bool checkEqualRoute(vector<int> route1, vector<int> route2){
    if(route1.size()!=route2.size()){
        return false;
    }
    for(int i=0;i<route1.size();i++){
        if(route1[i]!=route2[i]){
            return false;
        }
    }
    return true;
}

bool checkEqualSolutions(vector<vector<int>> solution1, vector<vector<int>> solution2){
    for(int i=0;i<_nTrucks;i++){
        bool equalRoute=false;
        for(int j=0;j<_nTrucks;j++){
            if(checkEqualRoute(solution1[i],solution2[j])){
                equalRoute=true;
                break;
            }
        }
        if(!equalRoute){
            return false;
        }
    }
    return true;
}

//Alternating edge crossover
//Constructs the child by alternating arcs from parents
vector<vector<int>> AEX(vector<vector<int>> parent1, vector<vector<int>> parent2)
{
    vector<vector<int>> child;
    vector<vector<bool>> childRouteTypes;
    vector<int> availableCapacities;
    bool turn = false;
    vector<int> unvisitedNodes;
    for (int i = 1; i < _nNodes; i++)
    {
        unvisitedNodes.push_back(i);
    }

    for (int truck = 0; truck < _nTrucks; truck++)
    {
        vector<int> route{0};
        vector<bool> routeTypes{false, false, false, false, false};
        child.push_back(route);
        childRouteTypes.push_back(routeTypes);
        availableCapacities.push_back(_capacities[truck]);
    }

    //printVector(parent1[0]);

    for (int truck = 0; truck < _nTrucks; truck++)
    {
        while (true)
        {
            int newNode;
            if (turn) //Finds the node on the current parent that connects to the current child node
            {
                newNode = findNodeConnection(child[truck].back(), parent1);
                //cout << "\n" << child[truck].back() << "\n" << newNode << "\n";
                //printVector(child[truck]);
            }
            else
            {
                newNode = findNodeConnection(child[truck].back(), parent2);
                //cout << "\n" << child[truck].back() << "\n" << newNode << "\n";
                //printVector(child[truck]);
            }
            if ( //If node is already visited or the node is not valid, a random valid node is selected
                !find(unvisitedNodes, newNode) ||
                !checkValidWasteTypes(childRouteTypes[truck], _types[newNode]) ||
                availableCapacities[truck] < _productions[newNode])
            {
                newNode = findRandomValidNode(unvisitedNodes, childRouteTypes[truck], availableCapacities[truck]);
                
            }
            if (newNode == -1)
            { //no valid node found
                break;
            }
            erase(newNode, unvisitedNodes);
            childRouteTypes[truck][_types[newNode]] = true;
            availableCapacities[truck] -= _productions[newNode];
            child[truck].push_back(newNode);
            turn = !turn; //Swap parent turn for the next iteration
        }
    }

    /* while (unvisitedNodes.size() > 0)
    {
        for (int truck = 0; truck < _nTrucks; truck++)//CAMBIAR A 1 CAMION A LA VES
        {
            int newNode;
            if (turn) //Finds the node on the current parent that connects to the current child node
            {
                newNode = findNodeConnection(*child[truck].end(), parent1);
            }
            else
            {
                newNode = findNodeConnection(*child[truck].end(), parent2);
            }
            if ( //If node is already visited or the node is not valid, a random valid node is selected
                !find(unvisitedNodes, newNode) ||
                !checkValidWasteTypes(childRouteTypes[truck], _types[newNode]) ||
                availableCapacities[truck] < _productions[newNode])
            {
                newNode = findRandomValidNode(unvisitedNodes, childRouteTypes[truck], availableCapacities[truck]);
            }
            if(newNode==-1){//no valid node found

                continue;
            }
            std::cout << newNode;
            erase(newNode, unvisitedNodes);
            childRouteTypes[truck][_types[newNode]] = true;
            availableCapacities[truck] -= _productions[newNode];
            child[truck].push_back(newNode);
            turn = !turn; //Swap parent turn for the next iteration
        }
    } */

    for (int truck = 0; truck < _nTrucks; truck++)
    {
        child[truck].push_back(0);
    }
    return child;
}

char* getCmdOption(char ** begin, char ** end, const std::string & option)
{
    char ** itr = std::find(begin, end, option);
    if (itr != end && ++itr != end)
    {
        return *itr;
    }
    return 0;
}

bool cmdOptionExists(char** begin, char** end, const std::string& option)
{
    return std::find(begin, end, option) != end;
}

vector<vector<int>> hillClimbing(vector<vector<int>> solution, int iterations){
    vector<vector<int>> bestSolution = solution;
    vector<vector<int>> newSolution = solution;
    
    for(int i=0; i<=iterations; i++){
        swapNodesFromRoute(newSolution);
        tuple<int, int> bestScore = evaluateSolution(bestSolution);
        tuple<int, int> newScore = evaluateSolution(newSolution);
        if(get<0>(newScore)+get<1>(newScore) < get<0>(bestScore)+get<1>(bestScore)){
            bestSolution = newSolution;
        } else {
            newSolution = bestSolution;
        }
    }
    return bestSolution;
}

//Calculates perimeter of the cuboid between two points
int calculateCrowdingDistance(double x1, double y1, double x2, double y2) {
    return (abs(x2-x1)+abs(y2-y1))*2;
}

//Calculate crowding distance and order a single front solutions by it
vector<int> orderByCrowdingDistance(vector<int> solutions, vector<int> distanceScores, vector<int> riskScores){
    vector<int> crowdingDistances;
    vector<int> orderedSolutions;
    for (int i=0; i < solutions.size(); i++){
        // Coordinate x represents distance, coordinate y represents risk.
        // Because all solutions are on the same front, the closest solutions on 
        // all objectives are the same as the closest on a single objective.
        int neighbour_1_x;
        int neighbour_1_y;
        int neighbour_2_x;
        int neighbour_2_y;
        int closest_score_top = 1000000000; // infinite
        int closest_score_bottom = 0;
        int solution_score = distanceScores[solutions[i]];
        for(int j=0; j<solutions.size(); j++){
            if(j==i) {
                continue;
            }
            int current_score = distanceScores[solutions[j]];
            if(current_score <= closest_score_top && current_score >= solution_score) {
                closest_score_top = current_score;
                neighbour_1_x = distanceScores[solutions[j]];
                neighbour_1_y = riskScores[solutions[j]];
            }
            if (current_score >= closest_score_bottom && current_score <= solution_score) { 
                closest_score_bottom = current_score;
                neighbour_2_x = distanceScores[solutions[j]];
                neighbour_2_y = riskScores[solutions[j]];
            }
        }
        if (closest_score_top == 1000000000) {
            neighbour_1_x = distanceScores[solutions[i]];
            neighbour_1_y = riskScores[solutions[i]]; 
            neighbour_2_x *= 2;
            neighbour_2_y *= 2;
        }
        if (closest_score_bottom == 0) {
            neighbour_2_x = distanceScores[solutions[i]];
            neighbour_2_y = riskScores[solutions[i]]; 
            neighbour_1_x *= 2;
            neighbour_1_y *= 2;
        }
        int crowdingDistance = calculateCrowdingDistance(neighbour_1_x, neighbour_1_y, neighbour_2_x, neighbour_2_y);
        if(i==0){
            crowdingDistances.push_back(crowdingDistance);
            orderedSolutions.push_back(solutions[i]);          
        } else {
            int size = crowdingDistances.size();
            bool inserted = false;
            for(int k=0; k<size; k++) {
                if(crowdingDistances[k]>crowdingDistance){
                    crowdingDistances.insert(crowdingDistances.begin()+k,crowdingDistance);
                    orderedSolutions.insert(orderedSolutions.begin()+k,solutions[i]);
                    inserted = true;
                    break; 
                }
            }
            if(!inserted){
                crowdingDistances.push_back(crowdingDistance);
                orderedSolutions.push_back(solutions[i]);   
            }
        }
    }
    return orderedSolutions;
}

//main
int main(int argc, char *argv[])
{   
    if(cmdOptionExists(argv, argv+argc, "-fp"))
    {
        filePath=getCmdOption(argv, argv+argc, "-fp");
    }
    if(cmdOptionExists(argv, argv+argc, "-ps"))
    {
        populationSize=stoi(getCmdOption(argv, argv+argc, "-ps"));
    }
    if(cmdOptionExists(argv, argv+argc, "-g"))
    {
        generations=stoi(getCmdOption(argv, argv+argc, "-g"));
    }    
    if(cmdOptionExists(argv, argv+argc, "-ts"))
    {
        tournamentSize=stoi(getCmdOption(argv, argv+argc, "-ts"));
    }    
    if(cmdOptionExists(argv, argv+argc, "-mc"))
    {
        mutationChance=stoi(getCmdOption(argv, argv+argc, "-mc"));
    }    
    if(cmdOptionExists(argv, argv+argc, "-s"))
    {
        seed=stoi(getCmdOption(argv, argv+argc, "-s"));
    }
    if(cmdOptionExists(argv, argv+argc, "-filePath"))
    {
        filePath=getCmdOption(argv, argv+argc, "-filePath");
    }
    if(cmdOptionExists(argv, argv+argc, "-populationSize"))
    {
        populationSize=stoi(getCmdOption(argv, argv+argc, "-populationSize"));
    }
    if(cmdOptionExists(argv, argv+argc, "-generations"))
    {
        generations=stoi(getCmdOption(argv, argv+argc, "-generations"));
    }    
    if(cmdOptionExists(argv, argv+argc, "-tournamentSize"))
    {
        tournamentSize=stoi(getCmdOption(argv, argv+argc, "-tournamentSize"));
    }    
    if(cmdOptionExists(argv, argv+argc, "-mutationChance"))
    {
        mutationChance=stoi(getCmdOption(argv, argv+argc, "-mutationChance"));
    }    
    if(cmdOptionExists(argv, argv+argc, "-seed"))
    {
        seed=stoi(getCmdOption(argv, argv+argc, "-seed"));
    }
    if(cmdOptionExists(argv, argv+argc, "-hc"))
    {
        hillClimbingIterations=stoi(getCmdOption(argv, argv+argc, "-hc"));
    }
    rng.seed(seed);
    readData();
    vector<vector<vector<int>>> population;
    vector<int> distanceScores;
    vector<int> riskScores;
    uniform_int_distribution<int> randomSolutionDistribution(0, populationSize/2 - 1);

    //Create initial population
    cout << "Generating initial population \n";
    int repeatedSolutions = 0;
    for (int i = 0; i < populationSize; i++)
    {
        if(repeatedSolutions != 0 && repeatedSolutions%1000 == 0){
            cout << repeatedSolutions << " repeated solutions. Population " << i << "/" << populationSize << ". consider setting a lower amount of HC iterations.\n";
        } 
        vector<vector<int>> solution = generateRandomSolution();
        solution = hillClimbing(solution, hillClimbingIterations);
        tuple<int, int> score = evaluateSolution(solution);
        bool repeatedSolution = false;
        for(int j=0; j<population.size();j++){
            if(checkEqualSolutions(solution, population[j])){
                repeatedSolution=true;
                break;
            }
        }
        if(repeatedSolution){
            i--;
            repeatedSolutions++;
            continue;
        }
        distanceScores.push_back(get<0>(score));
        riskScores.push_back(get<1>(score));
        population.push_back(solution);
    }

    for (int gen = 0; gen < generations; gen++)
    {
        vector<vector<vector<int>>> newPopulation;
        vector<int> newDistanceScores;
        vector<int> newRiskScores;
        vector<vector<int>> dominatedSolutions; //list of solutions dominated by each solution
        vector<int> dominationCount;            //count of solutions that dominate each solution
        if(gen%10==0){
            std::cout << "\nGeneration " << gen << "\n";
        }
        ////////Selection step/////////
        vector<int> firstFront;
        //Calculate domination count and dominated solutions for each solution O(mn^2)
        for (int i = 0; i < populationSize; i++)
        {
            vector<int> dominated;
            dominatedSolutions.push_back(dominated);
            dominationCount.push_back(0);
            for (int j = 0; j < populationSize; j++)
            {
                if (i == j)
                {
                    continue;
                }
                if ((riskScores[i] <= riskScores[j] && distanceScores[i] < distanceScores[j])
                    || (riskScores[i] < riskScores[j] && distanceScores[i] <= distanceScores[j])) //i dominates j
                {
                    dominatedSolutions[i].push_back(j);
                }
                else if ((riskScores[i] >= riskScores[j] && distanceScores[i] > distanceScores[j])
                    || (riskScores[i] > riskScores[j] && distanceScores[i] >= distanceScores[j])) //j dominates i
                {
                    dominationCount[i]++;
                }
            }
            if (dominationCount[i] == 0)
            {
                if(gen==generations-1){                       
                    writeSolution(population[i]);
                }
                firstFront.push_back(i);
            }
        }

        //Select best solutions for the next generation O(mn^2)
        vector<int> orderedSolutions;
        bool isFirstFront=true;
        while (firstFront.size() > 0)
        {
            vector<int> nextFront;
            if(gen%10==0 && isFirstFront){
                cout << "First front size: " << firstFront.size() << "\n";
            }
            //printVector(firstFront);
            //printVector(dominationCount);

            // Order first front by crowding distance
            firstFront = orderByCrowdingDistance(firstFront, distanceScores, riskScores);

            for (int i = 0; i < firstFront.size(); i++)
            {
                int currentSolution = firstFront[i];
                orderedSolutions.push_back(currentSolution);
                dominationCount[currentSolution] = -1; //flag to ignore it
                if(gen%10==0 && isFirstFront){
                    cout<<distanceScores[currentSolution]<<" ";
                    cout<<riskScores[currentSolution]<<"\n";
                }
                //cout << "Current solution:" << currentSolution << "\n";
                //cout << "Dominated solutions size: " << dominatedSolutions[currentSolution].size() << "\n";


                for (int j = 0; j < dominatedSolutions[currentSolution].size(); j++)
                {
                    int currentDominatedSolution = dominatedSolutions[currentSolution][j];
                    dominationCount[currentDominatedSolution]--;
                    if (dominationCount[currentDominatedSolution] == 0)
                    {
                        nextFront.push_back(currentDominatedSolution);
                    }
                }
            }
            firstFront = nextFront;
            isFirstFront=false;
        }

        for (int i = 0; i < populationSize / 2; i++)
        {
            newPopulation.push_back(population[orderedSolutions[i]]);
            tuple<int, int> score = evaluateSolution(population[orderedSolutions[i]]);
            newDistanceScores.push_back(get<0>(score));
            newRiskScores.push_back(get<1>(score));
        }

        for (int i = 0; i < populationSize / 2; i++)//Select the 2 best solutions of the tournament to generate a child
        {
            int randomSolution = randomSolutionDistribution(rng);
            int bestSolution=randomSolution;
            int secondBestSolution;

            randomSolution = randomSolutionDistribution(rng);
            if(randomSolution<bestSolution){
                secondBestSolution=bestSolution;
                bestSolution=randomSolution;
            } else{
                secondBestSolution=randomSolution;
            }

            for(int j=0;j<tournamentSize-3;j++){
                randomSolution = randomSolutionDistribution(rng);
                if(randomSolution<bestSolution){
                    secondBestSolution=bestSolution;
                    bestSolution=randomSolution;
                } else{
                    secondBestSolution=randomSolution;
                }
            }
            vector<vector<int>> newSolution=AEX(population[bestSolution],population[secondBestSolution]);
            //vector<vector<int>> newSolution=population[bestSolution];
            mutate(newSolution);
            newSolution = hillClimbing(newSolution, hillClimbingIterations);
            bool repeatedSolution=false;
            for(int j=0; j<newPopulation.size();j++){
                if(checkEqualSolutions(newSolution, newPopulation[j])){
                    repeatedSolution=true;
                    break;
                }
            }
            if(repeatedSolution){
                i--;
                continue;
            }
            newPopulation.push_back(newSolution);
            tuple<int, int> score = evaluateSolution(newSolution);
            newDistanceScores.push_back(get<0>(score));
            newRiskScores.push_back(get<1>(score));
        }

        riskScores = newRiskScores;
        distanceScores = newDistanceScores;
        population = newPopulation;
    }

    return 0;
}