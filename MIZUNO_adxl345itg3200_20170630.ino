/*
  IoT Golf Club Swing Analyzer
  Greg Brown June 30, 2017 - RUNNING
  PTC Japan
  
  Adafruit Feather Huzzah ESP8266
  Using Sparkfun SEN-10121 (integrated ITG3200 Gyro / ADXL345 Accelerometer)
  4 pixel Neopixel display
*/

#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <Wire.h>
#include <ADXL345.h> // 3-axis accelerometer
//#include "I2Cdev.h"
#include "ITG3200.h" // 3-axis gyro

#include <NeoPixelBus.h>
const uint16_t PixelCount = 4; // 
const uint8_t PixelPin = 2;  // make sure to set this to the correct pin but ***ignored for Esp8266***
#define colorSaturation 32 // set very low to save power!!! (0 - 255) 

#define I2CSDA    2
#define I2CSCL    14

#define LED1 12
#define LED2 13
#define LED3 15
#define LED4 4


NeoPixelBus<NeoGrbFeature, Neo800KbpsMethod> strip(PixelCount, PixelPin);
RgbColor red(colorSaturation, 0, 0);
RgbColor green(0, colorSaturation, 0);
RgbColor blue(0, 0, colorSaturation);
RgbColor white(colorSaturation);
RgbColor black(0);
HslColor hslRed(red);
HslColor hslGreen(green);
HslColor hslBlue(blue);
HslColor hslWhite(white);
HslColor hslBlack(black);


ADXL345 accelerometer;
ITG3200 gyro;
HTTPClient http;

// PRIVATE ROUTER SETUP
const char* ssid = "CREOIOT2017";
const char* password = "CREOIOT2017";
const String rest_url = "http://192.168.1.50:8080/Thingworx/Things/GregThing/Properties/*";

// THINGWORKS LOGIN
const char* username = "Administrator";
const char* user_password = "admin";

String msg = "TBD";

////////////////////////
// UTILITY FUNCTIONS
////////////////////////

void connect_to_thingworx()
{  
    http.setAuthorization (username, user_password);
    Serial.print("[HTTP] begin...\n");
    http.begin(rest_url);
    http.addHeader("Content-Type", "application/json");
    http.addHeader("appKey", "a576a83f-ada6-424e-aaa2-3cd67966dee8");   //For GregThing
    http.addHeader("Cache-Control", "no-cache");
    http.addHeader("x-thingworx-session", "true");
    strip.SetPixelColor(1, green);  //GREEN LIGHT
    strip.Show(); 
    for(int i=0;i<4;i++) {
      digitalWrite(LED1,LOW);
      delay(250); 
      digitalWrite(LED1,HIGH);
      delay(250); 
    }
}

void lightsOut() {
  for(int i=0;i<PixelCount;i++){
    strip.SetPixelColor(i, black);
  }   
  strip.Show(); // turn them all off at once  
}


void welcomeShow() {
  lightsOut();
  for(int i=0;i<PixelCount;i++){
    strip.SetPixelColor(i, green);
    if (i>1) {
      strip.SetPixelColor(i-1, black);
     }
    strip.Show(); // turn on one by one
    delay(100); 
  }
  lightsOut();
}


void sliceShow() {
  lightsOut();
  for(int i=0;i<2;i++){
    strip.SetPixelColor(i, blue);
  }
  for(int i=0;i<2;i++) {
    digitalWrite(LED1,LOW);
    delay(400); 
    digitalWrite(LED1,HIGH);
    delay(400);     
  }
  strip.Show(); 
  delay(500); 
  lightsOut();
}

void hookShow() {
  lightsOut();
  for(int i=2;i<PixelCount;i++){
    strip.SetPixelColor(i, red);
  }
  for(int i=0;i<4;i++) {
    digitalWrite(LED1,LOW);
    delay(200); 
    digitalWrite(LED1,HIGH);
    delay(200);     
  }
  strip.Show(); 
  delay(500); 

  lightsOut(); 
}

void fadeShow() {
  lightsOut();   
  strip.SetPixelColor(0, blue);
  strip.SetPixelColor(2, green);
  strip.Show(); 
  delay(500); 
  lightsOut(); 
}

void greatShow() {
  lightsOut(); 
  for(int i=0;i<PixelCount;i++){
    strip.SetPixelColor(i, white); //all White
  }
    for(int i=0;i<8;i++) {
    digitalWrite(LED1,LOW);
    delay(100); 
    digitalWrite(LED1,HIGH);
    delay(100);     
    }
  strip.Show(); 
  delay(500); 
  lightsOut(); 
}


void checkSetup()
{
  Serial.print("Detecting tap on axis = "); 
  if (accelerometer.getTapDetectionX()) { Serial.print(" X "); }
  if (accelerometer.getTapDetectionY()) { Serial.print(" Y "); }
  if (accelerometer.getTapDetectionZ()) { Serial.print(" Z "); }
  Serial.println();

  Serial.print("Tap Threshold = "); Serial.println(accelerometer.getTapThreshold());
  Serial.print("Tap Duration = "); Serial.println(accelerometer.getTapDuration());
}


