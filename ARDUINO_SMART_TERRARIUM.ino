//Light

#include <Adafruit_NeoPixel.h>
#define PIN 4 
#define NUM_LIGHTS 16
#define LightsensorPin A0 
#define LCDPin 3 

Adafruit_NeoPixel strip = Adafruit_NeoPixel(NUM_LIGHTS,PIN,NEO_GRB+NEO_KHZ800);

//Humidity & Temperature

#include <DHT.h>//Include DHT sensor library
#define DHTPIN 2 //Define what pin the sensors digital output is connected to 
#define DHTTYPE DHT22 //Define type of DHT sensor
DHT dht (DHTPIN, DHTTYPE);

const int Min_Humidity = 95;
const int Max_Humidity = 100;

#define AtomizerTransistorPin 5 
#define MOSFETPin 7
#define PowerHBridgePin 13

const int Min_Temperature = 20;
const int Max_Temperature = 25;

//Soil Moisture

#define WaterPumpRelayPin 8                   //Sets digital pin 33 to the relay module 
#define SoilMoistureSensorPin A1               //Sets Analog pin to the soil moisture sensor

const int IdealDrySoil= 787;                //Sets the upper threshold for ideal soil moisture to 787
const int IdealWetSoil= 675;                 //Sets the lower threshold for ideal soil moisture to 675


void setup() {

  Serial.begin(9600);         //BAUD Rate
  
  // Humidity 
  dht.begin();
  pinMode (5,OUTPUT);//Set atomizer as output pin 5
  digitalWrite(5,LOW);//Start the system with the atomizer turned off

  //Temperature
  pinMode(MOSFETPin, OUTPUT);
  pinMode(11, OUTPUT); //Peltier
  pinMode(9, OUTPUT); //Peltier
  digitalWrite(13, HIGH);//set this pin to HIGH to enable HBridge
  digitalWrite(7, LOW); //Set the MOSFETPin, and therefore the fan, to be off
  digitalWrite (9,LOW);//Set the peltier to start off
  digitalWrite (11,LOW); //Set the peliter to start off
  
  //Light
  pinMode(LCDPin,OUTPUT); 
  pinMode (LightsensorPin,INPUT); 
  strip.begin();
  strip.show(); //Initialize all pixels to off
  
  //Soil Moisture
  pinMode(SoilMoistureSensorPin, INPUT);
  pinMode(WaterPumpRelayPin, OUTPUT);
  digitalWrite(WaterPumpRelayPin, HIGH);

}

