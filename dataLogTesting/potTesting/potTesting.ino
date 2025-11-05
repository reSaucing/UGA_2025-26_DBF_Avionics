const int Pin=20;
int potVal=0; 
float angle=0;
void setup() {
  Serial.begin(9600);  
}
void loop () {
  potVal = analogRead(Pin);
  float rangeFixed = (161+431)-potVal;//
  angle=(270.0/(862-1.0))*rangeFixed;
  Serial.println(angle);//potVal);//angle);
}