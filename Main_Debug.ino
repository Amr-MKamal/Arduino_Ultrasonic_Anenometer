#include <Arduino.h>
#include <SoftwareSerial.h>

#include "DHT.h"
#include "Wind_auto.h"
#define DHTPIN 53
#define mutual 0
#define soil_analog A0
#define soil_max 550
#define soil_min 10
//SoftwareSerial mySerial(10, 5); // RX, TX is not needed as ultrasonic triggers itself
#define DHTTYPE DHT21   // DHT 21 (AM2301) // Connect pin 1 (on the left) of the sensor to +5V // NOTE: If using a board with 3.3V logic like an Arduino Due connect pin 1 // to 3.3V instead of 5V! // Connect pin 2 of the sensor to whatever your DHTPIN is
// Connect pin 4 (on the right) of the sensor to GROUN // Connect a 10K resistor from pin 2 (data) to pin 1 (power) of the sensor // Initialize DHT sensor for normal 16mhz Arduino
#define Cycle_offset 10
extern uint8_t _tempmiss ;
DHT dht(DHTPIN, DHTTYPE);
JSNR04T Ux1(45,10,mutual,2000) ,Ux2(37,13,mutual,2000),Uy1(47,50,mutual,2000),Uy2(31,10,mutual,2000) ,Uz1(41,12,mutual,2000),Uz2(39,11,mutual,2000);
Wind WindX(&Ux1,&Ux2), WindY(&Uy1,&Uy2) ,WindZ(&Uz1,&Uz2);
float CycTemp , CycHum ,_CSS=340 , winspeed;
int  ultradis ;
int soilM;
uint8_t allmiss;
//SoftwareSerial mySerial(10,5);
void setup() {
  Serial.begin(9600);
  set_windsensor(dht);
  dht.begin();
  Set_nowinddistance();
  Test_allsensors();
}

