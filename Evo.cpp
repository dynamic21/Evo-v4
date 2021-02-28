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

using std::abs;
using std::cout;
using std::endl;
using std::ifstream;
using std::ios;
using std::max;
using std::ofstream;
using std::string;
using std::vector;
ofstream fileOut;
ifstream fileIn;

// Important Details:
// - Number of agents = number of structureNodes, this is because the number of mutation exploration is proportional to its size

// TODO:
// - change number of agents default +1
// - lobby fitting and clean up environment / lobby roles
// - finish up lobby and environment comments
// - model stability function and model mutations
// - batch

double randDouble()
{
    return (double)rand() / RAND_MAX;
}

class componentNode
{
public:
    int numberOfWeights;                   // number oe weights in weights vector
    bool currentlyActive;                  // if the node is active
    double defaultMemoryMutationAmplitude; // how big of a mutation will the default memory get
    double biasMutationAmplitude;          // how big of a mutation will the bias get
    double weightMutationAmplitude;        // how big of a mutation will the weights get
    double defaultMemory;                  // the default memory of the node
    double memory;                         // the current memory of the node
    double bias;                           // the bias that is added if the node is active
    vector<double> weights;                // the weights that corespond to the model's structure

    componentNode(bool download = false) // if download, initialize a componentNode using the stored training state, otherwise, initialize a default componentNode
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

    componentNode(componentNode *givenComponentNode) // copies a given component using its pointer
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

    void saveState() // stores all data of the componentNode to the storage file
    {
        fileOut << numberOfWeights << " ";
        fileOut << currentlyActive << " ";
        fileOut << defaultMemoryMutationAmplitude << " ";
        fileOut << biasMutationAmplitude << " ";
        fileOut << weightMutationAmplitude << " ";
        fileOut << defaultMemory << " ";
        fileOut << memory << " ";
        fileOut << bias << " ";
        for (int i = 0; i < numberOfWeights; i++)
        {
            fileOut << weights[i] << " ";
        }
    }

    void info() // prints somewhat readable componentNode data for the user
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

    void mutate() // mutates the weights and bias (etc) of the node
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
    int numberOfConnections;             // number of connections in connections vector
    double addNodeMutationRate;          // the rate that this node will add a node
    double deleteNodeMutationRate;       // the rate that this node will delete a node
    double addConnectionMutationRate;    // the rate that this node will add a connection
    double deleteConnectionMutationRate; // the rate that this node will delete a connection
    vector<int> connections;             // holds all the parent node's indexes

    structureNode(bool download = false) // if download, initialize a structureNode using the stored training state, otherwise, initialize a default structureNode
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

    structureNode(structureNode *givenStructureNode) // copies a given structureNode using its pointer
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

    void saveState() // stores all data of the structureNode to the storage file
    {
        fileOut << numberOfConnections << " ";
        fileOut << addNodeMutationRate << " ";
        fileOut << deleteNodeMutationRate << " ";
        fileOut << addConnectionMutationRate << " ";
        fileOut << deleteConnectionMutationRate << " ";
        for (int i = 0; i < numberOfConnections; i++)
        {
            fileOut << connections[i] << " ";
        }
    }

    void info() // prints somewhat readable structureNode data for the user
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

    void mutate() // mutate the mutation rates of the node
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
    double score;                                   // the score of the agent, used for evaluation and selection
    int numberOfComponentNodes;                     // number of componentNodes in componentNodes vector
    vector<structureNode *> *structureNodesPointer; // the pointer to the model structure
    vector<componentNode *> componentNodes;         // vector of nodes that holds weights and biases (etc)

