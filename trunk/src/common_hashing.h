#ifndef COMMON_HASHING_H_
#define COMMON_HASHING_H_

#include <fstream>
using namespace std;

/**
 * @file common_hashing.h
 * @brief Funciones de hashing para distintos casos
 */

/**
 * @brief Devuelve el hash MD5 de un archivo entero
 * @param nombre_arch Archivo que se leera
 * @param hash String donde se guardara el hash
 * @return True si el proceso fue exitoso, false de no serlo
 */
bool MD5_arch(string &nombre_arch, string &hash);

/**
 * @brief Devuelve el hash MD5 aplicado a cada bloque del archivo
 * @param arch Archivo que se leera
 * @param hash String donde se guardara el hash
 * @param tamBloq Longitud de los bloques a utilizar para hacer el calculo
 * @return True si el proceso fue exitoso, false de no serlo
 */
bool MD5_bloques_arch(ifstream &arch, string &hash, size_t tamBloq);

#endif /* COMMON_HASHING_H_ */