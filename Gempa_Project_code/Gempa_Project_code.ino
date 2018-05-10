
//Libraries 
#include<Wire.h>
#include <DFRobot_sim808.h>
#define ACTIVATED HIGH
#define MESSAGE_LENGTH 160
#define TINY_GSM_MODEM_SIM808
#include <TinyGsmClient.h>
#include <SoftwareSerial.h>
DFRobot_SIM808 sim808(&Serial);
TinyGsm modem(Serial);
TinyGsmClient client(modem);

//For connection to the GPRS network
// Your GPRS credentials
// Leave empty, if missing user or pass
const char apn[]  = "48months.liffeytelecom.com";
const char user[] = "";
const char pass[] = "";

//server address for database
char server[] = "https://gempapanicalarm.000webhostapp.com";

//Variables
char message[MESSAGE_LENGTH];
char phone[16];
char datetime[24];

const int MPU_addr = 0x68;  // I2C address of the MPU-6050
const int buttonPinA = 2;   //Interrupt pin number for button A 
const int buttonPinB = 3;   //Interrupt pin number for button B
const int buttonPinC = 18;  //Interrupt pin number for button C
const int ledPin =  48;     // Pin number of the first LED 
const int ledPin2 =  38;    // Pin number of the second LED

//accelerometer ACX variable
int16_t AcX;
//Set unread message value to 0
int messageIndex = 0;
// variable for reading the pushbutton status
int buttonStateA = 0;
int buttonStateB = 0;
int buttonStateC = 0;

//Emergency mobile phone numbers
#define PHONE_NUMBER "086#######"
#define PHONE_NUMBER2 "086#######"

//messages for database and texting
#define MESSAGE "Panic Button Activated, Please Help Me!!! \n \nCannot find GPS coordinates"
#define MESSAGE2 "Sorry False Alarm, but please check on me anyway"
#define MESSAGE3 "Cannot+find+GPS+coordinates"
#define MESSAGE4 "Panic+alarm+was+Activated"
#define MESSAGE5 "False+alarm+message+was+Activated"
#define MESSAGE6 "Reset+button+was+Activated"
#define MESSAGE7 "Credit+balance+is+getting+low"
#define MESSAGE8 "Cannot find last known location"

//GPS variables
float lat ;
float lon ;
char charlat [20];
char charlon [20];
char readings [150];
char readings2 [150];

// count for receving message delay
int count1=0;

//reqiured, runs only once, one time setup
void setup() {
  //Acceleormeter 
  Wire.begin();
  Wire.beginTransmission(MPU_addr);
  Wire.write(0x6B);  // PWR_MGMT_1 register
  Wire.write(0);     // set to zero (wakes up the MPU-6050)
  Wire.endTransmission(true);

  // initialize the LED pin as an output:
  pinMode(ledPin, OUTPUT);
  pinMode(ledPin2, OUTPUT);
  
  // initialize the pushbutton pins as an input:
  pinMode(buttonPinA, INPUT);
  pinMode(buttonPinB, INPUT);
  pinMode(buttonPinC, INPUT);

  //serial set to 9600baud rate
  Serial.begin(9600);
  
  //******** Initialize sim808 module *************
  Serial.println(F("Initializing modem..."));
  
  //Initialize modem and search for a network
  modem.init();
  Serial.print(F("Waiting for network..."));
  if (!modem.waitForNetwork()) {
    Serial.println(" fail");
    delay(10000);
    return;
  }
  Serial.println(" OK");

  Serial.print(F("Connecting to "));
  Serial.print(apn);
  if (!modem.gprsConnect(apn, user, pass)) {
    Serial.println(" fail");
    delay(10000);
    return;
  }
  Serial.println(" OK");

  
  //start GPS process
  if (sim808.attachGPS())
  {
    Serial.println("Open the GPS power success");
    delay(3000);
  }
  else
  {
    Serial.println("Open the GPS power failure");
  }
  
  //Attach all Interrupt pins to the correct function 
  attachInterrupt(digitalPinToInterrupt(2), PanicBut, CHANGE);
  attachInterrupt(digitalPinToInterrupt(3), FalseBut, CHANGE);
  attachInterrupt(digitalPinToInterrupt(18), ResetBut, CHANGE);
}

