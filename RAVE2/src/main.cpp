/*********
  Rui Santos
  Complete project details at https://randomnerdtutorials.com  
*********/

// Load Wi-Fi library
#include <WiFi.h>
#include <ESP32Servo.h>

// Replace with your network credentials
const char* ssid     = "ESP32-Access-Point";
const char* password = "123456789";

// Set web server port number to 80
WiFiServer server(80);

Servo ObjServo; // Make object of Servo motor from Servo library
// Objects are made for every servo motor,we want to control through this library
static const int ServoGPIO = 13; // define the GPIO pin with which servo is connected

// Variable to store the HTTP request
String header;

// These variables used to store slider position 
String valueString = String(0);
int positon1 = 0;
int positon2 = 0;
int positon3 = 0;
String rpmString = "";
String thresholdString  = "";

// Auxiliar variables to store the current output state
String output26State = "off";
String output27State = "off";

// Assign output variables to GPIO pins
const int output26 = 26;
const int output27 = 27;

void setup() {
  Serial.begin(115200);
  // Initialize the output variables as outputs
  pinMode(output26, OUTPUT);
  pinMode(output27, OUTPUT);
  // Set outputs to LOW
  digitalWrite(output26, LOW);
  digitalWrite(output27, LOW);

  ObjServo.attach(ServoGPIO); // it will attach the ObjServo to ServoGPIO pin which is 13 

  // Connect to Wi-Fi network with SSID and password
  Serial.print("Setting AP (Access Point)…");
  // Remove the password parameter, if you want the AP (Access Point) to be open
  WiFi.softAP(ssid, password);

  IPAddress IP = WiFi.softAPIP();
  Serial.print("AP IP address: ");
  Serial.println(IP);
  
  server.begin();
}

void loop(){
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
			client.println("<h1>ESP32 with Servo</h1>");
			client.println("<form action=\"\" method=\"get\">");
			client.println("<label for=\"fname\">RPM:</label>");
			client.println("<input type=\"number\" id=\"fname\" name=\"rpm\" min=7500 max=8500 value=7800 step=100><br><br>");
			client.println("<label for=\"lname\">Threshold:</label>");
			client.println("<input type=\"number\" id=\"lname\" name=\"threshold\" value=100 step=50><br><br>");
			client.println("<button type=\"submit\">Submit</button>");
			client.println("</form>");
			client.println("</body>");
			client.println("</html>");

			//GET /?value=180& HTTP/1.1
			if(header.indexOf("GET /?rpm=") >= 0) 
			{
				positon1 = header.indexOf('=');
				positon2 = header.indexOf('&');
				positon3 = header.indexOf('=', positon1+1);
				rpmString = header.substring(positon1+1, positon2);
				thresholdString = header.substring(positon3+1);

				//Rotate the servo
				ObjServo.write(rpmString.toInt());
				Serial.println(rpmString.toInt()); 
				Serial.println(thresholdString.toInt()); 
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
}
