void setup(){
  Serial.begin(115200);
}

void loop(){
  float sensorVolt = 0;
  float RS_air = 0;
  float R0 = 0;
  float sensorValue = 0;

  for(int i = 0; i < 100; i++){
    sensorValue += analogRead(35);
  }
  
  sensorValue /= 100.0;

  sensorVolt = sensorValue/1024*5.0;
  RS_air = (5.0 - sensorVolt)/sensorVolt;
  // Aprox 3.8 para elmq135 obtenido de la curva de aire del datasheet
  // Aprox 4.4 para elmq4 obtenido de la curva de aire del datasheet
  R0 = RS_air/4.4;

  Serial.print("Sensor volt: ");
  Serial.print(sensorVolt);
  Serial.println("V");

  Serial.print("R0: ");
  Serial.println(R0);
  delay(1000);

}