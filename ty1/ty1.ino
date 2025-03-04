//***************************************************************************************************************************************
/* Librería para el uso de la pantalla ILI9341 en modo 8 bits
   Basado en el código de martinayotte - https://www.stm32duino.com/viewtopic.php?t=637
   Adaptación, migración y creación de nuevas funciones: Pablo Mazariegos y José Morales
   Con ayuda de: José Guerra
   IE3027: Electrónica Digital 2 - 2019
*/
//***************************************************************************************************************************************
#include <stdint.h>
#include <stdbool.h>
#include <TM4C123GH6PM.h>
#include "inc/hw_ints.h"
#include "inc/hw_memmap.h"
#include "inc/hw_types.h"
#include "driverlib/debug.h"
#include "driverlib/gpio.h"
#include "driverlib/interrupt.h"
#include "driverlib/rom_map.h"
#include "driverlib/rom.h"
#include "driverlib/sysctl.h"
#include "driverlib/timer.h"
#include <SPI.h>
#include <SD.h>

File root;
File myFile;

#include "bitmaps.h"
#include "font.h"
#include "lcd_registers.h"

#define LCD_RST PD_0
#define LCD_CS PD_1
#define LCD_RS PD_2
#define LCD_WR PD_3
#define LCD_RD PE_1
int DPINS[] = {PB_0, PB_1, PB_2, PB_3, PB_4, PB_5, PB_6, PB_7};




#define note_cc 261
#define note_dd 294
#define note_ee 329
#define note_ff 349
#define note_g 391
#define note_gS 415
#define note_a 440
#define note_aS 455
#define note_b 466
#define note_cH 523
#define note_cSH 554
#define note_dH 587
#define note_dSH 622
#define note_eH 659
#define note_fH 698
#define note_fSH 740
#define note_gH 784
#define note_gSH 830
#define note_aH 880

int buzzerPin = 40; // pin utilizado para el buzzer para sonido 

void beep(int note, int duration) // funcion utilizada para generar un sonido en el buzzer 
{
  tone(buzzerPin, note, duration / 2);
  delay(duration / 2);
  noTone(buzzerPin);
  delay(duration / 2 + 20);
}


//***************************************************************************************************************************************
// Functions Prototypes
//***************************************************************************************************************************************
void LCD_Init(void);
void LCD_CMD(uint8_t cmd);
void LCD_DATA(uint8_t data);
void SetWindows(unsigned int x1, unsigned int y1, unsigned int x2, unsigned int y2);
void LCD_Clear(unsigned int c);
void H_line(unsigned int x, unsigned int y, unsigned int l, unsigned int c);
void V_line(unsigned int x, unsigned int y, unsigned int l, unsigned int c);
void Rect(unsigned int x, unsigned int y, unsigned int w, unsigned int h, unsigned int c);
void FillRect(unsigned int x, unsigned int y, unsigned int w, unsigned int h, unsigned int c);
void LCD_Print(String text, int x, int y, int fontSize, int color, int background);

void LCD_Bitmap(unsigned int x, unsigned int y, unsigned int width, unsigned int height, unsigned char bitmap[]);
void LCD_Sprite(int x, int y, int width, int height, unsigned char bitmap[], int columns, int index, char flip, char offset);

//***************************************************************************************************************************************
// Inicialización
//***************************************************************************************************************************************
void setup() {
  pinMode(buzzerPin, OUTPUT);//set del pin del buzzer como salida 
  SysCtlClockSet(SYSCTL_SYSDIV_2_5 | SYSCTL_USE_PLL | SYSCTL_OSC_MAIN | SYSCTL_XTAL_16MHZ);
  Serial.begin(9600);
  GPIOPadConfigSet(GPIO_PORTB_BASE, 0 | 1 | 2 | 3 | 4 | 5 | 6 | 7, GPIO_STRENGTH_8MA, GPIO_PIN_TYPE_STD_WPU);
  Serial.println("Inicio");
  LCD_Init();
  LCD_Clear(0x00);

   String text1 = "Metroid Rush"; //mensaje de inicio del programa con el nombre del juego 
  LCD_Print(text1, 10, 50, 2, 0xffff, 0x00);
  delay(700);
  FillRect(0, 0, 319, 70, 0X00);
  SPI.setModule(0);
  pinMode(10, OUTPUT);
  if (!SD.begin(32)) {
    Serial.println("initialization failed!");
    return;
  }
  root = SD.open("/");

  //LCD_Sprite(int x, int y, int width, int height, unsigned char bitmap[],int columns, int index, char flip, char offset);

  //LCD_Bitmap(unsigned int x, unsigned int y, unsigned int width, unsigned int height, unsigned char bitmap[]);

  for (int x = 0; x < 319; x++) {// seteo de los pines de input de los botones para los jugadores y despliege del background 

    LCD_Bitmap(x, 100, 16, 16, piso);
    LCD_Bitmap(x, 116, 16, 16, piso);
    LCD_Bitmap(x, 207, 16, 16, piso);
    LCD_Bitmap(x, 223, 16, 16, piso);
    x += 15;
    pinMode (PC_4, INPUT);
    pinMode (PC_5, INPUT);
    pinMode (PC_6, INPUT);
    pinMode (PC_7, INPUT);
  }
  //BALAN
  //SAI Arma izquierda
  //SAMUSidle parado antes de disparar
  //SAD arma derecha
  //SAMUSAidle parada despues de disparar
}


