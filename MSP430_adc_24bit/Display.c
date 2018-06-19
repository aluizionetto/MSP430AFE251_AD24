/*
 * Display.c
 *
 *  Created on: 17 de jun de 2018
 * Author : Aluizio d'Affonseca Netto
 * This file is for 4 bit mode LCD interfacing with msp430AFE chip com 74h595 para espansão de IO
 * 16x2 LCD is used
 * DB7 - QH 74H595
 * DB6 - QG 74H595
 * DB5 - QF 74H595
 * DB4 - QE 74H595
 * RS -  QD 74H595
 */


#include <msp430.h>
#include "delay.h"
#include "Display.h"

// pinos para controle do 74h595
#define SRCLK(s) (P1OUT = (s ? (P1OUT | BIT7): (P1OUT & ~BIT7)))
#define RCLK_E(s) (P2OUT = (s ? (P2OUT | BIT0): (P2OUT & ~BIT0)))
#define SER(s) (P1OUT = (s ? (P1OUT | BIT6): (P1OUT & ~BIT6)))
#define RS_bit 0x08


void PORT_INIT(void)
{
    P1DIR |= (BIT6 + BIT7);
    P2DIR |= BIT0;
}

void data_write(unsigned char data)
{
    int k;
    for (k = 0; k < 8; k++)
    {
        if (data & 0x80)
            SER(1);
        else
            SER(0);

        data <<= 1;

        SRCLK(0);
        delay(1);
        SRCLK(1);
    }
    delay(2);
    RCLK_E(1);
    delay(2);
    RCLK_E(0);
}

void lcd_send_data(unsigned char data)
{
    data_write((RS_bit| (data & 0xF0))); // send higher nibble
    data_write((RS_bit | ((data << 4) & 0xF0))); // send lower nibble
    delay(40);
}
void lcd_send_command(unsigned char data)
{
    data_write(data & 0xF0); // send higher nibble
    data_write((data << 4) & 0xF0); // send lower nibble
    delay(40);
}
void lcd_send_string(char *s)
{
    while(*s)
    {
        lcd_send_data(*s++);
    }
}
void lcd_init(void)
{
    PORT_INIT();
    RCLK_E(0);
    data_write(0);
    delay_ms(20);
    lcd_send_command(0x02); // 4 bit mode
    lcd_send_command(0x28); // 4 bit mode
    lcd_send_command(0x0C); // LCD on, cursor off
    lcd_send_command(0x06); // increment cursor
    lcd_send_command(0x01); // limpa display
    delay_ms(4);
}




