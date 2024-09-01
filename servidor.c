#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <winsock2.h>
#include <time.h>

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

char* generar_nombre_usuario(int longitud) {
    static const char vocales[] = "aeiou";
    static const char consonantes[] = "bcdfghjklmnpqrstvwxyz";
    char *nombre = (char*)malloc((longitud + 1) * sizeof(char));

    srand(time(NULL));
    int es_vocal = rand() % 2; // 0 para consonante, 1 para vocal

    for (int i = 0; i < longitud; i++) {
        if (es_vocal) {
            nombre[i] = vocales[rand() % 5];
        } else {
            nombre[i] = consonantes[rand() % 21];
        }
        es_vocal = !es_vocal;
    }
    nombre[longitud] = '\0';
    return nombre;
}

char* generar_contraseña(int longitud) {
    static const char caracteres[] = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789";
    char *contraseña = (char*)malloc((longitud + 1) * sizeof(char));

    srand(time(NULL));
    for (int i = 0; i < longitud; i++) {
        contraseña[i] = caracteres[rand() % strlen(caracteres)];
    }
    contraseña[longitud] = '\0';
    return contraseña;
}

void manejar_conexion(int socket_cliente) {
    char buffer[TAMANIO_BUFFER];
    while (1) {
        // Enviar menú
        enviar(socket_cliente, "\nSeleccione una de las siguientes opciones:\n\n 1. Generar nombre de usuario.\n 2. Generar contraseña.\n 3. Salir.\n\nOpción elegida: ");
        recibir(socket_cliente, buffer, TAMANIO_BUFFER);
        int opcion = atoi(buffer);

        switch(opcion) {
            case 1: {
                enviar(socket_cliente, "Ingrese la longitud del nombre de usuario: ");
                recibir(socket_cliente, buffer, TAMANIO_BUFFER);
                int longitud_usuario = atoi(buffer);
                if (longitud_usuario >= 5 && longitud_usuario <= 15) {
                    char* usuario = generar_nombre_usuario(longitud_usuario);
                    enviar(socket_cliente, usuario);
                    free(usuario);
                } else {
                    enviar(socket_cliente, "Error: Longitud de nombre de usuario inválida. Debe ser entre 5 y 15 inclusive.");
                }
                break;
            }
            case 2: {
                enviar(socket_cliente, "Ingrese la longitud de la contraseña: ");
                recibir(socket_cliente, buffer, TAMANIO_BUFFER);
                int longitud_contraseña = atoi(buffer);
                if (longitud_contraseña >= 8 && longitud_contraseña < 50) {
                    char* contraseña = generar_contraseña(longitud_contraseña);
                    enviar(socket_cliente, contraseña);
                    free(contraseña);
                } else {
                    enviar(socket_cliente, "Error: Longitud de contraseña inválida. Debe ser mayor o igual a 8 y menor que 50");
                }
                break;
            }
            case 3:
                enviar(socket_cliente, "Conexión cerrada.\n");
                closesocket(socket_cliente);
                printf("Cliente desconectado.\n");
                return;
            default:
                enviar(socket_cliente, "Error: Opción inválida.\n");
                break;
        }
    }
}

int main() {
    WSADATA wsa;
    SOCKET socket_servidor, socket_cliente;
    struct sockaddr_in servidor, cliente;
    int c;

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

    // Preparar estructura sockaddr_in
    servidor.sin_family = AF_INET;
    servidor.sin_addr.s_addr = INADDR_ANY;
    servidor.sin_port = htons(PUERTO);

    // Vincular socket
    if(bind(socket_servidor, (struct sockaddr *)&servidor, sizeof(servidor)) == SOCKET_ERROR) {
        printf("Enlace fallido. Código: %d", WSAGetLastError());
        return 1;
    }
    printf("Enlace exitoso.\n");

    // Escuchar por conexiones entrantes
    listen(socket_servidor, 3);

    printf("Esperando conexiones entrantes...\n");
    c = sizeof(struct sockaddr_in);

    while((socket_cliente = accept(socket_servidor, (struct sockaddr *)&cliente, &c)) != INVALID_SOCKET) {
        printf("Conexión aceptada.\n");
        manejar_conexion(socket_cliente);
    }

    if (socket_cliente == INVALID_SOCKET) {
        printf("Fallo en la aceptación de la conexión. Código: %d", WSAGetLastError());
        return 1;
    }

    closesocket(socket_servidor);
    WSACleanup();
    return 0;
}