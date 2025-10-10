#include <iostream>
#include <fstream>
#include <string>

using namespace std;

//nombre de los archivos
const string ArchivoLibros = "libros.txt";
const string ArchivoUsuarios = "usuarios.txt";
const string ArchivoPrestamos = "prestamos.txt";

//Estructuras
struct Libro {
    int id = 0;
    string titulo;
    string autor;
    string año;
    string categoria;
};

struct Usuario {
    int id = 0;
    string nombre;
    string correo;
};

struct Prestamo {
    int idPrestamo = 0;
    int idUsuario = 0;
    int idLibro = 0;
    string fechaPrestamo;
    string fechaDevolucion;
};


void crearArchivo(const string& nombre, const string& encabezado) {
    ifstream leerArchivo(nombre);
    if (!leerArchivo.is_open()) {
        ofstream escribirArchivo(nombre);
        escribirArchivo << encabezado << endl;
        cout << "Archivo creado: " << nombre << endl;
        return;
    }
    string texto;
    if (!getline(leerArchivo, texto) || texto.empty()) {
        leerArchivo.close();
        ofstream escribirArchivo(nombre, ios::trunc);
        escribirArchivo << encabezado << endl;
        cout << "Encabezado escrito en archivo existente: " << nombre << endl;
    }
}

int generarID(const string& nombreArchivo) {
    ifstream leerArchivo(nombreArchivo);
    if (!leerArchivo.is_open()) return 1;
    string texto;
    if (!getline(leerArchivo, texto)) { leerArchivo.close(); return 1; }
    int maxId = 0;
    while (getline(leerArchivo, texto)) {
        if (texto.empty()) continue;
        size_t posicion = texto.find('|');
        string idStr = (posicion == string::npos) ? texto : texto.substr(0, posicion);
        try {
            int id = stoi(idStr);
            if (id > maxId) maxId = id;
        }
        catch (...) {}
    }
    leerArchivo.close();
    return maxId + 1;
}

void separarCampos(const string& linea, string campos[], int maxCampos) {
    size_t prev = 0;
    int indice = 0;
    while (indice < maxCampos) {
        size_t posicion = linea.find('|', prev);
        if (posicion == string::npos) {
            campos[indice++] = linea.substr(prev);
            break;
        }
        campos[indice++] = linea.substr(prev, posicion - prev);
        prev = posicion + 1;
    }
    for (int i = indice; i < maxCampos; i++) campos[i] = "";
}

//Funciones del libro
void agregarLibro() {
    Libro nuevoLibro;
    nuevoLibro.id = generarID(ArchivoLibros);

    cout << endl << "== Agregar nuevo libro (ID " << nuevoLibro.id << ") ==" << endl;
    cout << "Titulo: "; getline(cin, nuevoLibro.titulo);
    cout << "Autor: "; getline(cin, nuevoLibro.autor);
    cout << "Año: "; getline(cin, nuevoLibro.año);
    cout << "Categoria: "; getline(cin, nuevoLibro.categoria);

    ofstream escribirArchivo(ArchivoLibros, ios::app);
    if (!escribirArchivo.is_open()) { cout << "Error al abrir archivo" << endl; return; }
    escribirArchivo << nuevoLibro.id << "|" << nuevoLibro.titulo << "|" << nuevoLibro.autor << "|" << nuevoLibro.año << "|" << nuevoLibro.categoria << endl;
    escribirArchivo.close();
    cout << "Libro agregado correctamente" << endl;
}

void listarLibros() {
    ifstream leerArchivo(ArchivoLibros);
    if (!leerArchivo.is_open()) { cout << "No se puede abrir archivo." << endl; return; }
    string texto;
    if (!getline(leerArchivo, texto)) { cout << "Archivo de libros vacío." << endl; leerArchivo.close(); return; }
    cout << "\n--- LIBROS ---\n";
    while (getline(leerArchivo, texto)) {
        if (texto.empty()) continue;
        string campos[5];
        separarCampos(texto, campos, 5);
        cout << "ID: " << campos[0] << " | Título: " << campos[1] << " | Autor: " << campos[2]
            << " | Año: " << campos[3] << " | Cat: " << campos[4] << endl;
    }
    leerArchivo.close();
}

