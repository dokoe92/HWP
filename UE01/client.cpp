#include <iostream>
#include <ostream>
#include <sys/socket.h>
#include <stdio.h>
#include <unistd.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>

#define MAX_LINE_LENGTH 100

int main() {
    int sock = socket(AF_INET, SOCK_STREAM, 0); // Erzeugt TCP Socket

    if (sock < 0) {
        perror("socket");
        return 1;
    }

    // Serveradresse vorbereiten
    sockaddr_in server;
    server.sin_family = AF_INET;
    inet_pton(AF_INET, "127.0.0.1", &server.sin_addr); // wandelt Adresse in 32 Bit Binärwert um
    server.sin_port = htons(8000);

    // Mit Server verbinden
    if (connect(sock, reinterpret_cast<sockaddr *>(&server), sizeof(server)) < 0) {
        perror("connect failed");
        return 1;
    }

    std::cout << "Connected" << std::endl;

    char line[MAX_LINE_LENGTH];
    int ret_val = 0;

    // 
    while ((ret_val = read(sock, line, MAX_LINE_LENGTH)) > 0) { // Liest von Server
        line[ret_val] = '\0';
        std::cout << std::endl;

        std::string input;
        std::getline(std::cin, input);

        write(sock, input.c_str(), input.length()); // Sendet an Server
    }

    close(sock);
    return 0;
}
