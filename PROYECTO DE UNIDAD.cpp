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
        out.close();
        cout << "Archivo creado: " << nombre << endl;
    }
    else {
        string linea;
        if (!getline(in, linea) || linea.empty()) {
            in.close();
            ofstream out(nombre, ios::trunc);
            out << encabezado << endl;
            out.close();
            cout << "Encabezado escrito en archivo existente: " << nombre << endl;
        }
        else {
            in.close();
        }
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

// Divide la linea separada por '|' en un arreglo de strings (maxCampos debe ser suficiente)
void splitLinea(const string& linea, string campos[], int maxCampos) {
    size_t prev = 0;
    int idx = 0;
    while (idx < maxCampos) {
        size_t pos = linea.find('|', prev);
        if (pos == string::npos) {
            campos[idx] = linea.substr(prev);
            break;
        }
        else {
            campos[idx] = linea.substr(prev, pos - prev);
            prev = pos + 1;
        }
        idx++;
    }
    // Rellenar restantes con cadena vacía (por si)
    for (int i = 0; i < maxCampos; ++i) {
        if (campos[i].size() == 0) { /* ok */ }
    }
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
    if (!out.is_open()) {
        cout << "Error: no se puede abrir " << LIBROS_FILE << " para escribir.\n";
        return;
    }
    out << lb.id << "|" << lb.titulo << "|" << lb.autor << "|" << lb.anio << "|" << lb.categoria << endl;
    out.close();
    cout << "Libro agregado correctamente.\n";
}

void listarLibros() {
    ifstream in(LIBROS_FILE);
    if (!in.is_open()) {
        cout << "No se puede abrir " << LIBROS_FILE << ".\n";
        return;
    }
    string linea;
    if (getline(in, linea)) {
        cout << "\n--- LIBROS ---\n";
    }
    else {
        cout << "Archivo de libros vacío.\n";
        in.close();
        return;
    }
    while (getline(in, linea)) {
        if (linea.empty()) continue;
        string campos[5];
        for (int i = 0; i < 5; i++) campos[i] = "";
        splitLinea(linea, campos, 5);
        cout << "ID: " << campos[0]
            << " | Título: " << campos[1]
            << " | Autor: " << campos[2]
            << " | Año: " << campos[3]
            << " | Cat: " << campos[4]
            << endl;
    }
    in.close();
}

void modificarLibro() {
    cout << "\n-- Modificar libro --\n";
    cout << "Ingrese ID del libro a modificar: ";
    string idStr; getline(cin, idStr);
    if (idStr.empty()) { cout << "ID inválido.\n"; return; }

    ifstream in(LIBROS_FILE);
    if (!in.is_open()) { cout << "No se puede abrir " << LIBROS_FILE << ".\n"; return; }

    ofstream temp("tmp_libros.txt");
    if (!temp.is_open()) { cout << "Error creando archivo temporal.\n"; in.close(); return; }

    string header;
    if (!getline(in, header)) { cout << "Archivo de libros sin contenido.\n"; in.close(); temp.close(); return; }
    temp << header << endl;

    string linea;
    bool encontrado = false;
    while (getline(in, linea)) {
        if (linea.empty()) continue;
        string campos[5];
        for (int i = 0; i < 5; i++) campos[i] = "";
        splitLinea(linea, campos, 5);
        if (campos[0] == idStr) {
            encontrado = true;
            // pedir nuevos datos
            string titulo, autor, anio, categoria;
            cout << "Título (actual: " << campos[1] << "): "; getline(cin, titulo);
            cout << "Autor (actual: " << campos[2] << "): "; getline(cin, autor);
            cout << "Año (actual: " << campos[3] << "): "; getline(cin, anio);
            cout << "Categoría (actual: " << campos[4] << "): "; getline(cin, categoria);
            // si el usuario deja vacío, mantenemos el valor anterior
            if (titulo.empty()) titulo = campos[1];
            if (autor.empty()) autor = campos[2];
            if (anio.empty()) anio = campos[3];
            if (categoria.empty()) categoria = campos[4];
            temp << campos[0] << "|" << titulo << "|" << autor << "|" << anio << "|" << categoria << endl;
        }
        else {
            temp << linea << endl;
        }
    }

    in.close();
    temp.close();

    if (!encontrado) {
        remove("tmp_libros.txt");
        cout << "No se encontró libro con ID " << idStr << ".\n";
        return;
    }

    // reemplazar archivo original
    remove(LIBROS_FILE.c_str());
    rename("tmp_libros.txt", LIBROS_FILE.c_str());
    cout << "Libro modificado correctamente.\n";
}

void eliminarLibro() {
    cout << "\n-- Eliminar libro --\n";
    cout << "Ingrese ID del libro a eliminar: ";
    string idStr; getline(cin, idStr);
    if (idStr.empty()) { cout << "ID inválido.\n"; return; }

    ifstream in(LIBROS_FILE);
    if (!in.is_open()) { cout << "No se puede abrir " << LIBROS_FILE << ".\n"; return; }

    ofstream temp("tmp_libros.txt");
    if (!temp.is_open()) { cout << "Error creando archivo temporal.\n"; in.close(); return; }

    string header;
    if (!getline(in, header)) { cout << "Archivo de libros sin contenido.\n"; in.close(); temp.close(); return; }
    temp << header << endl;

    string linea;
    bool eliminado = false;
    while (getline(in, linea)) {
        if (linea.empty()) continue;
        string campos[5];
        for (int i = 0; i < 5; i++) campos[i] = "";
        splitLinea(linea, campos, 5);
        if (campos[0] == idStr) {
            eliminado = true;
            // simplemente no copiar la línea (efecto de eliminar)
        }
        else {
            temp << linea << endl;
        }
    }

    in.close();
    temp.close();

    if (!eliminado) {
        remove("tmp_libros.txt");
        cout << "No se encontró libro con ID " << idStr << ".\n";
        return;
    }

    remove(LIBROS_FILE.c_str());
    rename("tmp_libros.txt", LIBROS_FILE.c_str());
    cout << "Libro eliminado correctamente.\n";
}

// --------------------- Operaciones usuarios ---------------------

void agregarUsuario() {
    Usuario us;
    us.id = obtenerSiguienteID(USUARIOS_FILE);

    cout << "\n-- Registrar nuevo usuario (ID " << us.id << ") --\n";
    cout << "Nombre: "; getline(cin, us.nombre);
    cout << "Correo: "; getline(cin, us.correo);

    ofstream out(USUARIOS_FILE, ios::app);
    if (!out.is_open()) {
        cout << "Error: no se puede abrir " << USUARIOS_FILE << " para escribir.\n";
        return;
    }
    out << us.id << "|" << us.nombre << "|" << us.correo << endl;
    out.close();
    cout << "Usuario registrado correctamente.\n";
}

void listarUsuarios() {
    ifstream in(USUARIOS_FILE);
    if (!in.is_open()) {
        cout << "No se puede abrir " << USUARIOS_FILE << ".\n";
        return;
    }
    string linea;
    if (getline(in, linea)) {
        cout << "\n--- USUARIOS ---\n";
    }
    else {
        cout << "Archivo de usuarios vacío.\n";
        in.close();
        return;
    }
    while (getline(in, linea)) {
        if (linea.empty()) continue;
        string campos[3];
        for (int i = 0; i < 3; i++) campos[i] = "";
        splitLinea(linea, campos, 3);
        cout << "ID: " << campos[0] << " | Nombre: " << campos[1] << " | Correo: " << campos[2] << endl;
    }
    in.close();
}

void modificarUsuario() {
    cout << "\n-- Modificar usuario --\n";
    cout << "Ingrese ID del usuario a modificar: ";
    string idStr; getline(cin, idStr);
    if (idStr.empty()) { cout << "ID inválido.\n"; return; }

    ifstream in(USUARIOS_FILE);
    if (!in.is_open()) { cout << "No se puede abrir " << USUARIOS_FILE << ".\n"; return; }

    ofstream temp("tmp_usuarios.txt");
    if (!temp.is_open()) { cout << "Error creando archivo temporal.\n"; in.close(); return; }

    string header;
    if (!getline(in, header)) { cout << "Archivo de usuarios sin contenido.\n"; in.close(); temp.close(); return; }
    temp << header << endl;

    string linea;
    bool encontrado = false;
    while (getline(in, linea)) {
        if (linea.empty()) continue;
        string campos[3];
        for (int i = 0; i < 3; i++) campos[i] = "";
        splitLinea(linea, campos, 3);
        if (campos[0] == idStr) {
            encontrado = true;
            string nombre, correo;
            cout << "Nombre (actual: " << campos[1] << "): "; getline(cin, nombre);
            cout << "Correo (actual: " << campos[2] << "): "; getline(cin, correo);
            if (nombre.empty()) nombre = campos[1];
            if (correo.empty()) correo = campos[2];
            temp << campos[0] << "|" << nombre << "|" << correo << endl;
        }
        else {
            temp << linea << endl;
        }
    }

    in.close();
    temp.close();

    if (!encontrado) {
        remove("tmp_usuarios.txt");
        cout << "No se encontró usuario con ID " << idStr << ".\n";
        return;
    }

    remove(USUARIOS_FILE.c_str());
    rename("tmp_usuarios.txt", USUARIOS_FILE.c_str());
    cout << "Usuario modificado correctamente.\n";
}

void eliminarUsuario() {
    cout << "\n-- Eliminar usuario --\n";
    cout << "Ingrese ID del usuario a eliminar: ";
    string idStr; getline(cin, idStr);
    if (idStr.empty()) { cout << "ID inválido.\n"; return; }

    ifstream in(USUARIOS_FILE);
    if (!in.is_open()) { cout << "No se puede abrir " << USUARIOS_FILE << ".\n"; return; }

    ofstream temp("tmp_usuarios.txt");
    if (!temp.is_open()) { cout << "Error creando archivo temporal.\n"; in.close(); return; }

    string header;
    if (!getline(in, header)) { cout << "Archivo de usuarios sin contenido.\n"; in.close(); temp.close(); return; }
    temp << header << endl;

    string linea;
    bool eliminado = false;
    while (getline(in, linea)) {
        if (linea.empty()) continue;
        string campos[3];
        for (int i = 0; i < 3; i++) campos[i] = "";
        splitLinea(linea, campos, 3);
        if (campos[0] == idStr) {
            eliminado = true;
            // no copiar -> eliminar
        }
        else {
            temp << linea << endl;
        }
    }

    in.close();
    temp.close();

    if (!eliminado) {
        remove("tmp_usuarios.txt");
        cout << "No se encontró usuario con ID " << idStr << ".\n";
        return;
    }

    remove(USUARIOS_FILE.c_str());
    rename("tmp_usuarios.txt", USUARIOS_FILE.c_str());
    cout << "Usuario eliminado correctamente.\n";
}

// --------------------- Operaciones préstamos ---------------------

// Verifica si un libro está disponible (no tiene préstamo activo)
bool libroDisponible(int idLibro) {
    ifstream in(PRESTAMOS_FILE);
    if (!in.is_open()) return true; // si no hay archivo, disponible

    string linea;
    getline(in, linea); // salto encabezado
    while (getline(in, linea)) {
        if (linea.empty()) continue;
        string campos[5];
        for (int i = 0; i < 5; i++) campos[i] = "";
        splitLinea(linea, campos, 5);
        int libroID = stoi(campos[2]);
        string devolucion = campos[4];
        if (libroID == idLibro && devolucion.empty()) { // préstamo activo
            in.close();
            return false;
        }
    }
    in.close();
    return true;
}

// Registrar préstamo
void registrarPrestamo() {
    cout << "\n-- Registrar préstamo --\n";
    cout << "ID Usuario: ";
    string idUsuarioStr; getline(cin, idUsuarioStr);
    cout << "ID Libro: ";
    string idLibroStr; getline(cin, idLibroStr);
    cout << "Fecha de préstamo (dd/mm/aaaa): ";
    string fechaPrestamo; getline(cin, fechaPrestamo);

    int idPrestamo = obtenerSiguienteID(PRESTAMOS_FILE);
    int idUsuario = stoi(idUsuarioStr);
    int idLibro = stoi(idLibroStr);

    if (!libroDisponible(idLibro)) {
        cout << "El libro ID " << idLibro << " no está disponible.\n";
        return;
    }

    ofstream out(PRESTAMOS_FILE, ios::app);
    if (!out.is_open()) {
        cout << "Error abriendo archivo de préstamos.\n";
        return;
    }

    out << idPrestamo << "|" << idUsuario << "|" << idLibro << "|" << fechaPrestamo << "|" << "" << endl;
    out.close();
    cout << "Préstamo registrado correctamente.\n";
}

// Registrar devolución
void registrarDevolucion() {
    cout << "\n-- Registrar devolución --\n";
    cout << "ID Libro a devolver: ";
    string idLibroStr; getline(cin, idLibroStr);

    ifstream in(PRESTAMOS_FILE);
    if (!in.is_open()) { cout << "No se puede abrir archivo de préstamos.\n"; return; }

    ofstream temp("tmp_prestamos.txt");
    if (!temp.is_open()) { cout << "Error creando archivo temporal.\n"; in.close(); return; }

    string header;
    getline(in, header);
    temp << header << endl;

    string linea;
    bool encontrado = false;
    while (getline(in, linea)) {
        if (linea.empty()) continue;
        string campos[5]; for (int i = 0; i < 5; i++) campos[i] = "";
        splitLinea(linea, campos, 5);

        int libroID = stoi(campos[2]);
        string fechaDevolucion = campos[4];

        if (libroID == stoi(idLibroStr) && fechaDevolucion.empty()) {
            encontrado = true;
            cout << "Ingrese fecha de devolución (dd/mm/aaaa): ";
            string fechaDev; getline(cin, fechaDev);
            campos[4] = fechaDev;
        }
        temp << campos[0] << "|" << campos[1] << "|" << campos[2] << "|" << campos[3] << "|" << campos[4] << endl;
    }

    in.close();
    temp.close();

    if (!encontrado) {
        remove("tmp_prestamos.txt");
        cout << "No se encontró préstamo activo para el libro ID " << idLibroStr << ".\n";
        return;
    }

    remove(PRESTAMOS_FILE.c_str());
    rename("tmp_prestamos.txt", PRESTAMOS_FILE.c_str());
    cout << "Devolución registrada correctamente.\n";
}

// Consultar libros prestados de un usuario
void consultarPrestamosUsuario() {
    cout << "\n-- Consultar préstamos de usuario --\n";
    cout << "ID Usuario: ";
    string idUsuarioStr; getline(cin, idUsuarioStr);
    int idUsuario = stoi(idUsuarioStr);

    ifstream in(PRESTAMOS_FILE);
    if (!in.is_open()) { cout << "No se puede abrir archivo de préstamos.\n"; return; }

    string linea;
    getline(in, linea); // encabezado
    bool tienePrestamos = false;
    while (getline(in, linea)) {
        if (linea.empty()) continue;
        string campos[5]; for (int i = 0; i < 5; i++) campos[i] = "";
        splitLinea(linea, campos, 5);
        int usuarioID = stoi(campos[1]);
        if (usuarioID == idUsuario) {
            tienePrestamos = true;
            cout << "ID Préstamo: " << campos[0] << " | Libro ID: " << campos[2]
                << " | Fecha Préstamo: " << campos[3] << " | Fecha Devolución: "
                << (campos[4].empty() ? "Aún no devuelto" : campos[4]) << endl;
        }
    }
    if (!tienePrestamos) cout << "El usuario no tiene préstamos.\n";
    in.close();
}

// --------------------- Consultas avanzadas ---------------------

// Consultar libros por título, autor o categoría
void consultarLibros() {
    cout << "\n-- Consultar libros --\n";
    cout << "Buscar por:\n1 - Título\n2 - Autor\n3 - Categoría\nElige opción: ";
    string opcion; getline(cin, opcion);

    cout << "Ingrese texto a buscar: ";
    string busqueda; getline(cin, busqueda);

    ifstream in(LIBROS_FILE);
    if (!in.is_open()) { cout << "No se puede abrir archivo de libros.\n"; return; }

    string linea;
    getline(in, linea); // encabezado
    bool encontrado = false;
    while (getline(in, linea)) {
        if (linea.empty()) continue;
        string campos[5]; for (int i = 0; i < 5; i++) campos[i] = "";
        splitLinea(linea, campos, 5);

        bool match = false;
        if (opcion == "1" && campos[1].find(busqueda) != string::npos) match = true;
        else if (opcion == "2" && campos[2].find(busqueda) != string::npos) match = true;
        else if (opcion == "3" && campos[4].find(busqueda) != string::npos) match = true;

        if (match) {
            encontrado = true;
            cout << "ID: " << campos[0]
                << " | Título: " << campos[1]
                << " | Autor: " << campos[2]
                << " | Año: " << campos[3]
                << " | Cat: " << campos[4] << endl;
        }
    }
    if (!encontrado) cout << "No se encontraron libros que coincidan.\n";
    in.close();
}

// Consultar usuarios por nombre o ID
void consultarUsuarios() {
    cout << "\n-- Consultar usuarios --\n";
    cout << "Buscar por:\n1 - ID\n2 - Nombre\nElige opción: ";
    string opcion; getline(cin, opcion);

    cout << "Ingrese texto a buscar: ";
    string busqueda; getline(cin, busqueda);

    ifstream in(USUARIOS_FILE);
    if (!in.is_open()) { cout << "No se puede abrir archivo de usuarios.\n"; return; }

    string linea;
    getline(in, linea); // encabezado
    bool encontrado = false;
    while (getline(in, linea)) {
        if (linea.empty()) continue;
        string campos[3]; for (int i = 0; i < 3; i++) campos[i] = "";
        splitLinea(linea, campos, 3);

        bool match = false;
        if (opcion == "1" && campos[0] == busqueda) match = true;
        else if (opcion == "2" && campos[1].find(busqueda) != string::npos) match = true;

        if (match) {
            encontrado = true;
            cout << "ID: " << campos[0] << " | Nombre: " << campos[1] << " | Correo: " << campos[2] << endl;
        }
    }
    if (!encontrado) cout << "No se encontraron usuarios que coincidan.\n";
    in.close();
}

// Listar libros disponibles
void listarLibrosDisponibles() {
    cout << "\n--- Libros disponibles ---\n";
    ifstream in(LIBROS_FILE);
    if (!in.is_open()) { cout << "No se puede abrir archivo de libros.\n"; return; }

    string linea;
    getline(in, linea); // encabezado
    bool alguno = false;
    while (getline(in, linea)) {
        if (linea.empty()) continue;
        string campos[5]; for (int i = 0; i < 5; i++) campos[i] = "";
        splitLinea(linea, campos, 5);

        int libroID = stoi(campos[0]);
        if (libroDisponible(libroID)) {
            alguno = true;
            cout << "ID: " << campos[0]
                << " | Título: " << campos[1]
                << " | Autor: " << campos[2]
                << " | Año: " << campos[3]
                << " | Cat: " << campos[4] << endl;
        }
    }
    if (!alguno) cout << "No hay libros disponibles actualmente.\n";
    in.close();
}



// --------------------- Inicialización ---------------------

void inicializarSistema() {
    asegurarArchivoConEncabezado(LIBROS_FILE, "ID|Título|Autor|Año|Categoría");
    asegurarArchivoConEncabezado(USUARIOS_FILE, "ID|Nombre|Correo");
    asegurarArchivoConEncabezado(PRESTAMOS_FILE, "IDPrestamo|IDUsuario|IDLibro|FechaPrestamo|FechaDevolucion");
}

// --------------------- Menú ---------------------

void mostrarMenu() {
    cout << "\n===== SISTEMA BIBLIOTECA (simple) =====\n";
    cout << "1 - Agregar libro\n";
    cout << "2 - Listar libros\n";
    cout << "3 - Modificar libro\n";
    cout << "4 - Eliminar libro\n";
    cout << "5 - Registrar usuario\n";
    cout << "6 - Listar usuarios\n";
    cout << "7 - Modificar usuario\n";
    cout << "8 - Eliminar usuario\n";
    cout << "9 - Registrar préstamo\n";
    cout << "10 - Registrar devolución\n";
    cout << "11 - Consultar préstamos de un usuario\n";
    cout << "12 - Consultar libros por título/autor/categoría\n";
    cout << "13 - Consultar usuarios por nombre/ID\n";
    cout << "14 - Listar libros disponibles\n";
    cout << "0 - Salir\n";
    cout << "Elige opción: ";
}


int main() {
    inicializarSistema();

    while (true) {
        mostrarMenu();
        string opcion;
        getline(cin, opcion);

        if (opcion == "1") agregarLibro();
        else if (opcion == "2") listarLibros();
        else if (opcion == "3") modificarLibro();
        else if (opcion == "4") eliminarLibro();
        else if (opcion == "5") agregarUsuario();
        else if (opcion == "6") listarUsuarios();
        else if (opcion == "7") modificarUsuario();
        else if (opcion == "8") eliminarUsuario();
        else if (opcion == "9") registrarPrestamo();
        else if (opcion == "10") registrarDevolucion();
        else if (opcion == "11") consultarPrestamosUsuario();
        else if (opcion == "12") consultarLibros();
        else if (opcion == "13") consultarUsuarios();
        else if (opcion == "14") listarLibrosDisponibles();

        else if (opcion == "0") {
            cout << "Saliendo. ¡Hasta luego!\n";
            break;
        }
        else {
            cout << "Opción no válida.\n";
        }
    }
    return 0;
}