void modificarLibro() {
    cout << "\n-- Modificar libro --\nIngrese ID: ";
    string idStr; getline(cin, idStr);
    ifstream leerArchivo(ArchivoLibros);
    if (!leerArchivo.is_open()) { cout << "No se puede abrir archivo." << endl; return; }
    ofstream escribirTemp("tmp_libros.txt");
    if (!escribirTemp.is_open()) { cout << "Error creando archivo temporal." << endl; leerArchivo.close(); return; }
    string header; getline(leerArchivo, header); escribirTemp << header << endl;
    string texto; bool encontrado = false;
    while (getline(leerArchivo, texto)) {
        if (texto.empty()) continue;
        string campos[5];
        separarCampos(texto, campos, 5);
        if (campos[0] == idStr) {
            encontrado = true;
            string tituloN, autorN, anioN, categoriaN;
            cout << "Título (actual: " << campos[1] << "): "; getline(cin, tituloN);
            cout << "Autor (actual: " << campos[2] << "): "; getline(cin, autorN);
            cout << "Año (actual: " << campos[3] << "): "; getline(cin, anioN);
            cout << "Categoría (actual: " << campos[4] << "): "; getline(cin, categoriaN);
            if (tituloN.empty()) tituloN = campos[1];
            if (autorN.empty()) autorN = campos[2];
            if (anioN.empty()) anioN = campos[3];
            if (categoriaN.empty()) categoriaN = campos[4];
            escribirTemp << campos[0] << "|" << tituloN << "|" << autorN << "|" << anioN << "|" << categoriaN << endl;
        }
        else {
            escribirTemp << texto << endl;
        }
    }
    leerArchivo.close();
    escribirTemp.close();
    if (!encontrado) { remove("tmp_libros.txt"); cout << "No se encontró libro." << endl; return; }
    remove(ArchivoLibros.c_str());
    rename("tmp_libros.txt", ArchivoLibros.c_str());
    cout << "Libro modificado correctamente." << endl;
}

void eliminarLibro() {
    cout << "\n-- Eliminar libro --\nIngrese ID: ";
    string idStr; getline(cin, idStr);
    ifstream leerArchivo(ArchivoLibros);
    if (!leerArchivo.is_open()) { cout << "No se puede abrir archivo." << endl; return; }
    ofstream escribirTemp("tmp_libros.txt");
    if (!escribirTemp.is_open()) { cout << "Error creando archivo temporal." << endl; leerArchivo.close(); return; }
    string header; getline(leerArchivo, header); escribirTemp << header << endl;
    string texto; bool eliminado = false;
    while (getline(leerArchivo, texto)) {
        if (texto.empty()) continue;
        string campos[5];
        separarCampos(texto, campos, 5);
        if (campos[0] == idStr) eliminado = true;
        else escribirTemp << texto << endl;
    }
    leerArchivo.close();
    escribirTemp.close();
    if (!eliminado) { remove("tmp_libros.txt"); cout << "No se encontró libro." << endl; return; }
    remove(ArchivoLibros.c_str());
    rename("tmp_libros.txt", ArchivoLibros.c_str());
    cout << "Libro eliminado correctamente." << endl;
}

// --------------------- Operaciones usuarios ---------------------

void agregarUsuario() {
    Usuario nuevoUsuario; nuevoUsuario.id = generarID(ArchivoUsuarios);
    cout << "\n-- Registrar usuario (ID " << nuevoUsuario.id << ") --" << endl;
    cout << "Nombre: "; getline(cin, nuevoUsuario.nombre);
    cout << "Correo: "; getline(cin, nuevoUsuario.correo);
    ofstream escribirArchivo(ArchivoUsuarios, ios::app);
    if (!escribirArchivo.is_open()) { cout << "Error al abrir archivo." << endl; return; }
    escribirArchivo << nuevoUsuario.id << "|" << nuevoUsuario.nombre << "|" << nuevoUsuario.correo << endl;
    escribirArchivo.close();
    cout << "Usuario registrado." << endl;
}

