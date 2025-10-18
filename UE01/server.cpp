#include <iostream>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <signal.h>

extern "C" void ServerProcess(int in, int ou);

static int next_pending_connection(int server_sock) { // Funktion kann man von außen nciht referenzieren
    int fd = accept(server_sock, nullptr, nullptr);
    std::cout << "\nAccepted connection" << std::endl;
    return fd;
}

int main() {
    signal(SIGCHLD, SIG_IGN);
    int sock = socket(AF_INET, SOCK_STREAM, 0);

    if (sock < 0) {
        perror("Socket creation failed");
        return 1;
    }

    sockaddr_in server{};
    server.sin_family = AF_INET;
    server.sin_addr.s_addr = INADDR_ANY;
    server.sin_port = htons(8000);

    if (bind(sock, reinterpret_cast<sockaddr *>(&server), sizeof(server)) < 0) {
        perror("Bind failed");
        return 1;
    };

    std::cout << "Bound to port 8000" << std::endl;

    if (listen(sock, 10) != 0 ) {
        perror("Listen failed");
        return 1;
    }

    std::cout << "\nWaiting for incoming connections..." << std::endl;

    while (true) {
        int fd = next_pending_connection(sock);

        if (fd < 0) {
            perror("Failed to get pending connection");
        } else {

            int pid = fork();

            if (pid < 0) {
                perror("Fork failed"); // noch im Elternprozess etwas schief gegangen
                continue;
            }

            if (pid == 0) { //Kindprozess gestartet
                std::cout << "Child process started" << std::endl;
                ServerProcess(fd, fd);
                exit(0);
            }
            close(fd);
        }
    }

    return 0;
}
