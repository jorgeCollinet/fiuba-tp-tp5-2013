#include <iostream>
#include "cliente.h"
#include "clientInterface.h"

/**
 * @file clientMain.cpp
 * @brief Main de cliente
 * 1er parametro: nombre_de_usuario
 * 2do parametro: contraseña
 * 3er parametro: ip_del_server
 * 4to parametro: puerto
 */
int main (int argc, char** argv)
{
	try
	{
		ClientInterface interface(argc, argv);
		interface.correr();
	}
	catch (std::exception &e)
	{
		std::cout << "Error: " << e.what() << std::endl;
	}
	return 0;
}