void listarUsuarios() {
    ifstream leerArchivo(ArchivoUsuarios);
    if (!leerArchivo.is_open()) { cout << "No se puede abrir archivo." << endl; return; }
    string texto;
    if (!getline(leerArchivo, texto)) { cout << "Archivo de usuarios vacío." << endl; leerArchivo.close(); return; }
    cout << "\n--- USUARIOS ---\n";
    while (getline(leerArchivo, texto)) {
        if (texto.empty()) continue;
        string campos[3];
        separarCampos(texto, campos, 3);
        cout << "ID: " << campos[0] << " | Nombre: " << campos[1] << " | Correo: " << campos[2] << endl;
    }
    leerArchivo.close();
}

void modificarUsuario() {
    cout << "\n-- Modificar usuario --\nIngrese ID: ";
    string idStr; getline(cin, idStr);
    ifstream leerArchivo(ArchivoUsuarios);
    if (!leerArchivo.is_open()) { cout << "No se puede abrir archivo." << endl; return; }
    ofstream escribirTemp("tmp_usuarios.txt");
    if (!escribirTemp.is_open()) { cout << "Error creando archivo temporal." << endl; leerArchivo.close(); return; }
    string header; getline(leerArchivo, header); escribirTemp << header << endl;
    string texto; bool encontrado = false;
    while (getline(leerArchivo, texto)) {
        if (texto.empty()) continue;
        string campos[3];
        separarCampos(texto, campos, 3);
        if (campos[0] == idStr) {
            encontrado = true;
            string nombreN, correoN;
            cout << "Nombre (actual: " << campos[1] << "): "; getline(cin, nombreN);
            cout << "Correo (actual: " << campos[2] << "): "; getline(cin, correoN);
            if (nombreN.empty()) nombreN = campos[1];
            if (correoN.empty()) correoN = campos[2];
            escribirTemp << campos[0] << "|" << nombreN << "|" << correoN << endl;
        }
        else {
            escribirTemp << texto << endl;
        }
    }
    leerArchivo.close();
    escribirTemp.close();
    if (!encontrado) { remove("tmp_usuarios.txt"); cout << "No se encontró usuario." << endl; return; }
    remove(ArchivoUsuarios.c_str());
    rename("tmp_usuarios.txt", ArchivoUsuarios.c_str());
    cout << "Usuario modificado." << endl;
}

void eliminarUsuario() {
    cout << "\n-- Eliminar usuario --\nIngrese ID: ";
    string idStr; getline(cin, idStr);
    ifstream leerArchivo(ArchivoUsuarios);
    if (!leerArchivo.is_open()) { cout << "No se puede abrir archivo." << endl; return; }
    ofstream escribirTemp("tmp_usuarios.txt");
    if (!escribirTemp.is_open()) { cout << "Error creando archivo temporal." << endl; leerArchivo.close(); return; }
    string header; getline(leerArchivo, header); escribirTemp << header << endl;
    string texto; bool eliminado = false;
    while (getline(leerArchivo, texto)) {
        if (texto.empty()) continue;
        string campos[3];
        separarCampos(texto, campos, 3);
        if (campos[0] == idStr) eliminado = true;
        else escribirTemp << texto << endl;
    }
    leerArchivo.close();
    escribirTemp.close();
    if (!eliminado) { remove("tmp_usuarios.txt"); cout << "No se encontró usuario." << endl; return; }
    remove(ArchivoUsuarios.c_str());
    rename("tmp_usuarios.txt", ArchivoUsuarios.c_str());
    cout << "Usuario eliminado." << endl;
}

