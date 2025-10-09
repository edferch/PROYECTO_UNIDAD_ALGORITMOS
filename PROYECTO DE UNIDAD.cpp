#include <iostream>
#include <fstream>
#include <string>

using namespace std;

// Nombres de archivos
const string LIBROS_FILE = "libros.txt";
const string USUARIOS_FILE = "usuarios.txt";
const string PRESTAMOS_FILE = "prestamos.txt";

// Structs
struct Libro {
    int id;
    string titulo;
    string autor;
    string anio;
    string categoria;
};

struct Usuario {
    int id;
    string nombre;
    string correo;
};

struct Prestamo {
    int idPrestamo;
    int idUsuario;
    int idLibro;
    string fechaPrestamo;
    string fechaDevolucion;
};

// --------------------- Funciones auxiliares ---------------------

void asegurarArchivoConEncabezado(const string& nombre, const string& encabezado) {
    ifstream in(nombre);
    if (!in.is_open()) {
        ofstream out(nombre);
        out << encabezado << endl;
        cout << "Archivo creado: " << nombre << endl;
        return;
    }
    string linea;
    if (!getline(in, linea) || linea.empty()) {
        in.close();
        ofstream out(nombre, ios::trunc);
        out << encabezado << endl;
        cout << "Encabezado escrito en archivo existente: " << nombre << endl;
    }
}

int obtenerSiguienteID(const string& nombreArchivo) {
    ifstream in(nombreArchivo);
    if (!in.is_open()) return 1;
    string linea;
    if (!getline(in, linea)) { in.close(); return 1; } // salto encabezado
    int maxId = 0;
    while (getline(in, linea)) {
        if (linea.empty()) continue;
        size_t pos = linea.find('|');
        string idStr = (pos == string::npos) ? linea : linea.substr(0, pos);
        try {
            int id = stoi(idStr);
            if (id > maxId) maxId = id;
        }
        catch (...) {}
    }
    in.close();
    return maxId + 1;
}

// Divide la linea separada por '|' en un arreglo de strings (maxCampos)
void splitLinea(const string& linea, string campos[], int maxCampos) {
    size_t prev = 0;
    int idx = 0;
    while (idx < maxCampos) {
        size_t pos = linea.find('|', prev);
        if (pos == string::npos) {
            campos[idx++] = linea.substr(prev);
            break;
        }
        campos[idx++] = linea.substr(prev, pos - prev);
        prev = pos + 1;
    }
    for (int i = idx; i < maxCampos; i++) campos[i] = "";
}

// --------------------- Operaciones libros ---------------------

void agregarLibro() {
    Libro lb;
    lb.id = obtenerSiguienteID(LIBROS_FILE);

    cout << "\n-- Agregar nuevo libro (ID " << lb.id << ") --\n";
    cout << "Título: "; getline(cin, lb.titulo);
    cout << "Autor: "; getline(cin, lb.autor);
    cout << "Año: "; getline(cin, lb.anio);
    cout << "Categoría: "; getline(cin, lb.categoria);

    ofstream out(LIBROS_FILE, ios::app);
    if (!out.is_open()) { cout << "Error al abrir archivo.\n"; return; }
    out << lb.id << "|" << lb.titulo << "|" << lb.autor << "|" << lb.anio << "|" << lb.categoria << endl;
    out.close();
    cout << "Libro agregado correctamente.\n";
}

void listarLibros() {
    ifstream in(LIBROS_FILE);
    if (!in.is_open()) { cout << "No se puede abrir archivo.\n"; return; }
    string linea;
    if (!getline(in, linea)) { cout << "Archivo de libros vacío.\n"; return; }
    cout << "\n--- LIBROS ---\n";
    while (getline(in, linea)) {
        if (linea.empty()) continue;
        string campos[5];
        splitLinea(linea, campos, 5);
        cout << "ID: " << campos[0] << " | Título: " << campos[1] << " | Autor: " << campos[2]
            << " | Año: " << campos[3] << " | Cat: " << campos[4] << endl;
    }
    in.close();
}

