import socket

class Cliente:
    _PUERTO = 8080
    _TAMANIO_BUFFER = 1024

    def __init__(self):
        self.sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)

    def enviar(self, mensaje):
        self.sock.sendall(mensaje.encode('utf-8'))

    def recibir(self):
        return self.sock.recv(self._TAMANIO_BUFFER).decode('utf-8')

    def conectar(self):
        try:
            self.sock.connect(('127.0.0.1', self._PUERTO))
            print("Conectado al servidor.")
            
            while True:
                # Recibir y mostrar menú
                respuesta = self.recibir()
                print(respuesta, end='')

                # Leer opción del usuario y enviarla al servidor
                opcion = input()
                self.enviar(opcion)

                # Recibir respuesta del servidor
                respuesta = self.recibir()
                print(respuesta, end='')

                # Si el servidor pide longitud, el cliente la ingresa
                if respuesta.startswith("Ingrese la longitud"):
                    longitud = input()
                    self.enviar(longitud)

                    # Recibir la cadena generada por el servidor
                    respuesta = self.recibir()
                    print(f"Respuesta del servidor: {respuesta}\n")

                # Manejar opción inválida
                elif "Opción inválida" in respuesta:
                    print("Error: Opción inválida. Por favor, ingrese una opción válida.\n")

                # Si el servidor indica que se cerrará la conexión, salir del bucle
                if respuesta.startswith("Conexión cerrada."):
                    break

        finally:
            self.sock.close()

if __name__ == "__main__":
    cliente = Cliente()
    cliente.conectar()
