/* dimodif dari file */
/* Web_Parms_1.pde - very simple Webduino example of parameter passing and parsing */
/* bergantung pada library webduino https://github.com/sirleech/Webduino */
/* AQUAPUNO_ETH 
   Deskripsi
   ---------
   AQUAPUNO adalah system terpadu AQUAPONIK dibantu dengan microcontroller arduino.
   menggunakan arduino UNO sebagai alat mengukur sensor dan menyalakan/mematikan relay
   (alat elektronik) seperti pompa atau pengatur pupuk.
 
  versi: 0.9
  by   : Irza pulungan  
  todo:
  -json format when requested
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

/* mengatur timer Relay (pin, waktu nyala, waktu mati, deskripsi)
   disini diatur dengan menggunakan milli time 3000 = 3 detik
   waktu nyala dan waktu mati akan otomatis dihitung dari nyala
   arduino
*/
RelayTimer Relay1(4, 0, 400000, "Pompa Air");
RelayTimer Relay2(5, 0, 350000, "Oksigen ikan");
RelayTimer Relay3(6, 0, 550000, "Makan ikan");
RelayTimer Relay4(7, 0, 650000, "Pompa Pupuk");

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


void getInfo(WebServer &server, WebServer::ConnectionType type, char *url_tail, bool tail_complete)
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


        
        String ckname = name;
        String ckvalue = value;
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

            String data1 = "ARD1";
            char test[20];

            String myold = "{ \"MYDT\" : [ { \"device_name\" : \"" + data1 + "\"}, ";
            myold = myold + "{ \"sensor\" : \"temperature\" , \"value\" : \"" + String(floatToString(test,t,2,4)) + "\"},";
            myold = myold + "{ \"sensor\" : \"humidity\" , \"value\" : \"" + String(floatToString(test,h,2,4)) + "\"},";
            myold = myold + "{ \"sensor\" : \"light\" , \"value\" : \"" + String(lhtval) + "\"},";
            myold = myold + "{ \"sensor\" : \"flow1\" , \"value\" : \"" + String(random(0,5)) + "\"},";
            myold = myold + "{ \"sensor\" : \"flow2\" , \"value\" : \"" + String(random(6,10)) + "\"},";
            myold = myold + "{ \"sensor\" : \"relay1_state\" , \"value\" : \"" + String(Relay1.State()*5) +  "\", \"description\" : \"" + Relay1.Desc() + "\", \"OnTime\" : \"" + Relay1.getOn() + "\", \"OffTime\" : \"" + Relay1.getOff() + "\" },";
            myold = myold + "{ \"sensor\" : \"relay2_state\" , \"value\" : \"" + String(Relay2.State()*10) + "\", \"description\" : \"" + Relay2.Desc() + "\", \"OnTime\" : \"" + Relay2.getOn() + "\", \"OffTime\" : \"" + Relay2.getOff() + "\" },";
            myold = myold + "{ \"sensor\" : \"relay3_state\" , \"value\" : \"" + String(Relay3.State()*15) + "\", \"description\" : \"" + Relay3.Desc() + "\", \"OnTime\" : \"" + Relay3.getOn() + "\", \"OffTime\" : \"" + Relay3.getOff() + "\" },";
            myold = myold + "{ \"sensor\" : \"relay4_state\" , \"value\" : \"" + String(Relay4.State()*20) + "\", \"description\" : \"" + Relay4.Desc() + "\", \"OnTime\" : \"" + Relay4.getOn() + "\", \"OffTime\" : \"" + Relay4.getOff() + "\" },";
            myold = myold + "{ \"sensor\" : \"analog1\" , \"value\" : \"" + String(analogRead(A1)) + "\"},";
            myold = myold + "{ \"sensor\" : \"analog2\" , \"value\" : \"" + String(analogRead(A2)) + "\"},";
            myold = myold + "{ \"sensor\" : \"analog3\" , \"value\" : \"" + String(analogRead(A3)) + "\"},";
            myold = myold + "{ \"sensor\" : \"analog4\" , \"value\" : \"" + String(analogRead(A4)) + "\"} ] }";

            server.print(myold);
}


void relay1(WebServer &server, WebServer::ConnectionType type, char *url_tail, bool tail_complete)
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
        if (ckname == "ON"){
           Relay1.setOn(atol(value));
         }
        if (ckname == "OFF"){
           Relay1.setOff(atol(value));
         }
        if (ckname == "SET_ON"){
           Relay1.TurnOn();
         }
        if (ckname == "SET_OFF"){
           Relay1.TurnOff();
         }

       }
      }
    }
  server.printP(Page_end);
}


void relay2(WebServer &server, WebServer::ConnectionType type, char *url_tail, bool tail_complete)
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
        if (ckname == "ON"){
           Relay2.setOn(atol(value));
         }
        if (ckname == "OFF"){
           Relay2.setOff(atol(value));
         }
        if (ckname == "SET_ON"){
           Relay2.TurnOn();
         }
        if (ckname == "SET_OFF"){
           Relay2.TurnOff();
         }

       }
      }
    }
  server.printP(Page_end);
}

void relay3(WebServer &server, WebServer::ConnectionType type, char *url_tail, bool tail_complete)
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
        if (ckname == "ON"){
           Relay3.setOn(atol(value));
         }
        if (ckname == "OFF"){
           Relay3.setOff(atol(value));
         }
        if (ckname == "SET_ON"){
           Relay3.TurnOn();
         }
        if (ckname == "SET_OFF"){
           Relay3.TurnOff();
         }
       }
      }
    }
  server.printP(Page_end);
}

void relay4(WebServer &server, WebServer::ConnectionType type, char *url_tail, bool tail_complete)
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
        if (ckname == "ON"){
           Relay4.setOn(atol(value));
         }
        if (ckname == "OFF"){
           Relay4.setOff(atol(value));
         }
        if (ckname == "SET_ON"){
           Relay4.TurnOn();
         }
        if (ckname == "SET_OFF"){
           Relay4.TurnOff();
         }
       }
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
  webserver.addCommand("getinfo.html", &getInfo);
  webserver.addCommand("relay1.html", &relay1);
  webserver.addCommand("relay2.html", &relay2);
  webserver.addCommand("relay3.html", &relay3);
  webserver.addCommand("relay4.html", &relay4);
 
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
