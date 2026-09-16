#include <iostream>
#include <bitset>
#include <random>
#include "funciones.h"
using namespace std;

char generar_ficha(unsigned short int i, unsigned short int f){
    random_device rd;
    mt19937 gen(rd());
    uniform_int_distribution<> distrib(i, f);
    unsigned char numero = distrib(gen);
    return numero;
}

void setFicha(unsigned char* arreglo, int indice, unsigned char valor){
    // 1. Limpiamos el valor para asegurar que solo contenga datos en sus 3 bits menos significativos
    valor = valor & 0x07; // 0x07 es 00000111 en binario

    // 2. Calculamos la posición exacta en el arreglo
    int bit_inicial = indice * 3;
    int byte_idx = bit_inicial / 8;
    int bit_offset = bit_inicial % 8; // Desplazamiento desde la izquierda (MSB)

    // Caso A: El elemento cabe completamente dentro de un solo byte
    // Si el offset es 5 o menos, quedan al menos 3 bits libres en este byte
    if (bit_offset <= 5) {
        // Calculamos cuánto debemos desplazar el valor hacia la izquierda
        int shift = 5 - bit_offset;

        // Creamos una máscara para poner en 0 solo los 3 bits que vamos a modificar
        unsigned char mascara = ~(0x07 << shift);

        // Aplicamos la máscara y escribimos el nuevo valor
        arreglo[byte_idx] = (arreglo[byte_idx] & mascara) | (valor << shift);
    }
    // Caso B: El elemento está partido entre el byte actual y el siguiente
    else {
        // ¿Cuántos bits caben en el primer byte y cuántos sobran para el segundo?
        int bits_en_primero = 8 - bit_offset;
        int bits_en_segundo = 3 - bits_en_primero;

        // --- Modificamos el PRIMER byte ---
        // Máscara para borrar los últimos 'bits_en_primero' bits del byte actual
        unsigned char mascara1 = ~((1 << bits_en_primero) - 1);
        // Desplazamos el valor a la derecha para quedarnos con su parte más significativa
        arreglo[byte_idx] = (arreglo[byte_idx] & mascara1) | (valor >> bits_en_segundo);

        // --- Modificamos el SEGUNDO byte ---
        // Máscara para borrar los primeros 'bits_en_segundo' bits del siguiente byte
        int shift2 = 8 - bits_en_segundo;
        unsigned char mascara2 = ~(((1 << bits_en_segundo) - 1) << shift2);
        // Extraemos solo los bits que faltan escribir y los empujamos a la izquierda del nuevo byte
        unsigned char valor_resto = valor & ((1 << bits_en_segundo) - 1);
        arreglo[byte_idx + 1] = (arreglo[byte_idx + 1] & mascara2) | (valor_resto << shift2);
    }
}

void entrada_fila_columna(unsigned int &c, unsigned int &f){
    cout << "Ingrese el numero de columnas: ";
    cin >> c;
    cout << "Ingrese el numero de filas: ";
    cin >> f;
    cout << endl;
}

void reservacion_memoria(unsigned char *&ptr, unsigned int c, unsigned int f, unsigned int &bytes_reservados){
    bytes_reservados = (c*f*3)/(sizeof(unsigned char)*8);
    if((c*f*3)%(sizeof(unsigned char)*8)!=0){
        bytes_reservados++;
    }
    ptr = new unsigned char[bytes_reservados];
}

void impresion_tablero_bits(unsigned char* ptr, unsigned int c, unsigned int f){
    unsigned char mascara = 7;
    unsigned char nprint;

    for(unsigned int i=0; i<f; i++)
    {
        for(unsigned int j = 0; j<c; j++)
        {
            unsigned int posicion = i*c+j;
            unsigned int bit_inicio = posicion*3;
            unsigned int byte = bit_inicio/8;
            unsigned int bit = bit_inicio%8;

            if(bit <= 5){
                nprint = (ptr[byte] >> (5 - bit)) & mascara;
            }else{
                nprint = ((ptr[byte] << (bit - 5)) | (ptr[byte + 1] >> (13 - bit))) & mascara;
            }
            cout << bitset<3>(nprint) << " ";
        }
        cout << endl;
    }
    cout << endl;
}

void impresion_tablero(unsigned char* ptr, unsigned int c, unsigned int f){
    unsigned char mascara = 7;
    unsigned char fichas[8] = {' ','!','#','@','$','?','*','^'};
    unsigned char nprint;

    for(unsigned int i=0; i<f; i++)
    {
        for(unsigned int j = 0; j<c; j++)
        {
            unsigned int posicion = i*c+j;
            unsigned int bit_inicio = posicion*3;
            unsigned int byte = bit_inicio/8;
            unsigned int bit = bit_inicio%8;

            if(bit <= 5){
                nprint = (ptr[byte] >> (5 - bit)) & mascara;
            }else{
                nprint = ((ptr[byte] << (bit - 5)) | (ptr[byte + 1] >> (13 - bit))) & mascara;
            }
            cout << fichas[nprint] << " ";
        }
        cout << endl;
    }
    cout << endl;
}


void generacion_inicial_aleatorio(unsigned char* ptr,unsigned int c, unsigned int f){
    for(unsigned int i=0; i<(c*f); i++){
        setFicha(ptr, i, generar_ficha(1,6));
    }
}

void inicializacion_juego(unsigned int &c, unsigned int &f,unsigned char *&ptr, unsigned int &bytes_reservados){
    entrada_fila_columna(c,f);
    reservacion_memoria(ptr, c, f, bytes_reservados);
    //cout << static_cast<void*>(ptr) << endl;
    impresion_tablero_bits(ptr, c, f);
    impresion_tablero(ptr, c, f);
    generacion_inicial_aleatorio(ptr, c, f);
    impresion_tablero_bits(ptr, c, f);
    impresion_tablero(ptr, c, f);

    //detectar_combinaciones(ptr, m, n, combinaciones)
    //caida_fichas(ptr, m, n) -> dentro deberia generar de una vez ficha_aleatoria
    //repetir desde detectar_combinaciones() hasta que no hayan mas
}

void turno(bool &salir_juego, unsigned int c, unsigned int f, unsigned char *&ptr, unsigned int &bytes_reservados){
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