void loop() {
   //if(dht.working)
   //doesn't update before 2 seconds

//   delay(10);
//  ultradis=Uz1.get_selfecho();
//  winspeed=Uz1.transform_read(ultradis);
//  //Uz2.disable();
//  Serial.print("ultrasonic z1 is ");Serial.println(ultradis);
// Serial.print("ultrasonic z1 windspeed  "); Serial.println(winspeed);
//  ultradis=Uz2.get_selfecho();
//  winspeed=Uz2.transform_read(ultradis);
//  Serial.print("ultrasonic z2 is ");Serial.println(ultradis);
//  //Uz1.disable();
//  Serial.print("averge is");Serial.println(winspeed);
//      delay(1000);
// winspeed=WindZ.get_avergewind();
// Serial.print("averge wind is ");Serial.println(winspeed);
 wind_cycle(&WindX ,&WindY, &WindZ);
allmiss=(WindX._selfmiss+WindY._selfmiss+WindZ._selfmiss)*selfecho_timeout;
allmiss+=(WindX._aheadmiss+WindY._aheadmiss+WindZ._aheadmiss)*headecho_timeout;
//allmiss+= _tempmiss;
////write debugging tools
////// if global windZ=global_windY=global_windX; it's static wind
////// repeat the loop and measure difference between readings
//
////_tempmiss=0;
//

  soilM= analogRead(soil_analog);
  soilM = map(soilM,1000,0,0,1000);
Serial.print("{ \"Temp\":");Serial.print("\"");Serial.println(CycTemp);Serial.print("\"");Serial.print(" , ");
Serial.print("\"Humidity\":");Serial.print("\"");Serial.println(CycHum);Serial.print("\"");Serial.print(" , ");
Serial.print("\"Soil_Mositure\":");Serial.print("\"");Serial.println(soilM);Serial.print("\"");Serial.print(" , ");
//  Serial.print("Wind_Speed ");Serial.print(WindX.avgwind);Serial.print(" X,");Serial.print(WindY.avgwind);Serial.print(" Y,");Serial.print(WindZ.avgwind);Serial.print(" Z");
//  Serial.print("Wind_frequency ");Serial.print(WindX.gust);Serial.print(" X/10,");Serial.print(WindY.gust);Serial.print(" Y/10,");Serial.print(WindZ.gust);Serial.print(" Z/10");
//  Serial.print("Minmimum Wind_Speed ");Serial.print(WindX.minwind);Serial.print(" X,");Serial.print(WindY.minwind);Serial.print(" Y,");Serial.print(WindZ.minwind);Serial.print(" Z");
//  Serial.print("Maximum Wind_Speed ");Serial.print(WindX.maxwind);Serial.print(" X,");Serial.print(WindY.maxwind);Serial.print(" Y,");Serial.print(WindZ.maxwind);Serial.print(" Z");
//  Serial.print("Cycle Overshoot");Serial.print(allmiss);
  Serial.print("\"Wind_Speed_x\": ");Serial.print("\"");Serial.print(WindX.avgwind);Serial.print("\"");Serial.print(" , ");
  Serial.print("\"Wind_Speed_y\": ");Serial.print("\"");Serial.print(WindY.avgwind);Serial.print("\"");Serial.print(" , ");
  Serial.print("\"Wind_Speed_z\":");Serial.print("\"");Serial.print(WindZ.avgwind);Serial.print("\"");Serial.print(" , ");
  Serial.print("\"Maximum_Speed_x\": ");Serial.print("\"");Serial.print(WindX.maxwind);Serial.print("\"");Serial.print(" , ");
  Serial.print("\"Maximum_Speed_y\": ");Serial.print("\"");Serial.print(WindY.maxwind);Serial.print("\"");Serial.print(" , ");
  Serial.print("\"Maximum_Speed_z\": ");Serial.print("\"");Serial.print(WindZ.maxwind);Serial.print("\"");Serial.print(" , ");
  Serial.print("\"Minmimum_Speed_x\": ");Serial.print("\"");Serial.print(WindX.minwind);Serial.print("\"");Serial.print(" , ");
  Serial.print("\"Minmimum_Speed_y\": ");Serial.print("\"");Serial.print(WindY.minwind);Serial.print("\"");Serial.print(" , ");
  Serial.print("\"Minmimum_Speed_z\" :");Serial.print("\"");Serial.print(WindZ.minwind);Serial.print("\"");Serial.print(" , ");
  Serial.print("\"Wind_frequency_x\": ");Serial.print("\"");Serial.print(WindX.gust);Serial.print("\"");Serial.print(" , ");
  Serial.print("\"Wind_frequency_y\" :");Serial.print("\"");Serial.print(WindY.gust);Serial.print("\"");Serial.print(" , ");
  Serial.print("\"Wind_frequency_z \":");Serial.print("\"");Serial.print(WindZ.gust);Serial.print("\"");Serial.print(" , ");
  Serial.print("\"Cycle Overshoot\":");Serial.print("\"");Serial.print(allmiss);Serial.print("\"");Serial.print(" }");
WindX.avgwind=0;
delay(90000);
}
//int readSerial() {
//  byte serialData[5];
//  byte serial_sum;
//  int discalc , d ;
//
//  while (mySerial.available()) {
//    byte b = mySerial.read();
//  }
//  mySerial.setTimeout(100);
//  if ( mySerial.readBytes(serialData, 4) > 0) {
//    serial_sum=serialData[0]+serialData[1]+serialData[2];
//    if(serialData[3]==serial_sum){
//    discalc = ((serialData[1] * 256) + serialData[2]);
//    discalc = (discalc * 1);  //distance is returned from sensor in mm - this will convert to inches.
//    }else {discalc=-1;}
//  }
//
//  return discalc;
//}
void Set_nowinddistance(){
    Ux1.nowind_distance=nowind_distancex1;
    Ux2.nowind_distance=nowind_distancex2;
    Uy1.nowind_distance=nowind_distancey1;
    Uy2.nowind_distance=nowind_distancey2;
    Uz1.nowind_distance=nowind_distancez1;
    Uz2.nowind_distance=nowind_distancez2;
}
void Test_allsensors(void){
  //Serial.print("sensors status are");
  renew_temp_hu(dht,&CycTemp,&CycHum); //doesn't update before 2 seconds
  if(isnan(CycTemp)){
    //Serial.print("AM2301 is not working , please recheck connection /n");
    dht.working=0;}
  else {dht.working=true; 
  //Serial.print("Temp:");Serial.print(CycTemp);Serial.println(" C");
  }
  ultradis=Ux1.get_selfecho();delay(50);
  ultradis+=Ux1.get_selfecho();delay(50);
  ultradis=Ux1.get_selfecho();
  Ux1.disable();
  if(ultradis){Ux1.working=true;
  //Serial.print("ultrasonic x1 is working with min_dis= ");Serial.println(ultradis);
  }
  else {Ux1.working=false;
  //Serial.print("ultrasonic x1 is not working and has been ignored");
  }
  ultradis=0;ultradis=Ux2.get_selfecho();delay(50);
  ultradis+=Ux2.get_selfecho();delay(50);
  ultradis+=Ux2.get_selfecho();ultradis=ultradis/3;Ux2.disable();
  if(ultradis){Ux2.working=true;
  //Serial.print("ultrasonic x2 is working with min_dis= ");Serial.println(ultradis);
  }
  else {Ux2.working=false;
  //Serial.print("ultrasonic x2 is not working and has been ignored");
  }
   ultradis=0; ultradis=Uy1.get_selfecho();delay(50);
  ultradis+=Uy1.get_selfecho();delay(50);
  ultradis+=Uy1.get_selfecho();ultradis=ultradis/3;Uy1.disable();
  if(ultradis){Uy1.working=true;
  //Serial.print("ultrasonic y1 is working with min_dis= ");Serial.println(ultradis);
  }
  else {Uy1.working=false;
  //Serial.print("ultrasonic y1 is not working and has been ignored");
  }
  ultradis=0;
  ultradis=Uy2.get_selfecho();delay(50);
  ultradis+=Uy2.get_selfecho();delay(50);
  ultradis+=Uy2.get_selfecho();ultradis=ultradis/3;Uy2.disable();
  if(ultradis){Uy2.working=true;
  //Serial.print("ultrasonic y2 is working with min_dis= ");Serial.println(ultradis);
  }
  else {Uy2.working=false;
  //Serial.print("ultrasonic y2 is not working and has been ignored");
  }
   ultradis=0; ultradis=Uz1.get_selfecho();delay(50);
  ultradis+=Uz1.get_selfecho();delay(50);
  ultradis+=Uz1.get_selfecho();ultradis=ultradis/3;Uz1.disable();
  if(ultradis){Uz1.working=true;
  //Serial.print("ultrasonic z1 is working with min_dis= ");Serial.println(ultradis);
  }
  else {Uz1.working=false;
  //Serial.print("ultrasonic z1 is not working and has been ignored");
  }
    ultradis=0;ultradis=Uz2.get_selfecho();delay(50);
  ultradis+=Uz2.get_selfecho();delay(50);
  ultradis+=Uz2.get_selfecho();ultradis=ultradis/3;Uz2.disable();
  if(ultradis){Uz2.working=true;
  //Serial.print("ultrasonic x2 is working with min_dis= ");Serial.println(ultradis);
  }
  else {Uz2.working=false;
  //Serial.print("ultrasonic x2 is not working and has been ignored");
  }
   
}
void disable_all(){
  Ux1.disable();Ux2.disable();
  Uy1.disable();Uy2.disable();
  Uz1.disable();Uz2.disable();
}
