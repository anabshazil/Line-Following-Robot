#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#include <SPI.h>
#include <SD.h>


File myFile;     //MAKING OBJECT FOR SD CARD

#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels

// Declaration for an SSD1306 display connected to I2C (SDA, SCL pins)
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);

int IR[5]={49,47,45,43,41};   //IR array sensor pins
int sensor[5]={};                     //IR sensor data storing array 

const int trigPin = 3;            // For ultrasonic sensor 
const int echoPin = 2;
long duration;
int distance;

#define ANALOG_IN_PIN_V A7   //Pin for voltage sensor
#define ANALOG_IN_PIN_C A6   //Pin for current sensor

float adc_voltage = 0.0;   // Floats for ADC voltage & Input voltage
float in_voltage = 0.0;
float R1 = 30000.0;   // Floats for resistor values in divider (in ohms)
float R2 = 7500.0;  
float ref_voltage = 5.0;   // Float for Reference Voltage
int adc_value = 0;    // Integer for ADC value

// Define color sensor pins
#define S0 33
#define S1 35
#define S2 37
#define S3 39
#define sensorOut 31

// Define motor pins
int motor1A = 48;
int motor1B = 22;
int motor2A = 46;
int motor2B = 24;

// Define enable pins
int enA = 25;
int enB = 7;


// Calibration Values
int redMin = 93; // Red minimum value
int redMax = 192; // Red maximum value
int greenMin = 107; // Green minimum value
int greenMax = 255; // Green maximum value
int blueMin = 87; // Blue minimum value
int blueMax = 209; // Blue maximum value
                                              
int redPW = 0;       // Variables for Color Pulse Width Measurements
int greenPW = 0;
int bluePW = 0;

int redValue;       // Variables for final Color values
int greenValue;
int blueValue;
//------------------------------------------------------------------------------------------------------------------------------------------------


void setup() {
  // put your setup code here, to run once:
  
  if(!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) { 
    Serial.println(F("SSD1306 allocation failed"));
    for(;;);
  }
  display.clearDisplay();

  pinMode(trigPin, OUTPUT); // Sets the trigPin as an Output
  // Set color sensor pins as outputs
  pinMode(S0, OUTPUT);
  pinMode(S1, OUTPUT);
  pinMode(S2, OUTPUT);
  pinMode(S3, OUTPUT);
  // Set motor pins as outputs
  pinMode(motor1A, OUTPUT);
  pinMode(motor1B, OUTPUT);
  pinMode(motor2A, OUTPUT);
  pinMode(motor2B, OUTPUT);

  // Set enable pins as outputs
  pinMode(enA, OUTPUT);
  pinMode(enB, OUTPUT);

  
  pinMode(sensorOut, INPUT);   // Set Color Sensor output as input
  pinMode(echoPin, INPUT);    // Sets the echoPin as an Input
 
  digitalWrite(S0,HIGH);    // Set Frequency scaling to 20% for color sensor 
  digitalWrite(S1,LOW);
  
  // Setup Serial Monitor
  Serial.begin(9600);

  for(int i=0;i<=4;i++)        
  pinMode(IR[i],INPUT);      //decalaring ir sensor pins as inputs 

  SD.begin(53);

}
//------------------------------------------------------------------------------------------------------------------------------------------------



