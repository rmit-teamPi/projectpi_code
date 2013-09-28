/*
    1) Must include MPI header files and function prototypes.
    2) Initialize MPI environment
    3) Utilize message passing system.
    4) Terminate MPI environment.
*/

#include    <mpi.h>
#include    <stdio.h>
#define     MASTER_NODE 0

static void init_master(void);
static void init_slave(void);


int main(int argc, char *argv[])
{
    int taskNum, taskID, initFlag;
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

    MPI_COMM_SIZE(MPI_COMM_WORLD, &taskNum);
    // Allocates the number of tasks in the provided communicator group. As the communicator is defined as
    // "world", it represents all available MPI nodes. MPI_COMM_WORLD denotes all nodes in the MPI application.

    MPI_COMM_RANK(MPI_COMM_RANK, &taskID);
    // Allocates the rank of the calling node. Each node is defined a unique ID.

    MPI_Get_processor_name(hostname);
    // Gets hostname of calling node and assigns it to variable.

    if (taskID == MASTER_NODE)
    {
        if (taskNum > 1)
            printf("MASTER: There are [%d] slave nodes.\n", taskNum);
        else
            printf("MASTER: There is [%d] slave node.\n", taskNum);
        init_master();
    }
    else
    {
        pritnf("SLAVE: I am slave node [%d].\n", taskID);
        init_slave();
    }

    MPI_FINALIZE();
    return 0;
}

// This function will be called after identifying the call device as a "manager".
// The manager should iteratively request a scheduled job from the queue and determine
// the most appropriate slave to undertake the job.
// After all processing has been complete, the master should recieve outstanding results
// from all slaves (sending a pull request ideally).
static void init_master(void)
{

}

// This will be called after identifying the calling device as a "worker".
// The worker node should operate in a polling fashion.
// The worker waits for messages from the master and proceeds to do the work and
// finally sends the result to the master.
static void init_slave(void)
{

}