#include <iostream>
#include <vector>
#include <ctime>
#include <algorithm>
#define defaultNumberOfInputNodes 3
#define defaultNumberOfOutputNodes 2
#define defaultMutationRate 0.1
#define defaultMutationAmplitude 0.1
#define mutationFactor 0.01
#define populationExposurePercent 0.2
#define agentsKeptPercent 0.2
#define speciesKeptPercent 0.2

using std::cout;
using std::endl;
using std::max;
using std::vector;

// TODO:
// - species stability function
// - batch and environment

double randDouble()
{
    return (double)rand() / RAND_MAX;
}

class componentNode
{
public:
    int numberOfWeights;
    bool currentlyActive;
    double defaultMemoryMutationAmplitude;
    double biasMutationAmplitude;
    double weightMutationAmplitude;
    double defaultMemory;
    double memory;
    double bias;
    vector<double> weights;

    componentNode()
    {
        numberOfWeights = 0;
        currentlyActive = true;
        defaultMemoryMutationAmplitude = defaultMutationAmplitude;
        biasMutationAmplitude = defaultMutationAmplitude;
        weightMutationAmplitude = defaultMutationAmplitude;
        defaultMemory = 0;
        memory = 0;
        bias = 0;
    }

    componentNode(componentNode *givenComponentNode)
    {
        numberOfWeights = givenComponentNode->numberOfWeights;
        currentlyActive = givenComponentNode->currentlyActive;
        defaultMemoryMutationAmplitude = givenComponentNode->defaultMemoryMutationAmplitude;
        biasMutationAmplitude = givenComponentNode->biasMutationAmplitude;
        weightMutationAmplitude = givenComponentNode->weightMutationAmplitude;
        defaultMemory = givenComponentNode->defaultMemory;
        memory = givenComponentNode->memory;
        bias = givenComponentNode->bias;
        for (int i = 0; i < numberOfWeights; i++)
        {
            weights.push_back(givenComponentNode->weights[i]);
        }
    }

    void info()
    {
        cout << "-----|numberOfWeights: " << numberOfWeights << endl;
        cout << "-----|currentlyActive: " << currentlyActive << endl;
        cout << "-----|defaultMemoryMutationAmplitude: " << defaultMemoryMutationAmplitude << endl;
        cout << "-----|biasMutationAmplitude: " << biasMutationAmplitude << endl;
        cout << "-----|weightMutationAmplitude: " << weightMutationAmplitude << endl;
        cout << "-----|defaultMemory: " << defaultMemory << endl;
        cout << "-----|memory: " << memory << endl;
        cout << "-----|bias: " << bias << endl;
        cout << "-----|weights: ";
        for (int i = 0; i < weights.size(); i++)
        {
            cout << weights[i] << " ";
        }
        cout << endl;
    }

    void mutate()
    {
        defaultMemory += (randDouble() * 2 - 1) * defaultMemoryMutationAmplitude;
        bias += (randDouble() * 2 - 1) * biasMutationAmplitude;
        for (int i = 0; i < numberOfWeights; i++)
        {
            weights[i] += (randDouble() * 2 - 1) * weightMutationAmplitude;
        }
        defaultMemoryMutationAmplitude += (randDouble() * 2 - 1) * mutationFactor;
        biasMutationAmplitude += (randDouble() * 2 - 1) * mutationFactor;
        weightMutationAmplitude += (randDouble() * 2 - 1) * mutationFactor;
    }
};

class structureNode
{
public:
    int numberOfConnections;
    double addNodeMutationRate;
    double deleteNodeMutationRate;
    double addConnectionMutationRate;
    double deleteConnectionMutationRate;
    vector<int> connections;

    structureNode()
    {
        numberOfConnections = 0;
        addNodeMutationRate = defaultMutationRate;
        deleteNodeMutationRate = defaultMutationRate;
        addConnectionMutationRate = defaultMutationRate;
        deleteConnectionMutationRate = defaultMutationRate;
    }

