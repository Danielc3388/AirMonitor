/*

Function :  To read data from arduino board and send the data
            collected to html websites and bluetooth

Author : Daniel Chan
Created on : Jan 2023
Last modified : 23 Feb 2023
Use with project : Gas sensor

*/


//library used
#include <SPI.h>
#include <Ethernet.h>
#include <SoftwareSerial.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>

int a, b, c, d; //variables for gas sensor data to store


byte mac[] = {
  0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED //mac address for website
};
IPAddress ip(192, 168, 1, 177); //IP address for the ethernet
                                //Unique in local network


EthernetServer server(80);
SoftwareSerial BTSerial (4, 3);   //bluetooth pin
LiquidCrystal_I2C lcd(0x27, 16, 2);   //LCD pin



void createWeb() {
  EthernetClient client = server.available();
  if (client) { //when accessed
    Serial.println("new client");
    boolean currentLineIsBlank = true;
    while (client.connected()) {
      if (client.available()) {
        char c = client.read();
        Serial.write(c);

//writing html
        if (c == '\n' && currentLineIsBlank) {
          client.println("HTTP/1.1 200 OK");
          client.println("Content-Type: text/html");
          client.println("Connection: close");
          client.println("Refresh: 0.1");
          client.println();
          client.println("<!DOCTYPE HTML>");
          client.println("<html>");
          client.println("<title>Data from Arduino</title>");
          client.println("<h1 style='font-size:55px;'>Data from Arduino Gas Sensor Project</h1>");
          client.println("<main id=\"main\" class=\"main-content\">");

          client.println("<TABLE BORDER=\"5\" WIDTH=\"50%\" CELLPADDING=\"4\" CELLSPACING=\"3\">");
          client.println("<TR>");
          client.println("<TH COLSPAN=\"3\"><BR><H3>SENSOR DATA</H3>");
          client.println("</TH>");
          client.println("</TR>");
          client.println("<TR>");
          client.println("<TH>Sensor Slot</TH>");
          client.println("<TH>VALUE</TH>");
          client.println("<TH>STATE</TH>");
          client.println("</TR>");
          //
          client.println("<TR ALIGN=\"CENTER\">");
          client.print("<TD class=\"chan\">");
          client.print(0);
          client.println("</TD>");

//create table data
          client.print("<TD class=\"value\">");
          client.print(a);
          if (a > 0) {
            client.print("<TD style='background-color:#ff0000;color:#ff0000;'>");
          } else {
            client.print("<TD style='background-color:#00FF00;color:#00FF00;'>");

          }

          client.println("</TD>");
          client.println("</TR>");
          //
          client.println("</TABLE>");

//end of table

          client.println("<p>System Created by Daniel Chan</p>");
          client.println("</main>");
          client.println("</html>");
          break;
        }
        if (c == '\n') {
          currentLineIsBlank = true;
        } else if (c != '\r') {
          currentLineIsBlank = false;
        }
      }
    }
    delay(1);
    client.stop();
    Serial.println("client disconnected");
  }
}

void EthDisabled() {
  EthernetClient client = server.available();
  if (client) {   //if accessed
    Serial.println("new client");
    boolean currentLineIsBlank = true;
    while (client.connected()) {
      if (client.available()) {
        char c = client.read();
        Serial.write(c);

//start creating html
        if (c == '\n' && currentLineIsBlank) {
          client.println("HTTP/1.1 200 OK");
          client.println("Content-Type: text/html");
          client.println("Connection: close");
          client.println("Refresh: 0.1");
          client.println();
          client.println("<!DOCTYPE HTML>");
          client.println("<html>");
          client.println("<title>Data from Arduino</title>");
          client.println("<h1 style='font-size:55px;'>Data from Arduino Gas Sensor Project</h1>");
          client.println("<main id=\"main\" class=\"main-content\">");
          client.println("<h1>Ethernet Disabled!</h1>");
          client.println("<p>Press the Button on the device to enable</p>");
          client.println("<p>System Created by Daniel Chan</p>");
          client.println("</main>");
          client.println("</html>");
          break;
        }
//end of html

        if (c == '\n') {
          currentLineIsBlank = true;
        } else if (c != '\r') {
          currentLineIsBlank = false;
        }
      }
    }
    delay(1);
    client.stop();
    Serial.println("client disconnected");
  }
}

void sendBT() {

//sending data via BlueTooth
  BTSerial.print(a);
  BTSerial.print(" ");
  BTSerial.print(b);
  BTSerial.print(" ");
  BTSerial.print(c);
  BTSerial.print(" ");
  BTSerial.println(d);
  delay(1000);
}

bool EthernetEnable = LOW;
bool BTEnable = LOW;

void setup() {
  lcd.init(); //initialize LCD
  lcd.backlight();
  Serial.begin(9600);
  BTSerial.begin(9600); //initialize bluetooth
  while (!Serial) {
  }
  Serial.println("Ethernet");

  Ethernet.begin(mac, ip); //initialize ethernet

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
  pinMode(2, INPUT_PULLUP);
  pinMode(5, INPUT_PULLUP);
  

}


void loop() {
  EthernetEnable = !digitalRead(2); //read if sending data with ethernet
  BTEnable = !digitalRead(5); //read if sending data with bluetooth

//reading sensor data
  a = analogRead(0);
  b = analogRead(1);
  c = analogRead(2);
  d = analogRead(3);
  
  lcd.clear();  //clear data on LCD
  lcd.setCursor(0,0);   //set the cursor of LCD to first row, first digit

  if (EthernetEnable == HIGH) {
    lcd.print("IP:192.168.1.177");  //notify the user the ip address of data
    createWeb();
  } else {
    EthDisabled();
    lcd.print("Ethernet Disabled");   //notify the user ethernet is disabled
  }
  lcd.setCursor(0,1); //set the cursor of LCD to second row, first digit
  if (BTEnable == HIGH) {
    lcd.print("BT Enabled");  //notify the user bluetooth is connected
    sendBT();
    
  } else{
    lcd.print("BT Disabled"); //norifying the user bluetooth is disabled
  }
  delay(50);
}
