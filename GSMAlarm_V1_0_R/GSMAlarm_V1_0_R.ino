

//*****************************************************************************************
//***    
//***    Compilado en Arduino v1.8.12
//***    
//***    Producto:
//***    Proyecto: Alarma GSM
//***    Version: V1.0
//***    Fecha: 20-Jul-20
//***    Modificacion: 
//***    Autor: Julio Peñaloza
//***    WhatsApp: +52 1 55 4469 1477
//***    E-mail: dinteltec@hotmail.com
//***    Tarjeta: Arduino Uno R3 ATMEGA328P-PU
//***    Modulos:
//***         1.  Arduino_Timer_Virtual_Library_V1_1.zip:
//***             https://drive.google.com/file/d/1gg8gyJfyiwkZyAqj2GIPtSjwE1WMtNoZ/view?usp=sharing
//***         2.  Modulo SIM800L Adafruit_FONA-master.zip:
//***             https://drive.google.com/file/d/1QNpE3cJBIm7Bz3ktVDO3KjsdrLt3Kbeb/view?usp=sharing
//***             
//***    
// ****************************************************************************************
//                        LIBRERIAS
// ****************************************************************************************
//#include <Timer_Virtual.h>
#include <arduino-timer.h>
#include <SoftwareSerial.h>
#include "Adafruit_FONA.h"

// ****************************************************************************************
//                        DEFINICIONES
// ****************************************************************************************

// ****************************************************************************************
//                        HARDWARE
// ****************************************************************************************
//Timer_Virtual Timer = Timer_Virtual();             //
Arduino_h = arduino-timer();             //
const uint8_t SWSerialTX_Pin = 3;                  // 
const uint8_t SWSerialRX_Pin = 4;                  // 
const uint8_t SIM800RST_Pin = 5;                   //
const uint8_t Magnetic_Pin = 6;                    //
const uint8_t BlinkLED_Pin =  13;                  //
SoftwareSerial SIM800 = SoftwareSerial( SWSerialRX_Pin, SWSerialTX_Pin );
SoftwareSerial *SIM800Serial = &SIM800;            //
Adafruit_FONA fona = Adafruit_FONA( SIM800RST_Pin );//

// ****************************************************************************************
//                        CONSTANTES DE TIEMPOS
// ****************************************************************************************
const unsigned long BeatTime = 1000;               // 
const unsigned int BlinkLEDTime = 1000;            // 

// ****************************************************************************************
//                        MEMORIA RAM
// ****************************************************************************************
//                        BANDERAS DE EVENTOS
// ****************************************************************************************
boolean BlinkLEDFlag = true;                       // 
boolean SWSGetHWSSendFlag = true;                  // 
boolean MagneticOpenFlag = true;                   // 
boolean MagneticCloseFlag = true;                  // 
boolean EventHandleFlag = true;                    // 

// ****************************************************************************************
//                        FUNCIONALIDAD
// ****************************************************************************************
#define BUFFER_SIZE         255                    //
const char CelNumber[ 20 ] = { "231715402078" };   // 
const char MessageOpen[ 141 ] = { "SE ABRIO LA PUERTA!\0" };//
const char MessageClose[ 141 ] = { "SE CERRO LA PUERTA.\0" };//
boolean SIM800Error = false;                       // 
unsigned long CurrentCounter;                      //
unsigned long PreviousCounter = 0;                 //
char packetBuffer[ BUFFER_SIZE ];                  //
TimerVirtual                                       //
  BlinkLEDTV,                                      //
  SWSGetHWSSendTV,                                 //
  MagneticOpenTV,                                  //
  MagneticCloseTV,                                 //
  EventHandleTV;                                   //

// ****************************************************************************************
//                        CONFIGURACION INICIAL
// ****************************************************************************************
void setup()
{
  // Inicializa Bus Serie
  while(!Serial);
  Serial.begin( 115200 );
  Serial.println("\r\n\r\nALARMA GSM CON SIM800L\r\n");

  // Inicializa SIM800L
  SIM800Serial->begin(4800);
  if ( !fona.begin( *SIM800Serial ) )
    SIM800Error = true;
  else
  {
    //type = fona.type();
    Serial.println(F("SIM800L OK"));
  }
  
  // Inicializa Hardware
  pinMode( BlinkLED_Pin, OUTPUT );                 // 
  pinMode( Magnetic_Pin, INPUT_PULLUP );           // 
  // Activa procesos Iniciales
  Timer.Start( &BlinkLEDTV, BlinkLEDTime );        // 
  SWSGetHWSSendTV.Ending = true;                   // 
  MagneticOpenTV.Ending = true;                    // 
  EventHandleTV.Ending = true;                     // 
}