    agent(bool download = false) // if download, initialize an agent using the stored training state, otherwise, initialize a default agent
    {
        if (download)
        {
            fileIn >> score;
            fileIn >> numberOfComponentNodes;
            structureNodesPointer = NULL; // defined by the model it is under after its creation
            for (int i = 0; i < numberOfComponentNodes; i++)
            {
                componentNodes.push_back(new componentNode(true));
            }
            resetMemory(); // resets/initializes memory and activity of each node, used for eraseing the memory stored by the agent
        }
        else
        {
            score = 0;
            numberOfComponentNodes = defaultNumberOfInputNodes + defaultNumberOfOutputNodes; // default number of nodes
            structureNodesPointer = NULL;                                                    // defined by the model it is under after its creation
            for (int i = 0; i < numberOfComponentNodes; i++)                                 // for each node, add a component node
            {
                componentNodes.push_back(new componentNode());
                if (i < defaultNumberOfInputNodes) // if an input node, number of connections are defaultNumberOfOutputNodes
                {
                    for (int j = 0; j < defaultNumberOfOutputNodes; j++)
                    {
                        componentNodes[i]->weights.push_back(0);
                    }
                    componentNodes[i]->numberOfWeights = defaultNumberOfOutputNodes;
                }
                else // if an output node, number of connections are defaultNumberOfInputNodes
                {
                    for (int j = 0; j < defaultNumberOfInputNodes; j++)
                    {
                        componentNodes[i]->weights.push_back(0);
                    }
                    componentNodes[i]->numberOfWeights = defaultNumberOfInputNodes;
                }
                componentNodes[i]->mutate(); // mutates the weights and bias (etc) of the component node
            }
            resetMemory(); // resets/initializes memory and activity of each node, used for eraseing the memory stored by the agent
        }
    }

    agent(agent *givenAgent) // copies a given agent using its pointer
    {
        score = givenAgent->score;
        numberOfComponentNodes = givenAgent->numberOfComponentNodes;
        structureNodesPointer = givenAgent->structureNodesPointer;
        for (int i = 0; i < numberOfComponentNodes; i++)
        {
            componentNodes.push_back(new componentNode(givenAgent->componentNodes[i]));
        }
    }

    void saveState() // stores all data of the agent to the storage file
    {
        fileOut << score << " ";
        fileOut << numberOfComponentNodes << " ";
        for (int i = 0; i < numberOfComponentNodes; i++)
        {
            componentNodes[i]->saveState();
        }
    }

    void erase() // deletes all objects in heap contained by the agent
    {
        for (int i = 0; i < numberOfComponentNodes; i++)
        {
            delete componentNodes[i];
        }
    }

    void info() // prints somewhat readable agent data for the user
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

    void mutate() // mutates the weights and bias (etc) of every component node in the agent
    {
        for (int i = 0; i < numberOfComponentNodes; i++)
        {
            componentNodes[i]->mutate();
        }
    }

    void resetMemory() // resets/initializes memory and activity of each node, used for eraseing the memory stored by the agent
    {
        for (int i = 0; i < numberOfComponentNodes; i++)
        {
            componentNodes[i]->memory = componentNodes[i]->defaultMemory; // memory gets set to default memory
            componentNodes[i]->currentlyActive = true;                    // default activity is true because the bias allows the existence of negative numbers
        }
    }

