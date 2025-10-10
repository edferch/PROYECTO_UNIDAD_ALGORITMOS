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

    cout << endl << "=== AGREGAR UN NUEVO LIBRO (ID " << nuevoLibro.id << ") ===" << endl;
    cout << "Titulo: "; getline(cin, nuevoLibro.titulo);
    cout << "Autor: "; getline(cin, nuevoLibro.autor);
    cout << "Año: "; getline(cin, nuevoLibro.año);
    cout << "Categoria: "; getline(cin, nuevoLibro.categoria);

    ofstream escribirArchivo(ArchivoLibros, ios::app);
    if (!escribirArchivo.is_open()) { cout << "Error al abrir archivo" << endl; return; }
    escribirArchivo << nuevoLibro.id << "|" << nuevoLibro.titulo << "|" << nuevoLibro.autor << "|" << nuevoLibro.año << "|" << nuevoLibro.categoria << endl;
    escribirArchivo.close();
    cout << endl << "Libro agregado correctamente" << endl;
}

void listarLibros() {
    ifstream leerArchivo(ArchivoLibros);
    if (!leerArchivo.is_open()) { cout << "No se puede abrir archivo" << endl; return; }
    string texto;
    if (!getline(leerArchivo, texto)) { cout << "Archivo de libros vacío" << endl; leerArchivo.close(); return; }
    cout << endl << "===LIBROS=== " << endl;
    while (getline(leerArchivo, texto)) {
        if (texto.empty()) continue;
        string campos[5];
        separarCampos(texto, campos, 5);
        cout << "ID: " << campos[0] << " | Titulo: " << campos[1] << " | Autor: " << campos[2]
            << " | Año: " << campos[3] << " | Cat: " << campos[4] << endl;
    }
    leerArchivo.close();
}

