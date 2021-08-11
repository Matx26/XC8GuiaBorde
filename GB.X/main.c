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
#include <pic16f88.h>
#include "ConfigBits.h"
#include "lcd_16x4.h"
#include <stdio.h>


//Definicion de los nemonicos para el puerto A
#define SW_AutoMan PORTAbits.RA0
#define SW_PosNeg  PORTAbits.RA1
#define LED_Left PORTAbits.RA2
#define LED_Stand PORTAbits.RA3
#define LED_Right PORTAbits.RA4
#define BT_DownLeft PORTAbits.RA5
#define BT_UpRight PORTAbits.RA6
#define BT_Menu PORTAbits.RA7


//Declaracionde Variables Globales
int AutMan = 0;     //indica si esta en modo manual o automatico
char FotoCelula = 0;   //para indicar si la fotocelula se activo
int CountSteps = 0;


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
    ANSEL = 0x00;   //Todo los puertos analogicos como digital I/O
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

//*****************************************************************************/
//Interrupt: Atencion de las interrupciones INT0 y cambio en RB7 
//*****************************************************************************/
void _interrupt()
{
    if(INTCONbits.RBIF == 1)    //pregunto si la fotocelula se activo 
    {
        FotoCelula = 1; //se activo la fotocelula
        INTCONbits.RBIF = 0;    //se borra el flag 
    }
    
    if(INTCONbits.INTF == 1)    //pregunto si el detector de cruce por cero    
    {
        CountSteps ++;  //incrementamos el contador de pasos
        INTCONbits.INTF = 0;    //se borra el flag
    }
}


void Retardo(void)
{
    __delay_ms(100);

}


void main(void) 
{
    //declaracion de variables locales
    int Count = 0;  //se usa para el sistema atirrebote
    int Steps = 5; //configuracion de la cantidad de pasos (steps)
    
    
    ConfigOscillator();     //llamo a la funcion que configura el oscilador
    ConfigPorts();      //llamo a la funcion que configura los puertos
    Lcd_Init();     //inicializamos el LCD
    Lcd_Clear();    //limpiamos el LCD
    
      
    Lcd_Set_Cursor(1,1);
    Lcd_Write_String("Hola Mundo");
    __delay_ms(100);
    Lcd_Set_Cursor(1,2);
    Lcd_Write_String("Soy Matias");
    __delay_ms(1000);
    
    //Conmfiguracion preliminar de las interrupciones
    INTCONbits.GIE = 1;     //habilito globalmente las interrupciones
    //Configuro INT0 (cruce por cero)
    OPTION_REGbits.INTEDG = 0;  //la interrupcion se activara con el flanco descendente en INT0
    INTCONbits.INTF = 0;    //se borra por las dudas el flag de INT0
    INTCONbits.INTE = 0;    //DesHabilito la interrupcion por INT0 por ahora
    //Configuro Cambio por RB7 (fotocelula)
    INTCONbits.RBIF = 0;    //se borra por las dudas el flag por cambio de RB7
    INTCONbits.RBIE = 0;    //DesHabilito la interrupcion por cambio de RB7 por ahora
    
    
    while(1)
    {
        if(SW_AutoMan == 1)
        {
            AutMan = 1;    //indicamos que esta en modo automatico
            INTCONbits.RBIE = 1;    //habilitamos la interrupcion por cambio de RB7 (foto celula)
            if(FotoCelula == 1 || PORTBbits.RB7 == 1) //pregunto si se activo la fotocelula
            {
                if(SW_PosNeg == 1)  //pregunto de esta en logica positiva o negativa
                {
                    LED_Left = 0;   //se apaga la bobina izquierda por las dudas ante de prender la derecha
                    LED_Right = 1;  //se prende la bobina derecha  
                    INTCONbits.INTE = 1;   //se habilita la interrupcion INT0(detection de cruce por cero)
                    if(CountSteps >= Steps) 
                    {
                        LED_Right = 0;  //se apaga la bobina derecha 
                        LED_Left = 0;   //se apaga la bobina izquierda
                        INTCONbits.INTE = 0;   //se deshabilita la interrupcion INT0(detection de cruce por cero)
                        CountSteps = 0;     //se reinica el contador de pasos
                        Retardo();
                    }                
                }
                else
                {
                    LED_Right = 0;  //se apaga la bobina derecha
                    LED_Left = 1;   //se prende la bobina izquierda 
                    INTCONbits.INTE = 1;   //se habilita la interrupcion INT0(detection de cruce por cero)
                    if(CountSteps >= Steps) 
                    {
                        LED_Right = 0;  //se apaga la bobina derecha 
                        LED_Left = 0;   //se apaga la bobina izquierda
                        INTCONbits.INTE = 0;   //se deshabilita la interrupcion INT0(detection de cruce por cero)
                        CountSteps = 0;     //se reinica el contador de pasos
                        Retardo();
                    } 
                    
                }
            }
            
        }
        else
        {
            AutMan = 0;   //indicamos que no esta en modo manual
            INTCONbits.RBIE = 1;    //deshabilitamos la interrupcion por cambio de RB7 (foto celula)
            
            if(BT_UpRight == 1)
            {
                __delay_ms(20);
                Count ++;   //incrementamos el contador antirrebote
                if(Count >= 5)
                {
                    Count = 0;
                    LED_Left = 0;   //por las dudas se apaga la bobina izquierda
                    LED_Right = 1;  //se energiza la bobina derecha
                }
                
            }
            else
            {
                LED_Right = 0;  //apagamos la bobina derecha
            }
            
            if(BT_DownLeft == 1)
            {
                __delay_ms(20);
                Count ++;   //incrementamos el contador antirrebote
                if(Count >= 5)
                {
                    Count = 0;
                    LED_Right = 0;   //por las dudas se apaga la bobina derecha
                    LED_Left = 1;  //se energiza la bobina izquierda
                }
            }
            else
            {
                LED_Left = 0;   //apagamos la bobina izquierda
            }
        }
                
            
        
    }
return;
}