    vector<double> evaluateInput(vector<double> givenInputs) // produces defaultNumberOfOutput number of values given defaultNumberOfInput number of inputs
    {
        int *futureMemory = new int[numberOfComponentNodes];       // substitute of components[i].futureMemory, stored here because it is a waste of space outside of this function
        bool *perviouslyActive = new bool[numberOfComponentNodes]; // substitute of components[i].perviouslyActive, stored here because it is a waste of space outside of this function
        for (int i = 0; i < defaultNumberOfInputNodes; i++)        // add each input to their coresponding input node memories
        {
            componentNodes[i]->memory += givenInputs[i];
        }
        for (int i = 0; i < numberOfComponentNodes; i++) // prepare each node for evaluating
        {
            futureMemory[i] = 0;                                      // future memory placeholder value
            perviouslyActive[i] = componentNodes[i]->currentlyActive; // present activity becomes past activity
            componentNodes[i]->currentlyActive = false;               // default activity value unless activated
        }
        for (int i = 0; i < numberOfComponentNodes; i++) // for every node
        {
            if (perviouslyActive[i]) // if the node (was) active send signals through its connections, otherwise skip over this node (nonlinear)
            {
                for (int j = 0; j < (*structureNodesPointer)[i]->numberOfConnections; j++) // for every connection
                {
                    double sum = componentNodes[i]->memory * componentNodes[i]->weights[j]; // get signal strength
                    if (sum >= 0)                                                           // if signal strength is positive(strong) let the signal pass through, otherwise drop it (nonlinear)
                    {
                        int connectionNumber = (*structureNodesPointer)[i]->connections[j]; // get parent node index
                        futureMemory[connectionNumber] += sum;                              // change the future memory of the parent node by the strength of the signal
                        componentNodes[connectionNumber]->currentlyActive = true;           // the parent node is now active
                    }
                }
            }
        }
        for (int i = 0; i < numberOfComponentNodes; i++) // for each node, add the bias of the node if it is active, otherwise the memory won't be used (nonlinear)
        {
            if (componentNodes[i]->currentlyActive)
            {
                componentNodes[i]->memory = futureMemory[i] + componentNodes[i]->bias;
            }
        }
        delete[] perviouslyActive; // !!! should I use delete[] or delete?
        delete[] futureMemory;     // !!! should I use delete[] or delete?
        vector<double> output;
        for (int i = defaultNumberOfInputNodes; i < defaultNumberOfInputNodes + defaultNumberOfOutputNodes; i++)
        {
            if (componentNodes[i]->currentlyActive) // if the node is active, pass the memory of the node
            {
                output.push_back(componentNodes[i]->memory);
            }
            else // if the node isn't active, pass 0
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

    environment(bool download = false) // if download, initialize an environment using the stored training state, otherwise, initialize a default environment
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

    environment(environment *givenEnvironment) // copies a given environment using its pointer
    {
        numberOfAgents = givenEnvironment->numberOfAgents;
        for (int i = 0; i < numberOfAgents; i++)
        {
            agents.push_back(new agent(givenEnvironment->agents[i]));
        }
    }

    void erase() // deletes all objects in heap contained by the environment
    {
        for (int i = 0; i < numberOfAgents; i++)
        {
            agents[i]->erase();
            delete agents[i];
        }
    }

    void saveState() // stores all data of the environment to the storage file
    {
        fileOut << setButton << " ";
        fileOut << setValue << " ";
        fileOut << expectedValue << " ";
        fileOut << numberOfAgents << " ";
        for (int i = 0; i < numberOfAgents; i++)
        {
            agents[i]->saveState();
        }
    }

    void info() // prints somewhat readable environment data for the user
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

    vector<double> getInput()
    {
        vector<double> input;
        input.push_back(setButton);
        input.push_back(setValue);
        return input;
    }

    void start()
    {
        int length = 100;
        while (length--)
        {
            setValue = rand() % 3 - 1;
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

    lobby(bool download = false) // if download, initialize a lobby using the stored training state, otherwise, initialize a default lobby
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

    lobby(lobby *givenLobby) // copies a given lobby using its pointer
    {
        numberOfAgents = givenLobby->numberOfAgents;
        for (int i = 0; i < numberOfAgents; i++)
        {
            agents.push_back(new agent(givenLobby->agents[i]));
        }
    }

    void erase() // deletes all objects in heap contained by the lobby
    {
        for (int i = 0; i < numberOfAgents; i++)
        {
            agents[i]->erase();
            delete agents[i];
        }
    }

    void saveState() // stores all data of the lobby to the storage file
    {
        fileOut << numberOfAgents << " ";
        for (int i = 0; i < numberOfAgents; i++)
        {
            agents[i]->saveState();
        }
    }

    void info() // prints somewhat readable lobby data for the user
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
        givenAgent->score = 0;
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
            newEnvironment.addAgent(agents[i]);
        }
        newEnvironment.start();
    }
};

class model
{
public:
    int numberOfStructureNodes;             // number of agents in agents vector
    agent *agentRepresentative;             // the figurehead agent used to mirror model alerations
    vector<structureNode *> structureNodes; // vector of nodes that point to other node(s)
    vector<agent *> agents;                 // vector of agent pointers under this model

    model(bool download = false) // if download, initialize a model using the stored training state, otherwise, initialize a default model
    {
        if (download)
        {
            fileIn >> numberOfStructureNodes;
            agentRepresentative = NULL;
            for (int i = 0; i < numberOfStructureNodes; i++)
            {
                structureNodes.push_back(new structureNode(true));
                agents.push_back(new agent(true));
                agents[i]->structureNodesPointer = &structureNodes; // give the agent access to the model so they can follow the structure when evaluating data
            }
        }
        else
        {
            numberOfStructureNodes = defaultNumberOfInputNodes + defaultNumberOfOutputNodes; // default number of nodes
            agentRepresentative = NULL;                                                      // defined in model mutation
            for (int i = 0; i < numberOfStructureNodes; i++)                                 // for each node, add a structure node and agent
            {
                structureNodes.push_back(new structureNode());
                if (i < defaultNumberOfInputNodes) // if an input node, connections are output node indexes
                {
                    for (int j = defaultNumberOfInputNodes; j < numberOfStructureNodes; j++)
                    {
                        structureNodes[i]->connections.push_back(j);
                    }
                    structureNodes[i]->numberOfConnections = defaultNumberOfOutputNodes;
                }
                else // if an output node, connections are input node indexes
                {
                    for (int j = 0; j < defaultNumberOfInputNodes; j++)
                    {
                        structureNodes[i]->connections.push_back(j);
                    }
                    structureNodes[i]->numberOfConnections = defaultNumberOfInputNodes;
                }
                structureNodes[i]->mutate(); // mutate the mutation rates of the node
                agents.push_back(new agent());
                agents[i]->structureNodesPointer = &structureNodes; // give the agent access to the model so they can follow the structure when evaluating data
            }
        }
    }

    model(model *givenModel) // copies a given model using its pointer
    {
        numberOfStructureNodes = givenModel->numberOfStructureNodes;
        agentRepresentative = givenModel->agentRepresentative;
        for (int i = 0; i < numberOfStructureNodes; i++)
        {
            structureNodes.push_back(new structureNode(givenModel->structureNodes[i]));
            agents.push_back(new agent(givenModel->agents[i]));
        }
    }

    void erase() // deletes all objects in heap contained by the model
    {
        for (int i = 0; i < numberOfStructureNodes; i++)
        {
            delete structureNodes[i];
            agents[i]->erase();
            delete agents[i];
        }
    }

    void saveState() // stores all data of the model to the storage file
    {
        fileOut << numberOfStructureNodes << " ";
        for (int i = 0; i < numberOfStructureNodes; i++)
        {
            structureNodes[i]->saveState();
            agents[i]->saveState();
        }
    }

    void info() // prints somewhat readable model data for the user
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

    void shuffleAgents() // shuffle the agents around in agents vector
    {
        for (int i = 0; i < numberOfStructureNodes; i++)
        {
            int tempAgentIndex = rand() % numberOfStructureNodes;
            agent *tempAgent = agents[tempAgentIndex];
            agents[tempAgentIndex] = agents[i];
            agents[i] = tempAgent;
        }
    }

    void sortAgents() // sorts the agents
    {
        sort(agents.begin(), agents.end(), [](agent *agent1, agent *agent2) {
            return (agent1->score > agent2->score);
        });
    }

    void selectionAndReplaceAgents() // replaces the bottom percent with mutated top percent
    {
        int topPercent = max(1, int(numberOfStructureNodes * agentsKeptPercent)); // get number of kept agents
        for (int i = topPercent; i < numberOfStructureNodes; i++)                 // for every failed agent, erase the data and replace with mutated top percent
        {
            agents[i]->erase();
            delete agents[i];
            agents[i] = new agent(agents[i % topPercent]);
            agents[i]->mutate();
        }
    }

    void evaluateAgents() // puts the agents into lobbies to be fitted into environments to be evaluated, then evaluated
    {
        lobby newLobby;
        for (int i = 0; i < numberOfStructureNodes; i++) // add all agents into the lobby
        {
            newLobby.addAgent(agents[i]);
        }
        newLobby.ready(); // ready the lobby, start fitting, and start evaluations
    }

    void evaluate_Select_Diversify() // evaluate the agents under the model, replaces the bottom percent with mutated top percent, and saves the state of the training
    {
        evaluateAgents();            // puts the agents into lobbies to be fitted into environments to be evaluated, then evaluated
        shuffleAgents();             // shuffle the agents around in agents vector, used so old top agents doesn't always get priority when sorted
        sortAgents();                // sorts the agents
        selectionAndReplaceAgents(); // replaces the bottom percent with mutated top percent
        // delete code after this comment, it is for saving program state only in the model scope, will transition to batch scope once complete
        fileOut.open("storage.txt");       // clears and opens storage file
        saveState();                       // stores all data of the model to the file
        fileOut << "f";                    // notifies that the file is complete
        fileOut.close();                   // closes the file
        fileOut.open("backupStorage.txt"); // repeat with backup because if program stops midway storage, one file is guaranteed to be "complete"
        saveState();
        fileOut << "f";
        fileOut.close();
        cout << agents[0]->score << endl; // for testing purposes
    }

    void diversifyAgentRepresentative() // clones and mutates the agent representative to the agents list, then deletes agent representative
    {
        for (int i = 0; i < numberOfStructureNodes; i++) // for size of structure, copy and mutate agent representative
        {
            if (i < agents.size()) // if current index is within the previous size of the list, erase the data and replace it
            {
                agents[i]->erase();
                delete agents[i];
                agents[i] = new agent(agentRepresentative);
            }
            else // if current index is outside the previous size of the list, add more agents
            {
                agents.push_back(new agent(agentRepresentative));
            }
            agents[i]->mutate();
        }
        while (agents.size() > numberOfStructureNodes) // if the current size is less then previous size, erase all leftover data and delete extra space
        {
            agents[agents.size() - 1]->erase();
            delete agents[agents.size() - 1];
            agents.pop_back();
        }
        agentRepresentative->erase(); // erase all data contained in the agent representative
        delete agentRepresentative;   // delete the representative, should I set it to null? seems unecessary since when used, contains an agent pointer
    }

    void getReverseStructure(vector<vector<int>> *givenReverseStructure) // generate the structure, parent nodes become child nodes and other way around
    {
        for (int i = 0; i < numberOfStructureNodes; i++) // fill in second layer of vector before generating
        {
            givenReverseStructure->push_back({});
        }
        for (int i = 0; i < numberOfStructureNodes; i++) // for all nodes, add the current node to all its parent node's reverse connections
        {
            for (int j = 0; j < structureNodes[i]->numberOfConnections; j++)
            {
                (*givenReverseStructure)[structureNodes[i]->connections[j]].push_back(i);
            }
        }
    }

    void mutateAndDiversify() // mutates the species structure proportional to its size and updates the agent representative's data to match alterations
    {
        agentRepresentative = new agent(agents[0]);                             // set the representative to the best agent
        vector<vector<int>> *reverseStructurePointer = new vector<vector<int>>; // initialize the reverse structure pointer
        getReverseStructure(reverseStructurePointer);                           // passes in the pointer, generate the structure, parent nodes become child nodes and other way around
        int perviousNumberOfStructureNodes = numberOfStructureNodes;            // sets the amount of mutations to an unchanging value since the forloop potentially changes the number of nodes
        for (int i = 0; i < perviousNumberOfStructureNodes; i++)
        {
            addNode();                                 // add a node in between a random connection of a random node
            addConnection();                           // adds a connetion from a random node to a random node
            deleteNode(reverseStructurePointer);       // deletes a random node that aren't input or output nodes and stretches the children node connections to the parent nodes
            deleteConnection(reverseStructurePointer); // deletes a connection from a random node if isn't illegal
        }
        delete reverseStructurePointer;                  // !!! do I do delete[] or just delete?
        for (int i = 0; i < numberOfStructureNodes; i++) // changes the mutation rates of every node
        {
            structureNodes[i]->mutate();
        }
        diversifyAgentRepresentative(); // clones and mutates the agent representative to the agents list, then deletes agent representative
    }

    void addNode() // add a node in between a random connection of a random node
    {
        int chosenNode = rand() % numberOfStructureNodes;                   // select the node to target
        if (structureNodes[chosenNode]->addNodeMutationRate > randDouble()) // if mutation rate allows this mutation
        {
            int chosenConnection = rand() % structureNodes[chosenNode]->numberOfConnections;                        // select a random connection to split
            structureNodes.push_back(new structureNode());                                                          // add the new structure node
            int chosenConenctionNode = structureNodes[chosenNode]->connections[chosenConnection];                   // gets the parent node index
            structureNodes[numberOfStructureNodes]->connections.push_back(chosenConenctionNode);                    // sets the new node's connection to point to the parent node
            structureNodes[chosenNode]->connections[chosenConnection] = numberOfStructureNodes++;                   // sets the targeted node to replace its connetion with the new node, then update number of structureNodes
            agentRepresentative->componentNodes.push_back(new componentNode());                                     // adds a component node to the agent to mirror the alteration
            agentRepresentative->componentNodes[agentRepresentative->numberOfComponentNodes]->weights.push_back(1); // adds a weight to the new component node to mirror alteration
            agentRepresentative->componentNodes[agentRepresentative->numberOfComponentNodes++]->numberOfWeights++;  //  then updates number of componentNodes and weights
        }
    }

    void addConnection() // adds a connetion from a random node to a random node
    {
        int chosenNode = rand() % numberOfStructureNodes;                         // select the node to be adding a connection to
        if (structureNodes[chosenNode]->addConnectionMutationRate > randDouble()) // if mutation rate allows this mutation
        {
            int secondChosenNode = rand() % numberOfStructureNodes;                   // select the second node to add the connection
            for (int i = 0; i < structureNodes[chosenNode]->numberOfConnections; i++) // check if the node already has the connection
            {
                if (structureNodes[chosenNode]->connections[i] == secondChosenNode)
                {
                    return;
                }
            }
            structureNodes[chosenNode]->connections.push_back(secondChosenNode);   // add the connection
            structureNodes[chosenNode]->numberOfConnections++;                     // update length of connections
            agentRepresentative->componentNodes[chosenNode]->weights.push_back(1); // add a weight to the agent representative to mirror the alteration
            agentRepresentative->componentNodes[chosenNode]->numberOfWeights++;    // update weight length
        }
    }

    void deleteNode(vector<vector<int>> *givenReverseStructurePointer) // deletes a random node that aren't input or output nodes and stretches the children node connections to the parent nodes
    {
        int chosenNode = rand() % numberOfStructureNodes;
    }

    void deleteConnection(vector<vector<int>> *givenReverseStructurePointer) // deletes a connection from a random node if isn't illegal
    {
        int chosenNode = rand() % numberOfStructureNodes;
    }
};

class batch // stores different models, not guaranteed to be different though
{
public:
};

string getStorageFile() // gets a completed file, if storage.txt doesn't have f at the end, then backupStorage.txt is guaranteed to have it
{
    fileIn.open("storage.txt");
    string a;
    while (fileIn >> a)
    {
        if (a == "f")
        {
            fileIn.close();
            return "storage.txt";
        }
    }
    fileIn.close();
    return "backupStorage.txt";
}

int main()
{
    srand(unsigned((time(NULL) % 9973 + 1) * (time(NULL) % 997 + 1) * (time(NULL) % 97 + 1) * (time(NULL) % 7 + 1))); // seeds the seed

    fileIn.open(getStorageFile()); // opens "uncorrupted" storage file
    model newModel(true);          // copies the stored model from last trained session
    fileIn.close();                // closes storage file
    while (true)
    {
        newModel.evaluate_Select_Diversify(); // forever trains the agents under this model
    }
}
