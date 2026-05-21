🧪 Prueba Individual 2: Control de Servomotor por Interrupción (Validación TRL 3)

Descripción de la Prueba

En concordancia con los requerimientos del nivel TRL 3 (Prueba experimental de concepto), se realizó la validación técnica e individual del módulo de generación de señales de control de posición para un servomotor. Debido a que el oscilador externo opera a una alta frecuencia (20 MHz), el uso del periférico PWM estándar (Timer2) limita la flexibilidad para alcanzar la frecuencia exacta de 50 Hz demandada por los servomotores sin sacrificar resolución. 

Por lo tanto, la estrategia implementada consiste en un control por software de alta precisión utilizando el módulo CCP1 en Modo Comparación (Compare Mode) emparejado con el Timer1, operando bajo un esquema de interrupciones concurrentes.

---

⚙️ Fundamento Técnico y Cálculos de Tiempo

Con un reloj de $F_{osc} = 20\text{ MHz}$, el ciclo de instrucción interna del PIC es de $0.2\ \mu\text{s}$. Al configurar el registro `T1CON` con un prescaler de 1:2, cada incremento ("tick") del Timer1 toma exactamente $0.4\ \mu\text{s}$.

1. Base de Tiempo (Periodo de 20 ms): 
   Se requieren $50,000$ ticks de reloj ($20\text{ ms} / 0.4\ \mu\text{s}$). Como el Timer1 es un registro de 16 bits que se desborda a los $65,536$ ticks, se calcula el valor de recarga en el vector de interrupción:
   $$\text{TMR1}_{\text{Recarga}} = 65,536 - 50,000 = 15,536$$

2. Modulación del Ancho de Pulso (Duty Cycle):
   El pulso en alto inicia inmediatamente en el desborde del Timer1 (forzando `RC2 = 1`). La caída del pulso (`RC2 = 0`) es gobernada por la interrupción de coincidencia del módulo CCP1 (`CCP1IF`).
   * **Posición 0% (~1.0 ms):** Requiere $2,500$ ticks $\rightarrow \text{CCPR1} = 15,536 + 2,500 = 18,036$.
   * **Posición 100% (~2.0 ms):** Requiere $5,000$ ticks $\rightarrow \text{CCPR1} = 15,536 + 5,000 = 20,536$.
   * **Ecuación de control implementada:** $\text{CCPR1} = (25 \cdot \text{Porcentaje}) + 18,036$.

---

💻 Resultados de la Simulación

A continuación se presentan las capturas del entorno de simulación donde se verifica la conmutación dinámica del ancho de pulso en el pin `RC2` mapeado a la lectura de los estados lógicos de los pines de entrada `RB0` y `RB1` (pines configurados con resistencias de Pull-Up o lógica digital):

1. Estado 00 (Modo 25% de Giro)

Al presentarse la condición lógica `RB1 = 0` y `RB0 = 0`, la función `Servo_Set(25)` modifica dinámicamente el par de registros `CCPR1H:CCPR1L` al valor de $18,661$. Esto se traduce en un ancho de pulso exacto de $1.25\text{ ms}$ en el osciloscopio, posicionando el servo de forma estable.

<img width="882" height="556" alt="image" src="https://github.com/user-attachments/assets/30db4c87-f723-456b-b723-3479c6ea7f37" />

<img width="778" height="193" alt="image" src="https://github.com/user-attachments/assets/285f3728-340b-4aa2-bcfb-e552ab296dd8" />


2. Estado 01 (Modo 50% de Giro - Posición Central)

Al presentarse la combinación `RB1 = 0` y `RB0 = 1`, el firmware ejecuta la rutina para fijar el ciclo en un $50\%$. El osciloscopio virtual registra un pulso en alto balanceado de $1.50\text{ ms}$ ($19,286$ ticks), forzando el alineamiento del servomotor en su centro mecánico.

<img width="907" height="570" alt="image" src="https://github.com/user-attachments/assets/34128cb6-e36e-4e77-8366-83b956b5b77e" />

<img width="794" height="169" alt="image" src="https://github.com/user-attachments/assets/98da8b3f-20ed-481a-a317-a42b960b6a92" />


3. Estado 10 (Modo 75% de Giro)

Con la entrada `RB1 = 1` y `RB0 = 0`, el sistema realiza el cálculo matemático en tiempo de ejecución, actualizando el punto de comparación a $19,911$ ticks. La señal expande su modulación a $1.75\text{ ms}$ y el motor avanza proporcionalmente.

<img width="953" height="598" alt="image" src="https://github.com/user-attachments/assets/0107317b-d0bf-4906-a540-ac728c6808c5" />

<img width="798" height="201" alt="image" src="https://github.com/user-attachments/assets/dd6943b2-44ed-4b5e-b035-43021430710c" />

4. Estado 11 (Modo 100% de Giro - Deflexión Máxima)

Al activarse ambas entradas en alto (`RB1 = 1`, `RB0 = 1`), el ciclo de comparación del bloque CCP1 alcanza su cota superior programada de $20,536$ ticks. En el osciloscopio se corrobora el pulso crítico de $2.0\text{ ms}$, llevando el servomotor a su límite de carrera superior de forma segura y sin desbordes.

<img width="899" height="590" alt="image" src="https://github.com/user-attachments/assets/66b15714-55f0-4968-9191-fc9804879737" />

<img width="792" height="246" alt="image" src="https://github.com/user-attachments/assets/e33046ad-284b-4d10-89c3-64949841ee7e" />

---

📌 Conclusiones de la Etapa

- **Precisión Temporal Concedida:** Al delegar la caída del pulso al hardware del módulo CCP1 en lugar de usar retardos por software (`__delay_ms`), el hilo principal (`main`) queda completamente libre de bloqueos temporales para procesar la lectura de los sensores o entradas en paralelo.
- **Factibilidad Técnica Demostrada (TRL 3):** El algoritmo de control por interrupciones responde instantáneamente a los cambios de estado en el puerto B, operando la señal periódica de 50 Hz de forma asíncrona y sin interrupciones en el refresco del servomotor.
- **Siguiente Paso (Hacia TRL 4):** Reemplazar los condicionales de testeo físico del puerto B (`RB0` y `RB1`) por la recepción de las variables de control provenientes de la trama UART (desarrollada en la Prueba 1). Con esto, el ciclo de trabajo del servomotor responderá de manera remota a los comandos Bluetooth del módulo ESP32.
