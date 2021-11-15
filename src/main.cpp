/*
UNIVERSAL SERVO EXHAUST VALVE CONTROL
*/

/*********
  J. Link
  supported by K. Braun
  
  Code parts from:
  Rui Santos
  Complete project details at https://randomnerdtutorials.com
  especially ESP32 Servo Motor Web Server with Arduino IDE

  RPM & RAVE CODE from 
  InterlinkKnight & el bodo es loco


*********/

// Load Wi-Fi library
#include <WiFi.h>
// Load Servo libary
#include <ESP32Servo.h>
#include <Wire.h>
// Load OLED 0.96" Display SSD1306 libary
#include <SSD1306Wire.h>
// Load preferences libary
#include <Preferences.h>
// // Counter
// #include "driver/pcnt.h"

// WIFI SERVER DEF
// Set web server port number to 80
WiFiServer server(80);
// Variable to store the HTTP request
String header;
// Replace with your network credentials
const char* ssid     = "USEVC_AP";
const char* password = "USEVC_AP";
// default setting for RAVE2
static const int rpm_default = 8100; // rpm to Open
static const int threshold_default = 100; // rpm_default - threshold = closing rpm
static const int PulsesPerRevolution_default = 1; //default ppr #### 1 für Drehzahlmessung am Pickup oder Zündspule; 6 für Drehzahlmessung an der Lima (ROTAX)

// SERVO DEF
Servo ObjServo; // Make object of Servo motor from Servo library
// Objects are made for every servo motor,we want to control through this library
static const int ServoGPIO = 13; // define the GPIO pin with which servo is connected
// Servo positions
static const int positionclose_default = 145; // Servo Position Closed RAVE
static const int positionopen_default = 90; // Servo Position Open RAVE

// OLED DEF
// OLED 0.96" Display SSD1306
SSD1306Wire display(0x3c, SDA, SCL);

// PREF DEF
Preferences preferences;

// miscellaneous DEF
// Variables for String to Int conversion
int position1 = 0;
int position2 = 0;
int position3 = 0;
int position4 = 0;
int position5 = 0;
int position6 = 0;
int position7 = 0;
int position8 = 0;
int position9 = 0;
int position10 = 0;

String rpmopenString = "";
String thresholdString  = "";
int rpmclose;
String positioncloseString = "";
String positionopenString = "";
String PulsesPerRevolutionString = "";

// Mod LED DEF
const int LED_PIN = 19;

// Mod Interrupt Pin
const int interrupt_PIN = 2;

/////////////////////////////////////////
// FROM InterlinkKnight & el bodo es loco

//Timeout & Genauigkeit//
const unsigned long ZeroTimeout = 100000;  
const byte numReadings = 5;  //Anzahl der Messwertblöcke zur Mittelwert bildung Je höher des to genauer aber auch trägerzwischen 3-10 ist in Ordnung
//muss nicht verändert werden 
 
/////////////
// Variablen:
/////////////

volatile unsigned long LastTimeWeMeasured;  
volatile unsigned long PeriodBetweenPulses = ZeroTimeout+1000;  
volatile unsigned long PeriodAverage = ZeroTimeout+1000;  
unsigned long FrequencyRaw;  
unsigned long FrequencyReal; 
unsigned long RPMEngine; 
unsigned int PulseCounter = 1;  

unsigned long PeriodSum; 
unsigned long LastTimeCycleMeasure = LastTimeWeMeasured;  
unsigned long CurrentMicros = micros(); 
unsigned int AmountOfReadings = 1;
unsigned int ZeroDebouncingExtra;  
unsigned long readings[numReadings];  
unsigned long readIndex;  
unsigned long total;  
unsigned long average; 
/////////////////////////////////////////
const int sensorPin = 33;

void Pulse_Event()
{
  PeriodBetweenPulses = micros() - LastTimeWeMeasured;  
  LastTimeWeMeasured = micros();  

 if(PulseCounter >= AmountOfReadings)  
  {
    PeriodAverage = PeriodSum / AmountOfReadings; 
    PulseCounter = 1;  
    PeriodSum = PeriodBetweenPulses; 
    int RemapedAmountOfReadings = map(PeriodBetweenPulses, 40000, 5000, 1, 10);  
    
    RemapedAmountOfReadings = constrain(RemapedAmountOfReadings, 1, 10);
    AmountOfReadings = RemapedAmountOfReadings;  
  }
  else
  {
    PulseCounter++; 
    PeriodSum = PeriodSum + PeriodBetweenPulses; 
  }
} 

