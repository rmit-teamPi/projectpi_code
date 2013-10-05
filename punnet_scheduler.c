/*
    Main program daemon.

    1) Must include MPI header files and function prototypes.
    2) Initialize MPI environment
    3) Utilize message passing system.
    4) Terminate MPI environment.
*/

#include    <mpi.h>
#include    <stdio.h>
#define     MASTER_NODE 0
#define     KILLFLAG 2
#define     JOBFLAG 1

static void init_master(void);
static void init_slave(int rank);
static worker_output_t do_job(worker_input_t);
static void process_work(worker_output_t);
static worker_input_t get_next_job(void);

typedef struct
{
    /* data */
} worker_input_t;

typedef struct
{
    /* data */
} worker_output_t;


int main(int argc, char *argv[])
{
    int rank, initFlag;
    char hostname[MAX_CHAR_HOSTNAME];

    initFlag = MPI_Init(&argc, &argv);
    if (initFlag != MPI_SUCCESS)
    {
        printf("Error in initializing MPI environment. Terminating...");
        MPI_Abort(MPI_COMM_WORLD, initFlag);
    }
    // Initialize MPI environment.
    // The function accepts argc and argv pointers in order to differentiate between
    // command line arguments provided on "mpirun".

    MPI_COMM_RANK(MPI_COMM_RANK, &rank);
    // Allocates the rank of the calling node. Each node is defined a unique ID.

    MPI_Get_processor_name(hostname);
    // Gets hostname of calling node and assigns it to variable.

    if (rank == MASTER_NODE)
        init_master();
    else
        init_slave(rank);

    MPI_FINALIZE();
    return 0;
}

// MASTER SECTION
// This function will be called after identifying the call device as a "manager".
// The manager should iteratively request a scheduled job from the queue and determine
// the most appropriate slave to undertake the job.
// After all processing has been complete, the master should recieve outstanding results
// from all slaves (sending a pull request ideally).
static void init_master(void)
{
    int taskNum, rank;
    worker_input_t job;
    worker_output_t result;
    MPI_STATUS status;

    MPI_COMM_SIZE(MPI_COMM_WORLD, &taskNum);
    // Allocates the number of tasks in the provided communicator group. As the communicator is defined as
    // "world", it represents all available MPI nodes. MPI_COMM_WORLD denotes all nodes in the MPI application

    if (taskNum > 1)
        printf("MASTER: There are [%d] slave nodes.\n", taskNum);
    else
        printf("MASTER: There is [%d] slave node.\n", taskNum);

    // Seed all workers, initialize each
    for (rank = 1, rank < taskNum; rank++)
    {
        job = get_next_job();
    }

    while (workCompleteNum < jobNum)
    {
        // Get result from workers
        MPI_Recv(&result, 1, MPI_UNSIGNED, MPI_ANY_SOURCE, DONE, MPI_COMM_WORLD, &status);
        // Determine which node was assigned that job
        rank = status.MPI_SOURCE;
        //
    }
}

// SLAVE SECTION
// This will be called after identifying the calling device as a "worker".
// The worker node should operate in a polling fashion.
// The worker waits for messages from the master and proceeds to do the work and
// finally sends the result to the master.
static void init_slave(int rank)
{
    worker_input_t job; // Job buffer recieved by master
    worker_output_t result; // Result buffer after processing job
    MPI_STATUS status;

    printf("SLAVE: I am slave node [%d].\n", rank);

    // Recieve all messages from master node. This is blocking IO
    while(true)
    {
        // Recv(buffer, count, datatype, destination, tag, WORLD, status)
        MPI_Recv(&job, 1, MPI_INT, 0, MPI_ANY_TAG, MPI_COMM_WORLD, &status)

        // Check to see if the slave has been sent a kill command
        if (status.MPI_TAG == KILLFLAG)
            return;

        result = do_job(job);
        // Send(buffer, count, datatype, destination, tab, WORLD)
        MPI_Send(&result, 1, MPI_DOUBLE, 0, 0, MPI_COMM_WORLD);
    }
}

// This is the function run to process a job on a worker node.
static worker_output_t do_job(worker_input_t job)
{

}

// This is a master function used to process the results returned by workers.
static void process_work(worker_output_t result)
{

}

// Function called by master in order to process next job in the queue.
static worker_input_t get_next_job(void)
{

}