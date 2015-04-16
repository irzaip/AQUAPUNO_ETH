/* Web_Parms_1.pde - very simple Webduino example of parameter passing and parsing */

/*
 * This is mostly a tool for testing and debugging the library, but can
 * also be used as an example of coding for it.
 *
 * To use it,  enter one of the following USLs into your browser.
 * Replace "host" with the IP address assigned to the Arduino.
 *
 * http://host/
 * http://host/index.html
 *
 * These return a "success" HTTP result and display the parameters
 * (if any) passed to them as a single string,  without attempting to
 * parse them.  This is done with a call to defaultCmd.
 * 
 * 
 * http://host/raw.html
 *
 * This is essentially the same as the index.html URL processing,
 * but is done by calling rawCmd.
 * 
 * 
 * http://host/parsed.html
 *
 * This invokes parsedCmd,  which displays the "raw" parameter string,
 * but also uses the "nexyURLparam" routine to parse out the individual
 * parameters, and display them.
 */


#define WEBDUINO_FAIL_MESSAGE "<h1>Request Failed</h1>"
#include "SPI.h" // new include
#include "avr/pgmspace.h" // new include
#include "Ethernet.h"
#include "WebServer.h"
#include "Relay.h"
#include "DHT.h"

#define VERSION_STRING "0.1"

#define DHTPIN A0     // what pin we're connected to
#define LHTPIN A5

#define DHTTYPE DHT22   // DHT 22  (AM2302)

/* CHANGE THIS TO YOUR OWN UNIQUE VALUE.  The MAC number should be
 * different from any other devices on your network or you'll have
 * problems receiving packets. */
static uint8_t mac[] = { 0x00, 0xAA, 0xBB, 0xCC, 0xDE, 0x02 };


/* CHANGE THIS TO MATCH YOUR HOST NETWORK.  Most home networks are in
 * the 192.168.0.XXX or 192.168.1.XXX subrange.  Pick an address
 * that's not in use and isn't going to be automatically allocated by
 * DHCP from your router. */
static uint8_t ip[] = { 192, 168, 30, 93 };

// ROM-based messages used by the application
// These are needed to avoid having the strings use up our limited
//    amount of RAM.

P(Page_start) = "<html><head><title>AQUAPUNO Version 0.1" VERSION_STRING "</title></head><body>\n";
P(Page_end) = "</body></html>";
P(Get_head) = "<h1>GET from ";
P(Post_head) = "<h1>POST to ";
P(Unknown_head) = "<h1>UNKNOWN request for ";
P(Default_head) = "unidentified URL requested.</h1><br>\n";
P(Raw_head) = "raw.html requested.</h1><br>\n";
P(Parsed_head) = "parsed.html requested.</h1><br>\n";
P(Good_tail_begin) = "URL tail = '";
P(Bad_tail_begin) = "INCOMPLETE URL tail = '";
P(Tail_end) = "'<br>\n";
P(Parsed_tail_begin) = "URL parameters:<br>\n";
P(Parsed_item_separator) = " = '";
P(Params_end) = "End of parameters<br>\n";
P(Post_params_begin) = "Parameters sent by POST:<br>\n";
P(Line_break) = "<br>\n";



/* This creates an instance of the webserver.  By specifying a prefix
 * of "", all pages will be at the root of the server. */
#define PREFIX ""
WebServer webserver(PREFIX, 80);
RelayTimer Relay1(4, 3000, 4000, "Pompa Air");
RelayTimer Relay2(5, 3000, 3500, "Oksigen ikan");
RelayTimer Relay3(6, 3000, 5500, "Makan ikan");
RelayTimer Relay4(7, 3000, 6500, "Pompa Pupuk");
DHT dht(DHTPIN, DHTTYPE);


