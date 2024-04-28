
#include <sys/socket.h>
#include <arpa/inet.h>
#include <string.h>
#include <string>
#include <unistd.h>
#include <iostream>
#include <cstdlib>
#include <sys/wait.h>
#include <random>
#include <sstream>
#include <vector>
#include <cstring>

using namespace std;

void jugar(int socket_cliente, struct sockaddr_in direccionCliente){
    int FILAS = 8;
    int COLUMNAS = 8;
    char buffer[1024];
    memset(buffer, '\0', sizeof(char)*1024);
    int n_bytes = 0;
    char matriz[FILAS][COLUMNAS];
    random_device rd;
    mt19937 generador(rd());
    uniform_int_distribution<int> jugada(1, 7);
    uniform_int_distribution<int> turno(0, 1);
    char ip[INET_ADDRSTRLEN];
    inet_ntop(AF_INET, &(direccionCliente.sin_addr), ip, INET_ADDRSTRLEN);
    std::stringstream ss;
    int contador=1;
    char aux;
    int columna;
    int puntoC=0;
    int puntoS=0;
    bool empate=false;
    //llenado de matriz
    for (int i = 0; i < FILAS; ++i) {
        for (int j = 0; j < COLUMNAS; ++j) {
            if (j == 0) {
                aux= '0'+contador;
                matriz[i][j] = aux;
            } else {
                matriz[i][j] = ' ';
            }

            if (i== FILAS-2){        
                if (j==0){
                    matriz[i][j]=' ';
                }else{
                    matriz[i][j]= '-';
                }  
            }
            
            if (i== FILAS-1)
            {
                if (j==0){
                    matriz[i][j]=' ';
                }else{
                    aux='0'+j;
                    matriz[i][j]=aux;
                }  
            }
            
        }
        contador++;
    }
    cout << "[" << ip << ":" << ntohs(direccionCliente.sin_port) << "] Nuevo jugador." << endl;
    //Turno aleatorio de quien empieza
    int t = turno(generador);
    int in=8;
    if(t==0){
        in=8;
        int y= jugada(generador);
        for (int i = 0; i <= 8; i++){
            in=in-1;
            if(matriz[in][y]==' '){
                matriz[in][y]='S';
                break;
            }
        }
    }

    for (int i = 0; i < FILAS; ++i) {
            for (int j = 0; j < COLUMNAS; ++j) {
                ss << matriz[i][j]; 
            }
            ss << "\n"; 
        }

    std::string matrixStr = ss.str();
    send(socket_cliente, matrixStr.c_str(), matrixStr.length(), 0);

    
    while ((n_bytes = recv(socket_cliente, buffer, 1024, 0))) {
        buffer[n_bytes] = '\0';
        if (buffer[0] == 'Q') {
            cout << "[" << ip << ":" << ntohs(direccionCliente.sin_port) << "] Sale del juego." << endl;
            close(socket_cliente);
            break;
        }
        //juega cliente
        columna = std::stoi(std::string(1, buffer[0]));
        if(columna>7 || columna<1){
            send(socket_cliente, "error\n", 6, 0);
            send(socket_cliente, "valor no valido\n", 17, 0);
        }else{
            cout << "[" << ip << ":" << ntohs(direccionCliente.sin_port) << "] jugó: " << columna << endl;
            in=8;
            for (int i = 0; i <= 8; i++){
                in=in-1;
                if(matriz[in][columna]==' '){
                    matriz[in][columna]='C';
                    break;
                }
            }
            //comprobacion fila
            for (int i = 0; i < FILAS; i++){
                for (int j = 0; j < COLUMNAS; ++j) {
                    if(matriz[i][j]=='C'){
                        puntoC++;
                        if (puntoC==4){
                            break;
                        }
                    }else{
                        puntoC=0;
                    }
                }
                if (puntoC==4){
                    break;
                }
            }
            if (puntoC==4){
                send(socket_cliente, "El cliente ganó el juego\n", 26, 0);
                send(socket_cliente, "type 'exit' to quit\n", 21, 0);
                cout << "[" << ip << ":" << ntohs(direccionCliente.sin_port) << "] gana cliente." << endl;
                close(socket_cliente);
                break;
            }
            //comprobacion columna
            for (int i = 0; i < FILAS; i++){
                for (int j = 0; j < COLUMNAS; ++j) {
                    if(matriz[j][i]=='C'){
                        puntoC++;
                        if (puntoC==4){
                            break;
                        }
                    }else{
                        puntoC=0;
                    }
                }
                if (puntoC==4){
                    break;
                }
            }
            if (puntoC==4){
                send(socket_cliente, "El cliente ganó el juego\n", 26, 0);
                send(socket_cliente, "type 'exit' to quit\n", 21, 0);
                cout << "[" << ip << ":" << ntohs(direccionCliente.sin_port) << "] gana cliente." << endl;
                close(socket_cliente);
                break;
            }
            //comprobacion diagonal positiba
            for (int i = 0; i < FILAS; i++){
                for (int j = 0; j < COLUMNAS; ++j) {
                    if(matriz[i][j]=='C'){
                        puntoC++;
                        for (int k = 1; k < 4; i++){
                            if (matriz[i+k][j+k]=='C'){
                                puntoC++;
                                if (puntoC==4){
                                break;
                                }
                            }else{
                                puntoC=0;
                                break;
                            }
                        }
                    }else{
                        puntoC=0;
                    }
                    if (puntoC==4){
                        break;
                    }
                }
                if (puntoC==4){
                    break;
                }
            }
            if (puntoC==4){
                send(socket_cliente, "El cliente ganó el juego\n", 26, 0);
                send(socket_cliente, "type 'exit' to quit\n", 21, 0);
                cout << "[" << ip << ":" << ntohs(direccionCliente.sin_port) << "] gana cliente." << endl;
                close(socket_cliente);
                break;
            }
            //comprobacion de diagonal negativa
            for (int i = 0; i < FILAS; i++){
                for (int j = 0; j < COLUMNAS; ++j) {
                    if(matriz[i][j]=='C'){
                        puntoC++;
                        for (int k = 1; k < 4; i++){
                            if (matriz[i+k][j-k]=='C'){
                                puntoC++;
                                if (puntoC==4){
                                    break;
                                }
                            }else{
                                puntoC=0;
                                break;
                            } 
                        }
                    }else{
                        puntoC=0;
                    }
                    if (puntoC==4){
                        break;
                    }
                }
                if (puntoC==4){
                    break;
                }
            }
            if (puntoC==4){
                send(socket_cliente, "El cliente ganó el juego\n", 26, 0);
                send(socket_cliente, "type 'exit' to quit\n", 21, 0);
                cout << "[" << ip << ":" << ntohs(direccionCliente.sin_port) << "] gana cliente." << endl;
                close(socket_cliente);
                break;
            }
            //juega el servidor de forma aleatoria
            in=8;
            int y= jugada(generador);
            for (int i = 0; i <= 8; i++){
                in=in-1;
                if(matriz[in][y]==' '){
                    matriz[in][y]='S';
                    break;
                }
            }
            ss.str("");
            for (int i = 0; i < FILAS; ++i) {
                for (int j = 0; j < COLUMNAS; ++j) {
                    ss << matriz[i][j]; 
                }
                ss << "\n"; 
            }
            matrixStr = ss.str();
            send(socket_cliente, matrixStr.c_str(), matrixStr.length(), 0);
            //comprobacion filas servidor
            for (int i = 0; i < FILAS; i++){
                for (int j = 0; j < COLUMNAS; ++j) {
                    if(matriz[i][j]=='S'){
                        puntoS++;
                        if (puntoS==4){
                            break;
                        }
                    }else{
                        puntoS=0;
                    }
                }
                if (puntoS==4){
                    break;
                }
            }
            if (puntoS==4){
                send(socket_cliente, "El servidor ganó el juego\n", 27, 0);
                send(socket_cliente, "type 'exit' to quit\n", 21, 0);
                cout << "[" << ip << ":" << ntohs(direccionCliente.sin_port) << "] gana servidor." << endl;
                close(socket_cliente);
                break;
            }
            //comprobacion columna servidor
            for (int i = 0; i < FILAS; i++){
                for (int j = 0; j < COLUMNAS; ++j) {
                    if(matriz[j][i]=='S'){
                        puntoS++;
                        if (puntoS==4){
                            break;
                        }
                    }else{
                        puntoS=0;
                    }
                }
                if (puntoS==4){
                    break;
                }
            }
            if (puntoS==4){
                send(socket_cliente, "El servidor ganó el juego\n", 27, 0);
                send(socket_cliente, "type 'exit' to quit\n", 21, 0);
                cout << "[" << ip << ":" << ntohs(direccionCliente.sin_port) << "] gana servidor." << endl;
                close(socket_cliente);
                break;
            }
            //comprobacion diagonal positiva servidor
            for (int i = 0; i < FILAS; i++){
                for (int j = 0; j < COLUMNAS; ++j) {
                    if(matriz[i][j]=='S'){
                        puntoS++;
                        for (int k = 1; k < 4; i++){
                            if (matriz[i+k][j+k]=='S'){
                                puntoS++;
                                if (puntoS==4){
                                    break;
                                }
                            }else{
                                puntoS=0;
                                break;
                            }
                        }
                    }else{
                        puntoS=0;
                    }
                    if (puntoS==4){
                        break;
                    }
                }
                if (puntoS==4){
                    break;
                }
            }
            if (puntoS==4){
                send(socket_cliente, "El servidor ganó el juego\n", 27, 0);
                send(socket_cliente, "type 'exit' to quit\n", 21, 0);
                cout << "[" << ip << ":" << ntohs(direccionCliente.sin_port) << "] gana servidor." << endl;
                close(socket_cliente);
                break;
            }
            //comprobacion diagonal negativa servidor
            for (int i = 0; i < FILAS; i++){
                for (int j = 0; j < COLUMNAS; ++j) {
                    if(matriz[i][j]=='S'){
                        puntoS++;
                        for (int k = 1; k < 4; i++){
                            if (matriz[i+k][j-k]=='S'){
                                puntoS++;
                                if (puntoS==4){
                                    break;
                                }
                            }else{
                                puntoS=0;
                                break;
                            }
                        
                        }
                    }else{
                        puntoS=0;
                    }
                    if (puntoS==4){
                        break;
                    }
                }
                if (puntoS==4){
                    break;
                }
            }
            if (puntoS==4){
                send(socket_cliente, "El servidor ganó el juego\n", 27, 0);
                send(socket_cliente, "type 'exit' to quit\n", 21, 0);
                cout << "[" << ip << ":" << ntohs(direccionCliente.sin_port) << "] gana servidor." << endl;
                close(socket_cliente);
                break;
            }
            //comprobacion de empate
            empate=true;
            for (int i = 0; i < FILAS; i++){
                for (int j = 0; j < COLUMNAS; ++j) {
                    if(matriz[i][j]==' '){
                        empate=false;
                        break;
                    }
                }
            }
            if(empate==true){
                send(socket_cliente, "Empate\n", 8, 0);
                send(socket_cliente, "type 'exit' to quit\n", 21, 0);
                cout << "[" << ip << ":" << ntohs(direccionCliente.sin_port) << "] empate." << endl;
                close(socket_cliente);
                break;
            }
        }
    }
    cout << "La sesion a terminado\n";
    close(socket_cliente);
}


