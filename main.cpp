#include <iostream>
#include "funciones.h"

using namespace std;

int main()
{
    bool salir_inicio = false, salir_juego;
    unsigned short int opc;
    unsigned int* ptr = nullptr;

    while(!salir_inicio){
        cout << "BIENVENIDO A SWEET CRUSH" << endl;
        cout << "1) Iniciar juego" << endl;
        cout << "2) Salir" << endl << endl;
        cout << "Ingrese una opcion: " << endl;
        cin >> opc;
        switch(opc){
        case 1:
            unsigned int m,n;
            salir_juego = false;
            inicializacion_juego(m, n, ptr);
            while(!salir_juego){
                turno(salir_juego, m, n, ptr);
            }
            break;

        case 2:
            salir_inicio = true;
            break;

        default:
            cout << "Opcion invalida" << endl;
            break;
        }
    }
}
