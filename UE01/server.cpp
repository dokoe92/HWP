#include <iostream>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <signal.h>

extern "C" void ServerProcess(int in, int ou);

// Funktion holt nächste eingehende Verbinung - Neuer Client Socket
static int next_pending_connection(int server_sock) { // Funktion kann man von außen nicht referenzieren
    int fd = accept(server_sock, nullptr, nullptr);
    std::cout << "\nAccepted connection" << std::endl;
    return fd;
}

int main() {
    signal(SIGCHLD, SIG_IGN); // beendete Kindprozesse werden automatisch aufgeräumt

    // ### #1 Socket Filedescriptor erzeugen

    // Gibt Filedescriptor des Sockets zurück. AF(Adress Family -> IPv4), SOCK_STREAM(TCP), Protocol (0 ist Standard)
    int sock = socket(AF_INET, SOCK_STREAM, 0);

    if (sock < 0) {
        perror("Socket creation failed");
        return 1;
    }

    // ### #2 Socket an Adresse (+ Port) binden

    // sockaddr ist Struktur für IPv4 Adresse
    // htons braucht man wegen little / big endian
    sockaddr_in server{};
    server.sin_family = AF_INET;
    server.sin_addr.s_addr = INADDR_ANY;
    server.sin_port = htons(8000);

    // Socket an eine Adresse + Port binden
    // Listen (Server) Socket gebunden
    if (bind(sock, reinterpret_cast<sockaddr *>(&server), sizeof(server)) < 0) {
        perror("Bind failed");
        return 1;
    };

    std::cout << "Bound to port 8000" << std::endl;

    // ### #3 Socket soll nun eingehende Verbindungen annehmen können. Hier max 10 gleichzeitige Verbindungen
    // Server Socket lauscht nun
    if (listen(sock, 10) != 0 ) {
        perror("Listen failed");
        return 1;
    }

    std::cout << "\nWaiting for incoming connections..." << std::endl;

    // ### #4 Verbindungen bearbeiten

    while (true) { // Server wartet immer wieder auf neue Verbindungen
        int fd = next_pending_connection(sock);  // File Descriptor der Client Verbindung

        if (fd < 0) {
            perror("Failed to get pending connection");
        } else {

            int pid = fork(); // erzeugt neuen Prozess (Kopie des Elternprozess). Jeder Client beomm eigenen Prozess

            if (pid < 0) {
                perror("Fork failed"); // noch im Elternprozess etwas schief gegangen. Weiter zur nächsten Verbindung
                continue;
            }

            if (pid == 0) { //Kindprozess gestartet. Client wird behandelt. Kind wird terminiert
                std::cout << "Child process started" << std::endl;
                ServerProcess(fd, fd);
                exit(0);
            }
            close(fd); // Parent schließt nun Kindverbindung
        }
    }

    return 0;
}
