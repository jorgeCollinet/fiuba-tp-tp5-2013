#ifndef MONITORINTERFACE_H_
#define MONITORINTERFACE_H_

#include <gtkmm.h>
#include <gtkmm/drawingarea.h>
#include <glibmm.h>
#include <gtkmm/drawingarea.h>
#include <ctime>
#include <cmath>
#include <cairomm/cairomm.h>
#include <pangomm.h>
#include <cairomm/context.h>
#include <gtkmm/main.h>
#include <gtkmm/window.h>
#include "common_util.h"
#include "defines.h"
#include <list>
#include <iostream>
#include <string>
#include "common_base_de_datos_usuario.h"
using namespace std;



class MonitorInterface
{
public:
	MonitorInterface();
	/**
	 * @brief Pone a "correr" la ventana, mostrandola.
	 */
	void correr();

	~MonitorInterface();
private:
	size_t test_aux;
	// para testing
	bool bajando;
	list<double>medidas;
	double max_medida;
	// largo de los divisores
	double largo;
	Gtk::Window* window;
	Glib::RefPtr<Gtk::Builder> builder;
	BaseDeDatosUsuario* bd_usr;
	/** @brief Carga las preferencias del archivo de preferencias */
	void cargarPreferencias();
	/** @brief Guarda las preferencias del archivo de preferencias */
	void guardarPreferencias();
	/** @brief Registra un nuevo usuario */
	void registrarUsuario();
	/** @brief Borra un usuario */
	void borrarUsuario();
	/** @brief Carga la base de datos o la crea si no existe */
	void cargarDB();
	/** @brief Guarda los cambios efectuados a la base de datos */
	void guardarDB();
	bool on_timeout();
	bool graficar(GdkEventExpose* event);
	void dibujar_division_x(Cairo::RefPtr < Cairo::Context >& cr, size_t cant_div, double offsetx);
	void dibujar_division_y(Cairo::RefPtr < Cairo::Context >& cr, size_t cant_div, double offsety);

	Gtk::Entry* entry_puerto1;
	Gtk::Entry* entry_puerto2;
	Gtk::FileChooserButton* chooser_dir;
	Gtk::Label* dir_uso;
	Gtk::Button* pBotGuardCfg;
	Gtk::Button* pBotNeuvUs;
	Gtk::Button* pBotElimUS;
	Gtk::FileChooserButton* db_dir;
	Gtk::Button* pCargar;
	Gtk::Button* pGuardar;
	Gtk::Entry* entry_nombre_reg;
	Gtk::Entry* entry_pass_reg;
	Gtk::Entry* entry_nombre_borr;
	Gtk::DrawingArea* draw_area;
};

#endif /* MONITORINTERFACE_H_ */