void song() { // cancion del juego 
  beep(note_ee, 600);
  beep(note_b, 600);
  beep(note_gH, 600);
  beep(note_eH, 600);
  beep(note_b, 610);
  beep(note_eH, 625);
  beep(note_ee, 630);
  beep(note_b, 640);
  delay(75);
  beep(note_fH, 650);
  beep(note_fH, 650);
  beep(note_eH, 650);
  beep(note_dd, 650);
  beep(note_a, 650);
  beep(note_dH, 650);
  beep(note_gH, 650);
  beep(note_aH, 650);
  beep(note_fH, 650);
  beep(note_dH, 650);
  delay(75);
  beep(note_a, 600);
  beep(note_dH, 600);
  beep(note_fH, 610);
  beep(note_aH, 640);
  beep(note_a, 650);
  beep(note_g, 650);
  beep(note_ee, 650);
  delay(90);

  beep(note_ee, 550);
  beep(note_b, 500);
  beep(note_gH, 550);
  beep(note_eH, 500);
  beep(note_b, 500);
  beep(note_eH, 550);
  beep(note_ee, 600);
  beep(note_b, 600);
  delay(75);
  beep(note_a, 650);
  beep(note_dH, 650);
  beep(note_fH, 650);
  beep(note_aH, 600);
  beep(note_a, 600);
  beep(note_g, 650);
  beep(note_ee, 600);
  delay(90);
  beep(note_fH, 590);
  beep(note_fH, 580);
  beep(note_eH, 570);
  beep(note_dd, 560);
  beep(note_a, 550);
  beep(note_dH, 570);
  beep(note_gH, 580);
  beep(note_aH, 590);
  beep(note_fH, 570);
  beep(note_dH, 600);
  delay(75);
}


//***************************************************************************************************************************************
// Loop Infinito
//***************************************************************************************************************************************