void modificarLibro() {
    cout << "\n-- Modificar libro --\nIngrese ID: ";
    string idStr; getline(cin, idStr);
    ifstream in(LIBROS_FILE);
    if (!in.is_open()) { cout << "No se puede abrir archivo.\n"; return; }
    ofstream temp("tmp_libros.txt");
    if (!temp.is_open()) { cout << "Error creando archivo temporal.\n"; in.close(); return; }
    string header; getline(in, header); temp << header << endl;
    string linea; bool encontrado = false;
    while (getline(in, linea)) {
        if (linea.empty()) continue;
        string campos[5]; splitLinea(linea, campos, 5);
        if (campos[0] == idStr) {
            encontrado = true;
            string titulo, autor, anio, categoria;
            cout << "Título (actual: " << campos[1] << "): "; getline(cin, titulo);
            cout << "Autor (actual: " << campos[2] << "): "; getline(cin, autor);
            cout << "Año (actual: " << campos[3] << "): "; getline(cin, anio);
            cout << "Categoría (actual: " << campos[4] << "): "; getline(cin, categoria);
            if (titulo.empty()) titulo = campos[1];
            if (autor.empty()) autor = campos[2];
            if (anio.empty()) anio = campos[3];
            if (categoria.empty()) categoria = campos[4];
            temp << campos[0] << "|" << titulo << "|" << autor << "|" << anio << "|" << categoria << endl;
        }
        else temp << linea << endl;
    }
    in.close(); temp.close();
    if (!encontrado) { remove("tmp_libros.txt"); cout << "No se encontró libro.\n"; return; }
    remove(LIBROS_FILE.c_str()); rename("tmp_libros.txt", LIBROS_FILE.c_str());
    cout << "Libro modificado correctamente.\n";
}

void eliminarLibro() {
    cout << "\n-- Eliminar libro --\nIngrese ID: ";
    string idStr; getline(cin, idStr);
    ifstream in(LIBROS_FILE);
    if (!in.is_open()) { cout << "No se puede abrir archivo.\n"; return; }
    ofstream temp("tmp_libros.txt");
    if (!temp.is_open()) { cout << "Error creando archivo temporal.\n"; in.close(); return; }
    string header; getline(in, header); temp << header << endl;
    string linea; bool eliminado = false;
    while (getline(in, linea)) {
        if (linea.empty()) continue;
        string campos[5]; splitLinea(linea, campos, 5);
        if (campos[0] == idStr) eliminado = true;
        else temp << linea << endl;
    }
    in.close(); temp.close();
    if (!eliminado) { remove("tmp_libros.txt"); cout << "No se encontró libro.\n"; return; }
    remove(LIBROS_FILE.c_str()); rename("tmp_libros.txt", LIBROS_FILE.c_str());
    cout << "Libro eliminado correctamente.\n";
}

// --------------------- Operaciones usuarios ---------------------

void agregarUsuario() {
    Usuario us; us.id = obtenerSiguienteID(USUARIOS_FILE);
    cout << "\n-- Registrar usuario (ID " << us.id << ") --\n";
    cout << "Nombre: "; getline(cin, us.nombre);
    cout << "Correo: "; getline(cin, us.correo);
    ofstream out(USUARIOS_FILE, ios::app);
    if (!out.is_open()) { cout << "Error al abrir archivo.\n"; return; }
    out << us.id << "|" << us.nombre << "|" << us.correo << endl;
    out.close(); cout << "Usuario registrado.\n";
}

void listarUsuarios() {
    ifstream in(USUARIOS_FILE);
    if (!in.is_open()) { cout << "No se puede abrir archivo.\n"; return; }
    string linea;
    if (!getline(in, linea)) { cout << "Archivo de usuarios vacío.\n"; return; }
    cout << "\n--- USUARIOS ---\n";
    while (getline(in, linea)) {
        if (linea.empty()) continue;
        string campos[3]; splitLinea(linea, campos, 3);
        cout << "ID: " << campos[0] << " | Nombre: " << campos[1] << " | Correo: " << campos[2] << endl;
    }
    in.close();
}

void modificarUsuario() {
    cout << "\n-- Modificar usuario --\nIngrese ID: ";
    string idStr; getline(cin, idStr);
    ifstream in(USUARIOS_FILE);
    if (!in.is_open()) { cout << "No se puede abrir archivo.\n"; return; }
    ofstream temp("tmp_usuarios.txt");
    if (!temp.is_open()) { cout << "Error creando archivo temporal.\n"; in.close(); return; }
    string header; getline(in, header); temp << header << endl;
    string linea; bool encontrado = false;
    while (getline(in, linea)) {
        if (linea.empty()) continue;
        string campos[3]; splitLinea(linea, campos, 3);
        if (campos[0] == idStr) {
            encontrado = true;
            string nombre, correo;
            cout << "Nombre (actual: " << campos[1] << "): "; getline(cin, nombre);
            cout << "Correo (actual: " << campos[2] << "): "; getline(cin, correo);
            if (nombre.empty()) nombre = campos[1];
            if (correo.empty()) correo = campos[2];
            temp << campos[0] << "|" << nombre << "|" << correo << endl;
        }
        else temp << linea << endl;
    }
    in.close(); temp.close();
    if (!encontrado) { remove("tmp_usuarios.txt"); cout << "No se encontró usuario.\n"; return; }
    remove(USUARIOS_FILE.c_str()); rename("tmp_usuarios.txt", USUARIOS_FILE.c_str());
    cout << "Usuario modificado.\n";
}

