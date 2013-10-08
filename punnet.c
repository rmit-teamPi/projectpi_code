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
        //error
    }

    address.sun_family = AF_INET;
    address.sin_port = 9999;

    // Conenct to daemon socket.
    if (connect(socket_fd, (struct sockaddr *) &address, sizeof (address)) < 0)
    {
        // connection failed
    }

    write(socket_fd, buffer, nbytes);

    // Read response from daemon software.
    while()
    {

    }

}