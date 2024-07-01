void setup() {
 Serial.begin(9600);

}

void loop() {
  float sensor_volt = 0;
  float RS_gas = 0;
  float ratio = 0;
  int sensorValue = analogRead(A0);
  sensor_volt = (float) sensorValue/1024*5.0;
  RS_gas = (5.0 - sensor_volt)/sensor_volt;

  // Remplazar el valor de R0 para mq135 obtenido en el programa calibracion 
  // Remplazar el valor de R0 para mq4 obtenido en el programa calibracion
  ratio = RS_gas/4.0;

  Serial.print("sensor_volt: ");
  Serial.println(sensor_volt);
  Serial.print("RS ratio: ");
  Serial.println(RS_gas);
  Serial.print("RS/R0: ");
  Serial.println(ratio);

  Serial.print("\n\n");

  delay(1000);

}