    structureNode(structureNode *givenStructureNode)
    {
        numberOfConnections = givenStructureNode->numberOfConnections;
        addNodeMutationRate = givenStructureNode->addNodeMutationRate;
        deleteNodeMutationRate = givenStructureNode->deleteNodeMutationRate;
        addConnectionMutationRate = givenStructureNode->addConnectionMutationRate;
        deleteConnectionMutationRate = givenStructureNode->deleteConnectionMutationRate;
        for (int i = 0; i < numberOfConnections; i++)
        {
            connections.push_back(givenStructureNode->connections[i]);
        }
    }

    void info()
    {
        cout << "---|numberOfConnections: " << numberOfConnections << endl;
        cout << "---|addNodeMutationRate: " << addNodeMutationRate << endl;
        cout << "---|deleteNodeMutationRate: " << deleteNodeMutationRate << endl;
        cout << "---|addConnectionMutationRate: " << addConnectionMutationRate << endl;
        cout << "---|deleteConnectionMutationRate: " << deleteConnectionMutationRate << endl;
        cout << "---|connections: ";
        for (int i = 0; i < numberOfConnections; i++)
        {
            cout << connections[i] << " ";
        }
        cout << endl;
    }

    void mutate()
    {
        addNodeMutationRate += (randDouble() * 2 - 1) * mutationFactor;
        deleteNodeMutationRate += (randDouble() * 2 - 1) * mutationFactor;
        addConnectionMutationRate += (randDouble() * 2 - 1) * mutationFactor;
        deleteConnectionMutationRate += (randDouble() * 2 - 1) * mutationFactor;
    }
};

class agent
{
public:
    int score;
    int numberOfComponentNodes;
    vector<structureNode *> *structureNodesPointer;
    vector<componentNode *> componentNodes;

    agent()
    {
        score = 0;
        numberOfComponentNodes = defaultNumberOfInputNodes + defaultNumberOfOutputNodes;
        structureNodesPointer = NULL;
        for (int i = 0; i < numberOfComponentNodes; i++)
        {
            componentNodes.push_back(new componentNode());
            if (i < defaultNumberOfInputNodes)
            {
                for (int j = 0; j < defaultNumberOfOutputNodes; j++)
                {
                    componentNodes[i]->weights.push_back(0);
                }
                componentNodes[i]->numberOfWeights = defaultNumberOfOutputNodes;
            }
            else
            {
                for (int j = 0; j < defaultNumberOfInputNodes; j++)
                {
                    componentNodes[i]->weights.push_back(0);
                }
                componentNodes[i]->numberOfWeights = defaultNumberOfInputNodes;
            }
            componentNodes[i]->mutate();
        }
        resetMemory();
    }

    agent(agent *givenAgent)
    {
        score = givenAgent->score;
        numberOfComponentNodes = givenAgent->numberOfComponentNodes;
        structureNodesPointer = givenAgent->structureNodesPointer;
        for (int i = 0; i < numberOfComponentNodes; i++)
        {
            componentNodes.push_back(new componentNode(givenAgent->componentNodes[i]));
        }
    }

    void erase()
    {
        for (int i = 0; i < numberOfComponentNodes; i++)
        {
            delete componentNodes[i];
        }
    }

    void info()
    {
        cout << "---|score: " << score << endl;
        cout << "---|numberOfComponentNodes: " << numberOfComponentNodes << endl;
        cout << "---|structureNodesPointer: " << structureNodesPointer << endl;
        cout << "---|componentNodes: " << endl;
        cout << "__________" << endl;
        for (int i = 0; i < numberOfComponentNodes; i++)
        {
            componentNodes[i]->info();
            cout << "__________" << endl;
        }
    }

    void mutate()
    {
        for (int i = 0; i < numberOfComponentNodes; i++)
        {
            componentNodes[i]->mutate();
        }
    }

    void resetMemory()
    {
        for (int i = 0; i < numberOfComponentNodes; i++)
        {
            componentNodes[i]->memory = componentNodes[i]->defaultMemory;
            componentNodes[i]->currentlyActive = true;
        }
    }

