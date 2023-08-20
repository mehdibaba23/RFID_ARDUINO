#include <SPI.h>
#include <RFID.h>


RFID rfid(10, 9);  //D10:pin of tag reader SDA. D9:pin of tag reader RST

unsigned char str[MAX_LEN];  //MAX_LEN is 16: size of the array

String accessGranted[2] = { "15353441511" };  //RFID serial numbers to grant access to
int accessGrantedSize = 2;                    //The number of serial numbers if u want to add a 2 serial


int redlight = 2;
int greenlight = 5;
int bluelight = 6;

void color(int redvalue, int bluevalue, int greenvalue) {
  digitalWrite(redlight, redvalue);
  digitalWrite(bluelight, bluevalue);
  digitalWrite(greenlight, greenvalue);
}

void setup() {
  Serial.begin(9600);         //Serial monitor is only required to get tag ID numbers and for troubleshooting
  SPI.begin();                //Start SPI communication with reader
  rfid.init();                //initialization
  pinMode(redlight, OUTPUT);  //LED startup sequence
  pinMode(greenlight, OUTPUT);
  pinMode(bluelight, OUTPUT);

  color(255, 0, 255);
  delay(1000);
  color(255, 255, 255);
  Serial.println("Place card/tag near reader...");
}

void loop() {
  if (rfid.findCard(PICC_REQIDL, str) == MI_OK)  //Wait for a tag to be placed near the reader
  {
    Serial.println("Card found");
    String temp = "";                 //Temporary variable to store the read RFID number
    if (rfid.anticoll(str) == MI_OK)  //Anti-collision detection, read tag serial number
    {
      Serial.print("The card's ID number is : ");
      for (int i = 0; i < 4; i++)  //Record and display the tag serial number
      {
        temp = temp + (0x0F & (str[i] >> 4));
        temp = temp + (0x0F & str[i]);
      }
      Serial.println(temp);
      checkAccess(temp);  //Check if the identified tag is an allowed to open tag
    }
    rfid.selectTag(str);  //Lock card to prevent a redundant read, removing the line will make the sketch read cards continually
  }
  rfid.halt();
}

void checkAccess(String temp)  //Function to check if an identified tag is registered to allow access
{
  boolean granted = false;
  for (int i = 0; i <= (accessGrantedSize - 1); i++)  //Runs through all tag ID numbers registered in the array
  {
    if (accessGranted[i] == temp)  //If a tag is found then open/close the lock
    {
      Serial.println("Access Granted");
      granted = true;
      color(255, 255, 0);  //green color
      delay(1000);
      color(255, 255, 255);
    }
  }
  if (granted == false)  //If the tag is not found
  {
    Serial.println("Access Denied");

    color(0, 255, 255);  //red color
    delay(500);
    color(255, 255, 255);
  }
}