void setup()
{

  Serial.begin(115200);

  // Pref namespace RAVE2
  preferences.begin("RAVE2", false);
  // preferences.clear(); // if you want to clear all default values uncomment this line flash , comment and flash again!
  // Load Vars
  rpmopenString = preferences.getUInt("rpm", rpm_default);
  thresholdString = preferences.getUInt("threshold", threshold_default);
  positioncloseString = preferences.getUInt("positionclose", positionclose_default);
  positionopenString = preferences.getUInt("positionopen", positionopen_default);
  PulsesPerRevolutionString = preferences.getUInt("PulsesPerRevolution", PulsesPerRevolution_default);
  preferences.end();

  Serial.println(rpmopenString);
  Serial.println(thresholdString);
  Serial.println(positioncloseString);
  Serial.println(positionopenString);
  Serial.println(PulsesPerRevolutionString);
  
  // Initialize the output variables as outputs
  pinMode(LED_PIN, OUTPUT);
  // Set outputs to LOW
  digitalWrite(LED_PIN, LOW);

  ObjServo.attach(ServoGPIO); // it will attach the ObjServo to ServoGPIO pin which is 13 

  // Connect to Wi-Fi network with SSID and password
  Serial.print("Setting AP (Access Point)…");
  // Remove the password parameter, if you want the AP (Access Point) to be open
  WiFi.softAP(ssid, password);
  IPAddress IP = WiFi.softAPIP();
  Serial.print("AP IP address: ");
  Serial.println(IP);
  server.begin();

  // Initialize the display
  display.init();
  display.setContrast(255);
  display.setTextAlignment(TEXT_ALIGN_CENTER_BOTH);
  display.setFont(ArialMT_Plain_10);
  //display.drawString(display.getWidth() / 2, display.getHeight() / 2, "RAVE 2 CONTROL\nIP: 192.168.4.1\nOpen RPM: " + rpmopenString + "\nThreshold: " + thresholdString );
  //display.display();
  display.drawString(display.getWidth() / 2, display.getHeight() / 2, "RAVE 2 CONTROL\n\nIP: 192.168.4.1\nOpen RPM: " + rpmopenString + "\nThreshold: " + thresholdString + "\nPosition_Close: "+ positioncloseString + "\nPosition_Open: "+ positionopenString );
  display.display();
  
  // Servo test
  ObjServo.write(positioncloseString.toInt());
  delay(500);
  ObjServo.write(positionopenString.toInt());
  delay(500);
  ObjServo.write(positioncloseString.toInt());
  delay(500);

  /////////////////////////////////////////
  // FROM InterlinkKnight & el bodo es loco
  pinMode (interrupt_PIN, INPUT_PULLUP);
  attachInterrupt(interrupt_PIN, Pulse_Event, RISING);  // changed GPIO_3 to GPIO_2
  delay(100);
  /////////////////////////////////////////

  /////////////////////////////////////////
  Serial.println("Setup Finish!");

}
void loop()
{

  WiFiClient client = server.available();   // Listen for incoming clients

  if (client) {                             // If a new client connects,
    Serial.println("New Client.");          // print a message out in the serial port
    String currentLine = "";                // make a String to hold incoming data from the client
    while (client.connected()) {            // loop while the client's connected
      if (client.available()) {             // if there's bytes to read from the client,
        char c = client.read();             // read a byte, then
        // Serial.write(c);                    // print it out the serial monitor
        header += c;
        if (c == '\n') {                    // if the byte is a newline character
          // if the current line is blank, you got two newline characters in a row.
          // that's the end of the client HTTP request, so send a response:
          if (currentLine.length() == 0) 
		  {

      preferences.begin("RAVE2", false);
      rpmopenString = preferences.getUInt("rpm", rpm_default);
      thresholdString = preferences.getUInt("threshold", threshold_default);
      positioncloseString= preferences.getUInt("positionclose", positionclose_default);
      positionopenString= preferences.getUInt("positionopen", positionopen_default);


			client.println("HTTP/1.1 200 OK");
			client.println("Content-type:text/html");
			client.println("Connection: close");
			client.println();

			// Display the HTML web page
			client.println("<!DOCTYPE html><html>");
			client.println("<head><meta name=\"viewport\" content=\"width=device-width, initial-scale=1\">");
			client.println("<link rel=\"icon\" href=\"data:,\">");
			client.println("<style>body { text-align: center; }</style>");
			client.println("</head>");
			client.println("<body>");
			client.println("<h1>RAVE2 Control</h1>");
			client.println("<form action=\"\" method=\"get\">");
			client.println("<label for=\"rname\">RPM:</label>");
			client.print("<input type=\"number\" id=\"rname\" name=\"rpm\" value="+ rpmopenString +"min=7000 max=9000 step=1><br><br>");
			client.println("<label for=\"tname\">Threshold:</label>");
			client.print("<input type=\"number\" id=\"tname\" name=\"threshold\" value="+ thresholdString +"min=100 max=500 step=1><br><br>");
      client.println("<label for=\"cname\">Servo Close Position:</label>");
			client.print("<input type=\"number\" id=\"cname\" name=\"positionclose\" value="+ positioncloseString +"min=0 max=180 step=1><br><br>");
      client.println("<label for=\"oname\">Servo Open Position:</label>");
			client.print("<input type=\"number\" id=\"oname\" name=\"positionopen\" value="+ positionopenString +"min=0 max=180 step=1><br><br>");
      client.println("<label for=\"pname\">Pulse per Rev:</label>");
			client.print("<input type=\"number\" id=\"pname\" name=\"\" value="+ PulsesPerRevolutionString +"min=1 max=999 step=1><br><br>");
			client.println("<button type=\"submit\">Submit</button>");
			client.println("</form>");
			client.println("</body>");
			client.println("</html>");

			//GET /?value=180& HTTP/1.1
			if(header.indexOf("GET /?rpm=") >= 0) 
			{
        position1 = header.indexOf('=');
				position2 = header.indexOf('&');
				position3 = header.indexOf('=', position1+1);
        position4 = header.indexOf('&', position3+1);
				position5 = header.indexOf('=', position3+1);
        position6 = header.indexOf('&', position5+1);
        position7 = header.indexOf('=', position5+1);
        position8 = header.indexOf('&', position7+1);
        position9 = header.indexOf('=', position7+1);
        position10 = header.indexOf(' ', position9+1);

				rpmopenString = header.substring(position1+1, position2);
				thresholdString = header.substring(position3+1, position4);
				positioncloseString = header.substring(position5+1, position6);
				positionopenString = header.substring(position7+1, position8);
        PulsesPerRevolutionString = header.substring(position9+1, position10);

				// Rotate the servo
        // ObjServo.write(positioncloseString.toInt()); // test application

        Serial.println(rpmopenString);
				Serial.println(thresholdString);
        Serial.println(positioncloseString); 
				Serial.println(positionopenString);
        Serial.println(PulsesPerRevolutionString);

        if (rpmopenString.toInt() >= 6999 && rpmopenString.toInt() <= 9001)
        {
          preferences.putUInt("rpm", rpmopenString.toInt());
        }
        else 
        {
          rpmopenString = preferences.getUInt("rpm");
        }

        if (thresholdString != "")
        {
          preferences.putUInt("threshold", thresholdString.toInt());
        }
        else 
        {
          thresholdString = preferences.getUInt("threshold");
        }

        if (positioncloseString != "")
        {
          preferences.putUInt("positionclose", positioncloseString.toInt());
        }
        else
        { 
          positioncloseString= preferences.getUInt("positionclose"); 
        }

        if (positionopenString != "")
        {
          preferences.putUInt("positionopen", positionopenString.toInt()); 
        }
        else 
        {
          positionopenString = preferences.getUInt("positionopen");
        }

        if (PulsesPerRevolutionString != "")
        {
          preferences.putUInt("PulsesPerRevolution", PulsesPerRevolutionString.toInt()); 
        }
        else 
        {
          PulsesPerRevolutionString = preferences.getUInt("PulsesPerRevolution");
        }


        preferences.end();

        Serial.println("RPM: " + rpmopenString);
				Serial.println("Threshold: " + thresholdString);
        Serial.println("Close: " + positioncloseString); 
				Serial.println("Open: " + positionopenString);
        Serial.println("PPR: " + PulsesPerRevolutionString);
                
        display.init();
        display.setContrast(255);
        display.setTextAlignment(TEXT_ALIGN_CENTER_BOTH);
        display.drawString(display.getWidth() / 2, display.getHeight() / 2, "RAVE 2 CONTROL\n\nIP: 192.168.4.1\nOpen RPM: " + rpmopenString + "\nThreshold: " + thresholdString + "\nPosition_Close: "+ positioncloseString + "\nPosition_Open: "+ positionopenString);
        display.display();

        rpmclose = rpmopenString.toInt() - thresholdString.toInt();

        // digitalWrite(LED_PIN, HIGH); // LED ON => modified settings
			}
            
      // The HTTP response ends with another blank line
      client.println();
      // Break out of the while loop
      break;
      } else { // if you got a newline, then clear currentLine
        currentLine = "";
        }
      } else if (c != '\r') {  // if you got anything else but a carriage return character,
        currentLine += c;      // add it to the end of the currentLine
      }
      }
    }
    // Clear the header variable
    header = "";
    // Close the connection
    client.stop();
    Serial.println("Client disconnected.");
    Serial.println("");
  }

  LastTimeCycleMeasure = LastTimeWeMeasured;
    CurrentMicros = micros(); 
    if(CurrentMicros < LastTimeCycleMeasure)
    {
        LastTimeCycleMeasure = CurrentMicros;
    }

    FrequencyRaw = 10000000000 / PeriodAverage;
    if(PeriodBetweenPulses > ZeroTimeout - ZeroDebouncingExtra || CurrentMicros - LastTimeCycleMeasure > ZeroTimeout - ZeroDebouncingExtra)
    {  
        FrequencyRaw = 0;   
        ZeroDebouncingExtra = 2000;
    }
    else
    {
        ZeroDebouncingExtra = 0;  
    }
    FrequencyReal = FrequencyRaw / 10000;  
    RPMEngine = FrequencyRaw / PulsesPerRevolutionString.toInt() * 60;  
    RPMEngine = RPMEngine / 10000; 
    total = total - readings[readIndex];  
    readings[readIndex] = RPMEngine;  
    total = total + readings[readIndex];  
    readIndex = readIndex + 1;  

    if (readIndex >= numReadings) 
    {
        readIndex = 0;  
    }
    average = total / numReadings; 
    // Setpoint = analogRead (PIN_ADJUST);  // Ausgeklammert für Feste Öffnungsdrehzahl
    // set = map(Setpoint, 0, 1024, 6000, 10500 );  // Ausgeklammert für Feste Öffnungsdrehzahl
    rpmclose = rpmopenString.toInt() - thresholdString.toInt() ;

    if (RPMEngine <= rpmclose)
    { // unter diesem Wert bleibt der Auslassschieber geschlossen
      digitalWrite(LED_PIN,LOW); 
      ObjServo.write(positioncloseString.toInt());
        
    }
    if (RPMEngine >= rpmopenString.toInt())
    { // über diesem Wert öffnet der Auslassschieber
      digitalWrite(LED_PIN,HIGH);  
      ObjServo.write(positionopenString.toInt());
    }
    Serial.print("RPM: ");
    Serial.print(RPMEngine);
    Serial.print(" TEST RPM: ");
    Serial.println(int(t*60));

}
////ARDUINO CODE FOR TEST
//// Connect Arduino PIN 13 with ESP PIN 2 & GND to GND
// int sensorPin = A0;
// int ledPin = 13;
// int rpmPin = 2;

// void setup () {
//   pinMode(ledPin, OUTPUT);
//   pinMode(rpmPin, OUTPUT);
//   Serial.begin (9600);
// }

// void loop () {
  
// float  t = map (analogRead (sensorPin), 0, 1023, 32, 220);          

//   tone (13, t);
  
//   Serial.print ("RPM:");
//   Serial.println(t*60); 
// }