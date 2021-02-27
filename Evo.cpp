#include <iostream>
#include <fstream>
#include <vector>
#include <ctime>
#include <algorithm>
#define defaultNumberOfInputNodes 2
#define defaultNumberOfOutputNodes 1
#define defaultMutationRate 0.1
#define defaultMutationAmplitude 0.1
#define mutationFactor 0.01
#define populationExposurePercent 0.2
#define agentsKeptPercent 0.2
#define speciesKeptPercent 0.2

using std::cout;
using std::endl;
using std::ifstream;
using std::max;
using std::ofstream;
using std::vector;
ofstream fileOut;
ifstream fileIn;

// TODO:
// - species stability function and evaluation function
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

    componentNode(bool download = false)
    {
        if (download)
        {
            fileIn >> numberOfWeights;
            fileIn >> currentlyActive;
            fileIn >> defaultMemoryMutationAmplitude;
            fileIn >> biasMutationAmplitude;
            fileIn >> weightMutationAmplitude;
            fileIn >> defaultMemory;
            fileIn >> memory;
            fileIn >> bias;
            for (int i = 0; i < numberOfWeights; i++)
            {
                weights.push_back(0);
                fileIn >> weights[i];
            }
        }
        else
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

    void saveState()
    {
        fileOut << numberOfWeights << endl;
        fileOut << currentlyActive << endl;
        fileOut << defaultMemoryMutationAmplitude << endl;
        fileOut << biasMutationAmplitude << endl;
        fileOut << weightMutationAmplitude << endl;
        fileOut << defaultMemory << endl;
        fileOut << memory << endl;
        fileOut << bias << endl;
        for (int i = 0; i < numberOfWeights; i++)
        {
            fileOut << weights[i] << " ";
        }
        fileOut << endl;
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
        for (int i = 0; i < numberOfWeights; i++)
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

    structureNode(bool download = false)
    {
        if (download)
        {
            fileIn >> numberOfConnections;
            fileIn >> addNodeMutationRate;
            fileIn >> deleteNodeMutationRate;
            fileIn >> addConnectionMutationRate;
            fileIn >> deleteConnectionMutationRate;
            for (int i = 0; i < numberOfConnections; i++)
            {
                connections.push_back(0);
                fileIn >> connections[i];
            }
        }
        else
        {
            numberOfConnections = 0;
            addNodeMutationRate = defaultMutationRate;
            deleteNodeMutationRate = defaultMutationRate;
            addConnectionMutationRate = defaultMutationRate;
            deleteConnectionMutationRate = defaultMutationRate;
        }
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

    void saveState()
    {
        fileOut << numberOfConnections << endl;
        fileOut << addNodeMutationRate << endl;
        fileOut << deleteNodeMutationRate << endl;
        fileOut << addConnectionMutationRate << endl;
        fileOut << deleteConnectionMutationRate << endl;
        for (int i = 0; i < numberOfConnections; i++)
        {
            fileOut << connections[i] << " ";
        }
        fileOut << endl;
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

    agent(bool download = false)
    {
        if (download)
        {
            fileIn >> score;
            fileIn >> numberOfComponentNodes;
            structureNodesPointer = NULL; // model handles this
            for (int i = 0; i < numberOfComponentNodes; i++)
            {
                componentNodes.push_back(new componentNode(true));
            }
            resetMemory();
        }
        else
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

    void saveState()
    {
        fileOut << score << endl;
        fileOut << numberOfComponentNodes << endl;
        // model adds the reference
        for (int i = 0; i < numberOfComponentNodes; i++)
        {
            componentNodes[i]->saveState();
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

    void resetScore()
    {
        score = 0;
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

class environment
{
public:
    bool setButton;
    int setValue;
    int expectedValue;
    int numberOfAgents;
    vector<agent *> agents;

    environment(bool download = false)
    {
        if (download)
        {
            fileIn >> setButton;
            fileIn >> setValue;
            fileIn >> expectedValue;
            fileIn >> numberOfAgents;
            for (int i = 0; i < numberOfAgents; i++)
            {
                agents.push_back(new agent(true));
            }
        }
        else
        {
            setButton = true;
            setValue = 0;
            expectedValue = 0;
            numberOfAgents = 0;
        }
    }

    environment(environment *givenEnvironment)
    {
        numberOfAgents = givenEnvironment->numberOfAgents;
        for (int i = 0; i < numberOfAgents; i++)
        {
            agents.push_back(new agent(givenEnvironment->agents[i]));
        }
    }

    void erase()
    {
        for (int i = 0; i < numberOfAgents; i++)
        {
            agents[i]->erase();
            delete agents[i];
        }
    }

    void saveState()
    {
        fileOut << setButton << endl;
        fileOut << setValue << endl;
        fileOut << expectedValue << endl;
        fileOut << numberOfAgents << endl;
        for (int i = 0; i < numberOfAgents; i++)
        {
            agents[i]->saveState();
        }
    }

    void info()
    {
        cout << "setButton: " << setButton << endl;
        cout << "setValue: " << setValue << endl;
        cout << "expectedValue: " << expectedValue << endl;
        cout << "numberOfAgents: " << numberOfAgents << endl;
        cout << "agents: ";
        for (int i = 0; i < numberOfAgents; i++)
        {
            cout << agents[i] << " ";
        }
        cout << endl;
        cout << "__________" << endl;
    }

    vector<double> getInput()
    {
        vector<double> input;
        input.push_back(setButton);
        input.push_back(setValue);
        return input;
    }

    void addAgent(agent *givenAgent)
    {
        givenAgent->resetMemory();
        agents.push_back(givenAgent);
        numberOfAgents++;
    }

    void removeAgent(int givenIndex)
    {
        agents.erase(agents.begin() + givenIndex);
        numberOfAgents--;
    }

    void start()
    {
        int length = 100;
        while (length--)
        {
            if (setButton)
            {
                expectedValue = setValue;
            }
            for (int i = 0; i < numberOfAgents; i++)
            {
                // agents[i]->score -= abs(expectedValue - agents[i]->evaluateInput(getInput())[0]);
                agents[i]->score -= abs(10 - agents[i]->evaluateInput(getInput())[0]);
            }
            setButton = false;
            if (rand() % 10 == 0)
            {
                if (setValue++ == 1)
                {
                    setValue = -1;
                }
                setButton = true;
            }
        }
    }
};

class lobby
{
public:
    int numberOfAgents;
    vector<agent *> agents;

    lobby(bool download = false)
    {
        if (download)
        {
            fileIn >> numberOfAgents;
            for (int i = 0; i < numberOfAgents; i++)
            {
                agents.push_back(new agent(true));
            }
        }
        else
        {
            numberOfAgents = 0;
        }
    }

    lobby(lobby *givenLobby)
    {
        numberOfAgents = givenLobby->numberOfAgents;
        for (int i = 0; i < numberOfAgents; i++)
        {
            agents.push_back(new agent(givenLobby->agents[i]));
        }
    }

    void erase()
    {
        for (int i = 0; i < numberOfAgents; i++)
        {
            agents[i]->erase();
            delete agents[i];
        }
    }

    void saveState()
    {
        fileOut << numberOfAgents << endl;
        for (int i = 0; i < numberOfAgents; i++)
        {
            agents[i]->saveState();
        }
    }

    void info()
    {
        cout << "numberOfAgents: " << numberOfAgents << endl;
        cout << "agents: ";
        for (int i = 0; i < numberOfAgents; i++)
        {
            cout << agents[i] << " ";
        }
        cout << endl;
        cout << "__________" << endl;
    }

    void addAgent(agent *givenAgent)
    {
        givenAgent->resetMemory();
        agents.push_back(givenAgent);
        numberOfAgents++;
    }

    void removeAgent(int givenIndex)
    {
        agents.erase(agents.begin() + givenIndex);
        numberOfAgents--;
    }

    void ready()
    {
        environment newEnvironment;
        for (int i = 0; i < numberOfAgents; i++)
        {
            agents[i]->resetScore();
            newEnvironment.addAgent(agents[i]);
        }
        newEnvironment.start();
    }
};

class model
{
public:
    int numberOfStructureNodes;
    agent *agentRepresentative;
    vector<structureNode *> structureNodes;
    vector<agent *> agents;

    model(bool download = false)
    {
        if (download)
        {
            fileIn >> numberOfStructureNodes;
            agentRepresentative = NULL;
            for (int i = 0; i < numberOfStructureNodes; i++)
            {
                structureNodes.push_back(new structureNode(true));
                agents.push_back(new agent(true));
                agents[i]->structureNodesPointer = &structureNodes;
            }
        }
        else
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

    void saveState()
    {
        fileOut << numberOfStructureNodes << endl;
        for (int i = 0; i < numberOfStructureNodes; i++)
        {
            structureNodes[i]->saveState();
            agents[i]->saveState();
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
        agentRepresentative = new agent(agents[0]);
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
        agentRepresentative->erase();
        delete agentRepresentative;
    }

    void evaluateAgents()
    {
        lobby newLobby;
        for (int i = 0; i < numberOfStructureNodes; i++)
        {
            agents[i]->resetScore();
            newLobby.addAgent(agents[i]);
        }
        newLobby.ready();
    }

    void evaluateSelectDiversify()
    {
        evaluateAgents();
        shuffleAgents();
        sortAgents();
        selectionAndReplaceAgents();
        // delete code after this comment, it is for saving program state of the model scope
        fileOut.open("storage.txt");
        saveState();
        fileOut.close();
        cout << agents[0]->score << endl;
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

class batch
{
public:
};

int main()
{
    srand(unsigned((time(NULL) % 9973 + 1) * (time(NULL) % 997 + 1) * (time(NULL) % 97 + 1) * (time(NULL) % 7 + 1)));

    fileIn.open("storage.txt");
    model newModel(true);
    fileIn.close();
    while (true)
    {
        newModel.evaluateSelectDiversify();
    }
}
