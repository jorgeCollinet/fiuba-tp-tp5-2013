#include "common_base_de_datos.h"
#include <dirent.h>		// Explorar directorios
#include <sys/stat.h>	// Stat
#include <cstring> 		// memcpy
#include <sstream>

bool BaseDeDatos::abrir(const std::string &directorio)
{
	this->directorio = directorio;
	pathArchivo = directorio;
	pathArchivo.append(NOMBRE_ARCH_DEF);
	archivo.open(pathArchivo.c_str(),std::ios::in | std::ios::out | std::ios::binary);
	if (!archivo.is_open())
	{
		archivo.open(pathArchivo.c_str(),std::ios::out | std::ios::binary);
		archivo.close();
		archivo.open(pathArchivo.c_str(),std::ios::in | std::ios::out | std::ios::binary);
		return false;
	}
	cargarARam();
	return true;
}

//----- Modificacion de archivos en el directorio

bool BaseDeDatos::abrir_para_escribir(const string& nombre_archivo, ofstream &ofstream)
{
	string dir(directorio);
	dir.append(nombre_archivo);
	ofstream.open(dir.c_str(), ios::out | ios::binary);
	return (ofstream.is_open());
}

bool BaseDeDatos::abrir_para_escribir_temporal(const string& nombre_archivo, ofstream &ofstream)
{
	string nombre(nombre_archivo);
	nombre.append(EXT_TMP);
	return abrir_para_escribir(nombre, ofstream);
}

bool BaseDeDatos::renombrar(const string &viejo_nombre,const string &nuevo_nombre) {
	string path1(directorio);
	string path2(directorio);
	path1.append(viejo_nombre);
	path2.append(nuevo_nombre);
	// A criterio si conviene levantar una excepcion si rename != 0
	return rename(path1.c_str(), path2.c_str());
}

bool BaseDeDatos::renombrar_temporal(const string &nombre_archivo)
{
	string viejo_nombre(nombre_archivo);
	viejo_nombre.append(EXT_TMP);
	return renombrar(viejo_nombre, nombre_archivo);
}

bool BaseDeDatos::eliminar_archivo(const string &nombre_archivo)
{
	// A criterio si conviene levantar una excepcion si rename != 0
	return remove( nombre_archivo.c_str() );
}

bool BaseDeDatos::abrir_para_leer(const string &nombre_archivo, ifstream &ifstream)
{
	string path(directorio);
	path.append(nombre_archivo);
	ifstream.open(path.c_str(), ios::in | ios::binary);
	return ifstream.is_open();
}

//----- Registracion en el indice de eventos

std::vector<Modificacion> BaseDeDatos::comprobar_cambios_locales()
{
	std::vector<Modificacion> modifs;

	DIR* dir = opendir(directorio.c_str());
	if (dir == NULL) return modifs;
	struct dirent* dirEnt = readdir(dir);
	while(dirEnt != NULL)
	{
		string path(directorio);
		path.append(dirEnt->d_name);
		struct stat buf;
		int val = stat(path.c_str(), &buf);
		//Todo: hacer algo con esto
		//buf.st_size; //Tamaño en bytes
		//buf.st_mtim; //ultima modificacion
		dirEnt = readdir(dir);
	}
	closedir(dir);
	//No liberar dirent
	return modifs;
}

vector<Modificacion> comparar_indices(fstream &otro)
{
	//TODO: Terminar
	return vector<Modificacion>();
}



//----- Registracion en el indice de eventos

bool BaseDeDatos::registrar_nuevo(const string &nombre_archivo)
{
	//TODO: Terminar
	return true;
}

bool BaseDeDatos::registrar_eliminado(const string &nombre_archivo)
{
	//TODO: Terminar
	return true;
}

bool BaseDeDatos::registrar_modificado(const string &nombre_archivo)
{
	//TODO: Terminar
	return true;
}

bool BaseDeDatos::registrar_renombrado(const string &nombre_viejo, const string &nombre_nuevo)
{
	//TODO: Terminar
	return true;
}

//----- Metodos privados

void BaseDeDatos::cargarARam()
{
	archivo.seekg(0);
	while(archivo.good())
	{
		uint8_t prefijo;
		archivo.read((char*)&prefijo,1);
		//char* buffer
		//delete buffer;
	}
}

//----- Registro Indice

BaseDeDatos::RegistroIndice::RegistroIndice(const string &nombre, time_t modif, off_t tam, const string &hash)
	: nombre(nombre), modif(modif), tam(tam), hash(hash) {}

BaseDeDatos::RegistroIndice::RegistroIndice(string &bytes, uint8_t tamNombre) : modif(0), tam(0)
{
	const char* src = bytes.c_str();
	nombre.append(src, tamNombre);
	memcpy((void*) &modif,(void*) (src+tamNombre), sizeof(time_t));
	memcpy((void*) &tam,(void*)(src+tamNombre+sizeof(time_t)), sizeof(off_t));
	hash.append(src+tamNombre+sizeof(time_t)+sizeof(off_t), BYTES_HASH);
}

string BaseDeDatos::RegistroIndice::serializar()
{
	stringstream result;
	uint8_t tamString = nombre.size();
	result.write((char*)&tamString, BYTES_PREF_NOMBRE);
	result.write(nombre.c_str(), nombre.size());
	result.write((char*)&modif, sizeof(time_t));
	result.write((char*)&tam, sizeof(off_t));
	result.write(hash.c_str(), BYTES_HASH);
	return result.str();
}
