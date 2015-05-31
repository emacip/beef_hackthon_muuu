
#include <Wire.h> // Must include Wire library for I2C
#include <SFE_MMA8452Q.h> // Includes the SFE_MMA8452Q library
#include <SPI.h> 
#include <WiFi.h>

//char ssid[] = "Guest_DCU_Innovation";     //  your network SSID (name) 
//char pass[] = "innovate5443";  // your network password
//int status = WL_IDLE_STATUS;     // the Wifi radio's status


MMA8452Q accel;
   
char ssid[] = "Guest_DCU_Innovation";     //  your network SSID (name) 
char pass[] = "innovate5443";  // your network password
int status = WL_IDLE_STATUS;     // the Wifi radio's status

int red_led = 12;
int green_led = 13;
int led_blue   = 12;


int TEMP_PIN = A2;

int VIBRO_PIN =A1;
//int VIBRO_GROUND =A0;

static double oldValueMicro = 0;
static double oldValueVibro = 0;

int MICRO_PIN = A0;
double alpha = 0.75;
int period = 100;
double change = 0.0;
double minval = 0.0; 

int counter = 0;
static const int max_counter = 1;
char buf[1000];
double lastRead = 0.0;
unsigned long initialTime;
unsigned long endTime; 
  
int vibrationValue = 0;
int microValue = 0;
double tempValue = 0.0;
  
unsigned long readLatency = 1000;
  
boolean debug = false;
  


// The setup function simply starts serial and initializes the
//  accelerometer.
void setup()
{
  
   //Initialize serial and wait for port to open:
  Serial.begin(9600); 
  while (!Serial) {
    ; // wait for serial port to connect. Needed for Leonardo only
  }
  
  // check for the presence of the shield:
  if (WiFi.status() == WL_NO_SHIELD) {
    Serial.println("WiFi shield not present"); 
    // don't continue:
    while(true);
  } 

  String fv = WiFi.firmwareVersion();
  if( fv != "1.1.0" )
    Serial.println("Please upgrade the firmware");
  
 // attempt to connect to Wifi network:
  while ( status != WL_CONNECTED) { 
    Serial.print("Attempting to connect to WPA SSID: ");
    Serial.println(ssid);
    // Connect to WPA/WPA2 network:    
    status = WiFi.begin(ssid, pass);

    // wait 10 seconds for connection:
    delay(10000);
  }
   
  // you're connected now, so print out the data:
  Serial.print("You're connected to the network");
  printCurrentNet();
  printWifiData();
  delay(5000);
  Serial.begin(9600);      // open the serial port at 9600 bps:
  
  pinMode(red_led, OUTPUT);
  pinMode(green_led, OUTPUT);  
  pinMode(led_blue,  OUTPUT);   
  
 // pinMode(VIBRO_GROUND, OUTPUT);
 // digitalWrite(VIBRO_GROUND, LOW);
  
  accel.init();
}

// The loop function will simply check for new data from the
//  accelerometer and print it out if it's available.
void loop()
{
    
       
  // Use the accel.available() function to wait for new data
  //  from the accelerometer.
   
    initialTime = millis();
    // First, use accel.read() to read the new variables:
    
    
//     EthernetClient client;
   ///  lastRead = readAndSendHeart();

   vibrationValue = readVibration();
   microValue = readMicro();
   tempValue = readTemp();
     
     
     
   digitalWrite(red_led, LOW);    // turn the LED off by making the voltage LOW
   digitalWrite(green_led, LOW);    // turn the LED off by making the voltage LOW 
     
  endTime = millis();

  if ( debug ) {

    Serial.print("Galileo IP: ");
    //Serial.println(Ethernet.localIP());
    Serial.print("Initial time: ");
    Serial.println(initialTime);
    Serial.print("End time: ");
    Serial.println(endTime);
    Serial.print("difference ");    
    endTime = endTime - initialTime;
    Serial.println(endTime); 

    Serial.print("Delay: ");    
    Serial.println(endTime > readLatency ? 0 : readLatency - endTime);
    
  }

     endTime = endTime - initialTime;

    // printVibro(vibrationValue);
    // printMicro(microValue);
   //  printTemp(tempValue);
     
     
  if (accel.available())
  {
    int isEating = logicOrientation(microValue - oldValueMicro);
    int isWalking = 0;
    if ( vibrationValue >= oldValueVibro){
      isWalking = 2;
    }
   
    Serial.println("....");
    Serial.println(tempValue);
    Serial.println("....");
    Serial.println(microValue);
    Serial.println("....");
    Serial.println(vibrationValue);
    // COMMUNICATION        

    
    sprintf(buf, "curl http://www.myspotplease.com/beefhackathon/guille/web/app_dev.php/api/sensors?food=%d\\&microphone=%d\\&vibration=%d\\&theromemeter=%2.2f", isEating, microValue, isWalking, tempValue);
    //sprintf(buf,"curl http://169.254.26.128:3000/ecgs/2/new_ecgs?value=%2.6f",lastRead);       
    int errorCode = system(buf);
    
           
     //sprintf(buf,"curl http://169.254.26.128:3000/ecgs/2/new_ecgs?value=%2.6f",lastRead);

     
     
     
  }
       
     
     delay( endTime > readLatency ? 0 : readLatency - endTime );
  //  digitalWrite(green_led, HIGH);
     //delay( 100);
     

     


}

// The function demonstrates how to use the accel.x, accel.y and
//  accel.z variables.
// Before using these variables you must call the accel.read()
//  function!
void printAccels()
{
  Serial.print(accel.x, 3);
  Serial.print("\t");
  Serial.print(accel.y, 3);
  Serial.print("\t");
  Serial.print(accel.z, 3);
  Serial.print("\t");
}

