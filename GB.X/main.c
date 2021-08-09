/*
 * File:   main.c
 * Author: Matias
 * El proyeto creara un control para el guiaborde de una cortadora
 *
 * Created on 8 de agosto de 2021, 13:02
 */

#define _XTAL_FREQ 2000000      //definimos TCY o ciclo de instruccion  como 8MHz/4
                                //ya que cada ciclo de instruccion le lleva 4 ciclos de reloj
                                //por lo tanto el reloj es de 8Mhz que se definira cuando se configure el oscilador
                                //esto lo usan los __delay_ms() y __delay_us()

//definicion de los puertos para conectar el LCD 16*2 antes de definir la libreria
#define RS RB1
#define EN RB2
#define D4 RB3
#define D5 RB4
#define D6 RB5
#define D7 RB6

#include <xc.h>
#include "ConfigBits.h"
#include "lcd_16x4.h"
#include <stdio.h>


//Definicion de los nemonicos para el puerto A
#define SW_AutoMan PORTAbits.RA0
#define SW_PosNeg  PORTAbits.RA1
#define LED_Left PORTAbits.RA2
#define LED_Stand PORTAbits.RA3
#define LED_Right PORTAbits.RA4
#define BT_Down PORTAbits.RA5
#define BT_Up PORTAbits.RA6
#define BT_Menu PORTAbits.RA7
#define Auto_Man PORTAbits.RA0


/*****************************************************************************/
//ConfigOscillator: config el oscilador a 8Mhz.  
/*****************************************************************************/
void ConfigOscillator(void)
{
    OSCCONbits.IRCF = 0b111;    //8Mhz de frecuencia del osc interno
    OSCCONbits.SCS = 0b00;      //la fuente del cock se define con los bits de configuracion
                                //en este caso es el oscilador interno INTOSC y RA6 y RA7 como i/o
}


/*****************************************************************************/
//ConfigPorts: config los puertos  
/*****************************************************************************/
void ConfigPorts(void)
{
    ANSEL = 0x00;   //Todo el puerto A como digital I/O
    PORTA = 0x00;   // ponemos a cero todo el puerto A
    PORTB = 0x00;   //ponemos a cero todo el puerto B
    
    TRISAbits.TRISA0 = 0b1; //RA0 como entrada Auto/Man
    TRISAbits.TRISA1 = 0b1; //RA1 como entrada Pos/Neg
    TRISAbits.TRISA2 = 0b0; //RA2 como salida Led Left
    TRISAbits.TRISA3 = 0b0; //RA3 como salida Led Stand
    TRISAbits.TRISA4 = 0b0; //RA4 como salida Led Right
    TRISAbits.TRISA5 = 0b1; //RA5 como entrada Down
    TRISAbits.TRISA6 = 0b1; //RA6 como entrada Up
    TRISAbits.TRISA7 = 0b1; //RA7 como entrada Menu
    
    TRISBbits.TRISB0 = 0b1; //RB0 como entrada Detector de cruce por cero
    TRISBbits.TRISB1 = 0b0; //RB0 como salida RS
    TRISBbits.TRISB2 = 0b0; //RB0 como salida EN
    TRISBbits.TRISB3 = 0b0; //RB0 como salida D4
    TRISBbits.TRISB4 = 0b0; //RB0 como salida D5
    TRISBbits.TRISB5 = 0b0; //RB0 como salida D6
    TRISBbits.TRISB6 = 0b0; //RB0 como salida D7
    TRISBbits.TRISB7 = 0b1; //RB0 como entrada de FotoCelula    
}


void main(void) 
{
    ConfigOscillator();     //llamo a la funcion que configura el oscilador
    ConfigPorts();      //llamo a la funcion que configura los puertos
    Lcd_Init();     //inicializamos el LCD
    Lcd_Clear();    //limpiamos el LCD
    
    Lcd_Set_Cursor(1,1);
    Lcd_Write_String("Hola Mundo");
    __delay_ms(1000);
    Lcd_Set_Cursor(1,2);
    Lcd_Write_String("Soy Matias");
    __delay_ms(1000);
    while(1)
    {
    
    }
return;
}
