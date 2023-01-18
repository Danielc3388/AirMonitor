#include <SPI.h>
#include <Ethernet.h>


byte mac[] = {
  0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED
};
IPAddress ip(192, 168, 1, 177);


EthernetServer server(80);

void setup() {

  Serial.begin(9600);
  while (!Serial) {
  }
  Serial.println("Ethernet WebServer Example");

  Ethernet.begin(mac, ip);

  if (Ethernet.hardwareStatus() == EthernetNoHardware) {
    Serial.println("Ethernet shield was not found.  Sorry, can't run without hardware. :(");
    while (true) {
      delay(1); 
    }
  }
  if (Ethernet.linkStatus() == LinkOFF) {
    Serial.println("Ethernet cable is not connected.");
  }

  server.begin();
  Serial.print("server is at ");
  Serial.println(Ethernet.localIP());
}


void loop() {
  EthernetClient client = server.available();
  if (client) {
    Serial.println("new client");
    boolean currentLineIsBlank = true;
    while (client.connected()) {
      if (client.available()) {
        char c = client.read();
        Serial.write(c);
        
        if (c == '\n' && currentLineIsBlank) {
          client.println("HTTP/1.1 200 OK");
          client.println("Content-Type: text/html");
          client.println("Connection: close");  
          client.println("Refresh: 1");
          client.println();
          client.println("<!DOCTYPE HTML>");
          client.println("<html>");
          client.println("<main id=\"main\" class=\"main-content\">"); //add some titles and contect classifications
          /*build the table****************************/
          client.println("<TABLE BORDER=\"5\" WIDTH=\"50%\" CELLPADDING=\"4\" CELLSPACING=\"3\">");
          client.println("<TR>");
          client.println("<TH COLSPAN=\"2\"><BR><H3>SENSOR DATA</H3>");
          client.println("</TH>");
          client.println("</TR>");
          client.println("<TR>");
          client.println("<TH>CHANNEL</TH>");
          client.println("<TH>VALUE</TH>");
          client.println("</TR>");
          /*******************************************/
          // output the value of each analog input pin to the table
          for (int analogChannel = 0; analogChannel < 6; analogChannel++) {
            int sensorReading = analogRead(analogChannel); //read analogs
            /*Add the channel to the table with the class identifier*/
            client.println("<TR ALIGN=\"CENTER\">");
            client.print("<TD class=\"chan\">");
            client.print(analogChannel);
            client.println("</TD>");
            /*******************************************************/
            /*Add the coorosponding value to the table*************/
            client.print("<TD class=\"value\">");
            client.print(sensorReading);
            client.println("</TD>");
            client.println("</TR>");
            /******************************************************/
          }
          /*End the HTML****************/
          client.println("</TABLE>");
          client.println("</main>");
          client.println("</html>");
          /***************************/
          break;
        }
        if (c == '\n') {
          // you're starting a new line
          currentLineIsBlank = true;
        } else if (c != '\r') {
          // you've gotten a character on the current line
          currentLineIsBlank = false;
        }
      }
    }
    // give the web browser time to receive the data
    delay(1);
    // close the connection:
    client.stop();
    Serial.println("client disconnected");
  }
}
