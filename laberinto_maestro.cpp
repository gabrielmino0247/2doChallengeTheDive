#include <iostream>
#include <vector>
#include <random>
#include <chrono>
#include <algorithm>
#include <tuple>  
#include <iomanip>

// esto se usa para evitar el uso de std:: al llamar a las funciones de la biblioteca estandar
using namespace std;


// Estructura para representar una celda del laberinto
// Cada celda tiene muros en las cuatro direcciones y un estado de visitado
// y en_camino para el algoritmo de resolución
struct Celda {
    bool visitado = false;
    bool arriba = true;
    bool abajo = true;
    bool izquierda = true;
    bool derecha = true;
    bool en_camino = false;
};


// Clase que representa el laberinto
class Laberinto {
    // Atributos privados para buenas practicas de encapsulamiento, osea que solo se puede cambiar desde el metodo cambiar_dimension
private:
    int dimension;
    vector<vector<Celda>> lab;
    mt19937 rng; // Generador de números aleatorios mersenne twister (es mejor que rand() por su calidad y velocidad)


    //metodo publico ya normal como siempre usamos en python
public:
    // Constructor que inicializa el laberinto con una dimensión por defecto

    // aca basicamente lo que hacemos es inicializar en 11 el laberinto y se inicializa el generador de nros aleatorios con la semilla del tiempo actual
    // si se quiere cambiar la dimension se usa el metodo cambiar_dimension
    Laberinto(int dim = 11) : dimension(dim), rng(chrono::steady_clock::now().time_since_epoch().count()) {
        crear_tablero();
    }

    // Método para crear el tablero del laberinto
    void crear_tablero() {
        lab.clear(); // Limpiamos el laberinto actual
        lab.resize(dimension, vector<Celda>(dimension)); // Redimensionamos el laberinto a la nueva dimensión si es que se cambio
    }

    // Método para imprimir el laberinto en la consola imprime por fuera los muros y por dentro las celdas
    // con esto evitamos problemas de doble salto de linea y que se vea mejor
    void imprimir_laberinto() {
        int filas = lab.size();
        int columnas = lab[0].size(); //al principio marea pero aca ve el largo de la columna usando cualquiera de las filas

        // Muro superior
        cout << "⬜";
        for (int j = 0; j < columnas; j++) {
            cout << "⬜⬜";  //esto ayuda a que se vea mejor el laberinto, los limites de arriba
        }
        cout << endl; //nunca olvidar el salto de linea porque sino puede dar problemas de visualización o errores raros.
        //investigar impacto de endl. 

        for (int i = 0; i < filas; i++) {
            // Línea de celdas + muros derechos
            string linea_celdas = "⬜";
            for (int j = 0; j < columnas; j++) {
                // Contenido de la celda
                string contenido;
                if (i == 0 && j == 0) { //si es la celda de inicio
                    contenido = "⚫";
                } else if (i == filas - 1 && j == columnas - 1) { //si es la celda de fin
                    contenido = "🟢";
                } else if (lab[i][j].en_camino) { //si es el recorrido del laberinto resuelto
                    contenido = "░░";
                } else if (lab[i][j].visitado) { //si es una celda visitada pero no en el camino queda nomas vacio
                    contenido = "  ";
                } else {
                    contenido = "⬜"; // si no es visitada ni en el camino, mostramos un muro
                }

                // Conexión a la derecha
                string muro_derecha;
                if (lab[i][j].derecha) {
                    muro_derecha = "⬜";
                } else {
                    // Si la celda actual y la vecina están en el camino, mostramos pasillo visual
                    if (j + 1 < columnas && lab[i][j].en_camino && lab[i][j + 1].en_camino) { // esta parte es importante porque valida que el camino sea continuo y correcto
                        muro_derecha = "░░";
                    } else {
                        muro_derecha = "  ";
                    }
                }

                linea_celdas += contenido + muro_derecha; // esto es lo que se va a imprimir en la linea de celdas, el contenido de la celda y el muro derecho
            }
            cout << linea_celdas << endl; // Imprimimos la línea de celdas

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
                string esquina = "⬜"; // esto es el muro de la esquina inferior derecha, cada uno tiene su muro y esquina
                linea_abajo += muro_abajo + esquina;
            }
            cout << linea_abajo << endl;
        }
    }

    void generar_laberinto(int fila, int columna) {
        lab[fila][columna].visitado = true; // Marcamos la celda actual como visitada
        
        vector<pair<int, int>> direcciones = {{0, 1}, {1, 0}, {0, -1}, {-1, 0}};
        shuffle(direcciones.begin(), direcciones.end(), rng);

        for (auto& d : direcciones) { // auto& d es una forma de iterar sobre un vector de pares, donde cada par representa una dirección (dy, dx)
            // Calculamos la nueva fila y columna basándonos en la dirección actual
            //first es la fila, second es la columna
            int nueva_fila = fila + d.first;
            int nueva_columna = columna + d.second;

            if (nueva_fila >= 0 && nueva_fila < dimension && 
                nueva_columna >= 0 && nueva_columna < dimension && 
                !lab[nueva_fila][nueva_columna].visitado) { // Verificamos que la nueva celda esté dentro de los límites y no haya sido visitada
                

                //make pair es una forma de crear un par de valores, en este caso (dy, dx) que representan la dirección
                // Actualizamos los muros entre la celda actual y la nueva celda
                // Esto es importante para que el laberinto sea válido y no tenga muros donde no debería
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

        Celda& celda = lab[fila][columna];  // Referencia a la celda actual para no escribir lab[fila][columna] cada rato

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

        for (auto& dir : direcciones) { // esta es la parte que itera sobre las direcciones posibles y hace la recursividad
            // Desempaquetamos la tupla para obtener la dirección y el muro actual
            // get<0>(dir) es la fila, get<1>(dir) es la columna y get<2>(dir) es el muro actual
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

    void cambiar_dimension(int nueva_dimension) {// Método para cambiar la dimensión del laberinto
        // Validamos que la nueva dimensión sea mayor o igual a 5 para evitar laberintos demasiado chicos
        if (nueva_dimension >= 5) {
            dimension = nueva_dimension;
            crear_tablero();
            cout << "Tamaño actualizado a " << dimension << "x" << dimension  << endl;
        } else {
            cout << "El tamaño debe ser mayor o igual a 5" << endl;
        }
    }

    int get_dimension() const { // Método para obtener la dimensión actual del laberinto
        // Este método es útil para saber el tamaño actual del laberinto sin modificarlo
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
        cin >> opcion; //cin lee la opcion del usuario

        switch (opcion) {
            case 1: {
                laberinto.crear_tablero();
                laberinto.generar_laberinto(0, 0); // Generamos el laberinto comenzando desde la celda (0, 0)
                laberinto_generado = true;
                laberinto.imprimir_laberinto();
                break;
            }
            case 2: {
                int nueva_dimension; 
                cout << "Ingresá el nuevo tamaño (mínimo 5): ";
                cin >> nueva_dimension;
                laberinto.cambiar_dimension(nueva_dimension);
                laberinto_generado = false; // Reseteamos el estado de laberinto generado para que no se pueda resolver hasta que se genere uno nuevo
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
                        cout << fixed << setprecision(6); //esto sirve para mostrar los segundos con 6 decimales
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