//Function for setting the button states
void PanicBut()
{
  buttonStateA = HIGH;
  buttonStateB = LOW;
  buttonStateC = LOW;
}
void FalseBut()
{
  buttonStateB = HIGH;
  buttonStateA = LOW;
  buttonStateC = LOW;
}
void ResetBut()
{
  buttonStateC = HIGH;
  buttonStateA = LOW;
  buttonStateB = LOW;
}

//function for tracking with GPS , by asking for "Location" via sms
//Also for checking message from 48months saying credit is low
void Message()
{
    //*********** Detecting unread SMS ************************
    messageIndex = sim808.isSMSunread();
    //*********** At least, there is one UNREAD SMS ***********
    if (messageIndex > 0)
    {
     sim808.readSMS(messageIndex, message, MESSAGE_LENGTH, phone, datetime);
     delay(3000);
     //Search for the first specified phone number and message content that has Location in it 
     if((!strcmp(phone,"+086#######"))&&(!strcmp(message,"Location")))
     {
        //If coordinates can be found send message to specified phone number
        //with last know GPS coordinates
        if(lat > 0)
        {
          sim808.sendSMS(PHONE_NUMBER, readings2);
          sim808.deleteSMS(messageIndex);
        }
        //If coordinates can not be found send message to specified phone number
        //saying cannot find GPS coordinates
        else
        {
        sim808.sendSMS(PHONE_NUMBER, MESSAGE8);
        //Delete the message from GEMPA device to save memory
        sim808.deleteSMS(messageIndex);
        }
     }
     
     //Search for the second specified phone number and message content that has Location in it 
     //Process is repeated same as above
     else if((!strcmp(phone,"+086#######"))&&(!strcmp(message,"Location")))
     {
        if(lat > 0)
        {
          sim808.sendSMS(PHONE_NUMBER2, readings2);
          sim808.deleteSMS(messageIndex);
        }
        else
        {
        sim808.sendSMS(PHONE_NUMBER2, MESSAGE8);
        sim808.deleteSMS(messageIndex);
        }
     }
     
     //Search for 48months phone number and upload message to database saying credit is low
     //2 numbers specified here are for testing purposes
     //1745 is the number that will be specified
      else if((!strcmp(phone,"086#######")) || (!strcmp(phone,"086#######")))//(1745)48months
      {
        //Connect to the server
         if (client.connect(server, 80))
        {
          //Upload date, time and Credit balance is getting low to the database if coordinates are found
          if(lat>0){
            client.print("GET /data2.php?"); 
            client.print("date=");
            client.print(sim808.GPSdata.day);
            client.print("/");
            client.print(sim808.GPSdata.month);
            client.print("/");
            client.print(sim808.GPSdata.year);
            client.print("&time=");
            client.print(sim808.GPSdata.hour);
            client.print(":");
            client.print(sim808.GPSdata.minute);
            client.print("&coordinates="); 
            client.print(MESSAGE7);
            client.println(" HTTP/1.1"); // Part of the GET request
            client.println("Host: gempapanicalarm.000webhostapp.com"); 
            client.println("Connection: close"); // Part of the GET request telling the server that we are over transmitting the message
            client.println(); // Empty line     
            client.println(); // Empty line
            delay(10000); 
            client.stop();    
          }
        else{
          //Upload N/A for date and time and Credit balance is getting low to the database if coordinates are not found
          client.print("GET /data2.php?");
          client.print("date=");
          client.print("N/A");
          client.print("&time=");
          client.print("N/A");
          client.print("&coordinates="); 
          client.print(MESSAGE7);
          client.println(" HTTP/1.1"); // Part of the GET request
          client.println("Host: gempapanicalarm.000webhostapp.com"); 
          client.println("Connection: close"); // Part of the GET request telling the server that we are over transmitting the message
          client.println(); // Empty line     
          client.println(); // Empty line
          delay(10000); 
          //Stop the client connection
          client.stop();    
          sim808.deleteSMS(messageIndex);
        }
        }
        else 
        {
          // If Arduino can't connect to the server (your computer or web page)
          Serial.println("--> connection failed\n");
        }
          }
      } 
}

