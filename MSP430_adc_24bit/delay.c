/*
 * delay.c
 *
 *  Created on: 18 de jun de 2018
 *      Author: Aluizio
 */

#include "delay.h"

void delay(unsigned int k)
{
    //multiplo de us
    while (k--)
        __delay_cycles(CICLOS_US);
}


void delay_ms(int t)
{
    while(t--)
        __delay_cycles(CICLOS_MS);
}




