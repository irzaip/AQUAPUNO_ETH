class RelayTimer
{
	// Class Member Variables
	// These are initialized at startup
	int RelayPin;      // the number of the Relay pin
	long OnTime;     // milliseconds of on-time
	long OffTime;    // milliseconds of off-time
        String RelayDescr;
	// These maintain the current state
	int RelayState;             		// RelayState used to set the Relay
	unsigned long previousMillis;  	// will store last time Relay was updated
 
  // Constructor - creates a RelayTimer 
  // and initializes the member variables and state
  public:
  RelayTimer(int pin, long on, long off, String RelayDesc)
  {
	RelayPin = pin;
	pinMode(RelayPin, OUTPUT);     
	  
	OnTime = on;
	OffTime = off;
	RelayDescr = RelayDesc;

	RelayState = LOW; 
	previousMillis = 0;
  }
 
  void TurnOn(){
    digitalWrite(RelayPin, HIGH);
   }

  void TurnOff(){
    digitalWrite(RelayPin, LOW);
   }
  
  int State(){
    if (RelayState == HIGH){
      return 1;}
      else {
        return 0;}
  }
  
  String Desc(){
    return RelayDescr;
  }

  long getOn(){
    return OnTime;
  }

  long getOff(){
    return OffTime;
  }

  void setOn(long on){
    OnTime = on;
  }
  
  void setOff(long off){
    OffTime = off;
  }
  
  void Update()
  {
    // check to see if it's time to change the state of the Relay
    unsigned long currentMillis = millis();
    int readRelay = digitalRead(RelayPin);
    if(RelayState != readRelay){
            RelayState = readRelay;
            previousMillis = currentMillis;  // Remember the time
           digitalWrite(RelayPin, RelayState);  // Update the actual Relay
        }
    if((RelayState == HIGH) && (currentMillis - previousMillis >= OnTime))
    {
    	RelayState = LOW;  // Turn it off
      previousMillis = currentMillis;  // Remember the time
      digitalWrite(RelayPin, RelayState);  // Update the actual Relay
    }
    else if ((RelayState == LOW) && (currentMillis - previousMillis >= OffTime))
    {
      RelayState = HIGH;  // turn it on
      previousMillis = currentMillis;   // Remember the time
      digitalWrite(RelayPin, RelayState);	  // Update the actual Relay
     }
    } 
};

