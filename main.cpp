//
// Created by Ben Taylor on 10/30/19.
//
#include <iostream>
#include <cstdlib>
#include <mpi.h>
#include <unistd.h>
#include <stdlib.h>
#define MCW MPI_COMM_WORLD
#define SIZE 10000
using namespace std;


class queue
{
    int *arr;		// array to store queue elements
    int capacity;	// maximum capacity of the queue
    int front;		// front points to front element in the queue (if any)
    int rear;		// rear points to last element in the queue
    int count;		// current size of the queue

public:
    queue(int size = SIZE); 	// constructor
    ~queue();   				// destructor

    int dequeue();
    void enqueue(int x);
    int peek();
    int size();
    bool isEmpty();
    bool isFull();
};

// Constructor to initialize queue
queue::queue(int size)
{
    arr = new int[size];
    capacity = size;
    front = 0;
    rear = -1;
    count = 0;
}

// Destructor to free memory allocated to the queue
queue::~queue()
{
    delete arr;
}

// Utility function to remove front element from the queue
int queue::dequeue()
{
    // check for queue underflow
    if (isEmpty())
    {
        cout << "UnderFlow\nProgram Terminated\n";
        exit(EXIT_FAILURE);
    }

//    cout << "Removing " << arr[front] << '\n';
    int deq = arr[front];

    front = (front + 1) % capacity;
    count--;
    return deq;
}

// Utility function to add an item to the queue
void queue::enqueue(int item)
{
    // check for queue overflow
    if (isFull())
    {
        cout << "OverFlow\nProgram Terminated\n";
        exit(EXIT_FAILURE);
    }

//    cout << "Inserting " << item << '\n';

    rear = (rear + 1) % capacity;
    arr[rear] = item;
    count++;
}

// Utility function to return front element in the queue
int queue::peek()
{
    if (isEmpty())
    {
        cout << "UnderFlow\nProgram Terminated\n";
        exit(EXIT_FAILURE);
    }
    return arr[front];
}

// Utility function to return the size of the queue
int queue::size()
{
    return count;
}

// Utility function to check if the queue is empty or not
bool queue::isEmpty()
{
    return (size() == 0);
}

// Utility function to check if the queue is full or not
bool queue::isFull()
{
    return (size() == capacity);
}

int main(int argc, char **argv){

    int rank, size;
    int data;
    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MCW, &rank);
    MPI_Comm_size(MCW, &size);
    srand(rank);
    MPI_Status mystatus;
    int tag;
    int source;
    int generatedTasks = 0;
    int maxTasks = rand() % 1024 + 1024;
    int received[3];
    int token;
    int flag = 0;

    queue taskQueue(2048*size);
    auto start = std::chrono::system_clock::now();
    // Generate sections of the grid
    taskQueue.enqueue(rand() % 1024 + 1);
    generatedTasks++;

//    if(rank==0){  // I am the animal salesperson
    while (!taskQueue.isEmpty() && generatedTasks < maxTasks){
        int task = taskQueue.dequeue();
        for (int i = 0; i < task*task; i++);
        int numToGenerate = rand()%3 + 1;
        for (int i =0; i < numToGenerate; i++) {
            taskQueue.enqueue(rand()%1024 + 1);
            generatedTasks++;
        }
        int next[2];
        if (taskQueue.size() >= 16) {
            next[0] = taskQueue.dequeue(); next[1] = taskQueue.dequeue();
            MPI_Send(&next,2,MPI_INT,rand()%size,0,MCW);
        }

        MPI_Iprobe(MPI_ANY_SOURCE,MPI_ANY_TAG,MCW,&flag,&mystatus);
        if (flag) {
            tag = mystatus.MPI_TAG;
            source = mystatus.MPI_SOURCE;
            MPI_Recv(&received,tag,MPI_INT,source,tag,MCW,MPI_STATUS_IGNORE);
            if(tag==4){
                token = received[0];
            }
            else{
                taskQueue.enqueue(received[0]);
                taskQueue.enqueue(received[1]);
            }
        }
    }
    cout << "Process " << rank << " finished the generation of tasks" << endl;
    while (!taskQueue.isEmpty()) {
        int task = taskQueue.dequeue();
        for (int i = 0; i < task*task; i++);
    }
    cout << "Process " << rank << " is about to terminate" << endl;

//    }else{        // I am a farmer
//        sleep(rand()%5);
//        if(!(rand()%2)){
//             I want a pig
//            weight=rand()%30;
//            cout<<"Farmer "<<rank<<" asked for a pig that weighs " << weight << "." <<endl;
//            MPI_Send(&weight,1,MPI_INT,0,0,MCW);
//        }else{
            // I want a horse
//            weight=rand()%400;
//            cout<<"Farmer "<<rank<<" asked for a horse that weighs " << weight << "." <<endl;
//            MPI_Send(&weight,1,MPI_INT,0,1,MCW);
//
//        }
//
//        MPI_Recv(&animalType,1,MPI_INT,0,0,MCW,MPI_STATUS_IGNORE);
//        if(animalType==0){
//            cout<<"Farmer "<<rank<<" received a pig that weighs " << weight << "." <<endl;
//        }else{
//            cout<<"Farmer "<<rank<<" received a horse that weighs " << weight << "." <<endl;
//        }
//    }

    MPI_Finalize();

    return 0;
}




