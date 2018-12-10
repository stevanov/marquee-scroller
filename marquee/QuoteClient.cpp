/** The MIT License (MIT)

Copyright (c) 2018 Nathan Glaus & David Payne

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
*/

#include "QuoteClient.h"

#define arr_len( x )  ( sizeof( x ) / sizeof( *x ) )

QuoteClient::QuoteClient() {

}

void QuoteClient::updateQuote() {
  JsonStreamingParser parser;
  parser.setListener(this);
  WiFiClient quoteClient;

  String apiGetData = "GET /v1/cookie HTTP/1.1";

  Serial.println("Getting Quote Data");
  Serial.println(apiGetData);

  if (quoteClient.connect(servername, 80)) {  //starts client connection, checks for connection
    quoteClient.println(apiGetData);
    quoteClient.println("Host: " + String(servername));
    quoteClient.println("User-Agent: ArduinoWiFi/1.1");
    quoteClient.println("Connection: close");
    quoteClient.println();
  } 
  else {
    Serial.println("connection for quote data failed: " + String(servername)); //error message if no client connect
    Serial.println();
    return;
  }
  
  while(quoteClient.connected() && !quoteClient.available()) delay(1); //waits for data
 
  Serial.println("Waiting for data");

  int size = 0;
  char c;
  boolean isBody = false;
  while (quoteClient.connected() || quoteClient.available()) { //connected or data available
    c = quoteClient.read(); //gets byte from ethernet buffer
    if (c == '{' || c == '[') {
      isBody = true;
    }
    if (isBody) {
      parser.parse(c);
    }
  }
  quoteClient.stop(); //stop client
}

String QuoteClient::getQuote() {
  return quote.qUote;
}

void QuoteClient::whitespace(char c) {

}

void QuoteClient::startDocument() {
  
}

void QuoteClient::key(String key) {
  currentKey = key;
}

void QuoteClient::value(String value) {
  if (currentKey == "message") {
    quote.qUote = cleanText(value);
  }
  Serial.println(currentKey + "=" + value);
}

void QuoteClient::endArray() {
}

void QuoteClient::endObject() {
}
void QuoteClient::startArray() {
}

void QuoteClient::startObject() {
}

void QuoteClient::endDocument() {
}

String QuoteClient::cleanText(String text) {
  text.replace("’", "'");
  text.replace("“", "\"");
  text.replace("”", "\"");
  text.replace("`", "'");
  text.replace("‘", "'");
  text.replace("\\\"", "'");
  text.replace("•", "-");
  return text;
}