// --------------------- Operaciones prestamos ---------------------

bool libroDisponible(int idLibro) {
    ifstream leerArchivo(ArchivoPrestamos);
    if (!leerArchivo.is_open()) return true; // si no hay archivo, disponible
    string texto; getline(leerArchivo, texto); // salto encabezado
    while (getline(leerArchivo, texto)) {
        if (texto.empty()) continue;
        string campos[5];
        separarCampos(texto, campos, 5);
        int libroID = 0;
        try { libroID = stoi(campos[2]); }
        catch (...) { libroID = -1; }
        if (libroID == idLibro && campos[4].empty()) { leerArchivo.close(); return false; }
    }
    leerArchivo.close();
    return true;
}

void registrarPrestamo() {
    cout << "\n-- Registrar préstamo --" << endl;
    cout << "ID Usuario: "; string idUsuarioStr; getline(cin, idUsuarioStr);
    cout << "ID Libro: "; string idLibroStr; getline(cin, idLibroStr);
    cout << "Fecha de préstamo (dd/mm/aaaa): "; string fechaPrestamo; getline(cin, fechaPrestamo);

    int idPrestamo = generarID(ArchivoPrestamos);
    int idUsuario = 0;
    int idLibro = 0;
    try { idUsuario = stoi(idUsuarioStr); }
    catch (...) { idUsuario = 0; }
    try { idLibro = stoi(idLibroStr); }
    catch (...) { idLibro = 0; }

    if (!libroDisponible(idLibro)) { cout << "El libro ID " << idLibro << " no está disponible." << endl; return; }

    ofstream escribirArchivo(ArchivoPrestamos, ios::app);
    if (!escribirArchivo.is_open()) { cout << "Error abriendo archivo de préstamos." << endl; return; }
    escribirArchivo << idPrestamo << "|" << idUsuario << "|" << idLibro << "|" << fechaPrestamo << "|" << "" << endl;
    escribirArchivo.close();
    cout << "Préstamo registrado correctamente." << endl;
}

void registrarDevolucion() {
    cout << "\n-- Registrar devolución --" << endl;
    cout << "ID Libro a devolver: ";
    string idLibroStr; getline(cin, idLibroStr);
    ifstream leerArchivo(ArchivoPrestamos);
    if (!leerArchivo.is_open()) { cout << "No se puede abrir archivo de préstamos." << endl; return; }
    ofstream escribirTemp("tmp_prestamos.txt");
    if (!escribirTemp.is_open()) { cout << "Error creando archivo temporal." << endl; leerArchivo.close(); return; }
    string header; getline(leerArchivo, header); escribirTemp << header << endl;
    string texto; bool encontrado = false;
    while (getline(leerArchivo, texto)) {
        if (texto.empty()) continue;
        string campos[5];
        separarCampos(texto, campos, 5);
        if (campos[2] == idLibroStr && campos[4].empty()) {
            encontrado = true;
            cout << "Ingrese fecha de devolución (dd/mm/aaaa): ";
            getline(cin, campos[4]);
        }
        escribirTemp << campos[0] << "|" << campos[1] << "|" << campos[2] << "|" << campos[3] << "|" << campos[4] << endl;
    }
    leerArchivo.close();
    escribirTemp.close();
    if (!encontrado) { remove("tmp_prestamos.txt"); cout << "No se encontró préstamo activo para el libro ID " << idLibroStr << "." << endl; return; }
    remove(ArchivoPrestamos.c_str());
    rename("tmp_prestamos.txt", ArchivoPrestamos.c_str());
    cout << "Devolución registrada correctamente." << endl;
}

