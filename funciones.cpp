#include <iostream>
#include <bitset>
#include "funciones.h"
using namespace std;

void entrada_fila_columna(unsigned int &c, unsigned int &f){
    cout << "Ingrese el numero de columnas: ";
    cin >> c;
    cout << "Ingrese el numero de filas: ";
    cin >> f;
    cout << endl;
}

void reservacion_memoria(char *&ptr, unsigned int c, unsigned int f, unsigned int &bytes_reservados){
    bytes_reservados = (c*f*3)/(sizeof(char)*8);
    if((c*f*3)%(sizeof(char)*8)!=0){
        bytes_reservados++;
    }
    ptr = new char[bytes_reservados];
}

void impresion_tablero_bits(char* ptr,unsigned int c,unsigned int f){
    unsigned short int bit = 0;
    unsigned int byte = 0;
    char mascara = 7, nprint;       //00000111
    for(unsigned int i=0; i<f; i++){
        for(unsigned int j=0; j<c; j++, bit+=3){
            if(bit>7){
                byte+=1;
                bit%=7;
            }
            if(bit>5){
                nprint = ((*(ptr+byte)<<(bit-5))&(*(ptr+byte+1)>>(13-bit)))&mascara;
                cout << bitset<3>(nprint);
            } else{
                nprint = (*(ptr+byte)>>(5-bit))&mascara;
                cout << bitset<3>(nprint);
            }
            cout << " ";
        }
        cout << endl;
    }
    cout << endl;
}

void impresion_tablero(char* ptr,unsigned int c,unsigned int f){
    unsigned short int bit = 0;
    char fichas[8] = {' ','!','#','@','$','?','*','^'};
    unsigned int byte = 0;
    char mascara = 7, nprint;       //00000111
    for(unsigned int i=0; i<f; i++){
        for(unsigned int j=0; j<c; j++, bit+=3){
            if(bit>7){
                byte+=1;
                bit%=7;
            }
            if(bit>5){
                nprint = ((*(ptr+byte)<<(bit-5))&(*(ptr+byte+1)>>(13-bit)))&mascara;
                cout << fichas[nprint];
            } else{
                nprint = (*(ptr+byte)>>(5-bit))&mascara;
                cout << fichas[nprint];
            }
            cout << " ";
        }
        cout << endl;
    }
    cout << endl;
}

void inicializacion_juego(unsigned int &c, unsigned int &f, char *&ptr, unsigned int &bytes_reservados){
    entrada_fila_columna(c,f);
    reservacion_memoria(ptr, c, f, bytes_reservados);
    impresion_tablero_bits(ptr, c, f);  //
    impresion_tablero(ptr, c, f);       //
    //generacion_inicial_aleatorio(ptr, c, f)

    //detectar_combinaciones(ptr, m, n, combinaciones)
    //caida_fichas(ptr, m, n) -> dentro deberia generar de una vez ficha_aleatoria
    //repetir desde detectar_combinaciones() hasta que no hayan mas
}

void turno(bool &salir_juego, unsigned int c, unsigned int f, char *&ptr, unsigned int &bytes_reservados){
    impresion_tablero_bits(ptr, c, f);
    impresion_tablero(ptr, c, f);
    //entrada_usuario()

    //case opciones disponibles()

    //quitar_columna()
    //agregar_columna()
    //quitar_fila()
    //agregar_columna()
    //quitar_ficha()

    //detectar_combinaciones(ptr, m, n, combinaciones)
    //caida_fichas(ptr, m, n)
    //repetir desde detectar_combinaciones() hasta que no hayan mas
}