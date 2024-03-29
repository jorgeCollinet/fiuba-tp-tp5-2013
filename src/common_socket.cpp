#include <stdexcept> 	// Excepciones genericas
#include <cstring>		// Para manejar los buffers
#include <cerrno> 		// Errores de C
#include <sys/socket.h> // Sockets
#include <netdb.h>		// Inet
#include "common_socket.h"

Socket::addrinfoWrap::addrinfoWrap() : res(NULL) {}
Socket::addrinfoWrap::~addrinfoWrap()
{
	freeaddrinfo(res);
}

Socket::Socket() : sockfd(-1) {}

Socket::Socket(int sockfd) : sockfd(sockfd) {}

#include <iostream>
using namespace std;
void Socket::conectar(const char* address, const char* puerto)
{
	//Todo: Encapsular addrinfo en un objeto para evitar problemas de memoria
	if (strlen(address) <= 0) throw std::invalid_argument("Address invalido");
	if (strlen(puerto) <= 0) throw std::invalid_argument("Puerto invalido");
	int status;
	struct addrinfo hints;
	memset(&hints, 0, sizeof(hints));
	hints.ai_family = AF_UNSPEC;		//IPV4/6
	hints.ai_socktype = SOCK_STREAM;	//TCP
	hints.ai_flags = AI_PASSIVE;			//Auto address
	addrinfoWrap wrap;
	status = getaddrinfo(address, puerto, &hints, &wrap.res);
	if (status != 0) throw std::runtime_error(gai_strerror(status));
	sockfd = socket(wrap.res->ai_family, wrap.res->ai_socktype, wrap.res->ai_protocol);
	if (sockfd == -1) throw std::runtime_error(strerror(errno));
	status = connect(sockfd, wrap.res->ai_addr, wrap.res->ai_addrlen);
	if (status == -1) throw std::runtime_error(strerror(errno));

}

void Socket::escuchar(const char* port, int maxCola)
{
	//Todo: Encapsular addrinfo en un objeto para evitar problemas de memoria
	if (strlen(port) <= 0) throw std::invalid_argument("Puerto invalido");
	int status;
	struct addrinfo hints;
	memset(&hints, 0, sizeof(hints));
	hints.ai_family = AF_UNSPEC;		//IPV4/6
	hints.ai_socktype = SOCK_STREAM;	//TCP
	hints.ai_flags = AI_PASSIVE;		//Auto address
	addrinfoWrap wrap;
	status = getaddrinfo(NULL, port, &hints, &wrap.res);
	if (status != 0) throw std::runtime_error(strerror(errno));
	sockfd = socket(wrap.res->ai_family, wrap.res->ai_socktype, wrap.res->ai_protocol);
	if (sockfd == -1) throw std::runtime_error(strerror(errno));
	status = bind(sockfd, wrap.res->ai_addr, wrap.res->ai_addrlen);
	if (status == -1) throw std::runtime_error(strerror(errno));
	status = listen(sockfd, maxCola);
	if (status == -1) throw std::runtime_error(strerror(errno));
}

int Socket::aceptar()
{
	struct sockaddr_in cli_addr;
	int cli_len = sizeof(sockaddr_storage);
	int sock_cli = accept(sockfd, (struct sockaddr*)&cli_addr, (socklen_t*)&cli_len);
	return sock_cli;
}

int Socket::enviar(void *msg, size_t len)
{
	int val = send(sockfd, msg, len, 0);
	if (val == -1) throw std::runtime_error(strerror(errno));
	return val;
}

int Socket::recibir(void *msg, size_t len)
{
	int val = recv(sockfd, msg, len, 0);
	if (val <= -1) throw std::runtime_error(strerror(errno));
	if (val == 0) throw std::runtime_error("Conexion perdida.");
	return val;
}

void Socket::enviarLen(const char *msg, size_t len)
{
	//TODO: Teastear con gran cantidad de datos
	size_t totalEnviados = 0;
	while (totalEnviados < len)
	{
		//Por cada envio vamos corriendo el puntero y reduciendo la cantidad
		int enviados = enviar((void*)(msg+totalEnviados), len-totalEnviados);
		totalEnviados += enviados;
	}
}

void Socket::recibirLen(char *msg, size_t len)
{
	//TODO: Teastear con gran cantidad de datos
	size_t totalRecibidos = 0;
	while (totalRecibidos < len)
	{
		// Por cada envio vamos corriendo el puntero y reduciendo la cantidad
		int recibidos = recibir(msg+totalRecibidos, len-totalRecibidos);
		totalRecibidos += recibidos;
	}
}

void Socket::cerrar()
{
	if (sockfd == -1) return; //Socket no inicializado
	//Llamo a shutdown primero o se cuelga en el accept el socket
	shutdown(sockfd,2);
	close(sockfd);
	sockfd = -1; //Pongo el sockfd en -1 por si alguien intenta volver a cerrarlo
}
