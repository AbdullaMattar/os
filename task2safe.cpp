#include <iostream>
#include <bits/stdc++.h>
#include <fstream>
#include <vector>
#include <string>
#include <pthread.h>

using namespace std;

// Global variables
int primeCounter = 0, palinCounter = 0, palinPrimeCounter = 0, totalNums = 0, scope = 0;
int rangeStart, rangeEnd, T; // Start and end range, number of threads
bool manyWorkers = false;
string *Threadinfo;
vector<int> PrimeList, PalinList, PalinPrimesList;
pthread_mutex_t mu_lock; // Mutex for thread safety

// Check if a number is prime
bool isPrime(int num)
{
    if (num < 2)
        return false;
    for (int i = 2; i * i <= num; i++)
    {
        if (num % i == 0)
            return false;
    }
    return true;
}

// Check if a number is a palindrome
bool isPalindrome(int num)
{ // convert it to string and then reverse it to see if it is equal , uses <bits/stdc++.h>
    string str;
    str = to_string(num);
    string rev = str;
    reverse(rev.begin(), rev.end());

    if (str == rev)
        return true;
    else
        return false;
}

// Thread function
void *processRange(void *arg)
{
    int threadID = *(int *)arg;

    int rangePerThread = (scope) / T;
    int start = rangeStart + threadID * rangePerThread;
    int end;
    // definning what the end is for each thread
    end = start + rangePerThread;
    // saving the thread info in the dynamic array
    Threadinfo[threadID] = string("ThreadID=") + to_string(threadID) + ", startNum=" + to_string(start) + ", endNum=" + to_string(end) + "\n";
    // getting the values
    for (int num = start + 1; num <= end; num++) // start + 1 because the pdf its like this , we dont start with first number
    {
        bool prime = isPrime(num);
        bool palindrome = isPalindrome(num);

        pthread_mutex_lock(&mu_lock); // locks to prevent race condition when writin on these global varibales
        if (prime)
        {
            primeCounter++;
            PrimeList.push_back(num);
        }
        if (palindrome)
        {
            palinCounter++;
            PalinList.push_back(num);
        }
        if (prime && palindrome)
        {
            palinPrimeCounter++;
            PalinPrimesList.push_back(num);
        }
        totalNums++;
        pthread_mutex_unlock(&mu_lock);
    }

    return nullptr; // Return null <pthread needs it no exit>
}

// Main function
int main(int argc, char *argv[])
{ // argc == argument count
    if (argc != 2)
    { // argv[0] is the ./program itself
        cout << "wrong syntax ./out <thread num> \n";
        return 1;
    }

    T = stoi(argv[1]); // stoi (string to integer) > string library

    // Read input range from file
    ifstream inputFile("in.txt");

    inputFile >> rangeStart >> rangeEnd;
    scope = rangeEnd - rangeStart;
    int noJob = 0; // couter to make easy debbugging only we can remove it
    int oldT = T;
    if (T > scope) // if threads is more than range . make threads equal range
    {
        noJob = T - scope;
        T = scope;
    }
    else
    {
        if (scope % T != 0) // if range is not devisibile on T , keep
        {
            while (scope % T != 0)
            {
                T--;
                noJob++;
            }
        }
    }
    pthread_t threads[T];
    int threadIDs[T]; // Array to hold  thread identifiers <must do before pthread_create >
    Threadinfo = new string[T];
    pthread_mutex_init(&mu_lock, NULL); // Initialize mutex

    // Create threads
    for (int i = 0; i < T; i++)
    {
        threadIDs[i] = i;
        pthread_create(&threads[i], NULL, processRange, (void *)&threadIDs[i]);
        // By passing NULL, the thread will use the default attributes.
        // (void *)&threadIDs[i] is the parameter for processRange
    }

    // Join threads <wait to finish>
    for (int i = 0; i < T; i++)
    {
        pthread_join(threads[i], NULL);
    }

    pthread_mutex_destroy(&mu_lock); // Destroy mutex

    // Output results to file
    ofstream outputFile("out.txt");
    outputFile << "The prime numbers are:\n";
    for (int prime : PrimeList)
        outputFile << prime << "\n";

    outputFile << "The palindrome numbers are:\n";
    for (int palindrome : PalinList)
        outputFile << palindrome << "\n";

    outputFile << "The palindromicPrime numbers are:\n";
    for (int palindromicPrime : PalinPrimesList)
        outputFile << palindromicPrime << "\n";

    // Print summary to terminal
    for (int i = 0; i < oldT; i++)
    {
        if (i + 1 > T) // Print all threads info and know those who has no job
        {
            cout << "ThreadID=" << i << " -This Thread has no job-\n";
            noJob--;
        }
        else
        {
            cout << Threadinfo[i];
        }
    }
    cout << "totalNums=" << totalNums << ", numOfPrime=" << primeCounter
         << ", numOfPalindrome=" << palinCounter
         << ", numOfPalindromicPrime=" << palinPrimeCounter << "\n";

    return 0;
}
