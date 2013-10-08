/*
    Main program daemon.

    1) Must include MPI header files and function prototypes.
    2) Initialize MPI environment
    3) Utilize message passing system.
    4) Terminate MPI environment.
*/

// These constants should ideally be defined in their own header file along with
// function prototypes
#include    "mpi.h"
#include    <stdio.h>
#include    <dirent.h>
#include    <pthread.h>
#include    <sys/types.h>
#include    <sys/socket.h>
#include    <sys/stat.h>
#include    <stdlib.h>
#include    <fcntl.h>
#include    <errno.h>
#include    <unistd.h>
#include    <syslog.h>
#include    <string.h>


#define     MASTER_NODE 0
#define     JOBFLAG 1
#define     KILLFLAG 2
#define     JOBDONEFLAG 3
#define     ALGORITHM_LONGEST_FIRST 4
#define     ALGORITHM_SHORTEST_FIRST 5
#define     ALGORITHM_FCFS 6
#define     ALGORITHM_ROUND_ROBIN 7

static void init_master(void);
static void init_slave(int rank);
static worker_output_t do_job(worker_input_t);
static void process_work(worker_output_t);
static worker_input_t get_next_job(void);
static char *jobFiles[20];

int main(int argc, char *argv[])
{
    pid_t pid, sid; // Process ID & session ID
    int rank, initFlag, algorithmFlag, commFlag;
    char hostname[MAX_CHAR_HOSTNAME];
    pthread_t schedulerThread;

    // Fork of parent process
    pid = fork();
    if (pid < 0)
        exit(EXIT_FAILURE);

    if (pid > 0)
        exit(EXIT_SUCCESS);

    sid = setsid();
    if (sid < 0)
        exit(EXIT_FAILURE);

    // Change working directory of daemon
    // TODO: This must be changed to the working directory of OpenMPI
    if ((chdir("/")) < 0)
        exit(EXIT_FAILURE);

    // Daemon cannot interact with STDIN, STDOUR, or STDERR
    close(STDIN_FILENO);
    //close(STDOUT_FILENO);
    close(STDERR_FILENO);

    // --------------------------------
    // DAEMON IS INITIALIZED HERE
    // --------------------------------

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
    {
        // Gather user input as to how the scheduler will operate.
        algorithmFlag = display_algorithm_menu();
        commFlag = display_comm_menu();
         // Create seperate thread for master scheduler
        pthread_create(&schedulerThread, NULL, init_master);
        gather_user_requests();
        // Merge main thread and master thread
        pthread_join(schedulerThread, NULL);
    }
    else
        init_slave(rank);

    // MPI environment must be destroyed.
    MPI_FINALIZE();
    return 0;
}

// This function allows the user to communicate via sockets to request
// scheduler statistics.
static void gather_user_requests(void)
{
    struct sockaddr_in address;
    int listen_fd, connection_fd;
    socklen_t address_length;
    char buffer[1024];

    // Create TCP/IP socket.
    // AF_NET: IPv4 address family
    // SOCK_STREAM: TCP type
    // 0: IP protocol
    // Function returns a file descriptor
    listen_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (listen_fd < 0)
    {
         perror("Failed socket creation");
         exit(1);
    }

    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = 9999;

    // Call socket bind
    if (bind(listen_fd, (struct sockaddr *) &address, sizeof(address)) < 0)
    {
        perror("Bind Failed");
        exit(1);
    }
    // Listen.
    // The second argument is a maximum length to which the queue of pending connections to the socket
    // may grow.
    if (listen(listen_fd, 1) != 0)
    {
        perror("Listen Failed");
        exit(1);
    }
    // Accept incoming connections
    while((connection_fd = accept(listen_fd, (struct sockaddr *) &address, &address_length)) > -1)
    {
        // Here the user will communicate with the daemon scheduler.
        // The user will run some program to initiate the socket connection and
        // the daemon will be sent command line arguments to send back the requested information.
        read(connection_fd, buffer, 255);
    }

    close(listen_fd);
    return;
}

// This function will parse data read from the input socket
// in order to interpret user requests for scheduler diagnostics.
static void parse_user_input(void)
{


}

// This menu should provide the master node an option for the end user to specificy what
// scheduling technique to use and whether to use blocking/non-blocking IO.

// TODO: Fix terminating while condition
static int display_algorithm_menu(void)
{
    int algorithmOption;
    printf("Please specify the scheduling algorithm you want to employ.\n");
    printf("1) Next job waiting.\n2) Longest job first.\n3) Shortest job first.\n");
    scanf("%d", algorithmOption);
    do
    {
        switch(algorithmOption)
        {
            case ALGORITHM_FCFS:
                printf("Next job waiting selected.\n");
                break;
            case ALGORITHM_LONGEST_FIRST:
                printf("Longest job first selected.\n");
                break;
            case ALGORITHM_SHORTEST_FIRST:
                printf("Shortest job first selected.\n");
                break;
            default:
                printf("Please specify the scheduling algorithm you want to employ.\n");
                printf("1) Next job waiting.\n2) Longest job first.\n3) Shortest job first.\n");
                scanf("%d", algorithmOption);
                break;
        }
    } while();
}