// This function demonstrates how to use the accel.cx, accel.cy,
//  and accel.cz variables.
// Before using these variables you must call the accel.read()
//  function!
void printCalculatedAccels()
{ 
  Serial.print(accel.cx, 3);
  Serial.print("\t");
  Serial.print(accel.cy, 3);
  Serial.print("\t");
  Serial.print(accel.cz, 3);
  Serial.print("\t");
}

// This function demonstrates how to use the accel.readPL()
// function, which reads the portrait/landscape status of the
// sensor.
void printOrientation()
{
  // accel.readPL() will return a byte containing information
  // about the orientation of the sensor. It will be either
  // PORTRAIT_U, PORTRAIT_D, LANDSCAPE_R, LANDSCAPE_L, or
  // LOCKOUT.
  byte pl = accel.readPL();
  switch (pl)
  {
  case PORTRAIT_U:
    Serial.print("Portrait Up");
    break;
  case PORTRAIT_D:
    digitalWrite(led_blue, HIGH);
    Serial.print("Portrait Down");
    break;
  case LANDSCAPE_R:
    Serial.print("Landscape Right");
    break;
  case LANDSCAPE_L:
    Serial.print("Landscape Left");
    break;
  case LOCKOUT:
    Serial.print("Flat");  
    digitalWrite(led_blue, LOW);
    break;
  }
}



void printTemp(double temp) {
  Serial.print(" Temp ");
  Serial.print( temp );
  Serial.println("  ");
}

double readTemp() {
  double rawValue = analogRead (TEMP_PIN);
  if ( debug ) {
    printTemp(rawValue);
   }    
   return   21 + ((144 - rawValue)/2.69);   
}

int logicOrientation( int micValue ) {  
  int eating = 0;
  // LOCKOUT.
  byte pl = accel.readPL();
  switch (pl)
  {
  case PORTRAIT_U:
    Serial.print("Portrait Up");
    break;
  case PORTRAIT_D:
    digitalWrite(led_blue, HIGH);
    Serial.println("*****Portrait Down***");   
    if ( micValue > 25  ) {
      digitalWrite(green_led, HIGH);
      eating = 1;
      delay( 1500 );
    }    
    break;
  case LANDSCAPE_R:
    Serial.print("Landscape Right");
    break;
  case LANDSCAPE_L:
    Serial.print("Landscape Left");
    break;
  case LOCKOUT:
    Serial.print("Wait ...");  
    digitalWrite(led_blue, LOW);
    break;
  }
  
  return eating;
  
  
}


void printMicro(int micro) {

    Serial.print(" Micro ");
    Serial.print( micro - oldValueMicro );
    Serial.println("  ");


}

int readMicro() {
  
    int rawValue = analogRead (MICRO_PIN);

 if ( debug ) {
    
    printMicro(rawValue);

 }
 
   int toReturn  = oldValueMicro;

   oldValueMicro = rawValue;//alpha * oldValueMicro + (1 - alpha) * rawValue;

   
 return   toReturn;  
  
  
}

void printVibro(int vibration) {

    Serial.print(" Vibro ");
    Serial.println( vibration );
    Serial.println("  ");


}

int readVibration() {
  
    int rawValue = analogRead (VIBRO_PIN);
    int toReturn  = oldValueVibro;
    
    oldValueVibro = rawValue;
     
 if ( debug ) {
    printVibro(rawValue);
 }
   
 return   toReturn;  
  
  
}


//int webUnixTime (EthernetClient &client)
int readAndSendHeart ()
{


 // Serial.println(Ethernet.localIP());
 // Serial.println("Trying to connect to the server \n");
 
 
    static double oldValue = 0;
    static double oldChange = 0;
 
    int rawValue = analogRead (MICRO_PIN);
    double value = alpha * oldValue + (1 - alpha) * rawValue;
 
 if ( debug ) {
 
    Serial.print (rawValue);
    Serial.print (",");
    Serial.println (value);

//value = 232.12;

 }

    oldValue = value;
 



     
 return   (value -970)*6;

}

void printWifiData() {
  // print your WiFi shield's IP address:
  IPAddress ip = WiFi.localIP();
    Serial.print("IP Address: ");
  Serial.println(ip);
  Serial.println(ip);
  
  // print your MAC address:
  byte mac[6];  
  WiFi.macAddress(mac);
  Serial.print("MAC address: ");
  Serial.print(mac[5],HEX);
  Serial.print(":");
  Serial.print(mac[4],HEX);
  Serial.print(":");
  Serial.print(mac[3],HEX);
  Serial.print(":");
  Serial.print(mac[2],HEX);
  Serial.print(":");
  Serial.print(mac[1],HEX);
  Serial.print(":");
  Serial.println(mac[0],HEX);
 
}

void printCurrentNet() {
 

  // print the MAC address of the router you're attached to:
  byte bssid[6];
  WiFi.BSSID(bssid);    
  Serial.print("BSSID: ");
  Serial.print(bssid[5],HEX);
  Serial.print(":");
  Serial.print(bssid[4],HEX);
  Serial.print(":");
  Serial.print(bssid[3],HEX);
  Serial.print(":");
  Serial.print(bssid[2],HEX);
  Serial.print(":");
  Serial.print(bssid[1],HEX);
  Serial.print(":");
  Serial.println(bssid[0],HEX);

  // print the received signal strength:
  long rssi = WiFi.RSSI();
  Serial.print("signal strength (RSSI):");
  Serial.println(rssi);

  // print the encryption type:
  byte encryption = WiFi.encryptionType();
  Serial.print("Encryption Type:");
  Serial.println(encryption,HEX);
  Serial.println();
}