void loop() {
    //Get the latitude and longitude from the GPS 
    sim808.getGPS();
    lat = sim808.GPSdata.lat; lon = sim808.GPSdata.lon;                                        // get two gps coordinates

    //If GPS coordinates have been found
    if(lat > 0)
    {
      // convert two GPS coordinates to char value from 7 digits and 6 digits after decimal point
      dtostrf(lat, 7, 6, charlat); dtostrf(lon, 7, 6, charlon);
      //add message content and the link to google maps in the readings array 
      strcat(readings, "Panic Button Activated, Please Help Me!!! \n \nClick on the link for my GPS Coordinates: \n \nhttps://www.google.ie/maps/place/");
      //add the converted latitude to the array 
      strcat (readings, charlat);  
      //coma is to make GPS coordinates open in google maps
      strcat (readings, ",");
      //add the converted longitude to the array 
      strcat (readings, charlon);
      strcat(readings2, "My last known Coordinates are: \n \nhttps://www.google.ie/maps/place/");
      strcat (readings2, charlat);  
      strcat (readings2, ","); 
      strcat (readings2, charlon);
      sim808.detachGPS();
            
      // Connect to the server (your computer or web page)
      if (client.connect(server, 80))
      {
        client.print("GET /data2.php?"); 
        client.print("date=");
        client.print(sim808.GPSdata.day);
        client.print("/");
        client.print(sim808.GPSdata.month);
        client.print("/");
        client.print(sim808.GPSdata.year);
        client.print("&time=");
        client.print(sim808.GPSdata.hour);
        client.print(":");
        client.print(sim808.GPSdata.minute);
        client.print("&coordinates="); 
        client.print(charlat);
        client.print(",");
        client.print(charlon);
        client.println(" HTTP/1.1"); // Part of the GET request
        client.println("Host: gempapanicalarm.000webhostapp.com"); 
        client.println("Connection: close"); // Part of the GET request telling the server that we are over transmitting the message
        client.println(); // Empty line     
        client.println(); // Empty line
        delay(10000);
        client.stop();
        Message();
       }
       else 
      {
      // If Arduino can't connect to the server (your computer or web page)
      Serial.println("--> connection failed\n");
      } 
    }

    //If GPS coordinates have not been found
    else
    { 
        // Connect to the server (your computer or web page)
        if (client.connect(server, 80))
        {
          client.print("GET /data2.php?"); // This
          client.print("date=");
          client.print("N/A");
          client.print("&time=");
          client.print("N/A");
          client.print("&coordinates="); // This
          client.print(MESSAGE3);
          client.println(" HTTP/1.1"); // Part of the GET request
          client.println("Host: gempapanicalarm.000webhostapp.com"); 
          client.println("Connection: close"); // Part of the GET request telling the server that we are over transmitting the message
          client.println(); // Empty line     
          client.println(); // Empty line
          delay(10000);  
          client.stop();
        }
        else 
        {
          // If Arduino can't connect to the server (your computer or web page)
          Serial.println("--> connection failed\n");
        }
      }

  //Call the message function
  Message();

  Wire.beginTransmission(MPU_addr);
  Wire.write(0x3B);  // starting with register 0x3B (ACCEL_XOUT_H)
  Wire.endTransmission(false);
  Wire.requestFrom(MPU_addr, 14, true); // request a total of 14 registers
  AcX = Wire.read() << 8 | Wire.read(); // 0x3B (ACCEL_XOUT_H) & 0x3C (ACCEL_XOUT_L)

  //Panic button activated and Accelerometer value is below 5000
  if ((buttonStateA == ACTIVATED) || (AcX < 5000))
  {
    Serial.println("Accelerometer value below 5000 detected or button has been pressed:");
    Serial.print("AcX = "); Serial.print(AcX);
    delay(3000);
    
    //******** define phone number and text to send **********
    //If GPS coordinates have been found send the data from readings array 
    if ((lat > 0) && (buttonStateA == ACTIVATED) || (lat > 0) && (AcX < 5000))
    {
      sim808.sendSMS(PHONE_NUMBER,readings);
      //Second responders phone number
      //sim808.sendSMS(PHONE_NUMBER2, readings);
    }
    //If GPS coordinates have been found send Message
    else
    {
      sim808.sendSMS(PHONE_NUMBER,MESSAGE);
      //Second responders phone number
      //sim808.sendSMS(PHONE_NUMBER2, MESSAGE);
    }
    delay(3000);
    if (client.connect(server, 80))
      {
        if(lat > 0){
          client.print("GET /data2.php?"); // This
          client.print("date=");
          client.print(sim808.GPSdata.day);
          client.print("/");
          client.print(sim808.GPSdata.month);
          client.print("/");
          client.print(sim808.GPSdata.year);
          client.print("&time=");
          client.print(sim808.GPSdata.hour);
          client.print(":");
          client.print(sim808.GPSdata.minute);
          client.print("&coordinates="); // This
          client.print(MESSAGE4);
          client.println(" HTTP/1.1"); // Part of the GET request
          client.println("Host: gempapanicalarm.000webhostapp.com"); 
          client.println("Connection: close"); // Part of the GET request telling the server that we are over transmitting the message
          client.println(); // Empty line     
          client.println(); // Empty line
          delay(10000); 
          client.stop();    
        }
      else{
        client.print("GET /data2.php?"); // This
        client.print("date=");
        client.print("N/A");
        client.print("&time=");
        client.print("N/A");
        client.print("&coordinates="); // This
        client.print(MESSAGE4);
        client.println(" HTTP/1.1"); // Part of the GET request
        client.println("Host: gempapanicalarm.000webhostapp.com"); 
        client.println("Connection: close"); // Part of the GET request telling the server that we are over transmitting the message
        client.println(); // Empty line     
        client.println(); // Empty line
        delay(10000); 
        client.stop();    
      }
      }
      else 
      {
        // If Arduino can't connect to the server (your computer or web page)
        Serial.println("--> connection failed\n");
      }

    do {
      Serial.println("Waiting for message to be sent to me");

      //*********** Detecting unread SMS ************************
      messageIndex = sim808.isSMSunread();
      
      //*********** At least, there is one UNREAD SMS ***********
      if (messageIndex > 0)
      {
        //Stop the counting loop read message details
        count1 = 7;
        sim808.readSMS(messageIndex, message, MESSAGE_LENGTH, phone, datetime);
        //***********In order not to full SIM Memory, is better to delete it**********
        sim808.deleteSMS(messageIndex);
        Serial.print("From number: ");
        Serial.println(phone);
        Serial.print("Datetime: ");
        Serial.println(datetime);
        Serial.print("Recieved Message: ");
        Serial.println(message);

        //Play message recieved tone and flash LEDS
         for (int i = 0; i < 3; i++)
        {
          digitalWrite(ledPin, HIGH);
          delay(300);
          digitalWrite(ledPin, LOW);
          delay(300);
          tone(8, 1250, 500);
          delay(1500); 
        }
      }

      //If message has not been recieved after 3 counts
      //Send panic message to second responder
      else
      {
        count1++;
        if (count1 == 3)
      {
        if ((lat > 0) && (buttonStateA == ACTIVATED) || (lat > 0) && (AcX < 5000))
      {
        sim808.sendSMS(PHONE_NUMBER2,readings);
        //sim808.sendSMS(PHONE_NUMBER, readings
      }
      else
      {
        sim808.sendSMS(PHONE_NUMBER,MESSAGE);
        //sim808.sendSMS(PHONE_NUMBER2, readings
      }
      }

      //If message has not been recieved after 6 counts
      //Send panic message to first responder again
      else if (count1 == 6)
      {
        if ((lat > 0) && (buttonStateA == ACTIVATED) || (lat > 0) && (AcX < 5000))
      {
        //sim808.sendSMS(PHONE_NUMBER,readings);
        sim808.sendSMS(PHONE_NUMBER2, readings);
        //Start counting from 0 again and keep loop until message has been recieved
        count1 = 0;
      }
        else
        {
          sim808.sendSMS(PHONE_NUMBER, MESSAGE);
          //sim808.sendSMS(PHONE_NUMBER2, readings
        }
      }
      }

      //Play emergency tone and flash LEDS
      for (int i = 0; i < 5; i++)
      {
        tone(8, 1250, 500);
        delay(100);
        digitalWrite(ledPin2, HIGH);
        delay(100);
        digitalWrite(ledPin2, LOW);
        delay(100);
        tone(8, 950, 500);
        delay(500);
        digitalWrite(ledPin, HIGH);
        delay(100);
        tone(8, 1250, 500);
        delay(100);
        digitalWrite(ledPin, LOW);
        delay(100);
      }

      //False alarm button activated
      if (buttonStateB == ACTIVATED)
      {
        // Connect to the server (your computer or web page)
        if (client.connect(server, 80))
        {
          if(lat > 0){
            client.print("GET /data2.php?"); // This
            client.print("date=");
            client.print(sim808.GPSdata.day);
            client.print("/");
            client.print(sim808.GPSdata.month);
            client.print("/");
            client.print(sim808.GPSdata.year);
            client.print("&time=");
            client.print(sim808.GPSdata.hour);
            client.print(":");
            client.print(sim808.GPSdata.minute);
            client.print("&coordinates="); // This
            client.print(MESSAGE5);
            client.println(" HTTP/1.1"); // Part of the GET request
            client.println("Host: gempapanicalarm.000webhostapp.com"); 
            client.println("Connection: close"); // Part of the GET request telling the server that we are over transmitting the message
            client.println(); // Empty line     
            client.println(); // Empty line  
            delay(10000);
            client.stop();
            count1 = 0;
          }
          else{
            client.print("GET /data2.php?"); // This
            client.print("date=");
            client.print("N/A");
            client.print("&time=");
            client.print("N/A");
            client.print("&coordinates="); // This
            client.print(MESSAGE5);
            client.println(" HTTP/1.1"); // Part of the GET request
            client.println("Host: gempapanicalarm.000webhostapp.com"); 
            client.println("Connection: close"); // Part of the GET request telling the server that we are over transmitting the message
            client.println(); // Empty line     
            client.println(); // Empty line
            delay(10000);
            client.stop();
            count1 = 0;
          }
        }
      else 
      {
      // If Arduino can't connect to the server (your computer or web page)
      Serial.println("--> connection failed\n");
      }

        //Send false alarm message to first responder
        sim808.sendSMS(PHONE_NUMBER, MESSAGE2);
        //sim808.sendSMS(PHONE_NUMBER2,MESSAGE2);
        
        //Reset the button states
        buttonStateB = LOW;
        buttonStateA = LOW;
        buttonStateC = LOW;
        //Go back to the start of programme 
        break;
      }

      //Reset button activated
      if (buttonStateC == ACTIVATED)
      {
        Serial.println("RESET");
        delay(3000);
        //Reset button states
        buttonStateC = LOW;
        buttonStateA = LOW;
        buttonStateB = LOW;
        
        if (client.connect(server, 80))
        {
          if(lat > 0){
            client.print("GET /data2.php?"); // This
            client.print("date=");
            client.print(sim808.GPSdata.day);
            client.print("/");
            client.print(sim808.GPSdata.month);
            client.print("/");
            client.print(sim808.GPSdata.year);
            client.print("&time=");
            client.print(sim808.GPSdata.hour);
            client.print(":");
            client.print(sim808.GPSdata.minute);
            client.print("&coordinates="); // This
            client.print(MESSAGE6);
            client.println(" HTTP/1.1"); // Part of the GET request
            client.println("Host: gempapanicalarm.000webhostapp.com"); 
            client.println("Connection: close"); // Part of the GET request telling the server that we are over transmitting the message
            client.println(); // Empty line     
            client.println(); // Empty line
            delay(10000); 
            client.stop(); 
            count1 = 0;   
          }
          else{
            client.print("GET /data2.php?"); // This
            client.print("date=");
            client.print("N/A");
            client.print("&time=");
            client.print("N/A");
            client.print("&coordinates="); // This
            client.print(MESSAGE6);
            client.println(" HTTP/1.1"); // Part of the GET request
            client.println("Host: gempapanicalarm.000webhostapp.com"); 
            client.println("Connection: close"); // Part of the GET request telling the server that we are over transmitting the message
            client.println(); // Empty line     
            client.println(); // Empty line
            delay(10000); 
            client.stop();
            count1 = 0;
          }
        }
        else 
        {
          // If Arduino can't connect to the server (your computer or web page)
          Serial.println("--> connection failed\n");
        }
        //Go back to the start of programme 
        break;
      }
    } while ((buttonStateA == ACTIVATED) || (AcX < 5000));
}
}
