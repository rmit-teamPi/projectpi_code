// This is the "client" application that utilizes sockets to communicate
// with the daemon scheduler in order to request current diagnostics of the
// job farming process.

#include <stdio.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <unistd.h>
#include <string.h>

int main(int argc, char *argv[])
{
    struct sockaddr_in address;
    int socket_fd, nbytes;
    char buffer[255];

    // Create socket.
    socket_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (socket_fd < 0)
    {
        printf("Error creating socket.");
        exit(1);
    }

    address.sun_family = AF_INET;
    address.sin_addr.s_addr = inet_addr("127.0.0.1");
    address.sin_port = htons(9999);

    // Conenct to daemon socket.
    if (connect(socket_fd, (struct sockaddr *) &address, sizeof (address)) < 0)
    {
        printf("Connection to daemon failed.");
        exit(1);
    }

    // No additional command line arguments.
    // TODOD: Change packet size.
    if (argc == 0)
    {
        nbytes = snprintf(buffer, 255, "SHOWALLSTATUS");
        write(socket_fd, buffer, nbytes);
        nbytes = read(socket_fd, buffer, 255);
        buffer[nbytes] = 0;
        printf(buffer);
    }

    close(socket_fd);
    return 0;
}