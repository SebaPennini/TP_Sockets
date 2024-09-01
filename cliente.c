#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <winsock2.h>

#pragma comment(lib, "ws2_32.lib")

#define PUERTO 8080
#define TAMANIO_BUFFER 1024

void enviar(int socket, const char *mensaje) {
    send(socket, mensaje, strlen(mensaje), 0);
}

void recibir(int socket, char *buffer, int tamanio) {
    memset(buffer, 0, tamanio);
    recv(socket, buffer, tamanio, 0);
}

int main() {
    WSADATA wsa;
    SOCKET socket_servidor;
    struct sockaddr_in servidor;
    char buffer[TAMANIO_BUFFER];

    printf("Iniciando Winsock...\n");
    if (WSAStartup(MAKEWORD(2,2),&wsa) != 0) {
        printf("Error en la inicialización de Winsock. Código: %d", WSAGetLastError());
        return 1;
    }

    // Crear socket
    if((socket_servidor = socket(AF_INET, SOCK_STREAM, 0)) == INVALID_SOCKET) {
        printf("No se pudo crear el socket. Código: %d", WSAGetLastError());
        return 1;
    }
    printf("Socket creado.\n");

    servidor.sin_addr.s_addr = inet_addr("127.0.0.1");
    servidor.sin_family = AF_INET;
    servidor.sin_port = htons(PUERTO);

    // Conectar al servidor
    if (connect(socket_servidor, (struct sockaddr *)&servidor, sizeof(servidor)) < 0) {
        printf("Conexión fallida. Código: %d", WSAGetLastError());
        return 1;
    }
    printf("Conectado al servidor.\n");

    while (1) {
        // Recibir y mostrar menú
        recibir(socket_servidor, buffer, TAMANIO_BUFFER);
        printf("%s", buffer);

        // Leer opción del usuario y enviarla al servidor
        fgets(buffer, TAMANIO_BUFFER, stdin);
        enviar(socket_servidor, buffer);

        // Recibir respuesta del servidor
        recibir(socket_servidor, buffer, TAMANIO_BUFFER);
        //printf("Respuesta del servidor: %s\n", buffer);
        printf(buffer);

        // Si el servidor pide longitud, el cliente la ingresa
        if (strncmp(buffer, "Ingrese la longitud", 19) == 0) {
            fgets(buffer, TAMANIO_BUFFER, stdin);
            enviar(socket_servidor, buffer);

            // Recibir la cadena generada por el servidor
            recibir(socket_servidor, buffer, TAMANIO_BUFFER);
            printf("Respuesta del servidor: %s\n\n", buffer);
        }

        // Si el servidor indica que se cerrará la conexión, salir del bucle
        if (strncmp(buffer, "Conexión cerrada.\n", 17) == 0) {
            break;
        }
    }

    closesocket(socket_servidor);
    WSACleanup();
    return 0;
}
