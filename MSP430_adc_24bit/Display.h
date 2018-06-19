/*
 * Display.h
 *
 *  Created on: 17 de jun de 2018
 *      Author: aluiz
 */

#ifndef DISPLAY_H_
#define DISPLAY_H_


void lcd_data_write(unsigned char data);

void lcd_send_command(unsigned char cmd);

void lcd_send_data(unsigned char data);

void lcd_send_string(char *s);

void lcd_init(void);




#endif /* DISPLAY_H_ */
