#include<iostream>
#include<string>
#include<fstream>

using namespace std;

struct Libro {
	int id;
	string titulo;
	string autor;
	string año;
	string categoria;
};

struct Usuario {
	int id;
	string nombre;
	string correo;
};

struct Libro {
	int id;
	int idUsuario;
	int idLibro;
	string FechaPrestamo;
	string FechaDevolucion;
};

int main() {

	return 0;
}