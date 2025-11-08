#include <iostream>

#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>


extern "C" {
    #include "service.h"
}

static int next_pending_connection(int server_sock) { // Funktion kann man von außen nciht referenzieren
    int fd = accept(server_sock, nullptr, nullptr);
    std::cout << "\nAccepted connection" << std::endl;
    return fd;
}

int main() {
    std::cout << "Select Server" << std::endl;

    int sock = socket(AF_INET, SOCK_STREAM, 0);

    if (sock < 0) {
        perror("Socket creation failed");
        return 1;
    }

    int yes = 1;
    setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(yes));

    sockaddr_in server{};
    server.sin_family = AF_INET;
    server.sin_addr.s_addr = INADDR_ANY; // wir hören auf jede Netzwerkschnittstelle
    server.sin_port = htons(8000); // muss man wegen endian machen

    if (bind(sock, reinterpret_cast<sockaddr *>(&server), sizeof(server)) < 0) { // bindet man auf port, wenn belegt dann Fehler
        perror("Bind failed");
        return 1;
    };

    std::cout << "Bound to port 8000" << std::endl;

    if (listen(sock, 10) != 0 ) { // wir warten auf Verbindung, man kann maximal 10 Verbindungen abarbeiten
        perror("Listen failed");
        return 1;
    }

    std::cout << "\nWaiting for incoming connections..." << std::endl;

    fd_set fds;
    FD_ZERO(&fds);
    FD_SET(sock, &fds);
    int max_fd = sock;

    while (true) {
        fd_set read_fds = fds;
        if (select(max_fd + 1, &read_fds, nullptr, nullptr, nullptr) < 0) { // nfds braucht man weil fd_set ein bit array hat und wissen muss wie viel bit es sich anschauen muss
            perror("Select failed");
            return 1;
        }; 

        for (int fd = 2; fd <= max_fd; fd++) { // ersten 2 überwacht man nicht
            if (FD_ISSET(fd, &read_fds)) {
                if (fd == sock) {
                    int client_fd = next_pending_connection(sock);
                    FD_SET(client_fd, &fds);
                    max_fd = std::max(client_fd, max_fd);

                    service_init(client_fd);
                } else {
                    if (service_do(fd) == 0) {
                        service_exit(fd);

                        std::cout << "client disconnected" << std::endl;
                        close(fd);

                        FD_CLR(fd, &fds);

                        if (max_fd == fd) {
                            int new_max_fd = sock;
                            for (int i = 2; i < max_fd; ++i) {
                                if (FD_ISSET(i, &fds)) {
                                    new_max_fd = std::max(new_max_fd, i);
                            
                                }
                            }
                            max_fd = new_max_fd;
                        }

                    }
                }
            }
        }
    }
    
    return 0;
}