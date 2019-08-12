import sys

capacities = {} #capacitites of trucks
nodes = {} #names of nodes
productions = {} #productions of nodes
types = {} #types of hazardous materials of nodes

def max(ctype, ntype):
    #print "ctype:" + str(ctype) + " and ntype: "  + str(ntype) 
    if (ctype == ntype) or (ntype == '-'): #does not change the type
        return ctype
    
    if (ctype == 'A' and ntype == 'B') or (ctype == 'B' and ntype == 'A') or (ctype == 'A' and ntype == 'E') or (ctype == 'E' and ntype == 'A'): #incompatible types
        print 'Error! Incompatible nodes in the same route. Route Type: ' + ctype + ' versus node type: ' + ntype + '.\n\n'
        sys.exit()
        
    
    if ntype > ctype: #If new type is higher than old type, update
        return ntype
    else: #Else, keep current type
        return ctype
    
def main():
    #python chequeador.py peligro-mezcla4-min-riesgo-zona7-2k-AE.2.hazmat solucion-mezcla4-min-riesgo-zona7-2k-AE.2.hazmat
    
    fin1=open(sys.argv[1], 'r')
    
    ntrucks=int(fin1.readline().strip('\n'))
    #print 'ntrucks:' + str(ntrucks)
    #fout.write(str(ntrucks) + "\n")
    
    line=fin1.readline()
    #print line
    tr = line.split() 
    i=0
    for t in tr:
        capacities.update({i:int(t)})
        i=i+1
    
    #print 'capacities:' 
    #print capacities
        
    nnodes=int(fin1.readline().strip('\n'))
    
    visited=[]
    for i in range(nnodes):
        visited.append(0) #zeroes for unvisited nodes.
        
    #print 'nnodes:' + str(nnodes)
    #fout.write(str(nnodes) + "\n")

    i=0
    while (i < nnodes):
        values=fin1.readline().split()
        nodes.update({i:int(values[0])})
        productions.update({i:int(values[1])})
        types.update({i:(values[2])})
        i=i+1

    #print 'nodes:'
    #print nodes
    #print productions
    #print types
        
    distance  = [0 for x in range(nnodes)]    
    distanceA = [[0 for x in range(nnodes)] for y in range(nnodes)] 
    distanceB = [[0 for x in range(nnodes)] for y in range(nnodes)] 
    distanceC = [[0 for x in range(nnodes)] for y in range(nnodes)] 
    distanceD = [[0 for x in range(nnodes)] for y in range(nnodes)] 
    distanceE = [[0 for x in range(nnodes)] for y in range(nnodes)] 

    #Distance from depot to nodes with an empty truck
    line = fin1.readline()
    #print line
    di = line.split() 
    i=0
    for d in di:
        distance[i] = int(d)
        i = i+1
        
    #print 'distance: '
    #print distance

    #Leer matrices de distancias entre nodos considerando material peligroso
    i=0
    while (i < (nnodes)):
        line = fin1.readline()
        di = line.split()
        j=0
        for d in di:
            distanceA[i][j] = int(d)
            j=j+1 
        i=i+1

    i=0
    while (i < (nnodes)):
        line = fin1.readline()
        di = line.split()
        j=0
        for d in di:
            distanceB[i][j] = int(d)
            j=j+1 
        i=i+1
 
    i=0
    while (i < (nnodes)):
        line = fin1.readline()
        di = line.split()
        j=0
        for d in di:
            distanceC[i][j] = int(d)
            j=j+1 
        i=i+1

    i=0
    while (i < (nnodes)):
        line = fin1.readline()
        di = line.split()
        j=0
        for d in di:
            distanceD[i][j] = int(d)
            j=j+1 
        i=i+1

    i=0
    while (i < (nnodes)):
        line = fin1.readline()
        di = line.split()
        j=0
        for d in di:
            distanceE[i][j] = int(d)
            j=j+1 
        i=i+1

    
    #Leer matrices de riesgo
    riskA = [[0 for x in range(nnodes)] for y in range(nnodes)] 
    riskB = [[0 for x in range(nnodes)] for y in range(nnodes)] 
    riskC = [[0 for x in range(nnodes)] for y in range(nnodes)] 
    riskD = [[0 for x in range(nnodes)] for y in range(nnodes)] 
    riskE = [[0 for x in range(nnodes)] for y in range(nnodes)] 

     #Leer matrices de riesgos entre nodos considerando material peligroso
    i=0
    while (i < (nnodes)):
        line = fin1.readline()
        ri = line.split()
        j=0
        for r in ri:
            riskA[i][j] = int(r)
            j=j+1 
        i=i+1

    i=0
    while (i < (nnodes)):
        line = fin1.readline()
        ri = line.split()
        j=0
        for r in ri:
            riskB[i][j] = int(r)
            j=j+1 
        i=i+1

    i=0
    while (i < (nnodes)):
        line = fin1.readline()
        ri = line.split()
        j=0
        for r in ri:
            riskC[i][j] = int(r)
            j=j+1 
        i=i+1

    i=0
    while (i < (nnodes)):
        line = fin1.readline()
        ri = line.split()
        j=0
        for r in ri:
            riskD[i][j] = int(r)
            j=j+1 
        i=i+1

    i=0
    while (i < (nnodes)):
        line = fin1.readline()
        ri = line.split()
        j=0
        for r in ri:
            riskE[i][j] = int(r)
            j=j+1 
        i=i+1
   
    #Solution reading
    fin2 = open(sys.argv[2], 'r')
    
    line = fin2.readline()
    total_distance, total_risk = line.split(" ")
    #print 'total_distance: ' + str(total_distance)
    #print 'total_risk: ' + str(total_risk)
    
    computed_total_distance = 0
    computed_total_risk = 0
    
    #Routes revision
    i=0
    while (i < ntrucks):
        line = fin2.readline()
        
        print 'Route: ' + str(i)
        route = line.split() 
        #Routes format
        if (int(route[0]) != int(nodes.values()[0])) or (int(route[len(route)-3]) != int(nodes.values()[0])):
            print 'Comparing: ' + str(nodes.values()[0]) + ' with: ' + str(route[0]) + ' and ' + route[len(route)-3]
            print 'Error! Route ' + str(i) + ' does not accomplish the output requirements. First and last node must be the depot id:' + str(nodes.values()[0]) + ' .\n\n'
            sys.exit()
            
        computed_distance = distance[int(nodes.values().index(int(route[1])))] #Distance of the first trip with the truck empty
        #print 'computed_distance: ' + str(computed_distance)
        computed_risk = 0 #Risk of the first trip with the truck empty
        #print 'computed_risk: ' + str(computed_risk)
        route_type = types.values()[int(nodes.values().index(int(route[1])))] #Type of the route
        #print 'route_type: ' + str(route_type)
        computed_production = productions.values()[int(nodes.values().index(int(route[1])))] #Production of the first visited node
        #print 'Chequeando el nodo: ' + str(int(nodes.values().index(int(route[1]))))
        
        #if visited[int(nodes.values().index(int(route[1])))]: #Nodes were visited just once.
            #print 'Error! Route ' + str(i) + ' node ' + str(route[1]) + ' was already visited.\n\n'
            #sys.exit()
        #else:
            #visited[int(nodes.values().index(int(route[1])))] = 1
            
        for r in range(1,len(route)-3):
            
            if visited[int(nodes.values().index(int(route[r])))]: #Nodes were visited just once.
                print 'Error! Route ' + str(i) + ' node ' + str(route[r]) + ' was already visited.\n\n'
                sys.exit()
            else:
                visited[int(nodes.values().index(int(route[r])))] = 1
            
            computed_production += productions.values()[int(nodes.values().index(int(route[r])))] #Production of the visited node

            #print 'from: ' + str(route[r]) + ' to: ' + str(route[r+1])
            #print ' transporting material type: ' + str(route_type)
            if route_type == 'A':
                #print ' Added distance: ' + str(distanceA[int(nodes.values().index(int(route[r])))][int(nodes.values().index(int(route[r+1])))])
                #print ' Added risk: ' + str(riskA[int(nodes.values().index(int(route[r])))][int(nodes.values().index(int(route[r+1])))])
                computed_distance += distanceA[int(nodes.values().index(int(route[r])))][int(nodes.values().index(int(route[r+1])))]
                computed_risk += riskA[int(nodes.values().index(int(route[r])))][int(nodes.values().index(int(route[r+1])))]
            else: 
                if route_type == 'B':
                    #print ' Added distance: ' + str(distanceB[int(nodes.values().index(int(route[r])))][int(nodes.values().index(int(route[r+1])))])
                    #print ' Added risk: ' + str(riskB[int(nodes.values().index(int(route[r])))][int(nodes.values().index(int(route[r+1])))])
                    computed_distance += distanceB[int(nodes.values().index(int(route[r])))][int(nodes.values().index(int(route[r+1])))]
                    computed_risk += riskB[int(nodes.values().index(int(route[r])))][int(nodes.values().index(int(route[r+1])))]
                else:
                    if route_type == 'C':
                        #print ' Added distance: ' + str(distanceC[int(nodes.values().index(int(route[r])))][int(nodes.values().index(int(route[r+1])))])
                        #print ' Added risk: ' + str(riskC[int(nodes.values().index(int(route[r])))][int(nodes.values().index(int(route[r+1])))])
                        computed_distance += distanceC[int(nodes.values().index(int(route[r])))][int(nodes.values().index(int(route[r+1])))]
                        computed_risk += riskC[int(nodes.values().index(int(route[r])))][int(nodes.values().index(int(route[r+1])))]        
                    else:
                        if route_type == 'D':
                            #print ' Added distance: ' + str(distanceD[int(nodes.values().index(int(route[r])))][int(nodes.values().index(int(route[r+1])))])
                            #print ' Added risk: ' + str(riskD[int(nodes.values().index(int(route[r])))][int(nodes.values().index(int(route[r+1])))])
                            computed_distance += distanceD[int(nodes.values().index(int(route[r])))][int(nodes.values().index(int(route[r+1])))]
                            computed_risk += riskD[int(nodes.values().index(int(route[r])))][int(nodes.values().index(int(route[r+1])))]        
                        else:
                            if route_type == 'E':
                                #print ' Added distance: ' + str(distanceE[int(nodes.values().index(int(route[r])))][int(nodes.values().index(int(route[r+1])))])
                                #print ' Added risk: ' + str(riskE[int(nodes.values().index(int(route[r])))][int(nodes.values().index(int(route[r+1])))])
                                computed_distance += distanceE[int(nodes.values().index(int(route[r])))][int(nodes.values().index(int(route[r+1])))]
                                computed_risk += riskE[int(nodes.values().index(int(route[r])))][int(nodes.values().index(int(route[r+1])))]        
                            else:
                                print 'Error! Unrecognized material type or incompatible materials: ' + str(route_type) + ' .\n\n'
                                sys.exit()
                                
            route_type = max(route_type, types.values()[int(nodes.values().index(int(route[r+1])))])
            #print 'route_type updated to: ' + str(route_type) + '\n'
                
        if computed_distance != int(route[len(route)-2]):
            print 'Error! Distance does not correspond to the output information: ' + str(computed_distance) + ' versus: ' + route[len(route)-2] + ' in route: ' + str(i) + ' .\n\n'
            sys.exit()
        if computed_risk != int(route[len(route)-1]):
            print 'Error! Total risk does not correspond to the output information: ' + str(computed_risk) + ' versus: ' + route[len(route)-1] + ' in route: ' + str(i) + ' .\n\n'
            sys.exit()
        if computed_production > int(capacities[i]):
            print 'Error! Total production collected: ' + str(computed_production) + ' surpases the truck capacity: ' + str(capacities[i]) + ' in route: ' + str(i) + ' .\n\n'
            sys.exit()
            
        computed_total_distance += int(computed_distance)
        print 'Computed distance: ' + str(computed_total_distance)
        computed_total_risk += int(computed_risk)
        print 'Computed cost: ' + str(computed_total_risk)
        print 'Computed production collected: ' + str(computed_production) + '/' + str(capacities[i])
        
        i=i+1

    #All nodes were visited.
    for j in range(1, nnodes): #Depot is not considered
        if visited[j] != 1:
            print 'Error! Node ' + str(nodes.values()[j]) + ' was not visited.\n\n'
            sys.exit()
   
    if int(computed_total_distance) != int(total_distance):
        print 'Error! Total distance does not correspond to the output information: ' + str(computed_total_distance) + ' versus: ' + str(total_distance) + '.\n\n'
        sys.exit()
    if int(computed_total_risk) != int(total_risk):
        print 'Error! Total risk does not correspond to the output information: ' + str(computed_total_risk) + ' versus: ' + str(total_risk) + '.\n\n'
        sys.exit()
    
    fin1.close()
    fin2.close()
    
if __name__ == "__main__":
    main();