void modificarLibro() {
    cout << endl << "===MODIFICAR LIBRO=== \n Ingrese ID: ";
    string idStr; getline(cin, idStr);
    ifstream leerArchivo(ArchivoLibros);
    if (!leerArchivo.is_open()) { cout << "No se puede abrir archivo" << endl; return; }
    ofstream escribirTemp("tmp_libros.txt");
    if (!escribirTemp.is_open()) { cout << "Error creando archivo temporal" << endl; leerArchivo.close(); return; }
    string header; getline(leerArchivo, header); escribirTemp << header << endl;
    string texto; bool encontrado = false;
    while (getline(leerArchivo, texto)) {
        if (texto.empty()) continue;
        string campos[5];
        separarCampos(texto, campos, 5);
        if (campos[0] == idStr) {
            encontrado = true;
            string tituloN, autorN, anioN, categoriaN;
            cout << "Titulo (actual: " << campos[1] << "): "; getline(cin, tituloN);
            cout << "Autor (actual: " << campos[2] << "): "; getline(cin, autorN);
            cout << "Año (actual: " << campos[3] << "): "; getline(cin, anioN);
            cout << "Categoria (actual: " << campos[4] << "): "; getline(cin, categoriaN);
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
    if (!encontrado) { remove("tmp_libros.txt"); cout << "No se encontro el libro" << endl; return; }
    remove(ArchivoLibros.c_str());
    rename("tmp_libros.txt", ArchivoLibros.c_str());
    cout << endl << "LIBRO MODIFICADO CORRECTAMENTE" << endl;
}

void eliminarLibro() {
    cout << endl << "===ELIMINAR LIBRO=== \nIngrese ID: ";
    string idStr; getline(cin, idStr);
    ifstream leerArchivo(ArchivoLibros);
    if (!leerArchivo.is_open()) { cout << "No se puede abrir archivo" << endl; return; }
    ofstream escribirTemp("tmp_libros.txt");
    if (!escribirTemp.is_open()) { cout << "Error creando archivo temporal" << endl; leerArchivo.close(); return; }
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
    if (!eliminado) { remove("tmp_libros.txt"); cout << "No se encontro el libro" << endl; return; }
    remove(ArchivoLibros.c_str());
    rename("tmp_libros.txt", ArchivoLibros.c_str());
    cout << endl << "LIBRO ELIMINADO CORRECTAMENTE" << endl;
}

//Usuarios
void agregarUsuario() {
    Usuario nuevoUsuario; nuevoUsuario.id = generarID(ArchivoUsuarios);
    cout << endl << "===REGISTRAR USUARIO (ID " << nuevoUsuario.id << ")===" << endl;
    cout << "Nombre: "; getline(cin, nuevoUsuario.nombre);
    cout << "Correo: "; getline(cin, nuevoUsuario.correo);
    ofstream escribirArchivo(ArchivoUsuarios, ios::app);
    if (!escribirArchivo.is_open()) { cout << "Error al abrir archivo" << endl; return; }
    escribirArchivo << nuevoUsuario.id << "|" << nuevoUsuario.nombre << "|" << nuevoUsuario.correo << endl;
    escribirArchivo.close();
    cout << endl << "USUARIO REGISTRADO CORRECTAMENTE" << endl;
}

void listarUsuarios() {
    ifstream leerArchivo(ArchivoUsuarios);
    if (!leerArchivo.is_open()) { cout << "No se puede abrir archivo" << endl; return; }
    string texto;
    if (!getline(leerArchivo, texto)) { cout << "Archivo de usuarios vacío" << endl; leerArchivo.close(); return; }
    cout << endl << "===USUARIOS===" << endl;
    while (getline(leerArchivo, texto)) {
        if (texto.empty()) continue;
        string campos[3];
        separarCampos(texto, campos, 3);
        cout << "ID: " << campos[0] << " | Nombre: " << campos[1] << " | Correo: " << campos[2] << endl;
    }
    leerArchivo.close();
}

void modificarUsuario() {
    cout << endl <<  "===MODIFICAR USUARIO=== \nIngrese ID: ";
    string idStr; getline(cin, idStr);
    ifstream leerArchivo(ArchivoUsuarios);
    if (!leerArchivo.is_open()) { cout << "No se puede abrir archivo" << endl; return; }
    ofstream escribirTemp("tmp_usuarios.txt");
    if (!escribirTemp.is_open()) { cout << "Error creando archivo temporal" << endl; leerArchivo.close(); return; }
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
    if (!encontrado) { remove("tmp_usuarios.txt"); cout << "No se encontro al usuario" << endl; return; }
    remove(ArchivoUsuarios.c_str());
    rename("tmp_usuarios.txt", ArchivoUsuarios.c_str());
    cout << endl << "USUARIO MODIFICADO CORRECTAMENTE" << endl;
}

void eliminarUsuario() {
    cout << endl << "===ELIMINAR USUARIO=== \nIngrese ID: ";
    string idStr; getline(cin, idStr);
    ifstream leerArchivo(ArchivoUsuarios);
    if (!leerArchivo.is_open()) { cout << "No se puede abrir archivo" << endl; return; }
    ofstream escribirTemp("tmp_usuarios.txt");
    if (!escribirTemp.is_open()) { cout << "Error creando archivo temporal" << endl; leerArchivo.close(); return; }
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
    if (!eliminado) { remove("tmp_usuarios.txt"); cout << "No se encontro al usuario" << endl; return; }
    remove(ArchivoUsuarios.c_str());
    rename("tmp_usuarios.txt", ArchivoUsuarios.c_str());
    cout << endl << "USUARIO ELIMINADO CORRECTAMENTE" << endl;
}

//Prestamos
bool libroDisponible(int idLibro) {
    ifstream leerArchivo(ArchivoPrestamos);
    if (!leerArchivo.is_open()) return true;
    string texto; getline(leerArchivo, texto);
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
    cout << endl << "===REGISTRAR PRESTAMO===" << endl;
    cout << "ID Usuario: "; string idUsuarioStr; getline(cin, idUsuarioStr);
    cout << "ID Libro: "; string idLibroStr; getline(cin, idLibroStr);
    cout << "Fecha de prestamo (dia/mes/año): "; string fechaPrestamo; getline(cin, fechaPrestamo);

    int idPrestamo = generarID(ArchivoPrestamos);
    int idUsuario = 0;
    int idLibro = 0;
    try { idUsuario = stoi(idUsuarioStr); }
    catch (...) { idUsuario = 0; }
    try { idLibro = stoi(idLibroStr); }
    catch (...) { idLibro = 0; }

    if (!libroDisponible(idLibro)) { cout << "El libro ID " << idLibro << " no esta disponible" << endl; return; }

    ofstream escribirArchivo(ArchivoPrestamos, ios::app);
    if (!escribirArchivo.is_open()) { cout << "Error abriendo archivo de prestamos." << endl; return; }
    escribirArchivo << idPrestamo << "|" << idUsuario << "|" << idLibro << "|" << fechaPrestamo << "|" << "" << endl;
    escribirArchivo.close();
    cout << endl << "PRESTAMO REGISTRADO CORRECTAMENTE" << endl;
}

void registrarDevolucion() {
    cout << endl << "===REGISTRAR DEVOLUCION===" << endl;
    cout << "ID Libro a devolver: ";
    string idLibroStr; getline(cin, idLibroStr);
    ifstream leerArchivo(ArchivoPrestamos);
    if (!leerArchivo.is_open()) { cout << "No se puede abrir archivo de prestamos" << endl; return; }
    ofstream escribirTemp("tmp_prestamos.txt");
    if (!escribirTemp.is_open()) { cout << "Error creando archivo temporal" << endl; leerArchivo.close(); return; }
    string header; getline(leerArchivo, header); escribirTemp << header << endl;
    string texto; bool encontrado = false;
    while (getline(leerArchivo, texto)) {
        if (texto.empty()) continue;
        string campos[5];
        separarCampos(texto, campos, 5);
        if (campos[2] == idLibroStr && campos[4].empty()) {
            encontrado = true;
            cout << "Ingrese fecha de devolucion (dia/mes/año): ";
            getline(cin, campos[4]);
        }
        escribirTemp << campos[0] << "|" << campos[1] << "|" << campos[2] << "|" << campos[3] << "|" << campos[4] << endl;
    }
    leerArchivo.close();
    escribirTemp.close();
    if (!encontrado) { remove("tmp_prestamos.txt"); cout << "No se encontro prestamo activo para el libro ID " << idLibroStr << "." << endl; return; }
    remove(ArchivoPrestamos.c_str());
    rename("tmp_prestamos.txt", ArchivoPrestamos.c_str());
    cout << endl << "DEVOLUCION REGISTRADA CORRECTAMENTE" << endl;
}

void consultarPrestamosUsuario() {
    cout << endl << "===CONSULTAR PRESTAMO DE USUARIO===" << endl;
    cout << "ID Usuario: ";
    string idUsuarioStr; getline(cin, idUsuarioStr);
    int idUsuario = 0;
    try { idUsuario = stoi(idUsuarioStr); }
    catch (...) { idUsuario = -1; }

    ifstream leerArchivo(ArchivoPrestamos);
    if (!leerArchivo.is_open()) { cout << "No se puede abrir archivo de prestamos" << endl; return; }
    string texto; getline(leerArchivo, texto);
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
            cout << "ID Prestamo: " << campos[0] << " | Libro ID: " << campos[2]
                << " | Fecha Prestamo: " << campos[3] << " | Fecha Devolucion: "
                << (campos[4].empty() ? "Aun no devuelto" : campos[4]) << endl;
        }
    }
    if (!tienePrestamos) cout << "EL USUARIO NO TIENE PRESTAMOS" << endl;
    leerArchivo.close();
}