////////////////////////////////////////////////////////////////////////
// NORMAL PROGRAMMING RESUMES NOW
////////////////////////////////////////////////////////////////////////

void setup(void) 
{
  //Wire.begin(I2CSDA, I2CSCL); // for Switchscience ESPr device !!!!WATCH OUT FOR THIS!!!!
  Wire.begin();                 // for Adafruit Feather Huzzah device 
  Serial.begin(9600);
  
  strip.Begin();
  strip.Show();

  pinMode(LED1, OUTPUT);
  pinMode(LED2, OUTPUT);
  pinMode(LED3, OUTPUT);
  pinMode(LED4, OUTPUT);  

  welcomeShow();

  // Connect to WiFi network
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    lightsOut();
    delay(250);   
    digitalWrite(LED2,LOW);  
    strip.SetPixelColor(0, blue);
    strip.Show(); 
    delay(250);
    digitalWrite(LED2,HIGH);
    Serial.print(".");
  }
  
  Serial.println("");
  Serial.println("WiFi connected!");
  lightsOut();     
  strip.SetPixelColor(0, green);
  strip.Show(); 
  digitalWrite(LED2,LOW);
  // Echo the Thing's IP address
  Serial.println(WiFi.localIP());

  // initialize Gyro 
  Serial.println("Initializing/Calibrating Gyro...");
  gyro.init();
  gyro.zeroCalibrate(200,10);//sample 200 times to calibrate and it will take 200*10ms

  // Initialize ADXL345
  Serial.println("Initializing ADXL345 accelerometer...");

  if (!accelerometer.begin())
  {
    lightsOut();
    strip.SetPixelColor(3,red);  
    strip.Show(); 
    Serial.println("Could not find a valid ADXL345 sensor, check the wiring!");
    delay(500);
  }

  accelerometer.setRange(ADXL345_RANGE_16G);
  accelerometer.setDataRate(ADXL345_DATARATE_3200HZ); 
  accelerometer.setTapDetectionX(1); 
//  accelerometer.setTapDetectionXYZ(1);   // OR THIS?  
  accelerometer.setTapThreshold(16.0);   // IS THIS in UNITS OF G or M/s2?
  accelerometer.setTapDuration(0.01);   // THIS in mS??

  // Select INT 1 for get activities
  accelerometer.useInterrupt(ADXL345_INT1);

  // Check settings on Accelerometer
  checkSetup();

  // Set up connection to Thingworx server
  connect_to_thingworx();
}

//#########################################################
void loop(void) 
{
  lightsOut();
  digitalWrite(LED2,HIGH);     
  delay(500); // take a breath
  strip.SetPixelColor(0,green);
  strip.Show();
  digitalWrite(LED2,LOW);     

  
  Activites activ = accelerometer.readActivites();

  if (activ.isTap)
  {
    Vector norm = accelerometer.readNormalize();
    
    Serial.print("Hit detected! ");
    strip.SetPixelColor(0,blue);
    strip.Show();
    
    float gx,gy,gz;
    gyro.getAngularVelocity(&gx,&gy,&gz);
  
    if ((abs(gx)<50.0)&&(abs(gy)<50.0)&&(abs(gz)<50.0)) { 
      Serial.println("GREAT SHOT!");
       msg="Great Shot!";
       greatShow(); // light it up!!
      } 
    else if (gz>50.0) {
      Serial.println("OOPS... Slice :(");
       msg="Slice";
       sliceShow();
      } 
    else if (gz<-50.0){
      Serial.println("OOPS... Hook :(");
      msg="Hook";
      hookShow();
      }
     else {
      Serial.println("Fade");
      msg="Fade";
      fadeShow();
     }
   //ALL the DATA is PUT!
    String put_json   = "{\"Value1\":\""+(String)(gx)
                      +"\",\"Value2\":\""+(String)(gy)
                      +"\",\"Value3\":\""+(String)(gz)
                      +"\",\"Value4\":\""+msg
                      +"\",\"AX\":\""+(String)(norm.XAxis)  
                      +"\",\"AY\":\""+(String)(norm.YAxis)  
                      +"\",\"AZ\":\""+(String)(norm.ZAxis)                                                             
                      +"\"}";   
    Serial.println(put_json);                                      
    lightsOut();
    digitalWrite(LED2,HIGH); // turn off the blue led while uploading to server

    int httpCode = http.PUT(put_json);             
    // httpCode will return a negative value on error
    if(httpCode > 0) {
        // file found at server
        if(httpCode == HTTP_CODE_OK) {
          String payload = http.getString();
          strip.SetPixelColor(0, green); 
          strip.Show(); // This sends the updated pixel color to the hardware.  
          digitalWrite(LED2,LOW);   // turn LED back on! All ok...       
        }
    } else {
        Serial.printf("[HTTP] PUT... failed, error: %s\n", http.errorToString(httpCode).c_str());
        strip.SetPixelColor(0, red); 
        strip.Show(); 
        delay(500); // take a breath
    }
  }
}
