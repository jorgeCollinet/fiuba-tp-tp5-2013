#include "common_base_de_datos.h"

void BaseDeDatos::IndiceRam::cargar(fstream &arch)
{
	arch.seekg(0);
	char* buffer = new char[RegistroIndice::tamMax()];
	while(arch.good())
	{
		uint8_t prefijo = 0;
		while (arch.good() && prefijo == 0) // Si hay 0s de prefijo sigo leyendo
			arch.read((char*)&prefijo,BYTES_PREF_NOMBRE);
		if (!arch.good() || prefijo == 0) break;
		streampos offset = arch.tellg();
		arch.read(buffer,RegistroIndice::tamReg(prefijo));
		RegistroIndice reg(buffer, prefijo, offset);
		almacenamiento.push_back(reg);
	}
	delete[] buffer;
	arch.clear(); // Ponemos el archivo en estado bueno
	arch.seekg(0);
}

void BaseDeDatos::IndiceRam::agregar(RegistroIndice &reg)
{
	almacenamiento.push_back(reg);
}

void BaseDeDatos::IndiceRam::eliminar(RegistroIndice &reg)
{
	almacenamiento.remove(reg);
}

void BaseDeDatos::IndiceRam::modificar(RegistroIndice &reg, const string &password, const string &dir)
{

	reg.calcularHash(dir, password, reg.hash);
}

void BaseDeDatos::IndiceRam::renombrar(RegistroIndice &reg, const string &nombre_nuevo)
{
	reg.nombre = nombre_nuevo;
}

BaseDeDatos::RegistroIndice* BaseDeDatos::IndiceRam::buscarNombre(const string &nombre)
{
	for (list<RegistroIndice>::iterator it = almacenamiento.begin(); it != almacenamiento.end(); ++it)
	{
		if (it->nombre == nombre) return &(*it);
	}
	return NULL;
}

list<BaseDeDatos::RegistroIndice*> BaseDeDatos::IndiceRam::buscarFecha(const time_t fecha)
{
	list<BaseDeDatos::RegistroIndice*> lista;
	for (list<RegistroIndice>::iterator it = almacenamiento.begin(); it != almacenamiento.end(); ++it)
	{
		if (it->modif == fecha) lista.push_back(&(*it));
	}
	return lista;
}

list<BaseDeDatos::RegistroIndice*> BaseDeDatos::IndiceRam::buscarTam(const off_t tam)
{
	list<BaseDeDatos::RegistroIndice*> lista;
	for (list<RegistroIndice>::iterator it = almacenamiento.begin(); it != almacenamiento.end(); ++it)
	{
		if (it->tam == tam) lista.push_back(&(*it));
	}
	return lista;
}

BaseDeDatos::RegistroIndice* BaseDeDatos::IndiceRam::buscarHash(const string &hash)
{
	for (list<RegistroIndice>::iterator it = almacenamiento.begin(); it != almacenamiento.end(); ++it)
	{
		if (it->hash == hash) return &(*it);
	}
	return NULL;
}

list<string> BaseDeDatos::IndiceRam::devolverNombres()
{
	list<string> nombres;
	for (list<RegistroIndice>::iterator it = almacenamiento.begin(); it != almacenamiento.end(); ++it)
	{
		nombres.push_back(it->nombre);
	}
	return nombres;
}