// TODO: Fix terminating while condition
static int display_comm_menu(void)
{
    int commOption;
    printf("Please specify whether you want communication to be blocking or non-blocking.\n");
    printf("1) Blocking IO.\n2) Non-blocking IO.\n");
    scanf("%d", commOption);
    do
    {
        switch(commOption)
        {
            case 1:
                printf("Blocking IO selected.\n");
                break;
            case 2:
                printf("non-blocking IO selected.\n");
                break;
            default:
                printf("Please specify whether you want communication to be blocking or non-blocking.\n");
                printf("1) Blocking IO.\n2) Non-blocking IO.\n");
                scanf("%d", comm);
                break;
        }
    } while(comm != 1 || comm != 2);
}

// MASTER SECTION
// This function will be called after identifying the call device as a "manager".
// The manager should iteratively request a scheduled job from the queue and determine
// the most appropriate slave to undertake the job.
// After all processing has been complete, the master should recieve outstanding results
// from all slaves (sending a pull request ideally).
static void *init_master(void)
{
    int nodeNum, rank, jobCompletedNum = 0, jobID = -1, outstandingJobNum = 0;
    worker_input_t job;
    worker_output_t result;
    MPI_STATUS status;

    MPI_COMM_SIZE(MPI_COMM_WORLD, &taskNum);
    // Allocates the number of tasks in the provided communicator group. As the communicator is defined as
    // "world", it represents all available MPI nodes. MPI_COMM_WORLD denotes all nodes in the MPI application

    if (nodeNum > 1)
        printf("MASTER: There are [%d] slave nodes.\n", nodeNum);
    else
        printf("MASTER: There is [%d] slave node.\n", nodeNum);

    // Seed slaves each one job. These jobs should be popped from the job queue that has been established
    // by the user.
    for (rank = 1; rank < nodeNum; rank++)
    {
        job = get_next_job();
        MPI_Send(&job, 1, MPI_INT, rank, JOBFLAG, MPI_COMM_WORLD);
        outstandingJobNum++;
    }

    while (outstandingJobNum != 0)
    {
        // Get result from workers
        MPI_Recv(&result, 1, MPI_UNSIGNED, MPI_ANY_SOURCE, DONE, MPI_COMM_WORLD, &status);
        outstandingJobNum--;

        // Determine which node completed that job.
        rank = status.MPI_SOURCE;

        job = get_next_job();

        // Assign a new job to now vacant node.
        MPI_Send(&job, 1, MPI_INT, rank, JOBFLAG, MPI_COMM_WORLD);
        outstandingJobNum++;
    }

    // Send a kill request to all workers, this signals a shutdown of cluster.
    for (rank = 1; rank < nodeNum; rank++)
    {
        MPI_Send(&s, 1, MPI_INT, s, KILLFLAG, MPI_COMM_WORLD);
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

    // Recieve all messages from master node. This is blocking IO
    while(true)
    {
        // Recv(buffer, count, datatype, destination, tag, WORLD, status)
        // TODO: Alter arguments to match job script identifies
        MPI_Recv(&job, 1, MPI_INT, 0, MPI_ANY_TAG, MPI_COMM_WORLD, &status)

        // Check to see if the slave has been sent a kill command
        if (status.MPI_TAG == KILLFLAG)
            return;

        result = do_job(job);
        // Send(buffer, count, datatype, destination, tab, WORLD)
         // TODO: Alter arguments to match job script identifies
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
// This function simply removes the next job from the queue and farms it to a worker.
static worker_input_t get_next_job(void)
{

}

// This function will be called by the user to add additional jobs to the queue.
// The queue determines what job will be issued to the workers next.
static void queue_job(int position)
{

}

static void parse_job_script(void)
{

}

// Check if job queue on master is empty.
static boolean is_queue_empty(void)
{

}

// This function reads the contents of the job directory, checks if a files is of the correct
// format then adds it to the job queue on the master node.
// Basically this function initializes the job default, unsorted job queue.
// Functionally, the queue must be able to be dynamically allocated filenames.

// TODO: Do fix, make file reading dynamic and be able to assign job to hash table/array
static void parse_job_directory(void)
{
    int i = 0, fileCount = 0;
    DIR *dir;
    struct dirent *d;
    dir = opendir(".");
    if (dir)
    {
        while ((d = readdir(dir)) != NULL)
        {
            if (d->)
            strcpy(jobFiles[i], d->d_name);
            i++;
        }
        closedir(dir);
    }

}