// ****************************************************************************************
//                        TASK MANAGER LOOP (ADMINISTRADOR DE PROCESOS)
// ****************************************************************************************
void loop()
{
  CurrentCounter = micros();                       // 
  if ( CurrentCounter - PreviousCounter >= BeatTime )// 
  {
    PreviousCounter = CurrentCounter;              // 
    Timer.Running();                               // 
    // Procesos
      if ( BlinkLEDFlag )
        BlinkLED();
      if ( SWSGetHWSSendFlag )
        SWSGetHWSSend();
      if ( MagneticOpenFlag )
        MagneticOpen();
      if ( MagneticCloseFlag )
        MagneticClose();
      if ( EventHandleFlag )
        EventHandle();
  }
}

// ****************************************************************************************
//                        ERROR ENCONTRADO DETENER EJECUCION
// ****************************************************************************************
void ErrorFind()
{
  Serial.flush();                                  // 
  abort();                                         // 
}

// ****************************************************************************************
//                        PROCESO DE PARPADEO DE LED DE RUN
// ****************************************************************************************
void BlinkLED()
{
  // Complementa el estado del LED
  if ( BlinkLEDTV.Ending )                         // 
  {
    if ( digitalRead( BlinkLED_Pin ) )
      digitalWrite( BlinkLED_Pin, LOW );
    else
      digitalWrite( BlinkLED_Pin, HIGH );
    Timer.Start( &BlinkLEDTV, BlinkLEDTime );      // 
    BlinkLEDTV.Ending = false;                     // 
  }
}

// ****************************************************************************************
//                        PROCESO DE SOFTWARE SERIAL -> HARDWARE SERIAL
// ****************************************************************************************
void SWSGetHWSSend()
{
  static uint16_t i = 0;
  
  if ( SWSGetHWSSendTV.Ending )                    // 
  {
    while ( fona.available() )                     // 
    {
      packetBuffer[ i ] = fona.read();             // 
      i++;                                         // 
    }
    if ( i )                                       // 
    {
      packetBuffer[ i ] = '\0';                    // 
      Serial.print( packetBuffer );                // 
      i = 0;                                       // 
    }    
  }
}


// ****************************************************************************************
//                        PROCESO DE DETECCION DE SENSOR MAGNETICO
// ****************************************************************************************
void MagneticOpen()
{
  if ( MagneticOpenTV.Ending )                     // 
  {
    if ( digitalRead( Magnetic_Pin ) )             // 
    {
      Serial.println("Se detecto apertura sensor");
      if ( !fona.sendSMS( CelNumber, MessageOpen ) )
        Serial.println("Mensaje enviado con exito");
      else
        Serial.println("Error en el envio");
      MagneticOpenTV.Ending = false;               // 
      Timer.Start( &MagneticCloseTV, 30000 );      // 
    }
  }
}

// ****************************************************************************************
//                        PROCESO DE DETECCION DE SENSOR MAGNETICO
// ****************************************************************************************
void MagneticClose()
{
  if ( MagneticCloseTV.Ending )                    // 
  {
    if ( !digitalRead( Magnetic_Pin ) )            // 
    {
      Serial.println("Se detecto cierre sensor");
      if ( !fona.sendSMS( CelNumber, MessageClose ) )
        Serial.println("Mensaje enviado con exito");
      else
        Serial.println("Error en el envio");
      MagneticCloseTV.Ending = false;              // 
      Timer.Start( &MagneticOpenTV, 30000 );       // 
    }
  }
}

// ****************************************************************************************
//                        PROCESO DE MANEJO DE FALLAS
// ****************************************************************************************
void EventHandle()
{
  if ( EventHandleTV.Ending )                      // 
  {
    if ( Timer.EventHandleError  )
    {
      Serial.println("\r\n\r\n - ERROR: Desborde de Timers Virtuales!\r\n");
      ErrorFind();
    }
    if ( SIM800Error )
    {
      Serial.println("\r\n\r\n - ERROR: Modulo SIM800L no encontrado!\r\n");
      ErrorFind();
    }
  }
}

// ****************************************************************************************
//                        FIN DE PROGRAMA
// ****************************************************************************************