void loop() {
  // put your main code here, to run repeatedly:
   myFile = SD.open("full.txt", FILE_WRITE);
  // write to it:
  myFile.println("--------------------------------------------------");
  int dis = ULTRA();
  float Amp=Current();
  float bat=Vol();

  Serial.println(Amp);
  Serial.println(bat);
  Serial.println(dis); 
  
   if(dis<=7)
  {
  stop();  
  colr();
  }

  for(int i=0;i<=4;i++)
  {
  sensor[i] = digitalRead(IR[i]);  //Storing data in of array 
  Serial.print(sensor[i]);
  myFile.print(sensor[i]);

  }
  myFile.println("_-");
  
if( ( sensor[0]==1 && sensor[1]==1 && sensor[2]==0 && sensor[3]==0 && sensor[4]==0 ) || (sensor[0]==1 && sensor[1]==1 && sensor[2]==1 && sensor[3]==0 && sensor[4]==0  ) || (sensor[0]==1 && sensor[1]==1 && sensor[2]==1 && sensor[3]==1 && sensor[4]==0  )||(sensor[0]==1 && sensor[1]==1 && sensor[2]==1 && sensor[3]==0 && sensor[4]==1  ))
{
  digitalWrite(motor1A,HIGH);
  digitalWrite(motor2A,LOW);   //RIGHT
  digitalWrite(motor1B,LOW);
  digitalWrite(motor2B,LOW);
  myFile.println("Turn Data = Right");
}
if((sensor[0]==0 && sensor[1]==0 && sensor[2]==0 && sensor[3]==1 && sensor[4]==1 )||(sensor[0]==0 && sensor[1]==0 && sensor[2]==1 && sensor[3]==1 && sensor[4]==1  )||(sensor[0]==0 && sensor[1]==1 && sensor[2]==1 && sensor[3]==1 && sensor[4]==1  )||(sensor[0]==1 && sensor[1]==0 && sensor[2]==1 && sensor[3]==1 && sensor[4]==1  ))
{
  digitalWrite(motor1A,LOW);
  digitalWrite(motor2A,LOW);   //LEFT
  digitalWrite(motor1B,HIGH);
  digitalWrite(motor2B,LOW); 
  myFile.println("Turn Data = Left");
}
if((sensor[0]==1 && sensor[1]==0 && sensor[2]==0 && sensor[3]==0 && sensor[4]==1 )||(sensor[0]==1 && sensor[1]==1 && sensor[2]==0 && sensor[3]==1 && sensor[4]==1  )||(sensor[0]==1 && sensor[1]==1 && sensor[2]==1 && sensor[3]==1 && sensor[4]==1))
{
  digitalWrite(motor1A,HIGH);
  digitalWrite(motor2A,LOW);   //FORWARD
  digitalWrite(motor1B,HIGH);
  digitalWrite(motor2B,LOW);
  myFile.println("Turn Data = Forward");
}
if((sensor[0]==0 && sensor[1]==0 && sensor[2]==0 && sensor[3]==0 && sensor[4]==0))
{
  digitalWrite(motor1A,LOW);
  digitalWrite(motor2A,LOW);   //STOP
  digitalWrite(motor1B,LOW);
  digitalWrite(motor2B,LOW);
  myFile.println("Turn Data = Stop");  
}
   myFile.println("Total Current = ");
   myFile.print(Amp);
   
 myFile.println("Total Voltage = ");
 myFile.print(bat);


  myFile.println("No Obstacle");
  myFile.println("----------------------------------------------");
  myFile.close();

  display.clearDisplay();

  //Voltage sensor 
  display.setCursor(0, 15);  
  display.setTextSize(1);
  display.setTextColor(WHITE);
  display.print("Voltage = ");
  display.print(bat); 
  display.print(" V");
  display.display();

  //Current sensor 
  display.setCursor(0, 25);  
  display.setTextSize(1);
  display.setTextColor(WHITE);
  display.print("Current = ");
  display.print(Amp); 
  display.print(" A");
  display.display();

  //Obstacle
  display.setCursor(0, 35);  
  display.setTextSize(1);
  display.setTextColor(WHITE);
  display.print("Anab-Abdullah");
  display.display(); 



  Serial.println("done.");


}
//-------------------------------------------------------------------------------------------------------------
int ULTRA()                      
{
 digitalWrite(trigPin, LOW);
 delayMicroseconds(2);
  // Sets the trigPin on HIGH state for 10 micro seconds
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);
  // Reads the echoPin, returns the sound wave travel time in microseconds
  duration = pulseIn(echoPin, HIGH);
  // Calculating the distance
  distance = duration * 0.034 / 2;
  // Prints the distance on the Serial Monitor
  return distance;
}
//-------------------------------------------------------------------------------------------------------------
float Current()
{
  int adc = analogRead(ANALOG_IN_PIN_C);
  float voltage = adc * 5 / 1023.0;
  float current = (voltage - 2.5) / 0.100;
  //if (current < 0.16) 
  //current = 0;
  return current;
   
}
//-------------------------------------------------------------------------------------------------------------
float Vol()
{
   // Read the Analog Input
   adc_value = analogRead(ANALOG_IN_PIN_V);
   
   // Determine voltage at ADC input
   adc_voltage  = (adc_value * ref_voltage) / 1024.0; 
   
   // Calculate voltage at divider input
   in_voltage = adc_voltage / (R2/(R1+R2)) ; 
   return in_voltage; 

}


