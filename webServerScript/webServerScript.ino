// Load Wi-Fi library
#include <WiFi.h>
#include <esp_task_wdt.h>

//reboot every 5 min
#define WDT_TIMEOUT 300
//reboot after 30 sec after the usage
#define TIME_OUT 30

// Replace with your network credentials
const char* ssid = "****";
const char* password = "****";

const int delayTime = 1000;

// Set web server port number to 80
WiFiServer server(80);

// Variable to store the HTTP request
String header;

// Current time
unsigned long currentTime = millis();
// Previous time
unsigned long previousTime = 0; 
// Define timeout time in milliseconds (example: 2000ms = 2s)
const long timeoutTime = 2000;

const int pinPortone = 5;

const int pinCancello = 4;

// you need to replace the string with a base64 encrypted username and password with the form of username:password (the length of both username and password are unrelevant).
const char* base64Encoding = "***:***";  // base64encoding casa:nostra

void setup() {
  Serial.begin(115200);
  // Initialize the output variables as outputs
  pinMode(pinPortone, OUTPUT);
  pinMode(pinCancello, OUTPUT);
  // Set outputs to LOW
  digitalWrite(pinPortone, HIGH);
  digitalWrite(pinCancello, HIGH);

  // Connect to Wi-Fi network with SSID and password
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

  //start watchdog to reboot esp.
  esp_task_wdt_init(WDT_TIMEOUT, true);
  esp_task_wdt_add(NULL);
}

void loop(){
  WiFiClient client = server.available();   // Listen for incoming clients

  if (client) {                             // If a new client connects,
    currentTime = millis();
    previousTime = currentTime;
    String currentLine = "";                // make a String to hold incoming data from the client
    while (client.connected() && currentTime - previousTime <= timeoutTime) {  // loop while the client's connected
      currentTime = millis();
      if (client.available()) {             // if there's bytes to read from the client,
        char c = client.read();             // read a byte, then
        header += c;
        if (c == '\n') {                    // if the byte is a newline character
          // if the current line is blank, you got two newline characters in a row.
          // that's the end of the client HTTP request, so send a response:
          if (currentLine.length() == 0) {
            // check base64 encode for authentication
            // Finding the right credentials
            if (header.indexOf(base64Encoding)>=0)
            {
            
              // HTTP headers always start with a response code (e.g. HTTP/1.1 200 OK)
              // and a content-type so the client knows what's coming, then a blank line:
              client.println("HTTP/1.1 200 OK");
              client.println("Content-type:text/html");
              client.println("Connection: close");
              client.println();
              
              // turns the GPIOs on and off
              if (header.indexOf("GET /opendoor") >= 0) {
                Serial.println("Apro portone");
                digitalWrite(5, LOW);
                delay(delayTime);
                digitalWrite(5, HIGH);

                //restart the device
                esp_task_wdt_delete(NULL);
                esp_task_wdt_init(TIME_OUT, true);
                esp_task_wdt_add(NULL);

                //redirect to the homepage
                client.print("<head>");
                client.print("<meta http-equiv=\"refresh\" content=\"0;url=/\">");
                client.print("</head>");
              } else if (header.indexOf("GET /opengate") >= 0) {
                Serial.println("Apro cancello");
                digitalWrite(4, LOW);
                delay(delayTime);
                digitalWrite(4, HIGH);
                
                //restart the device                
                esp_task_wdt_delete(NULL);
                esp_task_wdt_init(TIME_OUT, true);
                esp_task_wdt_add(NULL);

                //redirect to the homepage
                client.print("<head>");
                client.print("<meta http-equiv=\"refresh\" content=\"0;url=/\">");
                client.print("</head>"); 
              } else if (header.indexOf("GET /openall") >= 0) {
                Serial.println("Apro tutto"); 
                digitalWrite(5, LOW);
                delay(delayTime);
                digitalWrite(5, HIGH);
                delay(delayTime);  
                digitalWrite(4, LOW);
                delay(delayTime);
                digitalWrite(4, HIGH);

                //restart the device
                esp_task_wdt_delete(NULL);
                esp_task_wdt_init(TIME_OUT, true);
                esp_task_wdt_add(NULL);

                //redirect to the homepage
                client.print("<head>");
                client.print("<meta http-equiv=\"refresh\" content=\"0;url=/\">");
                client.print("</head>");  
              } 
              
              // Display the HTML web page
              client.println("<!DOCTYPE html>");
              client.println("<html>\n");
              client.println("<head>\n");
              client.println("<meta name=\"viewport\" content=\"width=device-width, initial-scale=1.0, user-scalable=no\">\n");
              client.println("<title>Pagina di apertura porta</title>\n");
              client.println("<style>html { font-family: Helvetica; display: inline-block; margin: 0px auto; text-align: center;}\n");
              client.println(".button {display: block;width: 80px;background-color: #3498db;border: none;color: white;padding: 13px 30px;text-decoration: none;font-size: 25px;margin: 0px auto 35px;cursor: pointer;border-radius: 4px;}\n");
              client.println("</style>\n");
              client.println("</head>\n");
              client.println("<body>\n");
              client.println("<div style=\"text-align:center\">\n");
              client.println("<h1 style=\"color: blue; font-size: 32px;\">Apri la porta</h1>\n");
              client.println("<p style=\"font-size: 18px;\">Clicca sul bottone sottostante per aprire la porta:</p>\n");
              client.println("<div>\n");
              client.println("<a class=\"button\" id='portone' href=\"/opendoor\">Apri portone</a>\n");
              client.println("<a class=\"button\" id='cancello' href=\"/opengate\">Apri Cancello</a>\n");
              client.println("<a class=\"button\" id='tutto' href=\"/openall\">Apri Tutto</a>\n");
              client.println("</div>\n");
              client.println("</div>\n");
              client.println("</body>\n");
              client.println("</html>\n");
              
              // The HTTP response ends with another blank line
              client.println();
              // Break out of the while loop
              break;
            }
            else{
              client.println("HTTP/1.1 401 Unauthorized");
              client.println("WWW-Authenticate: Basic realm=\"Secure\"");
              client.println("Content-Type: text/html");
              client.println();
              client.println("<html>Authentication failed</html>");
            }
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
  }
}

