#include <iostream>
#include <vector>
#include <random>
#include <chrono>
#include <algorithm>
#include <tuple>  

using namespace std;

struct Celda {
    bool visitado = false;
    bool arriba = true;
    bool abajo = true;
    bool izquierda = true;
    bool derecha = true;
    bool en_camino = false;
};

class Laberinto {
private:
    int dimension;
    vector<vector<Celda>> lab;
    mt19937 rng;

public:
    Laberinto(int dim = 11) : dimension(dim), rng(chrono::steady_clock::now().time_since_epoch().count()) {
        crear_tablero();
    }

    void crear_tablero() {
        lab.clear();
        lab.resize(dimension, vector<Celda>(dimension));
    }

    void imprimir_laberinto() {
        int filas = lab.size();
        int columnas = lab[0].size();

        // Muro superior
        cout << "⬜";
        for (int j = 0; j < columnas; j++) {
            cout << "⬜⬜";
        }
        cout << endl;

        for (int i = 0; i < filas; i++) {
            // Línea de celdas + muros derechos
            string linea_celdas = "⬜";
            for (int j = 0; j < columnas; j++) {
                // Contenido de la celda
                string contenido;
                if (i == 0 && j == 0) {
                    contenido = "⚫";
                } else if (i == filas - 1 && j == columnas - 1) {
                    contenido = "🟢";
                } else if (lab[i][j].en_camino) {
                    contenido = "░░";
                } else if (lab[i][j].visitado) {
                    contenido = "  ";
                } else {
                    contenido = "⬜";
                }

                // Conexión a la derecha
                string muro_derecha;
                if (lab[i][j].derecha) {
                    muro_derecha = "⬜";
                } else {
                    // Si la celda actual y la vecina están en el camino, mostramos pasillo visual
                    if (j + 1 < columnas && lab[i][j].en_camino && lab[i][j + 1].en_camino) {
                        muro_derecha = "░░";
                    } else {
                        muro_derecha = "  ";
                    }
                }

                linea_celdas += contenido + muro_derecha;
            }
            cout << linea_celdas << endl;

            // Línea de muros inferiores
            string linea_abajo = "⬜";
            for (int j = 0; j < columnas; j++) {
                string muro_abajo;
                if (lab[i][j].abajo) {
                    muro_abajo = "⬜";
                } else {
                    // Conexión visual si está en el camino y el de abajo también
                    if (i + 1 < filas && lab[i][j].en_camino && lab[i + 1][j].en_camino) {
                        muro_abajo = "░░";
                    } else {
                        muro_abajo = "  ";
                    }
                }
                string esquina = "⬜";
                linea_abajo += muro_abajo + esquina;
            }
            cout << linea_abajo << endl;
        }
    }

    void generar_laberinto(int fila, int columna) {
        lab[fila][columna].visitado = true;
        
        vector<pair<int, int>> direcciones = {{0, 1}, {1, 0}, {0, -1}, {-1, 0}};
        shuffle(direcciones.begin(), direcciones.end(), rng);

        for (auto& d : direcciones) {
            int nueva_fila = fila + d.first;
            int nueva_columna = columna + d.second;

            if (nueva_fila >= 0 && nueva_fila < dimension && 
                nueva_columna >= 0 && nueva_columna < dimension && 
                !lab[nueva_fila][nueva_columna].visitado) {
                
                if (d == make_pair(0, 1)) {  // Derecha
                    lab[fila][columna].derecha = false;
                    lab[nueva_fila][nueva_columna].izquierda = false;
                } else if (d == make_pair(1, 0)) {  // Abajo
                    lab[fila][columna].abajo = false;
                    lab[nueva_fila][nueva_columna].arriba = false;
                } else if (d == make_pair(0, -1)) {  // Izquierda
                    lab[fila][columna].izquierda = false;
                    lab[nueva_fila][nueva_columna].derecha = false;
                } else if (d == make_pair(-1, 0)) {  // Arriba
                    lab[fila][columna].arriba = false;
                    lab[nueva_fila][nueva_columna].abajo = false;
                }

                generar_laberinto(nueva_fila, nueva_columna);
            }
        }
    }

