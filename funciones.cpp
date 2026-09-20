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

bool detectar_horizontal(unsigned char* ptr, unsigned int f, unsigned int c, bool* marcado, unsigned int &total_comb_detec) {
    bool hubo_combinacion = false;
    for(unsigned int i = 0; i < f; i++){ //este ciclo itera en las filas del tablero, coge siempre el primer
        //caracter de esa fila para comparar con los siguiente, por eso contador siempre se reinica a 1 cada vuelta

        unsigned char valor_actual = getFicha(ptr, i*c + 0); //cogemos siempre la primera ficha de cada fila
        unsigned int col_inicio = 0;
        unsigned int contador = 1;

        for(unsigned int j = 1; j < c; j++){ //empezamos en la columna 1
            unsigned char valor_j = getFicha(ptr, i*c + j); //cogemos la ficha de esa columna (empieza desde columna 1)

            if(valor_j == valor_actual){
                contador++;
            }
            else{
                if(contador >= 3){
                    for(unsigned int k = 0; k < contador; k++){
                        unsigned int col_actual = col_inicio + k;
                        unsigned int indice = i*c + col_actual;
                        //cout << "Fichas eliminadas desde horizontal: "<<contador<<endl;
                        marcado[indice] = true;
                    }
                    total_comb_detec += 1;
                    hubo_combinacion = true;
                }
                valor_actual = valor_j;
                col_inicio = j;
                contador = 1;
            }
        }

        if(contador >= 3){
            for(unsigned int k = 0; k < contador; k++){
                unsigned int col_actual = col_inicio + k;
                unsigned int indice = i*c + col_actual;
                //cout << "Fichas eliminadas desde horizontal: "<<contador<<endl;
                marcado[indice] = true;
            }
            total_comb_detec += 1;
            hubo_combinacion = true;
        }
    }

    return hubo_combinacion;
}

bool detectar_vertical(unsigned char* ptr, unsigned int f, unsigned int c, bool* marcado, unsigned int &total_comb_detec) {
    bool hubo_combinacion = false;
    for(unsigned int j = 0; j < c; j++){

        unsigned char valor_actual = getFicha(ptr, 0*c + j);
        unsigned int fila_inicio = 0;
        unsigned int contador = 1;

        for(unsigned int i = 1; i < f; i++){
            unsigned char valor_i = getFicha(ptr, i*c + j);

            if(valor_i == valor_actual){
                contador++;
            }
            else{
                if(contador >= 3){
                    for(unsigned int k = 0; k < contador; k++){
                        unsigned int fila_actual = fila_inicio + k;
                        unsigned int indice = fila_actual*c + j;
                        //cout << "Fichas eliminadas desde vertical: "<<contador<<endl;
                        marcado[indice] = true;
                    }
                    total_comb_detec += 1;
                    hubo_combinacion = true;
                }
                valor_actual = valor_i;
                fila_inicio = i;
                contador = 1;
            }
        }

        if(contador >= 3){
            for(unsigned int k = 0; k < contador; k++){
                unsigned int fila_actual = fila_inicio + k;
                unsigned int indice = fila_actual*c + j;
                marcado[indice] = true;
                //cout << "Fichas eliminadas desde vertical: "<<contador<<endl;
            }
            total_comb_detec += 1;
            hubo_combinacion = true;
        }

    }

    return hubo_combinacion;
}

void aplicar_eliminaciones(unsigned char* ptr, unsigned int f, unsigned int c, bool* marcado) {
    for(unsigned int indice = 0; indice < f*c; indice++){
        if(marcado[indice]){
            setFicha(ptr, indice, 0); // 0 = espacio vacio
        }
    }
}

void rellenar_columna(unsigned char* ptr, unsigned int c, unsigned int j, int fila_destino) {
    for(int fila = 0; fila <= fila_destino; fila++){
        unsigned int indice = fila*c + j;
        unsigned char ficha_nueva = generar_ficha(1,6);
        setFicha(ptr, indice, ficha_nueva);
    }
}