//Consultas
void consultarLibros() {
    cout << endl << "===CONSULTAR LIBROS===" << endl;
    cout << "Buscar por:\n1 - Titulo\n2 - Autor\n3 - Categoria\nOpcion: ";
    string opcion; getline(cin, opcion);
    cout << "Ingrese texto a buscar: ";
    string busqueda; getline(cin, busqueda);

    ifstream leerArchivo(ArchivoLibros);
    if (!leerArchivo.is_open()) { cout << "No se puede abrir archivo de libros" << endl; return; }
    string texto; getline(leerArchivo, texto);
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
            cout << "ID: " << campos[0] << " | Titulo: " << campos[1] << " | Autor: " << campos[2]
                << " | Año: " << campos[3] << " | Cat: " << campos[4] << endl;
        }
    }
    if (!encontrado) cout << "NO SE ENCONTRARON LIBROS QUE COINCIDAB" << endl;
    leerArchivo.close();
}

void consultarUsuarios() {
    cout << endl << "===CONSULTAR USUARIO===" << endl;
    cout << "Buscar por:\n1 - ID\n2 - Nombre\nOpcion: ";
    string opcion; getline(cin, opcion);
    cout << "Ingrese texto a buscar: ";
    string busqueda; getline(cin, busqueda);

    ifstream leerArchivo(ArchivoUsuarios);
    if (!leerArchivo.is_open()) { cout << "No se puede abrir archivo de usuarios" << endl; return; }
    string texto; getline(leerArchivo, texto);
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
    if (!encontrado) cout << "NO SE ENCONTRARON USUARIOS QUE COINCIDAN" << endl;
    leerArchivo.close();
}

