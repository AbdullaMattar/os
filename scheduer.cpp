#include <iostream>
#include <fstream>
using namespace std;
struct Process
{
    char name[20];      // Name of the process
    int arrivalTime;    // When the process arrives
    int processingTime; // How long the process needs to run
    int rank;           // Priority rank (lower value = higher priority)
    int responseTime;   // Time from arrival to start of processing
    int turnaroundTime; // Total time from arrival to completion
    int delay;          // Time the process waited to start
};

// Function to read processes from "in.txt"
void readProcesses(Process processes[], int &numProcesses)
{
    ifstream inFile("in.txt");
    inFile >> numProcesses; // First line is the number of processes

    for (int i = 0; i < numProcesses; i++)
    {
        inFile >> processes[i].name >> processes[i].arrivalTime >> processes[i].processingTime >> processes[i].rank;
        processes[i].responseTime = -1; // Initialize to indicate not processed
    }
    inFile.close();
}

// Function to find the next process to execute based on arrival and rank
int findNextProcess(Process processes[], int numProcesses, int currentTime)
{
    int chosenProcess = -1; // Store the index of the process to execute next

    for (int i = 0; i < numProcesses; i++)
    {
        // Check if the process hasn't arrived yet or has already been processed
        if (processes[i].arrivalTime > currentTime)
        {
            continue; // Skip this process if it hasn't arrived yet
        }

        if (processes[i].responseTime != -1)
        {
            continue; // Skip this process if it has already been processed
        }

        // Check if no process has been chosen yet, or if we should choose this one based on rank and arrival time
        if (chosenProcess == -1)
        {
            chosenProcess = i;
        }
        else
        {
            // Compare the rank of the current process with the chosen one
            if (processes[i].rank < processes[chosenProcess].rank)
            {
                chosenProcess = i;
            }q
            // If ranks are equal, compare arrival times
            else if (processes[i].rank == processes[chosenProcess].rank)
            {
                if (processes[i].arrivalTime < processes[chosenProcess].arrivalTime)
                {
                    chosenProcess = i;
                }
                else
                {
                    return chosenProcess;
                }
            }
        }
    }
    return chosenProcess;
}

// Function to handle scheduling and writing results to "out.txt"
void scheduleProcesses(Process processes[], int numProcesses)
{
    int currentTime = 0;                      // Track the current time in the scheduler
    int executionOrder[1000], orderIndex = 0; // Store the order of executed processes

    for (int i = 0; i < numProcesses; i++)
    {
        int idx = findNextProcess(processes, numProcesses, currentTime);

        // If no process is ready, increase time and retry
        if (idx == -1)
        {
            currentTime++;
            i--; // Redo the loop iteration
            continue;
        }

        // Calculate response time and other metrics for the chosen process
        processes[idx].responseTime = currentTime - processes[idx].arrivalTime;
        processes[idx].turnaroundTime = processes[idx].responseTime + processes[idx].processingTime;
        processes[idx].delay = processes[idx].responseTime;

        // Move time forward by the processing time of the chosen process
        currentTime += processes[idx].processingTime;
        executionOrder[orderIndex] = idx; // Record the process execution order
        orderIndex++;
    }

    // Write the output to "out.txt"
    ofstream outFile("out.txt");
    for (int i = 0; i < orderIndex; i++)
    {
        int idx = executionOrder[i];
        outFile << processes[idx].name << ": (response=" << processes[idx].responseTime
                << ", turnaround=" << processes[idx].turnaroundTime
                << ", delay=" << processes[idx].delay << ")\n";
    }

    // Write the order of process execution
    for (int i = 0; i < orderIndex; i++)
    {
        outFile << processes[executionOrder[i]].name;
    }
    outFile << endl;
    outFile.close();
}

int main()
{
    Process processes[1000]; // Array to store up to 1000 processes
    int numProcesses = 0;    // Variable to store the number of processes

    readProcesses(processes, numProcesses);     // Read the processes from input file
    scheduleProcesses(processes, numProcesses); // Schedule processes and write output

    return 0;
}