void bajar_fichas_columna(unsigned char* ptr, unsigned int f, unsigned int c, unsigned int j) {
    int fila_origen = f - 1; //empezamos a recorrer de abajo hacía arriba
    int fila_destino = f - 1;   //empezamos a recorrer de abajo hacía arriba

    while(fila_origen >= 0) { //condicion parada, si no hay mas fichas que bajar para el ciclo
        unsigned int indice = fila_origen*c + j; //buscamos en esa columna
        unsigned char ficha_actual = getFicha(ptr, indice); //en esa columna traemos el valor de la última fila de esa columa con getFicha

        if(ficha_actual != 0){ //si no hay espacio vacío
            unsigned int indice_destino = fila_destino*c + j; //calculo indice destino usando fila_destino, o sea, la  fila de esa columna donde el vlor es 0
            setFicha(ptr, indice_destino, ficha_actual);       //pongo en esa posición donde había un 0, pongo el valor de la ficha de arriba

            if(fila_origen != fila_destino){ //solo ponemos la casilla en 0 si el indice de las filas es diferente, este es el caso donde toda la columna si está bien rellena
                unsigned int indice_origen = fila_origen*c + j;
                setFicha(ptr, indice_origen, 0);
            }

            fila_origen--; //subimos de fila
            fila_destino--; // subimos de fila
        }
        else{
            fila_origen--;
        }

    }
    rellenar_columna(ptr,c,j,fila_destino);
}


void bajar_fichas(unsigned char* ptr, unsigned int f, unsigned int c) {
    for(unsigned int col = 0; col < c; col++){ //itero en todas las columnas, por cada una de ellas llamo a la funcion que las organiza
        bajar_fichas_columna(ptr, f, c, col);
    }
}

void procesar_combinaciones(unsigned char* ptr, unsigned int f, unsigned int c, unsigned int &total_fichas_elim, unsigned int &total_comb_detec) { //funcion que procesa todas las combinaciones, sobreescribe el tablero, bajas las fichas
    //y las rellena, se ejecuta siempre que haya mínimo una combinacion
    //tambien dará el puntaje

    bool* marcado = new bool[f*c];
    bool hubo_combinacion;

    do {
        for(unsigned int i = 0; i < f*c; i++) marcado[i] = false;

        bool h = detectar_horizontal(ptr, f, c, marcado, total_comb_detec);
        bool v = detectar_vertical(ptr, f, c, marcado, total_comb_detec);
        hubo_combinacion = h || v;

        aplicar_eliminaciones(ptr, f, c, marcado);

        //calculo cantidad de fichas eliminadas en total y cantidad de combinaciones
        for(unsigned int i = 0;i < (f*c);i++){
            if(marcado[i]){ //si hay un true marcado en el arreglo, significa que
                total_fichas_elim += 1;
            }

        }

        bajar_fichas(ptr, f, c);

    } while(hubo_combinacion);


    //cantidad de eliminaciones realizadas por ususario ->cada vez que se invoque la opcion de eliminar ficha

    delete[] marcado;
}

void puntaje(unsigned int &total_fichas_elim, unsigned int &total_comb_detec){
    cout << "Total fichas eliminadas hasta el momento: "<<total_fichas_elim<<endl
         <<"Total combinaciones detectadas: "<< total_comb_detec<<endl;

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
    unsigned short int nf = 1, nc = 1;

    for(unsigned int i=0; i<f; i++, nf++){
        cout << nf << " ";
        for(unsigned int j=0; j<c; j++){
            unsigned char valor = getFicha(ptr, i*c + j);
            cout << fichas[valor] << " ";
        }
        cout << endl;
        if(nf==9){
            nf-=10;
        }
    }
    cout << "  ";
    for (unsigned int i=0; i<c; i++, nc++){
        cout << nc << " ";
        if(nc==9){
            nc-=10;
        }
    }
    cout << endl << endl;
}


void generacion_inicial_aleatorio(unsigned char* ptr,unsigned int c, unsigned int f){
    for(unsigned int i=0; i<(c*f); i++){
        setFicha(ptr, i, generar_ficha(1,6));
    }
}

void inicializacion_juego(unsigned int &c, unsigned int &f,unsigned char *&ptr, unsigned int &bytes_reservados, unsigned int &total_fichas_elim, unsigned int &total_comb_detec){
    entrada_fila_columna(c,f);
    reservacion_memoria(ptr, c, f, bytes_reservados);
    generacion_inicial_aleatorio(ptr, c, f);

    procesar_combinaciones(ptr, f, c, total_fichas_elim, total_comb_detec);
    puntaje(total_fichas_elim, total_comb_detec);
}

