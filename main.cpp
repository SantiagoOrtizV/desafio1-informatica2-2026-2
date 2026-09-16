#include <iostream>
#include "funciones.h"

using namespace std;

int main(){
    bool salir_inicio = false, salir_juego;
    unsigned short int opc;
    unsigned char* ptr = nullptr;
    unsigned int bytes_reservados;

    while(!salir_inicio){
        cout << "BIENVENIDO A SWEET CRUSH" << endl;
        cout << "1) Iniciar juego" << endl;
        cout << "2) Salir" << endl << endl;
        cout << "Ingrese una opcion: ";
        cin >> opc;
        cout << endl;

        switch(opc){
        case 1:
            unsigned int c,f;
            salir_juego = false;
            inicializacion_juego(c, f, ptr, bytes_reservados);
            while(!salir_juego){
                //turno(salir_juego, c, f, ptr, bytes_reservados);
            }
            delete ptr;
            ptr = nullptr;
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
