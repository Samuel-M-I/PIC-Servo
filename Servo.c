#include <xc.h>

void __interrupt() ISR(void) {
    if (TMR1IF) {
        TMR1IF = 0;
        TMR1 = 15536;   // Recarga para periodo de 20ms
        RC2 = 1;        // Inicia el pulso
    }
    if (CCP1IF) {
        CCP1IF = 0;
        RC2 = 0;        // Termina el pulso
    }
}

void Servo_Set(int porcentaje) {
    if ((porcentaje >= 0) && (porcentaje <= 100)) {
        //En caso de que el servo funcione con tiempo de 1ms a 2ms use este CCPR1 = 25 * porcentaje + 18036;
        CCPR1 = 45 * porcentaje + 17036;
    }
}

void Servo_Init(void) {
    TRISC2  = 0;              // RC2 como salida
    RC2     = 0;

    // Fosc=20MHz, prescaler 1:2 ? 0.4µs/tick
    T1CON   = 0b00010001;     // TMR1ON, prescaler 1:2

    CCP1CON = 0b00001010;     // Compare, software interrupt

    TMR1    = 15536;
    CCPR1   = 18036;          // Pulso inicial = 1ms (0%)

    TMR1IF  = 0;
    CCP1IF  = 0;

    PIE1    = 0b00000101;     // TMR1IE + CCP1IE
    INTCON  = 0b11000000;     // GIE + PEIE

    Servo_Set(0);
}