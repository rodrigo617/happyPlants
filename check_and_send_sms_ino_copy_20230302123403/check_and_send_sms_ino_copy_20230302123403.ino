#include <SoftwareSerial.h>
SoftwareSerial sim(10, 11);
int _timeout;
String _buffer;
//String number = "+5492317402078"; //-> change with your number
//String number = "+34634951792"; //-> change with your number
String number = "+13208522611"; //-> change with your number
//String number = "444"; //-> change with your number
void setup() {
  //delay(7000); //delay for 7 seconds to make sure the modules get the signal
  Serial.begin(9600);
  _buffer.reserve(50);
  Serial.println("System Started...");
  sim.begin(9600);
  delay(1000);


Serial.println("Initializing...");
delay(1000);

sim.println("AT"); //Once the handshake test is successful, it will back to OK
updateSerial();
delay(10000);

sim.println("AT+CFUN=4"); //Once the handshake test is successful, it will back to OK
updateSerial();
delay(10000);

sim.println("AT+CFUN=1"); //Once the handshake test is successful, it will back to OK
updateSerial();
delay(10000);

sim.println("AT+COPS=?"); //Check whether it has registered in the network
updateSerial();
delay(10000);

sim.println("AT+COPS=1,0,\"UNIFON\""); //Check whether it has registered in the network
updateSerial();
delay(20000);

sim.println("AT+COPS?"); //Check whether it has registered in the network
updateSerial();
delay(10000);

sim.println("AT+CSQ"); //Signal quality test, value range is 0-31 , 31 is the best
updateSerial();
delay(10000);

sim.println("AT+CFUN=4"); //Once the handshake test is successful, it will back to OK
updateSerial();
delay(10000);

sim.println("AT+CFUN=1"); //Once the handshake test is successful, it will back to OK
updateSerial();
delay(10000);

sim.println("AT+COPS=1,0,\"PERSONAL\""); //Check whether it has registered in the network
updateSerial();
delay(20000);

sim.println("AT+COPS?"); //Check whether it has registered in the network
updateSerial();
delay(10000);

sim.println("AT+CSQ"); //Signal quality test, value range is 0-31 , 31 is the best
updateSerial();
delay(10000);


  Serial.println("Type s to send an SMS, r to receive an SMS, and c to make a call");
}
void loop() {

  // updateSerial();

  if (Serial.available() > 0)
    switch (Serial.read())
    {
      case 's':
        SendMessage();
        break;
      case 'r':
        RecieveMessage();
        break;
      case 'c':
        callNumber();
        break;
    }
  if (sim.available() > 0)
    Serial.write(sim.read());
}
void SendMessage()
{
  Serial.println ("Sending Message");
  sim.println("AT+CMGF=1");    //Sets the GSM Module in Text Mode
  delay(200);
  Serial.println ("Set SMS Number");
  sim.println("AT+CMGS=\"" + number + "\"\r"); //Mobile phone number to send message
  delay(200);
  //String SMS = "Hello, how are you? greetings from miliohm.com admin";
  //String SMS = "Contestame gato!";
  String SMS = "BatteryLow";
  //String SMS = "saldo";
  sim.println(SMS);
  delay(100);
  sim.println((char)26);// ASCII code of CTRL+Z
  delay(200);
  _buffer = _readSerial();
  Serial.println ("Message sent");
}
void RecieveMessage()
{
  Serial.println ("SIM800L Read an SMS");
  sim.println("AT+CMGF=1");
  delay (200);
  sim.println("AT+CNMI=1,2,0,0,0"); // AT Command to receive a live SMS
  delay(200);
  Serial.write ("Unread Message done");
}
String _readSerial() {
  _timeout = 0;
  while  (!sim.available() && _timeout < 12000  )
  {
    delay(13);
    _timeout++;
  }
  if (sim.available()) {
    return sim.readString();
  }
}
void callNumber() {
  sim.print (F("ATD"));
  sim.print (number);
  sim.print (F(";\r\n"));
  _buffer = _readSerial();
  Serial.println(_buffer);
}

void updateSerial()
{
delay(500);
while (Serial.available())
{
sim.write(Serial.read());//Forward what Serial received to Software Serial Port
}
while(sim.available())
{
Serial.write(sim.read());//Forward what Software Serial received to Serial Port
}
}
