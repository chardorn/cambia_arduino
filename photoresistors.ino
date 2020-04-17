
const int sensorPin = 0;
const int sensorPin2 = 5;


int lightCal;
int lightVal;
int lightCal2;
int lightVal2;

void setup() {
 Serial.begin(9600);
 lightCal = analogRead(sensorPin);
 lightCal2 = analogRead(sensorPin2);

 Serial.println("Calibration: " + String(lightCal));
 Serial.println("Calibration: " + String(lightCal2));

 
}

void loop() {
  lightVal = analogRead(sensorPin);
  lightVal2 = analogRead(sensorPin2);

  


  int numSamples = 0;
  int totalVal1 = 0;
  int totalVal2 = 0;
  int averageVal1 = 0;
  int averageVal2 = 0;

  for(int i; i < 100; i = i + 1){
    numSamples += 1;
    totalVal1 += lightVal;
    totalVal2 += lightVal2;
    delay(50);
  }

  averageVal1 = totalVal1 / numSamples;
  averageVal2 = totalVal2 / numSamples;

  Serial.print(averageVal1);
  Serial.print(" -- ");
  Serial.println(averageVal2);
  

  // put your main code here, to run repeatedly:


}
