#include <iostream>
#include <bits/stdc++.h>
#include <fstream>
#include <vector>
#include <string>
#include <pthread.h>

using namespace std;

// Global variables
int numOfPrimes = 0, numOfPalindromes = 0, numOfPalindromicPrimes = 0, totalNums = 0;
int rangeStart, rangeEnd, T; // Start and end range, number of threads
string *Threadinfo;
vector<int> PrimeList, PalindromeList, PalindromicPrimesList;
pthread_mutex_t mu_lock; // Mutex for thread safety

// Check if a number is prime
bool isPrime(int num)
{ // function on web
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
{ // convert it to string and then reverse it to see if it is equal
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

    int rangePerThread = (rangeEnd - rangeStart) / T;
    int remainder = (rangeEnd - rangeStart) % T; // Extra numbers to be distributed

    int start = rangeStart + threadID * rangePerThread;
    int end;
    // definning what the end is for each thread
    if (threadID == T - 1)
    {
        end = rangeEnd; // Last thread handles the remaining range
    }
    else
    {
        end = start + rangePerThread;
    }

    Threadinfo[threadID] = string("ThreadID=") + to_string(threadID) + ", startNum=" + to_string(start) + ", endNum=" + to_string(end) + "\n";
    // cout << "ThreadID=" << threadID << ", startNum=" << start << ", endNum=" << end << "\n";

    for (int num = start + 1; num <= end; num++)
    {
        bool prime = isPrime(num);
        bool palindrome = isPalindrome(num);

        pthread_mutex_lock(&mu_lock); // Synchronize updates
        if (prime)
        {
            numOfPrimes++;
            PrimeList.push_back(num);
        }
        if (palindrome)
        {
            numOfPalindromes++;
            PalindromeList.push_back(num);
        }
        if (prime && palindrome)
        {
            numOfPalindromicPrimes++;
            PalindromicPrimesList.push_back(num);
        }
        totalNums++;
        pthread_mutex_unlock(&mu_lock);
    }

    return nullptr; // Return null <idk why , it solves the error>
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

    pthread_t threads[T]; // Array to hold  thread identifiers <must do before pthread_create >
    int threadIDs[T];
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
    for (int palindrome : PalindromeList)
        outputFile << palindrome << "\n";

    outputFile << "The palindromicPrime numbers are:\n";
    for (int palindromicPrime : PalindromicPrimesList)
        outputFile << palindromicPrime << "\n";

    // Print summary to terminal

    for (int i = 0; i < T; i++)
    {
        cout << Threadinfo[i];
    }
    cout << "totalNums=" << totalNums << ", numOfPrime=" << numOfPrimes
         << ", numOfPalindrome=" << numOfPalindromes
         << ", numOfPalindromicPrime=" << numOfPalindromicPrimes << "\n";

    return 0;
}