void listarLibrosDisponibles() {
    cout << endl << "===LIBROS DISPONIBLES===" << endl;
    ifstream leerArchivo(ArchivoLibros);
    if (!leerArchivo.is_open()) { cout << "No se puede abrir archivo de libros" << endl; return; }
    string texto; getline(leerArchivo, texto);
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
            cout << "ID: " << campos[0] << " | Titulo: " << campos[1] << " | Autor: " << campos[2]
                << " | Año: " << campos[3] << " | Cat: " << campos[4] << endl;
        }
    }
    if (!alguno) cout << "NO HAY LIBROS DISPONIBLES" << endl;
    leerArchivo.close();
}

//MEnus
void menuLibros() {
    while (true) {
        cout << endl << "===MENU LIBROS===" << endl;
        cout << "1 - Agregar libro" << endl;
        cout << "2 - Modificar libro" << endl;
        cout << "3 - Eliminar libro" << endl;
        cout << "4 - Listar libros" << endl;
        cout << "0 - Volver al menu principal" << endl;
        cout << "Opcion: ";
        string op; getline(cin, op);
        if (op == "1") agregarLibro();
        else if (op == "2") modificarLibro();
        else if (op == "3") eliminarLibro();
        else if (op == "4") listarLibros();
        else if (op == "0") break;
        else cout << "OPCION INVALIDA" << endl;
    }
}

void menuUsuarios() {
    while (true) {
        cout << endl << "===MENU USUARIOS===" << endl;
        cout << "1 - Agregar usuario" << endl;
        cout << "2 - Modificar usuario" << endl;
        cout << "3 - Eliminar usuario" << endl;
        cout << "4 - Listar usuarios" << endl;
        cout << "0 - Volver al menu principal" << endl;
        cout << "Opcion: ";
        string op; getline(cin, op);
        if (op == "1") agregarUsuario();
        else if (op == "2") modificarUsuario();
        else if (op == "3") eliminarUsuario();
        else if (op == "4") listarUsuarios();
        else if (op == "0") break;
        else cout << "OPCION INVALIDA" << endl;
    }
}

void menuPrestamos() {
    while (true) {
        cout << endl << "===MENU PRESTAMOS===" << endl;
        cout << "1 - Registrar prestamo" << endl;
        cout << "2 - Registrar devolucin" << endl;
        cout << "0 - Volver al menu principal" << endl;
        cout << "Opcion: ";
        string op; getline(cin, op);
        if (op == "1") registrarPrestamo();
        else if (op == "2") registrarDevolucion();
        else if (op == "0") break;
        else cout << "OPCION INVALIDA" << endl;
    }
}

void menuConsultas() {
    while (true) {
        cout << endl << "===MENU PRESTAMOS===" << endl;
        cout << "1 - Consultar prestamos de un usuario" << endl;
        cout << "2 - Consultar libros por titulo/autor/categoria" << endl;
        cout << "3 - Consultar usuarios por nombre/ID" << endl;
        cout << "4 - Listar libros disponibles" << endl;
        cout << "0 - Volver al menu principal" << endl;
        cout << "Opcion: ";
        string op; getline(cin, op);
        if (op == "1") consultarPrestamosUsuario();
        else if (op == "2") consultarLibros();
        else if (op == "3") consultarUsuarios();
        else if (op == "4") listarLibrosDisponibles();
        else if (op == "0") break;
        else cout << "OPCION INVALIDA" << endl;
    }
}

//Iniciar sistema
void inicializarSistema() {
    crearArchivo(ArchivoLibros, "ID|Titulo|Autor|Año|Categoria");
    crearArchivo(ArchivoUsuarios, "ID|Nombre|Correo");
    crearArchivo(ArchivoPrestamos, "IDPrestamo|IDUsuario|IDLibro|FechaPrestamo|FechaDevolucion");
}

int main() {
    inicializarSistema();
    while (true) {
        cout << endl << "===MENU PRINCIPAL===" << endl;
        cout << "1 - Libros" << endl;
        cout << "2 - Usuarios" << endl;
        cout << "3 - Prestamos" << endl;
        cout << "4 - Consultas" << endl;
        cout << "0 - Salir" << endl;
        cout << "Opción: ";
        string opcion; getline(cin, opcion);
        if (opcion == "1") menuLibros();
        else if (opcion == "2") menuUsuarios();
        else if (opcion == "3") menuPrestamos();
        else if (opcion == "4") menuConsultas();
        else if (opcion == "0") { cout << "Saliendo..." << endl; break; }
        else cout << "OPCION INVALIDA" << endl;
    }
    return 0;
}