void eliminarUsuario() {
    cout << "\n-- Eliminar usuario --\nIngrese ID: ";
    string idStr; getline(cin, idStr);
    ifstream in(USUARIOS_FILE);
    if (!in.is_open()) { cout << "No se puede abrir archivo.\n"; return; }
    ofstream temp("tmp_usuarios.txt");
    if (!temp.is_open()) { cout << "Error creando archivo temporal.\n"; in.close(); return; }
    string header; getline(in, header); temp << header << endl;
    string linea; bool eliminado = false;
    while (getline(in, linea)) {
        if (linea.empty()) continue;
        string campos[3]; splitLinea(linea, campos, 3);
        if (campos[0] == idStr) eliminado = true;
        else temp << linea << endl;
    }
    in.close(); temp.close();
    if (!eliminado) { remove("tmp_usuarios.txt"); cout << "No se encontró usuario.\n"; return; }
    remove(USUARIOS_FILE.c_str()); rename("tmp_usuarios.txt", USUARIOS_FILE.c_str());
    cout << "Usuario eliminado.\n";
}

// --------------------- Operaciones préstamos ---------------------

bool libroDisponible(int idLibro) {
    ifstream in(PRESTAMOS_FILE);
    if (!in.is_open()) return true;
    string linea; getline(in, linea);
    while (getline(in, linea)) {
        if (linea.empty()) continue;
        string campos[5]; splitLinea(linea, campos, 5);
        int libroID = stoi(campos[2]);
        if (libroID == idLibro && campos[4].empty()) { in.close(); return false; }
    }
    in.close(); return true;
}

void registrarPrestamo() {
    cout << "\n-- Registrar préstamo --\nID Usuario: "; string idUsuarioStr; getline(cin, idUsuarioStr);
    cout << "ID Libro: "; string idLibroStr; getline(cin, idLibroStr);
    cout << "Fecha de préstamo (dd/mm/aaaa): "; string fechaPrestamo; getline(cin, fechaPrestamo);

    int idPrestamo = obtenerSiguienteID(PRESTAMOS_FILE);
    int idUsuario = stoi(idUsuarioStr);
    int idLibro = stoi(idLibroStr);

    if (!libroDisponible(idLibro)) { cout << "Libro no disponible.\n"; return; }

    ofstream out(PRESTAMOS_FILE, ios::app);
    if (!out.is_open()) { cout << "Error archivo préstamos.\n"; return; }
    out << idPrestamo << "|" << idUsuario << "|" << idLibro << "|" << fechaPrestamo << "|" << "" << endl;
    out.close(); cout << "Préstamo registrado.\n";
}

void registrarDevolucion() {
    cout << "\n-- Registrar devolución --\nID Libro a devolver: ";
    string idLibroStr; getline(cin, idLibroStr);
    ifstream in(PRESTAMOS_FILE);
    if (!in.is_open()) { cout << "Error archivo préstamos.\n"; return; }
    ofstream temp("tmp_prestamos.txt");
    if (!temp.is_open()) { cout << "Error archivo temporal.\n"; in.close(); return; }
    string header; getline(in, header); temp << header << endl;
    string linea; bool encontrado = false;
    while (getline(in, linea)) {
        if (linea.empty()) continue;
        string campos[5]; splitLinea(linea, campos, 5);
        if (campos[2] == idLibroStr && campos[4].empty()) {
            encontrado = true;
            cout << "Fecha de devolución (dd/mm/aaaa): ";
            getline(cin, campos[4]);
        }
        temp << campos[0] << "|" << campos[1] << "|" << campos[2] << "|" << campos[3] << "|" << campos[4] << endl;
    }
    in.close(); temp.close();
    if (!encontrado) { remove("tmp_prestamos.txt"); cout << "No se encontró préstamo.\n"; return; }
    remove(PRESTAMOS_FILE.c_str()); rename("tmp_prestamos.txt", PRESTAMOS_FILE.c_str());
    cout << "Devolución registrada.\n";
}

void consultarPrestamosUsuario() {
    cout << "\n-- Consultar préstamos de usuario --\nID Usuario: ";
    string idUsuarioStr; getline(cin, idUsuarioStr); int idUsuario = stoi(idUsuarioStr);
    ifstream in(PRESTAMOS_FILE);
    if (!in.is_open()) { cout << "No se puede abrir archivo.\n"; return; }
    string linea; getline(in, linea);
    bool tienePrestamos = false;
    while (getline(in, linea)) {
        if (linea.empty()) continue;
        string campos[5]; splitLinea(linea, campos, 5);
        int usuarioID = stoi(campos[1]);
        if (usuarioID == idUsuario) {
            tienePrestamos = true;
            cout << "ID Préstamo: " << campos[0] << " | Libro ID: " << campos[2]
                << " | Fecha Préstamo: " << campos[3]
                << " | Fecha Devolución: " << (campos[4].empty() ? "Aún no devuelto" : campos[4]) << endl;
        }
    }
    if (!tienePrestamos) cout << "Usuario no tiene préstamos.\n";
    in.close();
}

