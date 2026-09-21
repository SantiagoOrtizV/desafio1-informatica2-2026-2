#ifndef FUNCIONES_H
#define FUNCIONES_H

void inicializacion_juego(unsigned int &c, unsigned int &f, unsigned char *&ptr, unsigned int &bytes_reservados,
                          unsigned int &total_fichas_elim, unsigned int &total_comb_detec, unsigned int &cascadas, unsigned int &cant_elim, unsigned int &puntaje_total);

void turno(bool &salir_juego,unsigned int &c,unsigned int &f, unsigned char *&ptr, unsigned int &bytes_reservados, unsigned int &total_fichas_elim,
           unsigned int &total_comb_detec, unsigned int &cascadas, unsigned int &cant_elim, unsigned int &puntaje_total);

#endif
