#include "common_arch_mutexcer.h"
#include "common_util.h"
#include <utility>
#include <dirent.h>
#include <sys/stat.h>
#include <cstring>
#include <iostream>
#include <syslog.h>
// hago algo raro para que compile todo., es probable q en el .h solo halla declarado todo. los bichos pero nada mas
//std::vector<ArchMutexcer*> ArchMutexcer::a_eliminar;
std::map<std::string, ArchMutexcer*> ArchMutexcer::hijitos;
std::map<ArchMutexcer*, size_t> ArchMutexcer::cant_hijitos;
Mutex ArchMutexcer::mutex_clase;
using namespace std;

ArchMutexcer* ArchMutexcer::generar_archmutexcer(const char* directorio) {
	Lock temp (ArchMutexcer::mutex_clase);
	if (ArchMutexcer::hijitos.count(directorio) == 1) {
		syslog(LOG_DEBUG, "ArchMutexcer::generar_archmutexcer(): Iinstancia ya estaba creada");
		ArchMutexcer::cant_hijitos[hijitos[directorio]]++;
		return ArchMutexcer::hijitos[directorio];
	}
	syslog(LOG_DEBUG, "ArchMutexcer::generar_archmutexcer(): Nueva instancia creada");
	ArchMutexcer* nuevo = new ArchMutexcer(directorio);

	ArchMutexcer::hijitos[directorio] = nuevo;
	ArchMutexcer::cant_hijitos[hijitos[directorio]] = 1;
	return nuevo;
}
ArchMutexcer::ArchMutexcer(const char* dir) : dir(dir) {
	this->mutex_archivos = new std::map <std::string,Mutex*>;
}

Mutex* ArchMutexcer::get_mutex(const char* dir_archivo) {
	Lock(this->mutex_loc);
	if (!is_mutex(dir_archivo)) {
		new_mutex(dir_archivo);
	}
	return (*mutex_archivos)[dir_archivo];
}
void ArchMutexcer::new_mutex(const char* dir_nuevo_archivo) {
	if(mutex_archivos->count(dir_nuevo_archivo) == 1){
		throw ios_base::failure("Se trata de crear un nuevo mutex para un archivo que ya tiene un mutex asociado");
	}
	(*mutex_archivos)[dir_nuevo_archivo] = new Mutex;
}
void ArchMutexcer::delete_mutex(const char* dir_archivo_a_eliminar){
	Lock(this->mutex_loc);
	if(mutex_archivos->count(dir_archivo_a_eliminar) == 1){
		throw ios_base::failure("Se trata de eliminar un mutex que no existe");
	}
	Mutex* mutx = (*mutex_archivos)[dir_archivo_a_eliminar];
	delete(mutx);
	mutex_archivos->erase(dir_archivo_a_eliminar);
}
bool ArchMutexcer::is_mutex(const char* dir_archivo){
	return (this->mutex_archivos->count(dir_archivo)==1);
}
void ArchMutexcer::borrar() {
	Lock* lock = new Lock(this->mutex_loc);

	// se pone para eliminarse , si solo queda él se elimina de verdad, sino se resta 1
	syslog(LOG_DEBUG, "ArchMutexcer::~ArchMutexcer(): Eliminando instancia de carpeta %s", dir.c_str());
	syslog(LOG_DEBUG, "ArchMutexcer::~ArchMutexcer():cant instancias: %i", ArchMutexcer::cant_instacias(this));
	if (1 == ArchMutexcer::cant_instacias(this)) {
		ArchMutexcer::cant_hijitos.erase(this);
		ArchMutexcer::hijitos.erase(dir);
		delete(lock);
		delete this;
	} else {
		Lock(this->mutex_clase);
		syslog(LOG_DEBUG, "ArchMutexcer::~ArchMutexcer(): solo se resta 1");
		ArchMutexcer::restar_instancia(this);
		delete(lock);
	}
}
ArchMutexcer::~ArchMutexcer() {
	syslog(LOG_DEBUG, "ArchMutexcer::~ArchMutexcer(): mutexs de carpeta %s se elimina permanentemente", dir.c_str());
	std::map<std::string, Mutex*>::iterator it;
	for (it = mutex_archivos->begin(); it != mutex_archivos->end(); it++) {
		delete it->second;
	}
	delete this->mutex_archivos;
}
size_t ArchMutexcer::cant_instacias(ArchMutexcer* mutexcer) {
	return cant_hijitos[mutexcer];
}
void ArchMutexcer::restar_instancia(ArchMutexcer* mutexcer) {
	cant_hijitos[mutexcer]--;
}

ostream& operator<<(std::ostream& os, ArchMutexcer& archm) {
	os << "directorio: " << archm.dir << endl;
	std::map<std::string,Mutex*>::iterator it;
	for(it=archm.mutex_archivos->begin(); it!=archm.mutex_archivos->end();it++){
		os<<it->first<<endl;
	}
	return os;
}
SmartP::SmartP(const std::string& dir) {
	mu = ArchMutexcer::generar_archmutexcer(dir.c_str());
}

SmartP::SmartP(ArchMutexcer* mutx){
	mu = mutx;
}
ArchMutexcer& SmartP::data() {
	return *mu;
}
SmartP::~SmartP(){
	mu->borrar();
}
