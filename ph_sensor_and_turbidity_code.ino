#define BLYNK_PRINT Serial
#include <WiFi.h>
#include <WiFiClient.h>
#include <BlynkSimpleEsp32.h>
#include <LiquidCrystal_I2C.h>

#define SensorPin 34          // the pH meter Analog output is connected with the Arduino’s Analog
unsigned long int avgValue;  //Store the average value of the sensor feedback
 
int buf[10],temp;
 char auth[] = "bXNMUTvSEZ_rKIELlu3XljA1VacE4jQ0";
int tarb=35;
char ssid[] = "door";
char pass[] = "12345678";

 
float volt;
float ntu;
 
void setup()
{
  Serial.begin(115200);
  Serial.println("Connecting to ");
  Serial.println(ssid);
  WiFi.begin(ssid, pass);
  while (WiFi.status() != WL_CONNECTED) 
     {
            delay(500);
            Serial.print(".");
     }   
      Serial.println("");
      Serial.println("WiFi connected");
        Blynk.begin(auth, ssid, pass);
  pinMode(13,OUTPUT);  
    pinMode(tarb,INPUT);  
  Serial.begin(9600);  
  Serial.println("Ready");    //Test the serial monitor
}
void loop()
{
  Blynk.run();
tarbidity();
  
  for(int i=0;i<10;i++)       //Get 10 sample value from the sensor for smooth the value
  { 
    buf[i]=analogRead(SensorPin);
    delay(10);
  }
  for(int i=0;i<9;i++)        //sort the analog from small to large
  {
    for(int j=i+1;j<10;j++)
    {
      if(buf[i]>buf[j])
      {
        temp=buf[i];
        buf[i]=buf[j];
        buf[j]=temp;
      }
    }
  }
  avgValue=0;
  for(int i=2;i<8;i++)                      //take the average value of 6 center sample
    avgValue+=buf[i];
  float phValue=(float)avgValue*5.0/1024/6; //convert the analog into millivolt
  phValue-=5;
  //float PhValue=3.5*phValue;                      //convert the millivolt into pH value
 Blynk.virtualWrite(V1,phValue);
  Serial.print("    pH:");  
  
   Serial.print(phValue,2);
 if(phValue>6.5 && phValue<8.00 )
 {
   Blynk.virtualWrite(V3,"drinkable water");
  }
  else
  {
       Blynk.virtualWrite(V3,"not drinkable water");
    }
  Serial.println(" ");
  digitalWrite(13, HIGH);       
  delay(800);
  digitalWrite(13, LOW); 
 
}

void tarbidity() 
{
 int sensorValue = analogRead(tarb);
 Serial.println(sensorValue);
  
  volt = 0;
    for(int i=0; i<800; i++)
    {
        volt += ((float)analogRead(tarb)/1023)*5;
    }
    volt = volt/800;
    volt = round_to_dp(volt,2);
    volt=volt*2.41;
    if(volt < 2.5)
    {
     ntu = 3000;
    }
    else
   {
     ntu = -1120.4*20+5742.3*volt-4353.8; 
   }
  ntu = 1120.4*20+5742.3*volt-4353.8;
  Blynk.virtualWrite(V2,volt);
 
    delay(10);  
}

float round_to_dp( float in_value, int decimal_place )
{
  float multiplier = powf( 10.0f, decimal_place );
  in_value = roundf( in_value * multiplier ) / multiplier;
  return in_value;
}
