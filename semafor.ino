
// Load Wi-Fi library
#include <ESP8266WiFi.h>

#include <Servo.h>

const char* ssid     = "GL-Students";
const char* password = "123wifinet321";

String header;

WiFiServer server(80);


int stav = 0;

void setup()
{
  Serial.begin(115200);
  Serial.print("Connecting to ");
  Serial.println(ssid);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  // Print local IP address and start web server
  Serial.println("");
  Serial.println("WiFi connected.");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
  server.begin();
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
        Serial.write(c);                    // print it out the serial monitor
        header += c;
        if (c == '\n') {                    // if the byte is a newline character
          // if the current line is blank, you got two newline characters in a row.
          // that's the end of the client HTTP request, so send a response:
          if (currentLine.length() == 0) {
            // HTTP headers always start with a response code (e.g. HTTP/1.1 200 OK)
            // and a content-type so the client knows what's coming, then a blank line:
            client.println("HTTP/1.1 200 OK");
            client.println("Content-type:text/html");
            client.println("Connection: close");
            client.println();

            // turns the GPIOs on and off
            if (header.indexOf("GET /zastav") >= 0) {
              Serial.println("Musim zastavit");
              stav = 1;
            } else if (header.indexOf("GET /spust") >= 0) {
              Serial.println("Musim spustit");
              stav = 3;
            }

            // Display the HTML web page
            client.println("<!DOCTYPE html><html>");
            client.println("<head><meta name=\"viewport\" content=\"width=device-width, initial-scale=1\">");
            client.println("<link rel=\"icon\" href=\"data:,\">");
            // CSS to style the on/off buttons
            // Feel free to change the background-color and font-size attributes to fit your preferences
            client.println("<style>html { font-family: Helvetica; display: inline-block; margin: 0px auto; text-align: center;}");
            client.println(".button { background-color: #CC0000; border: none; color: white; padding: 16px 40px;");
            client.println("text-decoration: none; font-size: 30px; margin: 2px; cursor: pointer;}");
            client.println(".button2 {background-color: #006600;}</style></head>");

            // Web Page Heading
            client.println("<body><h1>Smart Semafor</h1>");

            // Display current state, and ON/OFF buttons for GPIO 5
            client.println("<p> Toto tlacidlo okamzite zastavi premavku </p>");
            // If the output5State is off, it displays the ON button
              client.println("<p><a href=\"/zastav\"><button class=\"button button2\">Zastavit</button></a></p>");

            // Display current state, and ON/OFF buttons for GPIO 4
            client.println("<p> Toto tlacidlo okamzite spusti premavku </p>");
            // If the output4State is off, it displays the ON button
             client.println("<p><a href=\"/spust\"><button class=\"button\">Spustit</button></a></p>");
            client.println("</body></html>");

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
  }
      // Clear the header variable
      header = "";
      // Close the connection
      client.stop();
      Serial.println("Client disconnected.");
      Serial.println("");
}