void colr()
{
  for(int i=0;i<=5;i++)
 
 {
  // Read Red value
   redPW = getRedPW();
  // Map to value from 0-255
  redValue = map(redPW, redMin,redMax,255,0);
  // Delay to stabilize sensor
  delay(200);
  
  // Read Green value
  greenPW = getGreenPW();
  // Map to value from 0-255
  greenValue = map(greenPW, greenMin,greenMax,255,0);
  // Delay to stabilize sensor
  delay(200);
  
  // Read Blue value
  bluePW = getBluePW();
  // Map to value from 0-255
  blueValue = map(bluePW, blueMin,blueMax,255,0);
  // Delay to stabilize sensor
  delay(200);
  
  // Print output to Serial Monitor
  Serial.print("Red = ");
  Serial.print(redValue);
  Serial.print(" - Green = ");
  Serial.print(greenValue);
  Serial.print(" - Blue = ");
  Serial.println(blueValue);

 if(redValue <= 50 && greenValue <=50 && blueValue <=50 && redValue >= 10 && greenValue >=10 && blueValue >=10  ) 
  {
  myFile.println("Black Obstacle");
  myFile.println("Stopped!");
  myFile.println("----------------------------------------------");
  myFile.close();

  display.clearDisplay();
  //Black obstacle
  display.setCursor(27, 10);  
  display.setTextSize(1);
  display.setTextColor(WHITE);
  display.print("Black Obstacle");
  display.setCursor(40, 25);  
  display.setTextSize(1);
  display.setTextColor(WHITE);
  display.println("Stopped!");
  display.display();

   
    while(1)
    { stop(); } 
  }

 if(redValue>greenValue && redValue > blueValue && greenValue<=163 && greenValue>=76&& blueValue<=155 && blueValue>=59)
  {
  myFile.println("Red Obstacle");
  myFile.println("Returning to start");
  display.clearDisplay();
  //Red obstacle
  display.setCursor(27, 10);  
  display.setTextSize(1);
  display.setTextColor(WHITE);
  display.print("Red Obstacle");
  display.setCursor(10, 25);  
  display.setTextSize(1.5);
  display.setTextColor(WHITE);
  display.println("Returning to Start");
  display.display();
    red();
  }
} 
}
//-------------------------------------------------------------------------------------------------------------  
int getRedPW() {                            // Function to read Read Pulse Widths
  // Set sensor to read Red only
  digitalWrite(S2,LOW);
  digitalWrite(S3,LOW);
  // Define integer to represent Pulse Width
  int PW;
  // Read the output Pulse Width
  PW = pulseIn(sensorOut, LOW);
  // Return the value
  return PW;
 
}
//------------------------------------------------------------------------------------------------------------- 
int getGreenPW() {                        // Function to read Green Pulse Widths
  // Set sensor to read Green only
  digitalWrite(S2,HIGH);
  digitalWrite(S3,HIGH);
  // Define integer to represent Pulse Width
  int PW;
  // Read the output Pulse Width
  PW = pulseIn(sensorOut, LOW);
  // Return the value
  return PW;
 
}
//------------------------------------------------------------------------------------------------------------- 
int getBluePW() {                      // Function to read Blue Pulse Widths
  // Set sensor to read Blue only
  digitalWrite(S2,LOW);
  digitalWrite(S3,HIGH);
  // Define integer to represent Pulse Width
  int PW;
  // Read the output Pulse Width
  PW = pulseIn(sensorOut, LOW);
  // Return the value
  return PW;
 
}
//-------------------------------------------------------------------------------------------------------------
void stop()
{
  digitalWrite(motor1A,LOW);
  digitalWrite(motor2A,LOW);   //STOP
  digitalWrite(motor1B,LOW);
  digitalWrite(motor2B,LOW);  
  
}
//-------------------------------------------------------------------------------------------------------------
void red()
{

  // need work
while(1){
  myFile.println("Reached!");
  myFile.println("----------------------------------------------");
  myFile.close();

  display.clearDisplay();
  //Reached at starting point
  display.setCursor(40, 25);  
  display.setTextSize(1);
  display.setTextColor(WHITE);
  display.println("Reached!");
  display.display();
}
}
//-------------------------------------------------------------------------------------------------------------