void entrada_usuario(unsigned short int &opc,unsigned int &nc,unsigned int &nf, unsigned int c, unsigned int f){
    cout << "1) -ficha // 2) +columna // 3) +fila // 4) -columna // 5) -fila // 6) salir" << endl;
    do{
        cout << ">> ";
        cin >> opc;
    }while(opc<1||opc>6);

    switch(opc){

    case 1:
        cout << "Ingrese # de columna" << endl;
        do{
            cout << ">> ";
            cin >> nc;
        }while(nc<1||nc>c);
        cout << "Ingrese # de fila" << endl;
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

void quitar_ficha(unsigned char *ptr, unsigned int nc, unsigned int nf, unsigned int c, unsigned int f){
    nc--; nf--;
    setFicha(ptr, (nf*c)+nc, 0);
    for(unsigned int i=nf; i>0; i--){
        setFicha(ptr, (i*c)+nc, getFicha(ptr,((i-1)*c)+nc));
    }
    setFicha(ptr, nc, generar_ficha(1,6));
}

void suficiente_espacio(unsigned char *ptr, unsigned int c, unsigned int f, unsigned int bytes_reservados, bool &disponibilidad_bytes, unsigned int &bytes_necesarios){
    unsigned int bits_reservados = bytes_reservados*8;
    int bits_utilizados = c*f*3;
    if(bits_reservados>=bits_utilizados){
        disponibilidad_bytes = true;
    }else{
        bytes_necesarios = bits_utilizados/8;
        if(bits_utilizados%8!=0){
            bytes_necesarios++;
        }
        disponibilidad_bytes = false;
    }
}

void agregar_columna(unsigned char *&ptr, unsigned int nc, unsigned int &c, unsigned int f, unsigned int &bytes_reservados){
    unsigned int bytes_necesarios;
    bool disponibilidad_bytes;
    suficiente_espacio(ptr, c+1, f, bytes_reservados, disponibilidad_bytes, bytes_necesarios);
    if(disponibilidad_bytes){
        unsigned int ultimo_elemento = (c*f)-1;
        unsigned int nIzq = f;
        for(unsigned int i=c-nc; i>0; i--, ultimo_elemento--){
            setFicha(ptr, ultimo_elemento+nIzq, getFicha(ptr, ultimo_elemento));
        }
        nIzq--;
        for(unsigned int i=0; i<f-1; i++, nIzq--){
            setFicha(ptr, ultimo_elemento+nIzq+1, generar_ficha(1,6));
            for(unsigned int j=0; j<c; j++, ultimo_elemento--){
                setFicha(ptr, ultimo_elemento+nIzq, getFicha(ptr, ultimo_elemento));
            }
        }
        setFicha(ptr, ultimo_elemento+1, generar_ficha(1,6));
    } else{
        unsigned char *ptr2 = new unsigned char[bytes_necesarios];
        unsigned int ultimo_elemento = (c*f)-1;
        unsigned int nIzq = f;
        for(unsigned int i=c-nc; i>0; i--, ultimo_elemento--){
            setFicha(ptr2, ultimo_elemento+nIzq, getFicha(ptr,ultimo_elemento));
        }
        nIzq--;
        for(unsigned int i=0; i<f-1; i++, nIzq--){
            setFicha(ptr2, ultimo_elemento+nIzq+1, generar_ficha(1,6));
            for(unsigned int j=0; j<c; j++, ultimo_elemento--){
                setFicha(ptr2, ultimo_elemento+nIzq, getFicha(ptr,ultimo_elemento));
            }
        }
        setFicha(ptr2, ultimo_elemento+1, generar_ficha(1,6));
        for(unsigned int i=nc; i>0; i--, ultimo_elemento--){
            setFicha(ptr2, ultimo_elemento, getFicha(ptr,ultimo_elemento));
        }
        delete[] ptr;
        ptr = ptr2;
        ptr2 = nullptr;
        bytes_reservados = bytes_necesarios;
    }
    c++;
}

void agregar_fila(unsigned char *&ptr, unsigned int nf, unsigned int c, unsigned int &f, unsigned int &bytes_reservados){
    unsigned int bytes_necesarios;
    bool disponibilidad_bytes;
    suficiente_espacio(ptr, c, f+1, bytes_reservados, disponibilidad_bytes, bytes_necesarios);
    if(disponibilidad_bytes){
        unsigned int ultimo_elemento = (c*f)-1;
        for(unsigned int i=0; i<(f-nf)*c; i++, ultimo_elemento--){
            setFicha(ptr, ultimo_elemento+c, getFicha(ptr, ultimo_elemento));
        }
        for(unsigned int i=1; i<=c; i++){
            setFicha(ptr, ultimo_elemento+i, generar_ficha(1,6));
        }
    }else{
        unsigned char *ptr2 = new unsigned char[bytes_necesarios];
        unsigned int ultimo_elemento = (c*f)-1;
        for(unsigned int i=0; i<(f-nf)*c; i++, ultimo_elemento--){
            setFicha(ptr2, ultimo_elemento+c, getFicha(ptr, ultimo_elemento));
        }
        for(unsigned int i=1; i<=c; i++){
            setFicha(ptr2, ultimo_elemento+i, generar_ficha(1,6));
        }
        for(unsigned int i=0; i<nf*c; i++, ultimo_elemento--){
            setFicha(ptr2, ultimo_elemento, getFicha(ptr, ultimo_elemento));
        }
        delete[] ptr;
        ptr = ptr2;
        ptr2 = nullptr;
        bytes_reservados = bytes_necesarios;
    }
    f++;
}

void quitar_columna(unsigned char *ptr, unsigned int nc, unsigned int &c, unsigned int &f){
    if (nc == 0) {
        return;
    }
    unsigned int indice_elemento_1_columna = nc-1, indice_elemento_1_columna_siguiente = nc;
    for (unsigned int j=0; j<f; j++){
        for(unsigned int i=0; i<c-1; i++, indice_elemento_1_columna++, indice_elemento_1_columna_siguiente++){
            setFicha(ptr, indice_elemento_1_columna, getFicha(ptr, indice_elemento_1_columna_siguiente));
        }
        indice_elemento_1_columna_siguiente++;
    }
    c--;
}

void quitar_fila(unsigned char *ptr, unsigned int nf, unsigned int &c, unsigned int &f){
    for(unsigned int indice_elemento_1_fila = (nf-1)*c, indice_elemento_1_fila_siguiente = (nf)*c;
        indice_elemento_1_fila<=f*c; indice_elemento_1_fila++,indice_elemento_1_fila_siguiente++){
        setFicha(ptr, indice_elemento_1_fila, getFicha(ptr, indice_elemento_1_fila_siguiente));
    }
    f--;
}

void espacio_65(unsigned char*&ptr, unsigned int &bytes_reservados, unsigned int c, unsigned int f){
    unsigned int bits_reservados = bytes_reservados*8;
    unsigned int bits_utilizados = c*f*3;
    if(bits_reservados*0.65>bits_utilizados){
        unsigned int bytes_necesarios = bits_utilizados/8;
        if(bits_utilizados%8!=0){
            bytes_necesarios++;
        }
        unsigned char* ptr2 = new unsigned char[bytes_necesarios];
        for(unsigned int i=0;i<c*f;i++){
            setFicha(ptr2, i, getFicha(ptr, i));
        }
        bytes_reservados = bytes_necesarios;
        delete[] ptr;
        ptr = ptr2;
        ptr2 = nullptr;
    }
}

void turno(bool &salir_juego, unsigned int &c, unsigned int &f, unsigned char *&ptr, unsigned int &bytes_reservados, unsigned int &total_fichas_elim, unsigned int &total_comb_detec){
    unsigned short int opc;
    unsigned int nc, nf;
    impresion_tablero_bits(ptr, c, f);
    impresion_tablero(ptr, c, f);
    entrada_usuario(opc, nc, nf, c, f);
    //puntaje(total_fichas_elim, total_comb_detec);

    switch(opc){
    case 1:
        quitar_ficha(ptr, nc, nf, c, f);
        break;
    case 2:
        agregar_columna(ptr, nc, c, f, bytes_reservados);
        break;
    case 3:
        agregar_fila(ptr, nf, c, f, bytes_reservados);
        break;
    case 4:
        quitar_columna(ptr, nc, c, f);
        espacio_65(ptr, bytes_reservados, c, f);
        break;
    case 5:
        quitar_fila(ptr, nf, c, f);
        espacio_65(ptr, bytes_reservados, c, f);
        break;
    case 6:
        salir_juego = true;
        break;
    }
    procesar_combinaciones(ptr, f, c, total_fichas_elim, total_comb_detec);
    puntaje(total_fichas_elim, total_comb_detec);
    cout << endl;
}