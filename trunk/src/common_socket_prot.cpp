#include <cstring>	// Memcpy
#include "common_socket_prot.h"
#include "common_base_de_datos.h" // BYTES_PREF_NOMBRE

#define TAM_BUFFER 4096

SocketProt::SocketProt() : Socket() {}

SocketProt::SocketProt (int socketfd) : Socket(socketfd) {}

bool SocketProt::enviar_flag(const PacketID flag)
{
	if (enviar((void*) &flag, 1) <= 0) return false;
	return true;
}

bool SocketProt::recibir_flag(PacketID &flag)
{
	flag = ZERO;
	if (recibir(&flag, 1) <= 0) return false;
	return true;
}

bool SocketProt::enviar_msg_c_prefijo(const string &msg, uint8_t bytes_para_prefijo)
{
	size_t len = msg.length();
	if (enviarLen((char*)&len, bytes_para_prefijo) == false) return false;
	return enviarLen(msg.c_str(), msg.length());
}

bool SocketProt::recibir_msg_c_prefijo(string &msg, uint8_t bytes_para_prefijo)
{
	bool exito;
	char* buffer1 = new char[bytes_para_prefijo]; // Pido el prefijo como char[]
	exito = recibirLen(buffer1, bytes_para_prefijo);
	size_t tam = 0;
	if (exito) memcpy(&tam, buffer1, bytes_para_prefijo); // Copio bytes del char[] al size_t
	delete buffer1;
	if (!exito) return false;
	char* buffer2 = new char[tam];
	exito = recibirLen(buffer2, tam); // Recibo los bytes
	if (exito) msg.append(buffer2, tam);  // Append de los bytes
	delete buffer2;
	return exito;
}

bool SocketProt::enviar_modif(const Modificacion &modif)
{
	bool exito;
	exito = enviar((void*)&(modif.accion),1);
	if (!exito) return false;
	exito = enviar_msg_c_prefijo(modif.nombre_archivo, BYTES_PREF_NOMBRE);
	if (!exito) return false;
	exito = enviar_msg_c_prefijo(modif.nombre_archivo_alt, BYTES_PREF_NOMBRE);
	if (!exito) return false;
	return true;
}

bool SocketProt::recibir_modif(Modificacion &modif)
{
	modif.es_local = false;
	bool exito = recibir(&(modif.accion),1);
	if (!exito) return false;
	exito = recibir_msg_c_prefijo(modif.nombre_archivo, BYTES_PREF_NOMBRE);
	if (!exito) return false;
	exito = recibir_msg_c_prefijo(modif.nombre_archivo_alt, BYTES_PREF_NOMBRE);
	if (!exito) return false;
	return true;
}

bool SocketProt::enviar_pedazo_archivo(istream &arch, size_t offset, size_t len)
{
	return true;
}

bool SocketProt::recibir_pedazo_archivo(ostream &arch, size_t offset, size_t len)
{
	return true;
}

bool SocketProt::enviar_archivo(istream &arch)
{
	//TODO: Optimizar viendo como funciona el buffer interno de ifstream
	arch.seekg(0,ios::end);
	streampos fin = arch.tellg();
	arch.seekg(0);
	if (!enviarLen((const char*) &fin, sizeof(streampos))) return false; //Envio el prefijo de longitud
	char buffer[TAM_BUFFER];
	while (fin > 0)
	{
		streamsize aEnviar = TAM_BUFFER;
		if (fin < TAM_BUFFER) aEnviar = fin;
		arch.read(buffer, aEnviar);
		if (!enviarLen(buffer, aEnviar)) return false; //Envio el archivo
		fin -= aEnviar;
	}
	return true;
}

bool SocketProt::recibir_archivo(ostream &arch)
{
	//TODO: Optimizar viendo como funciona el buffer interno de ofstream
	streampos tam;
	if (!recibirLen((char*) &tam, sizeof(streampos))) return false;
	char buffer[TAM_BUFFER];
	while (tam > 0)
	{
		streamsize aRecibir = TAM_BUFFER;
		if (tam < TAM_BUFFER) aRecibir = tam;
		if (!recibirLen(buffer, aRecibir)) return false;  //Recibo el archivo
		arch.write(buffer, aRecibir);
		tam -= aRecibir;
	}
	return true;
}
void codificar(std::string& mensaje)
{
	// por ahora no hace nada
}
void decodificar(std::string& mensaje)
{
	// por ahora no hace nada
}