    vector<double> evaluateInput(vector<double> givenInputs)
    {
        int *futureMemory = new int[numberOfComponentNodes];
        bool *perviouslyActive = new bool[numberOfComponentNodes];
        vector<structureNode *> structureNodes = *structureNodesPointer;
        for (int i = 0; i < defaultNumberOfInputNodes; i++)
        {
            componentNodes[i]->memory += givenInputs[i];
        }
        for (int i = 0; i < numberOfComponentNodes; i++)
        {
            futureMemory[i] = 0;
            perviouslyActive[i] = componentNodes[i]->currentlyActive;
            componentNodes[i]->currentlyActive = false;
        }
        for (int i = 0; i < numberOfComponentNodes; i++)
        {
            if (perviouslyActive[i])
            {
                for (int j = 0; j < structureNodes[i]->numberOfConnections; j++)
                {
                    double sum = componentNodes[i]->memory * componentNodes[i]->weights[j];
                    if (sum >= 0)
                    {
                        int connectionNumber = structureNodes[i]->connections[j];
                        futureMemory[connectionNumber] += sum;
                        componentNodes[connectionNumber]->currentlyActive = true;
                    }
                }
            }
        }
        for (int i = 0; i < numberOfComponentNodes; i++)
        {
            if (componentNodes[i]->currentlyActive)
            {
                componentNodes[i]->memory = futureMemory[i] + componentNodes[i]->bias;
            }
        }
        delete[] perviouslyActive;
        delete[] futureMemory;
        vector<double> output;
        for (int i = defaultNumberOfInputNodes; i < defaultNumberOfInputNodes + defaultNumberOfOutputNodes; i++)
        {
            if (componentNodes[i]->currentlyActive)
            {
                output.push_back(componentNodes[i]->memory);
            }
            else
            {
                output.push_back(0);
            }
        }
        return output;
    }
};

class model
{
public:
    int numberOfStructureNodes;
    agent *agentRepresentative;
    vector<structureNode *> structureNodes;
    vector<agent *> agents;

    model()
    {
        numberOfStructureNodes = defaultNumberOfInputNodes + defaultNumberOfOutputNodes;
        agentRepresentative = NULL;
        for (int i = 0; i < numberOfStructureNodes; i++)
        {
            structureNodes.push_back(new structureNode());
            if (i < defaultNumberOfInputNodes)
            {
                for (int j = defaultNumberOfInputNodes; j < numberOfStructureNodes; j++)
                {
                    structureNodes[i]->connections.push_back(j);
                }
                structureNodes[i]->numberOfConnections = defaultNumberOfOutputNodes;
            }
            else
            {
                for (int j = 0; j < defaultNumberOfInputNodes; j++)
                {
                    structureNodes[i]->connections.push_back(j);
                }
                structureNodes[i]->numberOfConnections = defaultNumberOfInputNodes;
            }
            structureNodes[i]->mutate();
            agents.push_back(new agent());
            agents[i]->structureNodesPointer = &structureNodes;
        }
    }

    model(model *givenModel)
    {
        numberOfStructureNodes = givenModel->numberOfStructureNodes;
        agentRepresentative = givenModel->agentRepresentative;
        for (int i = 0; i < numberOfStructureNodes; i++)
        {
            structureNodes.push_back(new structureNode(givenModel->structureNodes[i]));
            agents.push_back(new agent(givenModel->agents[i]));
        }
    }

    void erase()
    {
        for (int i = 0; i < numberOfStructureNodes; i++)
        {
            delete structureNodes[i];
            agents[i]->erase();
            delete agents[i];
        }
    }

    void info()
    {
        cout << "-|numberOfStructureNodes: " << numberOfStructureNodes << endl;
        cout << "-|agentRepresentative: " << agentRepresentative << endl;
        cout << "-|structureNodes: " << endl;
        cout << "__________" << endl;
        for (int i = 0; i < numberOfStructureNodes; i++)
        {
            structureNodes[i]->info();
            cout << "__________" << endl;
        }
        cout << "-|agents: " << endl;
        cout << "__________" << endl;
        for (int i = 0; i < numberOfStructureNodes; i++)
        {
            agents[i]->info();
            cout << "__________" << endl;
        }
    }