char * floatToString(char * outstr, double val, byte precision, byte widthp){
 char temp[16];
 byte i;

 // compute the rounding factor and fractional multiplier
 double roundingFactor = 0.5;
 unsigned long mult = 1;
 for (i = 0; i < precision; i++)
 {
   roundingFactor /= 10.0;
   mult *= 10;
 }
 
 temp[0]='\0';
 outstr[0]='\0';

 if(val < 0.0){
   strcpy(outstr,"-\0");
   val = -val;
 }

 val += roundingFactor;

 strcat(outstr, itoa(int(val),temp,10));  //prints the int part
 if( precision > 0) {
   strcat(outstr, ".\0"); // print the decimal point
   unsigned long frac;
   unsigned long mult = 1;
   byte padding = precision -1;
   while(precision--)
     mult *=10;

   if(val >= 0)
     frac = (val - int(val)) * mult;
   else
     frac = (int(val)- val ) * mult;
   unsigned long frac1 = frac;

   while(frac1 /= 10)
     padding--;

   while(padding--)
     strcat(outstr,"0\0");

   strcat(outstr,itoa(frac,temp,10));
 }

 // generate space padding 
 if ((widthp != 0)&&(widthp >= strlen(outstr))){
   byte J=0;
   J = widthp - strlen(outstr);
   
   for (i=0; i< J; i++) {
     temp[i] = ' ';
   }

   temp[i++] = '\0';
   strcat(temp,outstr);
   strcpy(outstr,temp);
 }
 
 return outstr;
}


/* commands are functions that get called by the webserver framework
 * they can read any posted data from client, and they output to the
 * server to send data back to the web browser. */
void helloCmd(WebServer &server, WebServer::ConnectionType type, char *url_tail, bool tail_complete)
{
  /* this line sends the standard "we're all OK" headers back to the
     browser */
  server.httpSuccess();

  /* if we're handling a GET or POST, we can output our data here.
     For a HEAD request, we just stop after outputting headers. */
  if (type == WebServer::HEAD)
    return;

  server.printP(Page_start);
  switch (type)
    {
    case WebServer::GET:
        server.printP(Get_head);
        break;
    case WebServer::POST:
        server.printP(Post_head);
        break;
    default:
        server.printP(Unknown_head);
    }

    server.printP(Default_head);
    server.printP(tail_complete ? Good_tail_begin : Bad_tail_begin);
    server.print(url_tail);
    server.printP(Tail_end);
    server.printP(Page_end);

}


void rawCmd(WebServer &server, WebServer::ConnectionType type, char *url_tail, bool tail_complete)
{
  /* this line sends the standard "we're all OK" headers back to the
     browser */
  server.httpSuccess();

  /* if we're handling a GET or POST, we can output our data here.
     For a HEAD request, we just stop after outputting headers. */
  if (type == WebServer::HEAD)
    return;

  server.printP(Page_start);
  switch (type)
    {
    case WebServer::GET:
        server.printP(Get_head);
        break;
    case WebServer::POST:
        server.printP(Post_head);
        break;
    default:
        server.printP(Unknown_head);
    }

    server.printP(Raw_head);
    server.printP(tail_complete ? Good_tail_begin : Bad_tail_begin);
    server.print(url_tail);
    server.printP(Tail_end);
    server.printP(Page_end);

}

#define NAMELEN 32
#define VALUELEN 32