void loop() {
  
  myFile = SD.open("Fondo.ino");
  if (myFile) {
    
    while (myFile.available()) {
      Serial.write(myFile.read());
    }// close the file:
    myFile.close();
  } else { // if the file didn't open, print an error:
    Serial.println("error opening test.txt");
  }
  int f = 3;
  int w = 1;
  int g = 1;
  FillRect(0, 0, 319, 60, 0x00);
  while ( f > 0) { // loop infinito
    int b = random (4);
    if (b == 1) {//opcion 1 de obstaculo salto 
      for (int x = 320 - 17; x > 0; x --) {
        delay(15);
        if (digitalRead(PC_4) == HIGH) {// seleccion del sprite que se desplegara para P1 (esto se utiliza en cada uno de los obstaculos)
          if (digitalRead(PC_6) == HIGH) {//seleccion del sprite del P2
            int anim2 = (x / 3) % 3;
            FillRect(64, 191, 22, 16, 0X00);
            FillRect(64, 84, 22, 16, 0X00);
            LCD_Bitmap(64, 159 , 24, 32, SAMUSSALTO);
            LCD_Bitmap(x, 191 , 17, 32, SALTAR);
            LCD_Bitmap(x, 84 , 17, 32, SALTAR);
            LCD_Bitmap(64, 52 , 24, 32, SAMUSP2SALTO);
            FillRect(0 , 52 , 17 , 64, 0X00);
            FillRect(0 , 159, 17 , 64, 0X00);
          }
          else if (digitalRead(PC_7) == HIGH) {
            int anim2 = (x / 3) % 3;
            FillRect(64, 52, 26, 30, 0X00);
            FillRect(64, 191, 22, 16, 0X00);
            LCD_Bitmap(64, 159 , 24, 32, SAMUSSALTO);
            LCD_Bitmap(x, 191 , 17, 32, SALTAR);
            LCD_Bitmap(x, 84 , 17, 32, SALTAR);
            LCD_Sprite(64, 84 , 16, 14, SAMUSP2BALL, 3, anim2, 0, 0);
            FillRect(0 , 52 , 17 , 64, 0X00);
            FillRect(0 , 159, 17 , 64, 0X00);
          }
          else {
            int anim2 = (x / 3) % 3;
            FillRect(64, 191, 22, 16, 0X00);
            FillRect(64, 52, 32, 15, 0x00);
            LCD_Bitmap(64, 159 , 24, 32, SAMUSSALTO);
            LCD_Bitmap(x, 191 , 17, 32, SALTAR);
            LCD_Bitmap(x, 84 , 17, 32, SALTAR);
            LCD_Sprite(64, 68 , 22, 32, SAMUSP2, 3, anim2, 0, 0);
            FillRect(0 , 52 , 17 , 64, 0X00);
            FillRect(0 , 159, 17 , 64, 0X00);
          }
        }
        else if (digitalRead(PC_5) == HIGH) {
          if (digitalRead(PC_6) == HIGH) {
            int anim2 = (x / 3) % 3;
            FillRect(64, 159, 26, 30, 0X00);
            FillRect(64, 84, 22, 16, 0X00);
            LCD_Sprite(64, 191 , 16, 14, SAMUSBALL, 3, anim2, 0, 0);
            LCD_Bitmap(x, 191 , 17, 32, SALTAR);
            LCD_Bitmap(x, 84 , 17, 32, SALTAR);
            LCD_Bitmap(64, 52 , 24, 32, SAMUSP2SALTO);
            FillRect(0 , 52 , 17 , 64, 0X00);
            FillRect(0 , 159, 17 , 64, 0X00);
          }
          else if (digitalRead(PC_7) == HIGH) {
            int anim2 = (x / 3) % 3;
            FillRect(64, 52, 26, 30, 0X00);
            FillRect(64, 159, 26, 30, 0X00);
            LCD_Sprite(64, 191 , 16, 14, SAMUSBALL, 3, anim2, 0, 0);
            LCD_Bitmap(x, 191 , 17, 32, SALTAR);
            LCD_Bitmap(x, 84 , 17, 32, SALTAR);
            LCD_Sprite(64, 84 , 16, 14, SAMUSP2BALL, 3, anim2, 0, 0);
            FillRect(0 , 52 , 17 , 64, 0X00);
            FillRect(0 , 159, 17 , 64, 0X00);
          }
          else {
            int anim2 = (x / 3) % 3;
            FillRect(64, 159, 26, 30, 0X00);
            FillRect(64, 52, 32, 15, 0x00);
            LCD_Sprite(64, 191 , 16, 14, SAMUSBALL, 3, anim2, 0, 0);
            LCD_Bitmap(x, 191 , 17, 32, SALTAR);
            LCD_Bitmap(x, 84 , 17, 32, SALTAR);
            LCD_Sprite(64, 68 , 22, 32, SAMUSP2, 3, anim2, 0, 0);
            FillRect(0 , 52 , 17 , 64, 0X00);
            FillRect(0 , 159, 17 , 64, 0X00);
          }
        }
        else {
          if (digitalRead(PC_6) == HIGH) {
            int anim2 = (x / 3) % 3;
            FillRect (64, 159, 32, 15, 0X00);
            FillRect(64, 84, 22, 16, 0X00);
            LCD_Sprite(64, 175 , 22, 32, SAMUSD, 3, anim2, 0, 0);
            LCD_Bitmap(x, 191 , 17, 32, SALTAR);
            LCD_Bitmap(x, 84 , 17, 32, SALTAR);
            LCD_Bitmap(64, 52 , 24, 32, SAMUSP2SALTO);
            FillRect(0 , 52 , 17 , 64, 0X00);
            FillRect(0 , 159, 17 , 64, 0X00);
          }
          else if (digitalRead(PC_7) == HIGH) {
            int anim2 = (x / 3) % 3;
            FillRect(64, 52, 26, 30, 0X00);
            FillRect (64, 159, 32, 15, 0X00);
            LCD_Sprite(64, 175 , 22, 32, SAMUSD, 3, anim2, 0, 0);
            LCD_Bitmap(x, 191 , 17, 32, SALTAR);
            LCD_Bitmap(x, 84 , 17, 32, SALTAR);
            LCD_Sprite(64, 84 , 16, 14, SAMUSP2BALL, 3, anim2, 0, 0);
            FillRect(0 , 52 , 17 , 64, 0X00);
            FillRect(0 , 159, 17 , 64, 0X00);
          }
          else {
            int anim2 = (x / 3) % 3;
            FillRect (64, 159, 32, 15, 0X00);
            FillRect(64, 52, 32, 15, 0x00);
            LCD_Sprite(64, 175 , 22, 32, SAMUSD, 3, anim2, 0, 0);
            LCD_Bitmap(x, 191 , 17, 32, SALTAR);
            LCD_Bitmap(x, 84 , 17, 32, SALTAR);
            LCD_Sprite(64, 68 , 22, 32, SAMUSP2, 3, anim2, 0, 0);
            FillRect(0 , 52 , 17 , 64, 0X00);
            FillRect(0 , 159, 17 , 64, 0X00);
          }
        }
      }
    }
    else if (b == 2) {//Obstaculo número 2 volverse en pelota 
      for (int x = 320 - 17; x > 0; x --) {
        delay(15);
        if (digitalRead(PC_4) == HIGH) {
          if (digitalRead(PC_6) == HIGH) {
            int anim2 = (x / 3) % 3;
            FillRect(64, 191, 22, 16, 0X00);
            FillRect(64, 84, 22, 16, 0X00);
            LCD_Bitmap(64, 159 , 24, 32, SAMUSSALTO);
            LCD_Bitmap(x, 161 , 17, 32, BOLA);
            LCD_Bitmap(x, 54 , 17, 32, BOLA);
            LCD_Bitmap(64, 52 , 24, 32, SAMUSP2SALTO);
            FillRect(0 , 52 , 17 , 64, 0X00);
            FillRect(0 , 159, 17 , 64, 0X00);
          }
          else if (digitalRead(PC_7) == HIGH) {
            int anim2 = (x / 3) % 3;
            FillRect(64, 52, 26, 30, 0X00);
            FillRect(64, 191, 22, 16, 0X00);
            LCD_Bitmap(64, 159 , 24, 32, SAMUSSALTO);
            LCD_Bitmap(x, 161 , 17, 32, BOLA);
            LCD_Bitmap(x, 54 , 17, 32, BOLA);
            LCD_Sprite(64, 84 , 16, 14, SAMUSP2BALL, 3, anim2, 0, 0);
            FillRect(0 , 52 , 17 , 64, 0X00);
            FillRect(0 , 159, 17 , 64, 0X00);
          }
          else {
            int anim2 = (x / 3) % 3;
            FillRect(64, 191, 22, 16, 0X00);
            FillRect(64, 52, 32, 15, 0x00);
            LCD_Bitmap(64, 159 , 24, 32, SAMUSSALTO);
            LCD_Bitmap(x, 161 , 17, 32, BOLA);
            LCD_Bitmap(x, 54 , 17, 32, BOLA);
            LCD_Sprite(64, 68 , 22, 32, SAMUSP2, 3, anim2, 0, 0);
            FillRect(0 , 52 , 17 , 64, 0X00);
            FillRect(0 , 159, 17 , 64, 0X00);
          }
        }
        else if (digitalRead(PC_5) == HIGH) {
          if (digitalRead(PC_6) == HIGH) {
            int anim2 = (x / 3) % 3;
            FillRect(64, 159, 26, 30, 0X00);
            FillRect(64, 84, 22, 16, 0X00);
            LCD_Sprite(64, 191 , 16, 14, SAMUSBALL, 3, anim2, 0, 0);
            LCD_Bitmap(x, 161 , 17, 32, BOLA);
            LCD_Bitmap(x, 54 , 17, 32, BOLA);
            LCD_Bitmap(64, 52 , 24, 32, SAMUSP2SALTO);
            FillRect(0 , 52 , 17 , 64, 0X00);
            FillRect(0 , 159, 17 , 64, 0X00);
          }
          else if (digitalRead(PC_7) == HIGH) {
            int anim2 = (x / 3) % 3;
            FillRect(64, 52, 26, 30, 0X00);
            FillRect(64, 159, 26, 30, 0X00);
            LCD_Sprite(64, 191 , 16, 14, SAMUSBALL, 3, anim2, 0, 0);
            LCD_Bitmap(x, 161 , 17, 32, BOLA);
            LCD_Bitmap(x, 54 , 17, 32, BOLA);
            LCD_Sprite(64, 84 , 16, 14, SAMUSP2BALL, 3, anim2, 0, 0);
            FillRect(0 , 52 , 17 , 64, 0X00);
            FillRect(0 , 159, 17 , 64, 0X00);
          }
          else {
            int anim2 = (x / 3) % 3;
            FillRect(64, 159, 26, 30, 0X00);
            FillRect(64, 52, 32, 15, 0x00);
            LCD_Sprite(64, 191 , 16, 14, SAMUSBALL, 3, anim2, 0, 0);
            LCD_Bitmap(x, 161 , 17, 32, BOLA);
            LCD_Bitmap(x, 54 , 17, 32, BOLA);
            LCD_Sprite(64, 68 , 22, 32, SAMUSP2, 3, anim2, 0, 0);
            FillRect(0 , 52 , 17 , 64, 0X00);
            FillRect(0 , 159, 17 , 64, 0X00);
          }
        }
        else {
          if (digitalRead(PC_6) == HIGH) {
            int anim2 = (x / 3) % 3;
            FillRect (64, 159, 32, 15, 0X00);
            FillRect(64, 84, 22, 16, 0X00);
            LCD_Sprite(64, 175 , 22, 32, SAMUSD, 3, anim2, 0, 0);
            LCD_Bitmap(x, 161 , 17, 32, BOLA);
            LCD_Bitmap(x, 54 , 17, 32, BOLA);
            LCD_Bitmap(64, 52 , 24, 32, SAMUSP2SALTO);
            FillRect(0 , 52 , 17 , 64, 0X00);
            FillRect(0 , 159, 17 , 64, 0X00);
          }
          else if (digitalRead(PC_7) == HIGH) {
            int anim2 = (x / 3) % 3;
            FillRect(64, 52, 26, 30, 0X00);
            FillRect (64, 159, 32, 15, 0X00);
            LCD_Sprite(64, 175 , 22, 32, SAMUSD, 3, anim2, 0, 0);
            LCD_Bitmap(x, 161 , 17, 32, BOLA);
            LCD_Bitmap(x, 54 , 17, 32, BOLA);
            LCD_Sprite(64, 84 , 16, 14, SAMUSP2BALL, 3, anim2, 0, 0);
            FillRect(0 , 52 , 17 , 64, 0X00);
            FillRect(0 , 159, 17 , 64, 0X00);
          }
          else {
            int anim2 = (x / 3) % 3;
            FillRect (64, 159, 32, 15, 0X00);
            FillRect(64, 52, 32, 15, 0x00);
            LCD_Sprite(64, 175 , 22, 32, SAMUSD, 3, anim2, 0, 0);
            LCD_Bitmap(x, 161 , 17, 32, BOLA);
            LCD_Bitmap(x, 54 , 17, 32, BOLA);
            LCD_Sprite(64, 68 , 22, 32, SAMUSP2, 3, anim2, 0, 0);
            FillRect(0 , 52 , 17 , 64, 0X00);
            FillRect(0 , 159, 17 , 64, 0X00);
          }
        }
      }
    }
    else if (b == 3) {// Sin obstaculo
      for (int x = 320 - 17; x > 0; x --) {
        delay(15);
        if (digitalRead(PC_4) == HIGH) {
          if (digitalRead(PC_6) == HIGH) {
            int anim2 = (x / 3) % 3;
            FillRect(64, 191, 22, 16, 0X00);
            FillRect(64, 84, 22, 16, 0X00);
            LCD_Bitmap(64, 159 , 24, 32, SAMUSSALTO);
            LCD_Bitmap(x, 207 , 17, 32, piso);
            LCD_Bitmap(x, 100 , 17, 32, piso);
            LCD_Bitmap(64, 52 , 24, 32, SAMUSP2SALTO);
            FillRect(0 , 52 , 17 , 64, 0X00);
            FillRect(0 , 159, 17 , 64, 0X00);
          }
          else if (digitalRead(PC_7) == HIGH) {
            int anim2 = (x / 3) % 3;
            FillRect(64, 52, 26, 30, 0X00);
            FillRect(64, 191, 22, 16, 0X00);
            LCD_Bitmap(64, 159 , 24, 32, SAMUSSALTO);
            LCD_Bitmap(x, 207 , 17, 32, piso);
            LCD_Bitmap(x, 100 , 17, 32, piso);
            LCD_Sprite(64, 84 , 16, 14, SAMUSP2BALL, 3, anim2, 0, 0);
            FillRect(0 , 52 , 17 , 64, 0X00);
            FillRect(0 , 159, 17 , 64, 0X00);
          }
          else {
            int anim2 = (x / 3) % 3;
            FillRect(64, 191, 22, 16, 0X00);
            FillRect(64, 52, 32, 15, 0x00);
            LCD_Bitmap(64, 159 , 24, 32, SAMUSSALTO);
            LCD_Bitmap(x, 207 , 17, 32, piso);
            LCD_Bitmap(x, 100 , 17, 32, piso);
            LCD_Sprite(64, 68 , 22, 32, SAMUSP2, 3, anim2, 0, 0);
            FillRect(0 , 52 , 17 , 64, 0X00);
            FillRect(0 , 159, 17 , 64, 0X00);
          }
        }
        else if (digitalRead(PC_5) == HIGH) {
          if (digitalRead(PC_6) == HIGH) {
            int anim2 = (x / 3) % 3;
            FillRect(64, 159, 26, 30, 0X00);
            FillRect(64, 84, 22, 16, 0X00);
            LCD_Sprite(64, 191 , 16, 14, SAMUSBALL, 3, anim2, 0, 0);
            LCD_Bitmap(x, 207 , 17, 32, piso);
            LCD_Bitmap(x, 100 , 17, 32, piso);
            LCD_Bitmap(64, 52 , 24, 32, SAMUSP2SALTO);
            FillRect(0 , 52 , 17 , 64, 0X00);
            FillRect(0 , 159, 17 , 64, 0X00);
          }
          else if (digitalRead(PC_7) == HIGH) {
            int anim2 = (x / 3) % 3;
            FillRect(64, 52, 26, 30, 0X00);
            FillRect(64, 159, 26, 30, 0X00);
            LCD_Sprite(64, 191 , 16, 14, SAMUSBALL, 3, anim2, 0, 0);
            LCD_Bitmap(x, 207 , 17, 32, piso);
            LCD_Bitmap(x, 100 , 17, 32, piso);
            LCD_Sprite(64, 84 , 16, 14, SAMUSP2BALL, 3, anim2, 0, 0);
            FillRect(0 , 52 , 17 , 64, 0X00);
            FillRect(0 , 159, 17 , 64, 0X00);
          }
          else {
            int anim2 = (x / 3) % 3;
            FillRect(64, 159, 26, 30, 0X00);
            FillRect(64, 52, 32, 15, 0x00);
            LCD_Sprite(64, 191 , 16, 14, SAMUSBALL, 3, anim2, 0, 0);
            LCD_Bitmap(x, 207 , 17, 32, piso);
            LCD_Bitmap(x, 100 , 17, 32, piso);
            LCD_Sprite(64, 68 , 22, 32, SAMUSP2, 3, anim2, 0, 0);
            FillRect(0 , 52 , 17 , 64, 0X00);
            FillRect(0 , 159, 17 , 64, 0X00);
          }
        }
        else {
          if (digitalRead(PC_6) == HIGH) {
            int anim2 = (x / 3) % 3;
            FillRect (64, 159, 32, 15, 0X00);
            FillRect(64, 84, 22, 16, 0X00);
            LCD_Sprite(64, 175 , 22, 32, SAMUSD, 3, anim2, 0, 0);
            LCD_Bitmap(x, 207 , 17, 32, piso);
            LCD_Bitmap(x, 100 , 17, 32, piso);
            LCD_Bitmap(64, 52 , 24, 32, SAMUSP2SALTO);
            FillRect(0 , 52 , 17 , 64, 0X00);
            FillRect(0 , 159, 17 , 64, 0X00);
          }
          else if (digitalRead(PC_7) == HIGH) {
            int anim2 = (x / 3) % 3;
            FillRect(64, 52, 26, 30, 0X00);
            FillRect (64, 159, 32, 15, 0X00);
            LCD_Sprite(64, 175 , 22, 32, SAMUSD, 3, anim2, 0, 0);
            LCD_Bitmap(x, 207 , 17, 32, piso);
            LCD_Bitmap(x, 100 , 17, 32, piso);
            LCD_Sprite(64, 84 , 16, 14, SAMUSP2BALL, 3, anim2, 0, 0);
            FillRect(0 , 52 , 17 , 64, 0X00);
            FillRect(0 , 159, 17 , 64, 0X00);
          }
          else {
            int anim2 = (x / 3) % 3;
            FillRect (64, 159, 32, 15, 0X00);
            FillRect(64, 52, 32, 15, 0x00);
            LCD_Sprite(64, 175 , 22, 32, SAMUSD, 3, anim2, 0, 0);
            LCD_Bitmap(x, 207 , 17, 32, piso);
            LCD_Bitmap(x, 100 , 17, 32, piso);
            LCD_Sprite(64, 68 , 22, 32, SAMUSP2, 3, anim2, 0, 0);
            FillRect(0 , 52 , 17 , 64, 0X00);
            FillRect(0 , 159, 17 , 64, 0X00);
          }
        }
      }
    }
  }
}
//***************************************************************************************************************************************
// Función para inicializar LCD
//***************************************************************************************************************************************
void LCD_Init(void) {
  pinMode(LCD_RST, OUTPUT);
  pinMode(LCD_CS, OUTPUT);
  pinMode(LCD_RS, OUTPUT);
  pinMode(LCD_WR, OUTPUT);
  pinMode(LCD_RD, OUTPUT);
  for (uint8_t i = 0; i < 8; i++) {
    pinMode(DPINS[i], OUTPUT);
  }
  //****************************************
  // Secuencia de Inicialización
  //****************************************
  digitalWrite(LCD_CS, HIGH);
  digitalWrite(LCD_RS, HIGH);
  digitalWrite(LCD_WR, HIGH);
  digitalWrite(LCD_RD, HIGH);
  digitalWrite(LCD_RST, HIGH);
  delay(5);
  digitalWrite(LCD_RST, LOW);
  delay(20);
  digitalWrite(LCD_RST, HIGH);
  delay(150);
  digitalWrite(LCD_CS, LOW);
  //****************************************
  LCD_CMD(0xE9);  // SETPANELRELATED
  LCD_DATA(0x20);
  //****************************************
  LCD_CMD(0x11); // Exit Sleep SLEEP OUT (SLPOUT)
  delay(100);
  //****************************************
  LCD_CMD(0xD1);    // (SETVCOM)
  LCD_DATA(0x00);
  LCD_DATA(0x71);
  LCD_DATA(0x19);
  //****************************************
  LCD_CMD(0xD0);   // (SETPOWER)
  LCD_DATA(0x07);
  LCD_DATA(0x01);
  LCD_DATA(0x08);
  //****************************************
  LCD_CMD(0x36);  // (MEMORYACCESS)
  LCD_DATA(0x40 | 0x80 | 0x20 | 0x08); // LCD_DATA(0x19);
  //****************************************
  LCD_CMD(0x3A); // Set_pixel_format (PIXELFORMAT)
  LCD_DATA(0x05); // color setings, 05h - 16bit pixel, 11h - 3bit pixel
  //****************************************
  LCD_CMD(0xC1);    // (POWERCONTROL2)
  LCD_DATA(0x10);
  LCD_DATA(0x10);
  LCD_DATA(0x02);
  LCD_DATA(0x02);
  //****************************************
  LCD_CMD(0xC0); // Set Default Gamma (POWERCONTROL1)
  LCD_DATA(0x00);
  LCD_DATA(0x35);
  LCD_DATA(0x00);
  LCD_DATA(0x00);
  LCD_DATA(0x01);
  LCD_DATA(0x02);
  //****************************************
  LCD_CMD(0xC5); // Set Frame Rate (VCOMCONTROL1)
  LCD_DATA(0x04); // 72Hz
  //****************************************
  LCD_CMD(0xD2); // Power Settings  (SETPWRNORMAL)
  LCD_DATA(0x01);
  LCD_DATA(0x44);
  //****************************************
  LCD_CMD(0xC8); //Set Gamma  (GAMMASET)
  LCD_DATA(0x04);
  LCD_DATA(0x67);
  LCD_DATA(0x35);
  LCD_DATA(0x04);
  LCD_DATA(0x08);
  LCD_DATA(0x06);
  LCD_DATA(0x24);
  LCD_DATA(0x01);
  LCD_DATA(0x37);
  LCD_DATA(0x40);
  LCD_DATA(0x03);
  LCD_DATA(0x10);
  LCD_DATA(0x08);
  LCD_DATA(0x80);
  LCD_DATA(0x00);
  //****************************************
  LCD_CMD(0x2A); // Set_column_address 320px (CASET)
  LCD_DATA(0x00);
  LCD_DATA(0x00);
  LCD_DATA(0x01);
  LCD_DATA(0x3F);
  //****************************************
  LCD_CMD(0x2B); // Set_page_address 480px (PASET)
  LCD_DATA(0x00);
  LCD_DATA(0x00);
  LCD_DATA(0x01);
  LCD_DATA(0xE0);
  //  LCD_DATA(0x8F);
  LCD_CMD(0x29); //display on
  LCD_CMD(0x2C); //display on

  LCD_CMD(ILI9341_INVOFF); //Invert Off
  delay(120);
  LCD_CMD(ILI9341_SLPOUT);    //Exit Sleep
  delay(120);
  LCD_CMD(ILI9341_DISPON);    //Display on
  digitalWrite(LCD_CS, HIGH);
}
//***************************************************************************************************************************************
// Función para enviar comandos a la LCD - parámetro (comando)
//***************************************************************************************************************************************
void LCD_CMD(uint8_t cmd) {
  digitalWrite(LCD_RS, LOW);
  digitalWrite(LCD_WR, LOW);
  GPIO_PORTB_DATA_R = cmd;
  digitalWrite(LCD_WR, HIGH);
}
//***************************************************************************************************************************************
// Función para enviar datos a la LCD - parámetro (dato)
//***************************************************************************************************************************************
void LCD_DATA(uint8_t data) {
  digitalWrite(LCD_RS, HIGH);
  digitalWrite(LCD_WR, LOW);
  GPIO_PORTB_DATA_R = data;
  digitalWrite(LCD_WR, HIGH);
}
//***************************************************************************************************************************************
// Función para definir rango de direcciones de memoria con las cuales se trabajara (se define una ventana)
//***************************************************************************************************************************************
void SetWindows(unsigned int x1, unsigned int y1, unsigned int x2, unsigned int y2) {
  LCD_CMD(0x2a); // Set_column_address 4 parameters
  LCD_DATA(x1 >> 8);
  LCD_DATA(x1);
  LCD_DATA(x2 >> 8);
  LCD_DATA(x2);
  LCD_CMD(0x2b); // Set_page_address 4 parameters
  LCD_DATA(y1 >> 8);
  LCD_DATA(y1);
  LCD_DATA(y2 >> 8);
  LCD_DATA(y2);
  LCD_CMD(0x2c); // Write_memory_start
}
//***************************************************************************************************************************************
// Función para borrar la pantalla - parámetros (color)
//***************************************************************************************************************************************
void LCD_Clear(unsigned int c) {
  unsigned int x, y;
  LCD_CMD(0x02c); // write_memory_start
  digitalWrite(LCD_RS, HIGH);
  digitalWrite(LCD_CS, LOW);
  SetWindows(0, 0, 319, 239); // 479, 319);
  for (x = 0; x < 320; x++)
    for (y = 0; y < 240; y++) {
      LCD_DATA(c >> 8);
      LCD_DATA(c);
    }
  digitalWrite(LCD_CS, HIGH);
}
//***************************************************************************************************************************************
// Función para dibujar una línea horizontal - parámetros ( coordenada x, cordenada y, longitud, color)
//***************************************************************************************************************************************
void H_line(unsigned int x, unsigned int y, unsigned int l, unsigned int c) {
  unsigned int i, j;
  LCD_CMD(0x02c); //write_memory_start
  digitalWrite(LCD_RS, HIGH);
  digitalWrite(LCD_CS, LOW);
  l = l + x;
  SetWindows(x, y, l, y);
  j = l;// * 2;
  for (i = 0; i < l; i++) {
    LCD_DATA(c >> 8);
    LCD_DATA(c);
  }
  digitalWrite(LCD_CS, HIGH);
}
//***************************************************************************************************************************************
// Función para dibujar una línea vertical - parámetros ( coordenada x, cordenada y, longitud, color)
//***************************************************************************************************************************************
void V_line(unsigned int x, unsigned int y, unsigned int l, unsigned int c) {
  unsigned int i, j;
  LCD_CMD(0x02c); //write_memory_start
  digitalWrite(LCD_RS, HIGH);
  digitalWrite(LCD_CS, LOW);
  l = l + y;
  SetWindows(x, y, x, l);
  j = l; //* 2;
  for (i = 1; i <= j; i++) {
    LCD_DATA(c >> 8);
    LCD_DATA(c);
  }
  digitalWrite(LCD_CS, HIGH);
}
//***************************************************************************************************************************************
// Función para dibujar un rectángulo - parámetros ( coordenada x, cordenada y, ancho, alto, color)
//***************************************************************************************************************************************
void Rect(unsigned int x, unsigned int y, unsigned int w, unsigned int h, unsigned int c) {
  H_line(x  , y  , w, c);
  H_line(x  , y + h, w, c);
  V_line(x  , y  , h, c);
  V_line(x + w, y  , h, c);
}
//***************************************************************************************************************************************
// Función para dibujar un rectángulo relleno - parámetros ( coordenada x, cordenada y, ancho, alto, color)
//***************************************************************************************************************************************
void FillRect(unsigned int x, unsigned int y, unsigned int w, unsigned int h, unsigned int c) {
  unsigned int i;
  for (i = 0; i < h; i++) {
    H_line(x  , y  , w, c);
    H_line(x  , y + i, w, c);
  }
}
//***************************************************************************************************************************************
// Función para dibujar texto - parámetros ( texto, coordenada x, cordenada y, color, background)
//***************************************************************************************************************************************
void LCD_Print(String text, int x, int y, int fontSize, int color, int background) {
  int fontXSize ;
  int fontYSize ;

  if (fontSize == 1) {
    fontXSize = fontXSizeSmal ;
    fontYSize = fontYSizeSmal ;
  }
  if (fontSize == 2) {
    fontXSize = fontXSizeBig ;
    fontYSize = fontYSizeBig ;
  }

  char charInput ;
  int cLength = text.length();
  Serial.println(cLength, DEC);
  int charDec ;
  int c ;
  int charHex ;
  char char_array[cLength + 1];
  text.toCharArray(char_array, cLength + 1) ;
  for (int i = 0; i < cLength ; i++) {
    charInput = char_array[i];
    Serial.println(char_array[i]);
    charDec = int(charInput);
    digitalWrite(LCD_CS, LOW);
    SetWindows(x + (i * fontXSize), y, x + (i * fontXSize) + fontXSize - 1, y + fontYSize );
    long charHex1 ;
    for ( int n = 0 ; n < fontYSize ; n++ ) {
      if (fontSize == 1) {
        charHex1 = pgm_read_word_near(smallFont + ((charDec - 32) * fontYSize) + n);
      }
      if (fontSize == 2) {
        charHex1 = pgm_read_word_near(bigFont + ((charDec - 32) * fontYSize) + n);
      }
      for (int t = 1; t < fontXSize + 1 ; t++) {
        if (( charHex1 & (1 << (fontXSize - t))) > 0 ) {
          c = color ;
        } else {
          c = background ;
        }
        LCD_DATA(c >> 8);
        LCD_DATA(c);
      }
    }
    digitalWrite(LCD_CS, HIGH);
  }
}
//***************************************************************************************************************************************
// Función para dibujar una imagen a partir de un arreglo de colores (Bitmap) Formato (Color 16bit R 5bits G 6bits B 5bits)
//***************************************************************************************************************************************
void LCD_Bitmap(unsigned int x, unsigned int y, unsigned int width, unsigned int height, unsigned char bitmap[]) {
  LCD_CMD(0x02c); // write_memory_start
  digitalWrite(LCD_RS, HIGH);
  digitalWrite(LCD_CS, LOW);

  unsigned int x2, y2;
  x2 = x + width;
  y2 = y + height;
  SetWindows(x, y, x2 - 1, y2 - 1);
  unsigned int k = 0;
  unsigned int i, j;

  for (int i = 0; i < width; i++) {
    for (int j = 0; j < height; j++) {
      LCD_DATA(bitmap[k]);
      LCD_DATA(bitmap[k + 1]);
      //LCD_DATA(bitmap[k]);
      k = k + 2;
    }
  }
  digitalWrite(LCD_CS, HIGH);
}
//***************************************************************************************************************************************
// Función para dibujar una imagen sprite - los parámetros columns = número de imagenes en el sprite, index = cual desplegar, flip = darle vuelta
//***************************************************************************************************************************************
void LCD_Sprite(int x, int y, int width, int height, unsigned char bitmap[], int columns, int index, char flip, char offset) {
  LCD_CMD(0x02c); // write_memory_start
  digitalWrite(LCD_RS, HIGH);
  digitalWrite(LCD_CS, LOW);

  unsigned int x2, y2;
  x2 =   x + width;
  y2 =    y + height;
  SetWindows(x, y, x2 - 1, y2 - 1);
  int k = 0;
  int ancho = ((width * columns));
  if (flip) {
    for (int j = 0; j < height; j++) {
      k = (j * (ancho) + index * width - 1 - offset) * 2;
      k = k + width * 2;
      for (int i = 0; i < width; i++) {
        LCD_DATA(bitmap[k]);
        LCD_DATA(bitmap[k + 1]);
        k = k - 2;
      }
    }
  } else {
    for (int j = 0; j < height; j++) {
      k = (j * (ancho) + index * width + 1 + offset) * 2;
      for (int i = 0; i < width; i++) {
        LCD_DATA(bitmap[k]);
        LCD_DATA(bitmap[k + 1]);
        k = k + 2;
      }
    }


  }
  digitalWrite(LCD_CS, HIGH);
}
