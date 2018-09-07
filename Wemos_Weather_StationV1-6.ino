/*2017 Wemos-Weatherstation Written by Rusty Cain "Microrustyc" July 22,2017
Parts Wemos D1 Mini, OLED128X64, DHT11or DHT22, BMP180, Relay.
Baud rate 115200 for console Port.
Functions:
SplashDisplay - Displays Main Screen to OLED.
Temp_read - Reads DHT11 Temperture and Humidity and display to OLED and Console.
Relay_on - Turns on LED and Relay for 3 seconds and then Turns off Relay and LED.
test_bmp - Test to see if BMP180 is connected and working.
Read_BMP180 - Reads BMP180 Altitude and Air Pressure and display to OLED and Console
Version 1.5
*/
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#define OLED_RESET 0  // GPIO0
Adafruit_SSD1306 display(OLED_RESET);
//Set Up DHT Sensor - Uncomment whatever type DHT you're using!
// #include <WEMOS_DHT12.h>       //DHT 12 -Wemos
#include <DHT.h>                  //DHT sensor library by Adafruit Verision 1.2.30  
#define DHTPIN D3                 // The digital pin connected to on Standard                                    DHT11 & DHT21
#define DHTTYPE DHT11             // DHT 11 type selected
//#define DHTTYPE DHT22           // DHT 22  (AM2302), AM2321
//#define DHTTYPE DHT21           // DHT 21 (AM2301)
// DHT12 dht12;
// SCL GPIO5 D2
// SDA GPIO4 D1
// DHT12 D4
// DHT11 & DHT 21 D3
DHT dht(DHTPIN, DHTTYPE);         //Set up Pin and type for DHT sensor
                                  // Set up BMP180 
#include <Adafruit_BMP085.h>      //Adafruit BMP085 Library Version 1.0.0 or Adafruti BMP085 Unified Version 1.0.0? 
Adafruit_BMP085 bmp;              // ?????         
const int relayPin = D8;          //The digital Pin that is used to connect the Cooling Relay 
const long interval = 2000;       // pause for two seconds
int a = 0;                        //Global Variable for moving the DHT float f variable to a interger to enable the Relay engaged.
int bmp_ERR =0;

void setup() {
  // setup code, to run once:
  dht.begin();
  pinMode(relayPin, OUTPUT);
  pinMode(LED_BUILTIN, OUTPUT);     // Initialize the LED_BUILTIN pin as an output
  Serial.begin(115200);
  // by default, we'll generate the high voltage from the 3.3v line internally! (neat!)
  display.begin(SSD1306_SWITCHCAPVCC, 0x3C);  // initialize with the I2C addr 0x3C (for the 64x48)
  Serial.println ();
  Serial.println ();
  Serial.println("2017 Wemos-Weatherstation Written by Rusty Cain Microrustyc July 22,2017");
  Serial.println("Baud rate 115200 for console Port. Parts OLED128X64, DHT11, BMP180, Relay");
  Serial.println("Reads DHT11 Temperture and Humidity and display to OLED and Console");
  Serial.println("Reads BMP180 Altitude and Air Pressure and display to OLED and Console");
  Serial.println("Turns on LED and Relay for 3 seconds and then Turns off Relay and LED");
  delay(2000);
  digitalWrite(LED_BUILTIN, LOW);   // Turn the LED on (Note that LOW is the voltage level           
  delay(500);                       // Wait for a second
  //digitalWrite(LED_BUILTIN, HIGH);  // Turn the LED off by making the voltage HIGH
  //delay(500);                       // Wait for two seconds (to demonstrate the active low LED)
  Serial.println ();
}

void loop() {
  SplashDisplay ();
  Temp_read();
  Test_bmp();
  
 }

void SplashDisplay ()
{  
  // text display tests
  
  Serial.println("Printing SplashDisplay to OLED ");
  Serial.println();
  Clear_OLED();
  display.println("* Microrustyc 2017 *");
  display.println("Weather Station V1.1");
  display.println("Baud 115200 ");
  display.println("IP 192.168,1.xxx ");
  display.display();
  delay(2000);
  return;
}
  
 void Temp_read()  
  {
  // Reading temperature or humidity takes about 250 milliseconds!
  // Sensor readings may also be up to 2 seconds 'old' (its a very slow sensor)
  delay(2000);
 // digitalWrite(LED_BUILTIN, LOW);         //LED on during read
  float h = dht.readHumidity();             // Read Humidity 
  float t = dht.readTemperature();          // Read temperature as Celsius (the default)
  float f = dht.readTemperature(true);      // Read temperature as Fahrenheit (isFahrenheit = true)
  if (isnan(h) || isnan(t) || isnan(f)) 
  {   
    Serial.println("Failed to read from DHT sensor!");  // Display Error Message to Console
    Clear_OLED();                            // Display Error Message to OLED
    display.println("Failed DHT sensor!");
    display.display(); 
    delay(3000);
    return; 
   }
  
  float hif = dht.computeHeatIndex(f, h);        // Compute heat index in Fahrenheit (the default)
  float hic = dht.computeHeatIndex(t, h, false); // Compute heat index in Celsius (isFahreheit = false)
  Clear_OLED();
  display.print("Humidity: ");              //Dispaly Temp & Humi to OLED Dispaly
  display.print(h);
  display.println(" %\t");
  display.print("    Temp: ");
  display.print(t);
  display.println(" *C ");
  display.print("    Temp: ");
  display.print(f);
  display.println(" *F ");       //" *F\t"
  display.print("HeatIndx: ");
  display.print(hif);
  display.println(" *F ");
  display.display();                //End Dispaly Temp & Humi to OLED Dispaly
  // Display to Console Port
  Serial.println("DHT Humidity Temperature Sensor Connected!"); 
  Serial.println();
  Serial.print("Humidity:    ");       //Dispaly Temp & Humi to serial port
  Serial.print(h);
  Serial.println(" %\t");
  Serial.print("Temperature: ");
  Serial.print(t);
  Serial.print(" *C ");
  Serial.print(f);
  Serial.println(" *F\t");
  Serial.print("Heat index:  ");
  Serial.print(hic);
  Serial.print(" *C ");
  Serial.print(hif);
  Serial.println(" *F");          //End Dispaly Temp & Humi to serial port
  Serial.println();
  delay(4000);
  a = f;                          // Set up to check Temperature is over 80.
  if (a >= 80) {                  // if over call Relay-on
  Relay_on();                     // Go to Relay-on
  }
  return;                         // Return back
  }