int main(int argc, char **argv) {
    if (argc != 2) {
        cerr << "Usage: " << argv[0] << " <port>" << endl;
        return 1;
    }

    int port = atoi(argv[1]);
    int socket_server = 0;
    struct sockaddr_in direccionServidor, direccionCliente;

    cout << "Creating listening socket ..." << endl;
    if ((socket_server = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        cerr << "Error creating listening socket" << endl;
        return 1;
    }

    cout << "Configuring socket address structure ..." << endl;
    memset(&direccionServidor, 0, sizeof(direccionServidor));
    direccionServidor.sin_family = AF_INET;
    direccionServidor.sin_addr.s_addr = htonl(INADDR_ANY);
    direccionServidor.sin_port = htons(port);

    cout << "Binding socket ..." << endl;
    if (bind(socket_server, (struct sockaddr *) &direccionServidor, sizeof(direccionServidor)) < 0) {
        cerr << "Error calling bind()" << endl;
        return 1;
    }

    cout << "Calling listening ..." << endl;
    if (listen(socket_server, 1024) < 0) {
        cerr << "Error calling listen()" << endl;
        return 1;
    }

    cout << "Waiting client request ..." << endl;
    cout << "Type 'exit' to quit "<< endl;
    while (true) {
        int socket_cliente;
        socklen_t addr_size = sizeof(struct sockaddr_in);
        
        if ((socket_cliente = accept(socket_server, (struct sockaddr *)&direccionCliente, &addr_size)) < 0) {
            cerr << "Error calling accept()" << endl;
            continue;
        }

        // Crear un nuevo proceso para manejar la conexión
        pid_t pid = fork();

        if (pid == 0) { //hijo
            close(socket_server); 

            // Manejar la conexión
            jugar(socket_cliente, direccionCliente);

            // Cerrar el socket del cliente y salir
            close(socket_cliente);
            exit(0);
        } else if (pid < 0) { // Error
            cerr << "Error creating child process" << endl;
            close(socket_cliente); // Cerrar el socket en caso de error
        } else { //Padre
            close(socket_cliente); 
        }
    }

    close(socket_server);
    return 0;
}