void consultarPrestamosUsuario() {
    cout << "\n-- Consultar préstamos de usuario --" << endl;
    cout << "ID Usuario: ";
    string idUsuarioStr; getline(cin, idUsuarioStr);
    int idUsuario = 0;
    try { idUsuario = stoi(idUsuarioStr); }
    catch (...) { idUsuario = -1; }

    ifstream leerArchivo(ArchivoPrestamos);
    if (!leerArchivo.is_open()) { cout << "No se puede abrir archivo de préstamos." << endl; return; }
    string texto; getline(leerArchivo, texto); // encabezado
    bool tienePrestamos = false;
    while (getline(leerArchivo, texto)) {
        if (texto.empty()) continue;
        string campos[5];
        separarCampos(texto, campos, 5);
        int usuarioID = 0;
        try { usuarioID = stoi(campos[1]); }
        catch (...) { usuarioID = -1; }
        if (usuarioID == idUsuario) {
            tienePrestamos = true;
            cout << "ID Préstamo: " << campos[0] << " | Libro ID: " << campos[2]
                << " | Fecha Préstamo: " << campos[3] << " | Fecha Devolución: "
                << (campos[4].empty() ? "Aún no devuelto" : campos[4]) << endl;
        }
    }
    if (!tienePrestamos) cout << "El usuario no tiene préstamos." << endl;
    leerArchivo.close();
}

// --------------------- Consultas avanzadas ---------------------