void parsedCmd(WebServer &server, WebServer::ConnectionType type, char *url_tail, bool tail_complete)
{
  URLPARAM_RESULT rc;
  char name[NAMELEN];
  int  name_len;
  char value[VALUELEN];
  int value_len;

  /* this line sends the standard "we're all OK" headers back to the
     browser */
  server.httpSuccess();

  /* if we're handling a GET or POST, we can output our data here.
     For a HEAD request, we just stop after outputting headers. */
  if (type == WebServer::HEAD)
    return;

  server.printP(Page_start);
  switch (type)
    {
    case WebServer::GET:
        server.printP(Get_head);
        break;
    case WebServer::POST:
        server.printP(Post_head);
        break;
    default:
        server.printP(Unknown_head);
    }

    server.printP(Parsed_head);
    server.printP(tail_complete ? Good_tail_begin : Bad_tail_begin);
    server.print(url_tail);
    server.printP(Tail_end);

  if (strlen(url_tail))
    {
    server.printP(Parsed_tail_begin);
    while (strlen(url_tail))
      {
      rc = server.nextURLparam(&url_tail, name, NAMELEN, value, VALUELEN);
      if (rc == URLPARAM_EOS)
        server.printP(Params_end);
       else
        {
        server.print(name);
        server.printP(Parsed_item_separator);
        server.print(value);
        server.printP(Tail_end);
        String ckname = name;
        String ckvalue = value;
        if (ckname == "RLY1"){Relay1.TurnOn();server.print("1 ON");}
        if (ckname == "RLY2"){Relay2.TurnOn();server.print("2 ON");}
        if (ckname == "RLY3"){Relay3.TurnOn();server.print("3 ON");}
        if (ckname == "RLY4"){Relay4.TurnOn();server.print("4 ON");}
        if (ckname == "GETINFO"){
            float h = dht.readHumidity();
            float t = dht.readTemperature();

            if (isnan(t) || isnan(h)) 
                {
                    h = 1;
                    t = 1;
                } 

            int lhtval = analogRead(LHTPIN);
            //lhtval = int(lhtval / 10);

            char bbuffer[5];

            String mydt = "MYDT";
            String data1 = "ARD1";
            
            char test[20];

            String data2 = String(floatToString(test,t,2,4)); //temperature
            String data3 = String(floatToString(test,h,2,4)); //humidity
            String data4 = String(lhtval); //light
            String data5 = String(random(0,5)); //flow1
            String data6 = String(random(6,10)); //flow2
            String data7 = String(Relay1.State()*5); //relay1
            String data8 = String(Relay2.State()*10); //relay2
            String data9 = String(Relay3.State()*15); //relay3
            String data10 = String(Relay4.State()*20); //relay4
            String data11 = String(analogRead(A1));
            String data12 = String(analogRead(A2));
            String data13 = String(analogRead(A3));
            String data14 = String(analogRead(A4));
            String delim = ":";
            String retrn = "\r\n";
            String myold = mydt + delim + data1 + delim + data2 + delim + data3 + delim + data4 + delim + data5 + delim + data6 + delim + data7 + delim + data8 + delim + data9 + delim + data10 + delim + data11 + delim + data12 + delim + data13 + delim + data14 +delim + retrn;
            char mydata[70] = "";
 
            server.print(myold);
          }
        
        }
      }
    }
  if (type == WebServer::POST)
  {
    server.printP(Post_params_begin);
    while (server.nextURLparam(&url_tail, name, NAMELEN, value, VALUELEN))
    {
      server.print(name);
      server.printP(Parsed_item_separator);
      server.print(value);
      server.printP(Tail_end);
    }
  }
  server.printP(Page_end);

}

void my_failCmd(WebServer &server, WebServer::ConnectionType type, char *url_tail, bool tail_complete)
{
  /* this line sends the standard "we're all OK" headers back to the
     browser */
  server.httpFail();

  /* if we're handling a GET or POST, we can output our data here.
     For a HEAD request, we just stop after outputting headers. */
  if (type == WebServer::HEAD)
    return;

  server.printP(Page_start);
  switch (type)
    {
    case WebServer::GET:
        server.printP(Get_head);
        break;
    case WebServer::POST:
        server.printP(Post_head);
        break;
    default:
        server.printP(Unknown_head);
    }

    server.printP(Default_head);
    server.printP(tail_complete ? Good_tail_begin : Bad_tail_begin);
    server.print(url_tail);
    server.printP(Tail_end);
    server.printP(Page_end);

}

void setup()
{
  /* initialize the Ethernet adapter */
  Ethernet.begin(mac, ip);

  //setup sensor temp
  dht.begin();
   
  /* setup our default command that will be run when the user accesses
   * the root page on the server */
  webserver.setDefaultCommand(&helloCmd);

  /* setup our default command that will be run when the user accesses
   * a page NOT on the server */
  webserver.setFailureCommand(&my_failCmd);

  /* run the same command if you try to load /index.html, a common
   * default page name */
  webserver.addCommand("index.html", &helloCmd);

  /*This command  is called if you try to load /raw.html */
  webserver.addCommand("raw.html", &rawCmd);
  webserver.addCommand("parsed.html", &parsedCmd);

  /* start the webserver */
  webserver.begin();
}

void loop()
{
  char buff[64];
  int len = 64;
  
  Relay1.Update();
  Relay2.Update();
  Relay3.Update();
  Relay4.Update();
  
  /* process incoming connections one at a time forever */
  webserver.processConnection(buff, &len);
}