    void shuffleAgents()
    {
        for (int i = 0; i < numberOfStructureNodes; i++)
        {
            int tempAgentIndex = rand() % numberOfStructureNodes;
            agent *tempAgent = agents[tempAgentIndex];
            agents[tempAgentIndex] = agents[i];
            agents[i] = tempAgent;
        }
    }

    void sortAgents()
    {
        sort(agents.begin(), agents.end(), [](agent *agent1, agent *agent2) {
            return (agent1->score > agent2->score);
        });
    }

    void selectionAndReplaceAgents()
    {
        int topPercent = max(1, int(numberOfStructureNodes * agentsKeptPercent));
        for (int i = topPercent; i < numberOfStructureNodes; i++)
        {
            agents[i]->erase();
            delete agents[i];
            agents[i] = new agent(agents[i % topPercent]);
            agents[i]->mutate();
        }
    }

    void setAgentRepresentative()
    {
        agentRepresentative = agents[0];
    }

    void diversifyAgentRepresentative()
    {
        for (int i = 0; i < numberOfStructureNodes; i++)
        {
            agents[i]->erase();
            delete agents[i];
            agents[i] = new agent(agentRepresentative);
            agents[i]->mutate();
        }
    }

    vector<vector<int>> getReverseStructure()
    {
        vector<vector<int>> reverseStructure;
        for (int i = 0; i < numberOfStructureNodes; i++)
        {
            reverseStructure.push_back({});
        }
        for (int i = 0; i < numberOfStructureNodes; i++)
        {
            for (int j = 0; j < structureNodes[i]->numberOfConnections; j++)
            {
                reverseStructure[structureNodes[i]->connections[j]].push_back(i);
            }
        }
        return reverseStructure;
    }

    void mutate()
    {
        vector<vector<int>> reverseStructure = getReverseStructure();
        int perviousNumberOfStructureNodes = numberOfStructureNodes;
        for (int i = 0; i < perviousNumberOfStructureNodes; i++)
        {
            addNode();
            addConnection();
            deleteNode();
            deleteConnection();
        }
        for (int i = 0; i < numberOfStructureNodes; i++)
        {
            structureNodes[i]->mutate();
        }
    }

    void addNode()
    {
        int chosenNode = rand() % numberOfStructureNodes;
        if (structureNodes[chosenNode]->addNodeMutationRate > randDouble())
        {
            int chosenConnection = rand() % structureNodes[chosenNode]->numberOfConnections;
            structureNodes.push_back(new structureNode());
            int chosenConenctionNode = structureNodes[chosenNode]->connections[chosenConnection];
            structureNodes[numberOfStructureNodes]->connections.push_back(chosenConenctionNode);
            structureNodes[chosenNode]->connections[chosenConnection] = numberOfStructureNodes++;
        }
    }

    void addConnection()
    {
        int chosenNode = rand() % numberOfStructureNodes;
        if (structureNodes[chosenNode]->addConnectionMutationRate > randDouble())
        {
            int secondChosenNode = rand() % numberOfStructureNodes;
            for (int i = 0; i < structureNodes[chosenNode]->numberOfConnections; i++)
            {
                if (structureNodes[chosenNode]->connections[i] == secondChosenNode)
                {
                    return;
                }
            }
            structureNodes[chosenNode]->connections.push_back(secondChosenNode);
            structureNodes[chosenNode]->numberOfConnections++;
        }
    }

    void deleteNode()
    {
        int chosenNode = rand() % numberOfStructureNodes;
    }

    void deleteConnection()
    {
        int chosenNode = rand() % numberOfStructureNodes;
    }
};

class environment
{
public:
};

class batch
{
public:
};

int main()
{
    srand(unsigned((time(NULL) % 9973 + 1) * (time(NULL) % 997 + 1) * (time(NULL) % 97 + 1) * (time(NULL) % 7 + 1)));

    model newModel;
    vector<double> output = newModel.agents[0]->evaluateInput({1, 2, 3});
    for (int i = 0; i < defaultNumberOfOutputNodes; i++)
    {
        cout << output[i] << endl;
    }
    cout << endl;
    newModel.agents[0]->info();
}
