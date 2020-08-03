
//Takes care of serving index.html
void handleRoot() {
  server.send(200, "text/html", INDEX_HTML);
}

//Allows to update the temperature in the webpage
void handleTemp() {
 server.send(200, "text/plane", temperature); //Send temperature to client ajax request
}

//Allows to update the humidity in the webpage
void handleHum() {
 server.send(200, "text/plane", humidity); //Send temperature to client ajax request
}

//If you ask for a no existing web page
void handleNotFound() {
  String message = "File Not Found\n\n";
  message += "URI: ";
  message += server.uri();
  message += "\nMethod: ";
  message += (server.method() == HTTP_GET) ? "GET" : "POST";
  message += "\nArguments: ";
  message += server.args();
  message += "\n";
  
  for (uint8_t i = 0; i < server.args(); i++) {
    message += " " + server.argName(i) + ": " + server.arg(i) + "\n";
  }
  server.send(404, "text/plain", message);
}