void consultarLibros() {
    cout << "\n-- Consultar libros --" << endl;
    cout << "Buscar por:\n1 - Título\n2 - Autor\n3 - Categoría\nOpción: ";
    string opcion; getline(cin, opcion);
    cout << "Ingrese texto a buscar: ";
    string busqueda; getline(cin, busqueda);

    ifstream leerArchivo(ArchivoLibros);
    if (!leerArchivo.is_open()) { cout << "No se puede abrir archivo de libros." << endl; return; }
    string texto; getline(leerArchivo, texto); // encabezado
    bool encontrado = false;
    while (getline(leerArchivo, texto)) {
        if (texto.empty()) continue;
        string campos[5];
        separarCampos(texto, campos, 5);
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
    if (!encontrado) cout << "No se encontraron libros que coincidan." << endl;
    leerArchivo.close();
}

void consultarUsuarios() {
    cout << "\n-- Consultar usuarios --" << endl;
    cout << "Buscar por:\n1 - ID\n2 - Nombre\nOpción: ";
    string opcion; getline(cin, opcion);
    cout << "Ingrese texto a buscar: ";
    string busqueda; getline(cin, busqueda);

    ifstream leerArchivo(ArchivoUsuarios);
    if (!leerArchivo.is_open()) { cout << "No se puede abrir archivo de usuarios." << endl; return; }
    string texto; getline(leerArchivo, texto); // encabezado
    bool encontrado = false;
    while (getline(leerArchivo, texto)) {
        if (texto.empty()) continue;
        string campos[3];
        separarCampos(texto, campos, 3);
        bool match = false;
        if (opcion == "1" && campos[0] == busqueda) match = true;
        if (opcion == "2" && campos[1].find(busqueda) != string::npos) match = true;
        if (match) {
            encontrado = true;
            cout << "ID: " << campos[0] << " | Nombre: " << campos[1] << " | Correo: " << campos[2] << endl;
        }
    }
    if (!encontrado) cout << "No se encontraron usuarios que coincidan." << endl;
    leerArchivo.close();
}

void listarLibrosDisponibles() {
    cout << "\n--- Libros disponibles ---" << endl;
    ifstream leerArchivo(ArchivoLibros);
    if (!leerArchivo.is_open()) { cout << "No se puede abrir archivo de libros." << endl; return; }
    string texto; getline(leerArchivo, texto); // encabezado
    bool alguno = false;
    while (getline(leerArchivo, texto)) {
        if (texto.empty()) continue;
        string campos[5];
        separarCampos(texto, campos, 5);
        int libroID = 0;
        try { libroID = stoi(campos[0]); }
        catch (...) { libroID = -1; }
        if (libroDisponible(libroID)) {
            alguno = true;
            cout << "ID: " << campos[0] << " | Título: " << campos[1] << " | Autor: " << campos[2]
                << " | Año: " << campos[3] << " | Cat: " << campos[4] << endl;
        }
    }
    if (!alguno) cout << "No hay libros disponibles actualmente." << endl;
    leerArchivo.close();
}

// --------------------- Menús (principal + submenús) ---------------------

void menuLibros() {
    while (true) {
        cout << "\n--- Menú Libros ---\n";
        cout << "1 - Agregar libro\n";
        cout << "2 - Modificar libro\n";
        cout << "3 - Eliminar libro\n";
        cout << "4 - Listar libros\n";
        cout << "0 - Volver al menú principal\n";
        cout << "Opción: ";
        string op; getline(cin, op);
        if (op == "1") agregarLibro();
        else if (op == "2") modificarLibro();
        else if (op == "3") eliminarLibro();
        else if (op == "4") listarLibros();
        else if (op == "0") break;
        else cout << "Opción inválida.\n";
    }
}

void menuUsuarios() {
    while (true) {
        cout << "\n--- Menú Usuarios ---\n";
        cout << "1 - Agregar usuario\n";
        cout << "2 - Modificar usuario\n";
        cout << "3 - Eliminar usuario\n";
        cout << "4 - Listar usuarios\n";
        cout << "0 - Volver al menú principal\n";
        cout << "Opción: ";
        string op; getline(cin, op);
        if (op == "1") agregarUsuario();
        else if (op == "2") modificarUsuario();
        else if (op == "3") eliminarUsuario();
        else if (op == "4") listarUsuarios();
        else if (op == "0") break;
        else cout << "Opción inválida.\n";
    }
}

void menuPrestamos() {
    while (true) {
        cout << "\n--- Menú Préstamos ---\n";
        cout << "1 - Registrar préstamo\n";
        cout << "2 - Registrar devolución\n";
        cout << "0 - Volver al menú principal\n";
        cout << "Opción: ";
        string op; getline(cin, op);
        if (op == "1") registrarPrestamo();
        else if (op == "2") registrarDevolucion();
        else if (op == "0") break;
        else cout << "Opción inválida.\n";
    }
}

void menuConsultas() {
    while (true) {
        cout << "\n--- Menú Consultas ---\n";
        cout << "1 - Consultar préstamos de un usuario\n";
        cout << "2 - Consultar libros por título/autor/categoría\n";
        cout << "3 - Consultar usuarios por nombre/ID\n";
        cout << "4 - Listar libros disponibles\n";
        cout << "0 - Volver al menú principal\n";
        cout << "Opción: ";
        string op; getline(cin, op);
        if (op == "1") consultarPrestamosUsuario();
        else if (op == "2") consultarLibros();
        else if (op == "3") consultarUsuarios();
        else if (op == "4") listarLibrosDisponibles();
        else if (op == "0") break;
        else cout << "Opción inválida.\n";
    }
}

// --------------------- Inicialización y main ---------------------

void inicializarSistema() {
    crearArchivo(ArchivoLibros, "ID|Título|Autor|Año|Categoría");
    crearArchivo(ArchivoUsuarios, "ID|Nombre|Correo");
    crearArchivo(ArchivoPrestamos, "IDPrestamo|IDUsuario|IDLibro|FechaPrestamo|FechaDevolucion");
}

int main() {
    inicializarSistema();
    while (true) {
        cout << "\n=== Menú Principal ===\n";
        cout << "1 - Libros\n";
        cout << "2 - Usuarios\n";
        cout << "3 - Préstamos\n";
        cout << "4 - Consultas\n";
        cout << "0 - Salir\n";
        cout << "Opción: ";
        string opcion; getline(cin, opcion);
        if (opcion == "1") menuLibros();
        else if (opcion == "2") menuUsuarios();
        else if (opcion == "3") menuPrestamos();
        else if (opcion == "4") menuConsultas();
        else if (opcion == "0") { cout << "Saliendo. ¡Hasta luego!" << endl; break; }
        else cout << "Opción inválida.\n";
    }
    return 0;
}
