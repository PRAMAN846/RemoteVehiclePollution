#include <SoftwareSerial.h>
#include <String.h>

SoftwareSerial gprsSerial(9,10);


float PPM1=0; 
const int AOUTpin=A0;//the AOUT pin of the CO sensor goes into analog pin A0 of the arduino
const int DOUTpin=4;//the DOUT pin of the CO sensor goes into digital pin D8 of the arduino
const int ledPin=13;//the anode of the LED connects to digital pin D13 of the arduino

void setup()
{
    Serial.begin(9600);
    gprsSerial.begin(9600);

    // attach or detach from GPRS service 
    gprsSerial.println("AT+CGATT?");  // Attach to GPRS network or not?
    delay(100);
    toSerial();

    // bearer settings
    gprsSerial.println("AT+SAPBR=3,1,\"CONTYPE\",\"GPRS\"");   //Set the connection type to GPRS
    delay(2000);
    toSerial();

    // bearer settings
    gprsSerial.println("AT+SAPBR=3,1,\"APN\",\"airtelgprs.com\"");  //Set the APN to AIRTEL sim
    delay(2000);
    toSerial();

    // bearer settings
    gprsSerial.println("AT+SAPBR=1,1");     //Enable the GPRS
    delay(2000);
    toSerial();
    pinMode(DOUTpin, INPUT);//sets the pin as an input to the arduino
pinMode(ledPin, OUTPUT);//sets the pin as an output of the arduino
}
void loop()
{
    int sensorValue1 = analogRead(A0);
    float voltage1= sensorValue1 * (5.0 / 1023.0);
    float PPM1 = 3.027*exp(1.0698*voltage1);
    
    Serial.println(PPM1);
    if (PPM1>11)
    {
      SendMessage();
    }
    delay(2000);
    send2cloud();
    delay(1000);
}
void send2cloud()
{
  // initialize http service
   gprsSerial.println("AT+HTTPINIT");     // enabling the HTTP mode
   delay(2000); 
   toSerial();
   
   // set http param value
   gprsSerial.println("AT+HTTPPARA=\"URL\",\"https://api.thingspeak.com/update?api_key=HL2YPGUIKOKJCUG5&field1=0="+String(PPM1)+"\"");
   delay(2000);
   toSerial();

   // set http action type 0 = GET, 1 = POST, 2 = HEAD
   gprsSerial.println("AT+HTTPACTION=0");   //Start the HTTP GET session, by giving this command
   delay(6000);
   toSerial();

   // read server response
   gprsSerial.println("AT+HTTPREAD");      // read the received data
   delay(1000);
   toSerial();

   gprsSerial.println("");
   gprsSerial.println("AT+HTTPTERM");    //    terminate http service
   toSerial();
   delay(300);

   gprsSerial.println("");
   delay(1000);
  }
void SendMessage()
{
  gprsSerial.println("AT+CMGF=1");    //Sets the GSM Module in Text Mode
  delay(1000);  // Delay of 1000 milli seconds or 1 second
  gprsSerial.println("AT+CMGS=\"+918290076478\"\r"); // Replace x with mobile number
  delay(1000);
  gprsSerial.println("I am SMS from GSM Module");// The SMS text you want to send
  delay(100);
   gprsSerial.println((char)26);// ASCII code of CTRL+Z
  delay(1000);
}



void toSerial()
{
  while(gprsSerial.available()!=0)
  {
    Serial.write(gprsSerial.read());
  }
}

