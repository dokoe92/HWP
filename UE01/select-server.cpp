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
    setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(yes)); // erlaubt an Port zu binden auch wenn er wegen TIME_WAIT blockiert ist

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

    fd_set fds; // Menge an Filedescriptors
    FD_ZERO(&fds); // File deskriptoren leeren
    FD_SET(sock, &fds); // Listen Socket in die Menge aufnehmen
    int max_fd = sock; // Listen Socket ist nun max_fd --> höchster FD

    while (true) {
        fd_set read_fds = fds;
        // Menge aller Sockets die überwacht werden. Blockeirt bis mindestens 1 Socket bereit ist
        if (select(max_fd + 1, &read_fds, nullptr, nullptr, nullptr) < 0) { // schaut sich höchsten filedescriptor an. read_fds ist Menge an fds. nfds braucht man weil fd_set ein bit array hat und wissen muss wie viel bit es sich anschauen muss
            perror("Select failed");
            return 1;
        }; 

        for (int fd = 2; fd <= max_fd; fd++) { // ersten 2 überwacht man nicht (0 = stdin, 1 = stdout)
            if (FD_ISSET(fd, &read_fds)) { // prüfe ob select diesen socket als bereit markiert hat
                if (fd == sock) { // wenn es listening socket ist dann gibt es neue Verbindung
                    int client_fd = next_pending_connection(sock); // akzeptiere Verbindung
                    FD_SET(client_fd, &fds); // füge neuen Client sock zu fds hinu
                    max_fd = std::max(client_fd, max_fd); // neuer sock ist max 

                    service_init(client_fd); // starte service für diesen client
                } else { // Client Socket ist wieder bereit (frei, also fertig
                    if (service_do(fd) == 0) { // Client fertig, Verbindung geschlossen
                        service_exit(fd); // Aufräumen, evtl. Resourcen freigeben

                        std::cout << "client disconnected" << std::endl;
                        close(fd); // Socket schließen

                        FD_CLR(fd, &fds); // Socket nicht mehr überwachen

                        if (max_fd == fd) { // max fd muss neu berechnet werden falls der aktuelle socket der max fd socket ist
                            int new_max_fd = sock; // Listening Socket
                            for (int i = 2; i < max_fd; ++i) {
                                if (FD_ISSET(i, &fds)) { // FD noch in Überweachungsmenge?
                                    new_max_fd = std::max(new_max_fd, i); // wenn ja dann schauen ob der socket größer als new_max_fd ist
                            
                                }
                            }
                            max_fd = new_max_fd; // max_fd neu setzen
                        }

                    }
                }
            }
        }
    }
    
    return 0;
}