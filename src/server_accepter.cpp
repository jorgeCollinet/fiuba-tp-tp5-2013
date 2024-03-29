#include "server_accepter.h"
#include "common_util.h"
#include <sys/stat.h>	//mkdir
#include "defines.h"
#include "common_observador_recibido.h"
#include <syslog.h>

Accepter::Accepter(const string &dir, const string &puerto1, const string &puerto2) :
		base_datos_usu(unirPath(dir,DB_USU_ARCH)), dir(dir),
		puerto1(puerto1), puerto2(puerto2), comunicadores() {}

Accepter::~Accepter()
{
	sock_prot1.cerrar();
	sock_prot2.cerrar();
}
void Accepter::limpiar_procesos_terminados()
{
	vector<string>a_eliminar_en_map;
	map<string, list<ServerCommunicator*> >::iterator it;
	for(it = comunicadores.begin(); it!= comunicadores.end();it++){
		vector<ServerCommunicator*> a_eliminar;
		list<ServerCommunicator*>::iterator seg_it;
		for(seg_it = it->second.begin(); seg_it!= it->second.end(); seg_it++){
			if ((*seg_it)->correr == false){
				a_eliminar.push_back((*seg_it));
			}
		}
		for (size_t i=0;i<a_eliminar.size();++i) {
			list<ServerCommunicator*>::iterator ter_it;
			it->second.remove(a_eliminar[i]);
			delete(a_eliminar[i]);
		}
		if(it->second.size() == 0){
			a_eliminar_en_map.push_back(it->first);
		}
	}
	for(size_t j=0;j<a_eliminar_en_map.size();++j){
		comunicadores.erase(a_eliminar_en_map[j]);
	}

	// Veo si hay que borrar alguna base de datos
	for(it = comunicadores.begin(); it!= comunicadores.end();it++)
	{
		if (it->second.size() == 0) // Se borraron todos los de este usuario
		{
			delete bases_de_datos[it->first];
			bases_de_datos[it->first] = NULL;
		}
	}
}
void Accepter::ejecutar()
{
	try
		{
		correr = true;
		base_datos_usu.abrir();
		sock_prot1.escuchar(puerto1.c_str(), MAX_COLA);
		sock_prot2.escuchar(puerto2.c_str(), MAX_COLA);
		size_t contador = 0;
		ObservadorRecibido obs;
		//obs.start(); congelado hasta nuevo aviso
		while (correr)
		{
			bool exito = false;
			try
			{
				exito = aceptar_conexion();
			}
			catch (...) {} // No es lo más lindo pero el socket puede tirar una excepcion de seguridad
			if (exito){
				contador++;
				if(contador >= CONEXCIONES_ACEPTADAS_PARA_BORRAR_MUERTOS){
					limpiar_procesos_terminados();
					syslog(LOG_DEBUG, "Se realizo limpiado de conexiones inactivas");
				}
				syslog(LOG_INFO, "Conexion exitosa desde un cliente.");
			}
			syslog(LOG_INFO, "Aceptacion de cliente fallida.");
		}
		syslog(LOG_DEBUG, "Accepter termina ejecucion");
		//obs.stop();
		//obs.join();
	}
	catch(exception &e)
	{
		syslog(LOG_EMERG, "Error fatal: %s", e.what());
	}
}

void Accepter::stop()
{
	Thread::stop();
	Lock lock_temp (mutex);
	for (map<string, list<ServerCommunicator*> >::iterator it = comunicadores.begin(); it != comunicadores.end(); ++it)
	{
		for (list<ServerCommunicator*>::iterator itL = it->second.begin(); itL != it->second.end(); ++itL)
		{
			(*itL)->stop();
		}
	}
	syslog(LOG_DEBUG, "Cerrando conexiones");
	sock_prot1.cerrar();
	sock_prot2.cerrar();
	// JOIN DE los ServerCommunicator
	for (map<string, list<ServerCommunicator*> >::iterator it = comunicadores.begin(); it != comunicadores.end(); ++it) {
		for (list<ServerCommunicator*>::iterator itL = it->second.begin(); itL != it->second.end(); ++itL) {
			(*itL)->stop();
			(*itL)->join();
			delete (*itL);
		}
	}
	syslog(LOG_INFO, "Conexiones cerradas");

}

bool Accepter::aceptar_conexion()
{
	syslog(LOG_DEBUG, "Aguardando conexion.");
	int fd_nuevo_1 = sock_prot1.aceptar();
	if (fd_nuevo_1 < 0) return false;
	syslog(LOG_DEBUG, "Conexion recibida, esperando login.");
	// Recibo mensaje
	SocketProt sock1(fd_nuevo_1);
	PacketID login = FAIL;
	sock1.recibir_flag(login);
	if (login != LOGIN) return false; // Veo que sea el paquete correcto
	syslog(LOG_DEBUG, "Flag de LOGIN recibido.");
	string usuario;

	sock1.recibir_msg_c_prefijo(usuario, BYTES_USER_PASS);
	//sock1.recibir_msg_c_prefijo(contrasenia, BYTES_USER_PASS); <- Ya no se pasa la contraseña en texto plano
	char buff[BYTES_HASH];
	sock1.recibirLen(buff,BYTES_HASH);

	bool login_correcto = base_datos_usu.usuario_contrasenia_correcto
			(usuario, buff);
	if (!login_correcto)
	{
		syslog(LOG_DEBUG, "Password incorrecta.");
		sock1.enviar_flag(FAIL);
		sock1.cerrar();
		return false;
	}
	else
	{
		syslog(LOG_DEBUG, "Password correcta.");
		// Si la carpeta del usuario no existe la creo, sino esta llamada no hace nada
		mkdir(unirPath(dir,usuario).c_str(), 0700);
		// Si mensaje bueno creo el otro socket
		sock1.enviar_flag(OK);
		int fd_nuevo_2 = sock_prot2.aceptar();
		if(fd_nuevo_2 < 0)
		{
			sock1.cerrar();
			return false;
		}
		if (comunicadores[usuario].size() == 0) bases_de_datos[usuario] = new BaseDeDatos();
		ServerCommunicator* comu = new ServerCommunicator (unirPath(dir, usuario), fd_nuevo_1,
				fd_nuevo_2, base_datos_usu.get_pass(usuario), bases_de_datos[usuario]);
		comunicadores[usuario].push_back(comu);
		comu->setVinculados(&comunicadores[usuario]);
		comu->start();
	}
	// Nota: El delete de sock1 no llama a close, asi que con pasar su file descriptor esta bien
	return true;
}