// --------------------- Consultas avanzadas ---------------------

void consultarLibros() {
    cout << "\n-- Consultar libros --\nBuscar por:\n1 - Título\n2 - Autor\n3 - Categoría\nOpción: ";
    string opcion; getline(cin, opcion);
    cout << "Ingrese texto a buscar: "; string busqueda; getline(cin, busqueda);
    ifstream in(LIBROS_FILE);
    if (!in.is_open()) { cout << "Error archivo libros.\n"; return; }
    string linea; getline(in, linea);
    bool encontrado = false;
    while (getline(in, linea)) {
        if (linea.empty()) continue;
        string campos[5]; splitLinea(linea, campos, 5);
        bool match = false;
        if (opcion == "1" && campos[1].find(busqueda) != string::npos) match = true;
        if (opcion == "2" && campos[2].find(busqueda) != string::npos) match = true;
        if (opcion == "3" && campos[4].find(busqueda) != string::npos) match = true;
        if (match) {
            encontrado = true;
            cout << "ID: " << campos[0] << " | Título: " << campos[1] << " | Autor: " << campos[2]
                << " | Año: " << campos[3] << " | Cat: " << campos[4] << endl;
        }
    }
    if (!encontrado) cout << "No se encontraron libros.\n";
    in.close();
}

void consultarUsuarios() {
    cout << "\n-- Consultar usuarios --\nBuscar por:\n1 - ID\n2 - Nombre\nOpción: ";
    string opcion; getline(cin, opcion);
    cout << "Ingrese texto a buscar: "; string busqueda; getline(cin, busqueda);
    ifstream in(USUARIOS_FILE);
    if (!in.is_open()) { cout << "Error archivo usuarios.\n"; return; }
    string linea; getline(in, linea);
    bool encontrado = false;
    while (getline(in, linea)) {
        if (linea.empty()) continue;
        string campos[3]; splitLinea(linea, campos, 3);
        bool match = false;
        if (opcion == "1" && campos[0] == busqueda) match = true;
        if (opcion == "2" && campos[1].find(busqueda) != string::npos) match = true;
        if (match) {
            encontrado = true;
            cout << "ID: " << campos[0] << " | Nombre: " << campos[1] << " | Correo: " << campos[2] << endl;
        }
    }
    if (!encontrado) cout << "No se encontraron usuarios.\n";
    in.close();
}

// --------------------- Menú principal ---------------------

void menu() {
    asegurarArchivoConEncabezado(LIBROS_FILE, "ID|Titulo|Autor|Año|Categoria");
    asegurarArchivoConEncabezado(USUARIOS_FILE, "ID|Nombre|Correo");
    asegurarArchivoConEncabezado(PRESTAMOS_FILE, "IDPrestamo|IDUsuario|IDLibro|FechaPrestamo|FechaDevolucion");

    while (true) {
        cout << "\n--- Sistema Biblioteca ---\n";
        cout << "1 - Agregar libro\n2 - Modificar libro\n3 - Eliminar libro\n4 - Listar libros\n";
        cout << "5 - Agregar usuario\n6 - Modificar usuario\n7 - Eliminar usuario\n8 - Listar usuarios\n";
        cout << "9 - Registrar préstamo\n10 - Registrar devolución\n11 - Consultar préstamos de usuario\n";
        cout << "12 - Consultar libros\n13 - Consultar usuarios\n0 - Salir\nOpción: ";
        string opcion; getline(cin, opcion);
        if (opcion == "1") agregarLibro();
        else if (opcion == "2") modificarLibro();
        else if (opcion == "3") eliminarLibro();
        else if (opcion == "4") listarLibros();
        else if (opcion == "5") agregarUsuario();
        else if (opcion == "6") modificarUsuario();
        else if (opcion == "7") eliminarUsuario();
        else if (opcion == "8") listarUsuarios();
        else if (opcion == "9") registrarPrestamo();
        else if (opcion == "10") registrarDevolucion();
        else if (opcion == "11") consultarPrestamosUsuario();
        else if (opcion == "12") consultarLibros();
        else if (opcion == "13") consultarUsuarios();
        else if (opcion == "0") { cout << "Saliendo...\n"; break; }
        else cout << "Opción inválida.\n";
    }
}

// --------------------- Main ---------------------

int main() {
    menu();
    return 0;
}