void Test_bmp() {

  if (!bmp.begin()) {
    Serial.println("Could not find BMP180 or BMP085 sensor at 0x77"); // Display Error Message to Console
    Serial.println();
    Clear_OLED();                                                     // Display Error Message to OLED
    display.println("Failed BMT180 sensor!");
    display.display(); 
    delay(8000);
    bmp_ERR = 1;
    return;
  }
  Read_BMP180();
  return;
}
  
 void Read_BMP180()
 {
  float inch;
  float mbar;
  float Alt;
  float Temp1;
  float Press;
  // Display to Console Port
  Serial.println("BMP Senor Connected and Working ");
  Serial.println();
  Serial.print("Temperature = ");
  Serial.print(bmp.readTemperature());
  Serial.println(" °C");
  Serial.print("Pressure = ");
  Serial.print(bmp.readPressure());
  Serial.println(" Pa");
  // Calculate altitude assuming 'standard' barometric
  // pressure of 1013.25 millibar = 101325 Pascal
  Serial.print("Altitude = ");
  Serial.print(bmp.readAltitude());
  Serial.println(" meters");
  Serial.print("Altitude = ");
  Alt = bmp.readAltitude();
  Alt = Alt * 3.2808;
  Serial.print(Alt);
  Serial.println(" Feet");
  Serial.print("Pressure at sea level (calculated) = ");
  Serial.print(bmp.readSealevelPressure());
  Serial.println(" Pa");
  // you can get a more precise measurement of altitude
  // if you know the current sea level pressure which will
  // vary with weather and such. If it is 1015 millibars
  // that is equal to 101500 Pascals.
  Serial.print("Real altitude = ");
  Serial.print(bmp.readAltitude(101500));
  Serial.println(" meters");
  Serial.println();
  // Display to OLED
  Clear_OLED();
  display.print(" Temp:  ");
  Temp1 = bmp.readTemperature();
  display.print(Temp1);
  display.println(" C");
  display.print("Press:  ");
  Press = bmp.readPressure();
  mbar = Press / 100;
  inch = mbar * 0.029530;
  display.print(inch);
  display.println(" Inches");
  // Calculate altitude assuming 'standard' barometric pressure of 1013.25 millibar = 101325 Pascal
  Alt = bmp.readAltitude();
  Alt = Alt * 3.2808;
  display.println("  Altitude  ");
  display.print(Alt);
  display.println(" Ft");
  display.display();
  delay(8000);
  return;
}

void Clear_OLED()      //Clear Dispaly and setup for displaying to OLED 
{
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(WHITE);
  display.setCursor(0,0);
  return;
}

 void Relay_on()
 {  
  Clear_OLED();
  display.setTextSize(2);
  digitalWrite(LED_BUILTIN, HIGH);
  digitalWrite(relayPin, HIGH); // turn on relay with voltage HIGH
  Serial.println("Relay Engaged!");
  display.println("Relay ");
  display.println("Engaged!");
  display.display();
  Clear_OLED();
  dowhile_Temp_F();
  Serial.println("Relay Hold to validate temp has dropped.");
  delay(15000);  // pause
  Clear_OLED();
  display.setTextSize(2);
  digitalWrite(relayPin, LOW);  // turn off relay with voltage LOW
  display.println("Relay ");
  display.println("OFF!");
  display.display();             
  digitalWrite(LED_BUILTIN, LOW);
  Serial.println("Relay OFF!");
  delay(1000);                  // pause
  return;
  }
  
 void dowhile_Temp_F () { 
  do {
      float f = dht.readTemperature(true);      // Read temperature as Fahrenheit (isFahrenheit = true)  
      a = f;
      Serial.print(" Temp:  ");
      Serial.println (f);
      Clear_OLED();                             //  Clear the OLED screen
      display.println(" Relay On  ");
      display.println(" Cooling - ");
      display.print(" Temp:  ");
      display.println(f);
      display.display();
      delay (200); }
      while (a >= 80 );                         //If the tempature is over 80 then loop until it drops.
      Serial.println ("Turning");
      delay (500);
            return;
  }


      
// Check if any reads failed and exit early (to try again).



