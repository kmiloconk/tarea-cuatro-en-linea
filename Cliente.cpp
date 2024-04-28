#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <cstring>
#include <iostream>
#include <cstdlib>

using namespace std;

int main(int argc, char **argv) {
    if (argc != 3) {
        cerr << "Usage: " << argv[0] << " <server_ip> <port>" << endl;
        return 1;
    }

    const char *server_ip = argv[1];
    int port = atoi(argv[2]);

    int socket_cliente;
    struct sockaddr_in direccionServidor;

    cout << "Creating socket ..." << endl;
    if ((socket_cliente = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        cerr << "Error creating socket" << endl;
        return 1;
    }

    cout << "Configuring server address ..." << endl;
    memset(&direccionServidor, '0', sizeof(direccionServidor));
    direccionServidor.sin_family = AF_INET;
    direccionServidor.sin_port = htons(port);

    if (inet_pton(AF_INET, server_ip, &direccionServidor.sin_addr) <= 0) {
        cerr << "Invalid address/ Address not supported" << endl;
        return 1;
    }

    cout << "Connecting to server ..." << endl;
    if (connect(socket_cliente, (struct sockaddr *)&direccionServidor, sizeof(direccionServidor)) < 0) {
        cerr << "Connection failed" << endl;
        return 1;
    }

    cout << "Connected to server" << endl;
    cout << "Enter message to send (type 'exit' to quit): \n";
    char buffer[1024] = {0};
    read(socket_cliente, buffer, 1024);
    cout << "Server response: \n" << buffer << endl;
    while (true) {
        string message;
        getline(cin, message);

        if (message == "exit") {
            const char *message = "Q";
    	    send(socket_cliente, message, strlen(message), 0);
            break; 
        }

        send(socket_cliente, message.c_str(), message.length(), 0);
        read(socket_cliente, buffer, 1024);
        cout << "Server response: \n" << buffer << endl;
    }


    close(socket_cliente);
    return 0;
}

