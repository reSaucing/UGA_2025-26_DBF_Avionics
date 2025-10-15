const int potPin = 20;


int potVal = 0;

void setup() {
  Serial.begin(9600);
}

void loop() {
  potVal = analogRead(potPin);

  Serial.print("Pot Val: ");
  Serial.println(potVal);


  delay(100); 
}
