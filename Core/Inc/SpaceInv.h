/*
 * SpaceInv.h
 *
 *  Created on: Jun 25, 2024
 *      Author: Miron Andrei
 */

#ifndef INC_SPACEINV_H_
#define INC_SPACEINV_H_

extern uint8_t apasatDR;
extern uint8_t apasatSTG;

void initializare_racheta_jucator(void);
void initializare_ecran(void);
void pixel_adversar(uint8_t x, uint8_t y);
void initializare_adversar_nivel1(void);
void initializare_nivel_1(void);
void refresh_jucator(uint8_t x, uint8_t y);
void fizica_jucator(void);
void motor_joc(void);

#endif /* INC_SPACEINV_H_ */
