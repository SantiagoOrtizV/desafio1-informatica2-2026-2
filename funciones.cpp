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

unsigned char getFicha(unsigned char* ptr, unsigned int indice){
    unsigned char mascara = 7;
    unsigned char valor;

    unsigned int bit_inicio = indice * 3;
    unsigned int byte = bit_inicio / 8;
    unsigned int bit = bit_inicio % 8;

    if(bit <= 5){
        valor = (ptr[byte] >> (5 - bit)) & mascara;
    } else {
        valor = ((ptr[byte] << (bit - 5)) | (ptr[byte + 1] >> (13 - bit))) & mascara;
    }

    return valor;
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
    for(unsigned int i = 0; i < f; i++){
        for(unsigned int j = 0; j < c; j++){
            unsigned char valor = getFicha(ptr, i*c + j);
            cout << bitset<3>(valor) << " ";
        }
        cout << endl;
    }
    cout << endl;
}

void impresion_tablero(unsigned char* ptr, unsigned int c, unsigned int f){
    unsigned char fichas[8] = {' ','!','#','@','$','?','*','^'};

    for(unsigned int i = 0; i < f; i++){
        for(unsigned int j = 0; j < c; j++){
            unsigned char valor = getFicha(ptr, i*c + j);
            cout << fichas[valor] << " ";
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

void entrada_usuario(unsigned short int &opc,unsigned int &nc,unsigned int &nf, unsigned int c, unsigned int f){
    cout << "1) -ficha 2) +columna 3) +fila 4) -columna 5) -fila 6) salir" << endl;
    do{
        cout << ">> ";
        cin >> opc;
    }while(opc<1||opc>6);

    switch(opc){

    case 1:
        cout << "Ingrese # de fila" << endl;
        do{
            cout << ">> ";
            cin >> nc;
        }while(nc<1||nc>c);
        cout << "Ingrese # de columna" << endl;
        do{
            cout << ">> ";
            cin >> nf;
        }while(nf<1||nf>f);
        break;

    case 2:
        cout << "Agregar nueva columna a la derecha de la columna #" << endl;
        do{
            cout << ">> ";
            cin >> nc;
        }while(nc<0||nc>c);
        break;

    case 3:
        cout << "Agregar nueva fila a abajo de la fila #" << endl;
        do{
            cout << ">> ";
            cin >> nf;
        }while(nf<0||nf>f);
        break;

    case 4:
        cout << "Eliminar columna #" << endl;
        do{
            cout << ">> ";
            cin >> nc;
        }while(nc<1||nc>c);
        break;

    case 5:
        cout << "Eliminar fila #" << endl;
        do{
            cout << ">> ";
            cin >> nf;
        }while(nf<1||nf>f);
        break;
    }
}

void quitar_ficha(unsigned char *ptr, unsigned int nc, unsigned int nf){
    cout << "quitar_ficha" << endl;
    cout << nc << " " << nf << endl;
}

void agregar_columna(unsigned char *ptr, unsigned int nc){
    cout << "agregar_columna" << endl;
    cout << nc << endl;
}

void agregar_fila(unsigned char *ptr, unsigned int nf){
    cout << "agregar_fila" << endl;
    cout << nf << endl;
}

void quitar_columna(unsigned char *ptr, unsigned int nc){
    cout << "quitar_columna" << endl;
    cout << nc << endl;
}

void quitar_fila(unsigned char *ptr, unsigned int nf){
    cout << "quitar_fila" << endl;
    cout << nf << endl;
}

void turno(bool &salir_juego, unsigned int c, unsigned int f, unsigned char *&ptr, unsigned int &bytes_reservados){
    unsigned short int opc;
    unsigned int nc, nf;
    impresion_tablero_bits(ptr, c, f);
    impresion_tablero(ptr, c, f);
    entrada_usuario(opc, nc, nf, c, f);

    switch(opc){
    case 1:
        quitar_ficha(ptr, nc, nf);
        break;
    case 2:
        agregar_columna(ptr, nc);
        break;
    case 3:
        agregar_fila(ptr, nf);
        break;
    case 4:
        quitar_columna(ptr, nc);
        break;
    case 5:
        quitar_fila(ptr, nf);
        break;
    case 6:
        salir_juego = true;
        break;
    }
    cout << endl;

    //detectar_combinaciones(ptr, m, n, combinaciones)
    //caida_fichas(ptr, m, n)
    //repetir desde detectar_combinaciones() hasta que no hayan mas
}