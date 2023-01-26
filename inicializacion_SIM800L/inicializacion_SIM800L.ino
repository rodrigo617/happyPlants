#include <SoftwareSerial.h>

//Create software serial object to communicate with SIM800L
SoftwareSerial mySerial(10, 11); //SIM800L Tx & Rx is connected to Arduino #7 & #8

void setup()
{
//Begin serial communication with Arduino and Arduino IDE (Serial Monitor)
Serial.begin(9600);

//Begin serial communication with Arduino and SIM800L
mySerial.begin(9600);

Serial.println("Initializing...");
delay(1000);

mySerial.println("AT"); //Once the handshake test is successful, it will back to OK
updateSerial();
delay(10000);

mySerial.println("AT+CFUN=4"); //Once the handshake test is successful, it will back to OK
updateSerial();
delay(10000);

mySerial.println("AT+CFUN=1"); //Once the handshake test is successful, it will back to OK
updateSerial();
delay(10000);

mySerial.println("AT+COPS=?"); //Check whether it has registered in the network
updateSerial();
delay(10000);

mySerial.println("AT+COPS=1,0,\"UNIFON\""); //Check whether it has registered in the network
updateSerial();
delay(20000);

mySerial.println("AT+COPS?"); //Check whether it has registered in the network
updateSerial();
delay(10000);

mySerial.println("AT+CSQ"); //Signal quality test, value range is 0-31 , 31 is the best
updateSerial();
delay(10000);



mySerial.println("AT+CFUN=4"); //Once the handshake test is successful, it will back to OK
updateSerial();
delay(10000);

mySerial.println("AT+CFUN=1"); //Once the handshake test is successful, it will back to OK
updateSerial();
delay(10000);

mySerial.println("AT+COPS=1,0,\"PERSONAL\""); //Check whether it has registered in the network
updateSerial();
delay(20000);

mySerial.println("AT+COPS?"); //Check whether it has registered in the network
updateSerial();
delay(10000);

mySerial.println("AT+CSQ"); //Signal quality test, value range is 0-31 , 31 is the best
updateSerial();
delay(10000);

}

void loop()
{
updateSerial();
}

void updateSerial()
{
delay(500);
while (Serial.available())
{
mySerial.write(Serial.read());//Forward what Serial received to Software Serial Port
}
while(mySerial.available())
{
Serial.write(mySerial.read());//Forward what Software Serial received to Serial Port
}
}