    bool resolver_lab(int fila, int columna) {
        int filas = lab.size();
        int columnas = lab[0].size();

        // Fuera de los límites
        if (fila < 0 || fila >= filas || columna < 0 || columna >= columnas) {
            return false;
        }

        Celda& celda = lab[fila][columna];

        // Ya forma parte del camino
        if (celda.en_camino) {
            return false;
        }

        // Marcamos esta celda como parte del camino actual
        celda.en_camino = true;

        // Si llegamos a la salida, terminamos
        if (fila == filas - 1 && columna == columnas - 1) {
            return true;
        }

        // Direcciones posibles: (dy, dx, muro_actual)
        vector<tuple<int, int, bool*>> direcciones = {
            {0, 1, &celda.derecha},   // Derecha
            {1, 0, &celda.abajo},     // Abajo
            {0, -1, &celda.izquierda}, // Izquierda
            {-1, 0, &celda.arriba}    // Arriba
        };

        for (auto& dir : direcciones) {
            int df = get<0>(dir);
            int dc = get<1>(dir);
            bool* muro_actual = get<2>(dir);

            int nueva_fila = fila + df;
            int nueva_columna = columna + dc;

            if (nueva_fila >= 0 && nueva_fila < filas && 
                nueva_columna >= 0 && nueva_columna < columnas && 
                !(*muro_actual)) {  // Solo si no hay muro
                
                if (resolver_lab(nueva_fila, nueva_columna)) {
                    return true;
                }
            }
        }

        // Si llegamos acá, es un callejón sin salida
        celda.en_camino = false;
        return false;
    }

    void cambiar_dimension(int nueva_dimension) {
        if (nueva_dimension >= 5) {
            dimension = nueva_dimension;
            crear_tablero();
            cout << "Tamaño actualizado a " << dimension << "x" << dimension << "." << endl;
        } else {
            cout << "El tamaño debe ser mayor o igual a 5." << endl;
        }
    }

    int get_dimension() const {
        return dimension;
    }
};

void mostrar_menu() {
    cout << "\nBienvenido al laberinto" << endl;
    cout << "Selecciona una opción:" << endl;
    cout << "1. Generar laberinto" << endl;
    cout << "2. Cambiar dimensión del laberinto" << endl;
    cout << "3. Resolver laberinto" << endl;
    cout << "4. Salir" << endl;
    cout << "Selecciona una opción: ";
}

int main() {
    Laberinto laberinto;
    bool laberinto_generado = false;
    int opcion;

    while (true) {
        mostrar_menu();
        cin >> opcion;

        switch (opcion) {
            case 1: {
                laberinto.crear_tablero();
                laberinto.generar_laberinto(0, 0);
                laberinto_generado = true;
                laberinto.imprimir_laberinto();
                break;
            }
            case 2: {
                int nueva_dimension;
                cout << "🧱 Ingresá el nuevo tamaño (mínimo 5): ";
                cin >> nueva_dimension;
                laberinto.cambiar_dimension(nueva_dimension);
                laberinto_generado = false;
                break;
            }
            case 3: {
                if (!laberinto_generado) {
                    cout << "Primero generá el laberinto (opción 1)." << endl;
                } else {
                    auto inicio = chrono::high_resolution_clock::now();
                    bool exito = laberinto.resolver_lab(0, 0);
                    auto fin = chrono::high_resolution_clock::now();
                    
                    auto duracion = chrono::duration_cast<chrono::microseconds>(fin - inicio);
                    double segundos = duracion.count() / 1000000.0;

                    if (exito) {
                        laberinto.imprimir_laberinto();
                        cout << "Camino encontrado en " << segundos << " segundos." << endl;
                    } else {
                        cout << "No se encontró un camino." << endl;
                    }
                }
                break;
            }
            case 4: {
                cout << "Saliendo del programa." << endl;
                return 0;
            }
            default: {
                cout << "Opción no válida. Inténtalo de nuevo." << endl;
                break;
            }
        }
    }

    return 0;
}