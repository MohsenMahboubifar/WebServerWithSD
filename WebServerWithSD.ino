#include <SPI.h>
#include <Ethernet.h>
#include <SD.h>

#define IP IPAddress(192,168,0,200)
#define req_len 100

byte mac[] = { 0xDE, 0xED, 0xBE, 0xEF, 0xFE, 0xED };
byte req[req_len];
int req_index = 0;
File myFile;

EthernetServer server(80);       // create a server at port 80

byte StrContains(char *str, char *sfind)
{
    //byte len = strlen(str); 
    #define LEN strlen(str) 
    byte found = 0;
    byte index = 0;
      
    if (strlen(sfind) > LEN) return 0;
    while (index < LEN) 
    {
        if (str[index] == sfind[found]) 
        {
            found++;
            if (strlen(sfind) == found) return index;    
        }
        else found = 0;
        index++;
    }
    return 0;
}

void clearRequest()
{
  for(int i=0;i<req_len;i++)
    req[i] = "";
}

void setup() {
  pinMode(13, HIGH);
  // put your setup code here, to run once:
  Serial.begin(9600);       // for debugging    
  if(!Ethernet.begin(mac))
  {
    Ethernet.begin(mac, IP);  // initialize Ethernet device => Ethernet.begin(mac, ip);
  }
  Serial.println(Ethernet.localIP());
  if(!SD.begin(4))
  {
    Serial.println("SD initialization failed!");
    while(1);
  }
  Serial.println("Congratulations! SD initialization is Done!");
  server.begin();           // start to listen for clients
}

void loop() {
  // put your main code here, to run repeatedly:
  EthernetClient client = server.available();  // try to get client
  
  if(client)
  {
    Serial.println("Client is here");
    if(client.available())
    {
      char ch;
      bool currentLineIsBlank = false;
      while(client.connected())
      {
        if(client.available())
        {
          ch=client.read();
          if(req_index<(req_len-1))
          {
            req[req_index++] = ch;            
          }
          
          Serial.print(ch);
        }
        if(ch=='\n' && currentLineIsBlank)
        {          
          if(StrContains(req, "GET / "))
          {
            client.println("HTTP/1.1 200 OK");
            client.println("Content-Type: text/html");
            client.println("Connnection: close");
            client.println();
            if(!SD.exists("control.htm"))
            {
              Serial.println("control.htm file does not exist!");
            }
            myFile = SD.open("control.htm");
            while(myFile.available())
            {
              client.write(myFile.read());
            }
            myFile.close();
          }
          else if(StrContains(req, "GET /style.css "))
          {
            client.println("HTTP/1.1 200 OK");
            client.println("Content-Type: text/css");
            client.println("Connnection: close");
            client.println();
            if(!SD.exists("style.css"))
            {
              Serial.println("style.css file does not exist!");
            }
            myFile = SD.open("style.css");
            while(myFile.available())
            {
              client.write(myFile.read());
            }
            myFile.close();
          }
          else if(StrContains(req, "GET /LED_ON "))
          {
            digitalWrite(13, HIGH);
            client.println("HTTP/1.1 200 OK");
            client.println("Content-Type: text/html");
            client.println("Connnection: close");
            client.println();
          }
          else if(StrContains(req, "GET /LED_OFF "))
          {
            digitalWrite(13, LOW);
            client.println("HTTP/1.1 200 OK");
            client.println("Content-Type: text/html");
            client.println("Connnection: close");
            client.println();
          }
          delay(5);
          client.stop();
          delay(5);
          req_index = 0;
          clearRequest();
        }
        if(ch=='\n')
        {
          currentLineIsBlank = true;
        }
        else if(ch!='\r')
        {
          currentLineIsBlank = false;
        }
      }
    }
  }
}