void loop() {
 
    //Humidity 

   float h = dht.readHumidity();               //Read humidity
    Serial.print("Humidity: ");                //Set the display of humidity readings
    Serial.print(h); 
    Serial.print("%");  
    Serial.println(" ");                       //Line between values    

  if (h < Min_Humidity) {                   //Set condition for humidity being below the minimum required 
    Serial.println("Humidity too low");
    digitalWrite(5, HIGH);                  //If humidity is below minimum turn atomizer on
    Serial.println("Atomizer on"); 
    Serial.println(" ");                       //Line between values    
    delay(1500);
    digitalWrite (5,LOW);                      // Turn atomizer off
  }
 if (h > Max_Humidity) {                       //Set condition for humidity being below the maximum required  
    Serial.println("Humidity too high");   
    digitalWrite(5,LOW);                       //Make atomizer be off 
 }
 if (Min_Humidity < h > Max_Humidity) {
    digitalWrite (5,LOW);                      //Atomizer off
    digitalWrite (7,LOW);                      //Fan off

  }

    //Temperature

  float temperature = dht.readTemperature();
  Serial.print("Temperature: ");
  Serial.print(temperature);
  Serial.println("ºC");

  if(temperature < Min_Temperature)
{  
  Serial.println ("Too cold");   
  digitalWrite(7, HIGH);              //Turn fan on
  digitalWrite(11, LOW);              //Heats heat sink side
  digitalWrite(9, HIGH);              //Cools 'exposed side'
  Serial.println("Heating");
  Serial.println(" ");                            //Line between values    
}
  if(Min_Temperature< temperature> Max_Temperature)
{
  digitalWrite(7,LOW);                 //Turn fan off
  digitalWrite(11, LOW);              //Peltier off
  digitalWrite(9, LOW);               //Peltier off
}

  if(temperature>Max_Temperature) //+2 because? 
{
  Serial.println ("Too hot");
  digitalWrite(7, HIGH);               //Turn fan on
  digitalWrite(11, HIGH);             //Cools heat sink side
  digitalWrite(9, LOW);               //Heats 'exposed side'
  Serial.println ("Cooling");
  Serial.println(" ");                            //Line between values    

}
    //Light 

  int sensorValue = analogRead(A0);             // read the input from analog pin 0: 
  int Light = analogRead(LightsensorPin);           // defines light as input value

  float voltage = sensorValue*(5.0/1024.0);       // convert sensor value into voltage, (voltage)/(no. of bytes)
  float microamps =(voltage/1000.0)*1000000.0;    // convert value into microamps so Lux value can be read from graph (I=V/R)
  float LUX = (microamps/0.5);                     // converting light current value into illuminance (LUX)

  Serial.print("Light Intensity: ");             // prints prior to sensor value
  Serial.print(LUX);                              // print out the value calculated       
  Serial.print(" lux");                           // prints units of value read
  Serial.println(" ");                            //Line between values                   

  uint32_t low = strip.Color(0, 0, 0);            //set colour of neopixel ring for low
  uint32_t high = strip.Color(255, 0, 255);       //set colour of neopixel ring for high


  if(Light < 256){                                // condition if light value less than 307, (2500 lux)
    Serial.println("Too dim");
    
        for( int i = 0; i<NUM_LIGHTS; i++){
        strip.setPixelColor(i, high);
        strip.show();                             // Turn on Led light source
        }
        Serial.println ("Light on"); 
        Serial.println(" ");                      //Line between values    
       
                               
  }

  
  else if(Light >= 256 && Light <= 921){        // condition if light value in between 307 and 900, lighting is at optimal level (2500<L<10000 lux)
    for( int i = 0; i<NUM_LIGHTS; i++){
        strip.setPixelColor(i, low);
        strip.show();}                          // Turn off Led light source
    digitalWrite(LCDPin, LOW);                  // Turn off LCD shutter
  }

  
  else{                                         // condition if light value above 900, (10,000 lux)
    Serial.println ("Too bright");
    digitalWrite(LCDPin, HIGH);                 // Turn on LCD Shutter
    Serial.println ("LCD On");
    Serial.println(" ");                            //Line between values    
    for( int i = 0; i<NUM_LIGHTS; i++){
        strip.setPixelColor(i, low);
        strip.show();}                          // Turn off Led light source
  }

                                  
      //Soil Moisture 

 float MoistureLevel = analogRead (SoilMoistureSensorPin);       // Reading value from soil moisture sensor

  //Prints the value read by sensor i.e. moisture level, in the serial monitor window
  Serial.print ("Moisture Level: ");
  Serial.print (MoistureLevel);                                    
  Serial.println(" ");                                             //Line between values    


   if (MoistureLevel < IdealWetSoil) {                            // MoistureLevel drops below 675, pump is off, moisture level is too wet
    Serial.println("Too moist (soil)");
    digitalWrite (WaterPumpRelayPin, HIGH);                       // 5V power goes to the relay module breaking the circuit = Water Pump off    
    }
    
    else if (MoistureLevel > IdealDrySoil) {                     // MoistureLevel goes above 787, pump on, moisture level is too dry
      Serial.println ("Too dry (soil)");
      digitalWrite (WaterPumpRelayPin, LOW);                     // 0V power goes to the relay module, thus connecting the circuit = Water Pump on
      Serial.print ("Water Pump On");
      Serial.println(" ");                                        //Line between values    
    }
    
    else if (MoistureLevel < IdealDrySoil) {                     // MoistureLevel drops below 787, pump off, moisture level is within ideal range
      digitalWrite(WaterPumpRelayPin, HIGH);                     // 5V power goes to the relay module, thus breaking the circuit                                    
    }
    
delay (2000);

}
