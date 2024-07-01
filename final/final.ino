#include <AHT10.h>
#include <Wire.h>

/**************************************************************************** 
  Author : Andy @ MYBOTIC www.mybotic.com.my
  Date:5/7/2016 
  Modify by: Ing. Jonathan Flores @ Asdrome
  Date: 08/03/24
****************************************************************************/

/************************AHT10 instance************************************/
AHT10 myAHT10(0x38);

/************************Hardware Related Macros************************************/

const int calibrationLed = 13;                      //when the calibration start , LED pin 13 will light up , off when finish calibrating
const int MQ135_PIN = 34;                                //define which analog input channel you are going to use
const int MQ4_PIN = 35;                                //define which analog input channel you are going to use
int RL_VALUE = 5;                                     //define the load resistance on the board, in kilo ohms
float RO_MQ135_CLEAN_AIR_FACTOR = 3.8;                     //RO_CLEAR_AIR_FACTOR=(Sensor resistance in clean air)/RO,
float RO_MQ4_CLEAN_AIR_FACTOR = 4.4;                     //RO_CLEAR_AIR_FACTOR=(Sensor resistance in clean air)/RO,
                                                    //which is derived from the chart in datasheet
 
/***********************Software Related Macros************************************/
int CALIBARAION_SAMPLE_TIMES = 50;                    //define how many samples you are going to take in the calibration phase
int CALIBRATION_SAMPLE_INTERVAL = 500;                //define the time interal(in milisecond) between each samples in the
                                                    //cablibration phase
int READ_SAMPLE_INTERVAL = 50;                        //define how many samples you are going to take in normal operation
int READ_SAMPLE_TIMES = 5;                            //define the time interal(in milisecond) between each samples in 
                                                    //normal operation
 
/**********************Application Related Macros**********************************/
/*********************MQ135*********************/
#define         GAS_NH3               0   
#define         GAS_CO2               1   

/*********************MQ4*********************/
#define         GAS_CH4               2    
 
/*****************************Globals***********************************************/
/*********************MQ135*********************/
float           NH3Curve[3]  =  {1,2.6,-1.23};   //two points are taken from the curve. 
                                                    //with these two points, a line is formed which is "approximately equivalent"
                                                    //to the original curve. 
                                                    //data format:{ x, y, slope}; point1: (lg10, 2.6), point2: (lg200, 1) 
float           CO2Curve[3]  =  {1,2.4,-1};    //two points are taken from the curve. 
                                                    //with these two points, a line is formed which is "approximately equivalent" 
                                                    //to the original curve.
                                                    //data format:{ x, y, slope}; point1: (lg10, 2.4), point2: (lg200, 1.1)) 

/*********************MQ4*********************/
float           CH4Curve[3] ={2.3,1.8,-0.80};    //two points are taken from the curve. 
                                                    //with these two points, a line is formed which is "approximately equivalent" 
                                                    //to the original curve.
                                                    //data format:{ x, y, slope}; point1: (lg200, 1.8), point2: (lg10000,  0.44)                                                     
float           Ro_MQ135         =  10;                 //Ro is initialized to 10 kilo ohms
float           Ro_MQ4           =  10;                 //Ro is initialized to 10 kilo ohms

float           temp = 0;
float           hum = 0;
void setup()
{ 
  Wire.begin();
  Serial.begin(9600);
  // Si la comunicación del sensor falla se imprime el mensaje de error
  if(!myAHT10.begin()) {
    Serial.println("Error de conexión");
    while(1);
  }


  pinMode(calibrationLed,  OUTPUT);
  digitalWrite(calibrationLed, HIGH);
  Serial.print("Calibrating...");


  Ro_MQ135 = MQCalibration(MQ135_PIN, RO_MQ135_CLEAN_AIR_FACTOR);                         //Calibrating the sensor. Please make sure the sensor is in clean air         
  Ro_MQ4 = MQCalibration(MQ4_PIN, RO_MQ4_CLEAN_AIR_FACTOR);                         //Calibrating the sensor. Please make sure the sensor is in clean air         
  digitalWrite(calibrationLed, LOW);

  Serial.println("done!");
  Serial.print("R0_MQ135: ");
  Serial.println(Ro_MQ135);
  
  Serial.print("R0_MQ135: ");
  Serial.println(Ro_MQ4);

  delay(1000);
}
 
void loop()
{  
  long iPPM_NH3 = 0;
  long iPPM_CO2 = 0;
  long iPPM_CH4 = 0;

  iPPM_NH3 = MQGetGasPercentage(MQRead(MQ135_PIN)/Ro_MQ135,GAS_NH3);
  iPPM_CO2 = MQGetGasPercentage(MQRead(MQ135_PIN)/Ro_MQ135,GAS_CO2);
  iPPM_CH4 = MQGetGasPercentage(MQRead(MQ4_PIN)/Ro_MQ4,GAS_CH4);

  temp = myAHT10.readTemperature();
  hum = myAHT10.readHumidity();
  Serial.print("Temp: "); Serial.print(temp); Serial.print(" °C");
  Serial.print("\t\t");
  Serial.print("Humedad: "); Serial.print(hum); Serial.println(" %");

  
  Serial.println("Concentration of gas \n");
  
  Serial.print("NH3: ");
  Serial.print(iPPM_NH3);
  Serial.println(" ppm");   
  
  Serial.print("CO2: ");
  Serial.print(iPPM_CO2);
  Serial.println(" ppm");    
  
  Serial.print("CH4: ");
  Serial.print(iPPM_CH4);
  Serial.println(" ppm");    
  delay(15 * 60 * 1000);
  
}
 
/****************** MQResistanceCalculation ****************************************
Input:   raw_adc - raw value read from adc, which represents the voltage
Output:  the calculated sensor resistance
Remarks: The sensor and the load resistor forms a voltage divider. Given the voltage
         across the load resistor and its resistance, the resistance of the sensor
         could be derived.
************************************************************************************/ 
float MQResistanceCalculation(int raw_adc)
{
  return ( ((float)RL_VALUE*(1023-raw_adc)/raw_adc));
}
 
/***************************** MQCalibration ****************************************
Input:   mq_pin - analog channel
Output:  Ro of the sensor
Remarks: This function assumes that the sensor is in clean air. It use  
         MQResistanceCalculation to calculates the sensor resistance in clean air 
         and then divides it with RO_CLEAN_AIR_FACTOR. RO_CLEAN_AIR_FACTOR is about 
         10, which differs slightly between different sensors.
************************************************************************************/ 
float MQCalibration(int mq_pin, float Ro)
{
  int i;
  float val=0;

  for (i=0;i<CALIBARAION_SAMPLE_TIMES;i++) {            //take multiple samples
    val += MQResistanceCalculation(analogRead(mq_pin));
    delay(CALIBRATION_SAMPLE_INTERVAL);
  }
  val = val/CALIBARAION_SAMPLE_TIMES;                   //calculate the average value
  val = val/Ro;                        //divided by RO_CLEAN_AIR_FACTOR yields the Ro                                        
  return val;                                                      //according to the chart in the datasheet 

}
 
/*****************************  MQRead *********************************************
Input:   mq_pin - analog channel
Output:  Rs of the sensor
Remarks: This function use MQResistanceCalculation to caculate the sensor resistenc (Rs).
         The Rs changes as the sensor is in the different consentration of the target
         gas. The sample times and the time interval between samples could be configured
         by changing the definition of the macros.
************************************************************************************/ 
float MQRead(int mq_pin)
{
  int i;
  float rs=0;
 
  for (i=0;i<READ_SAMPLE_TIMES;i++) {
    rs += MQResistanceCalculation(analogRead(mq_pin));
    delay(READ_SAMPLE_INTERVAL);
  }
 
  rs = rs/READ_SAMPLE_TIMES;
 
  return rs;  
}
 
/*****************************  MQGetGasPercentage **********************************
Input:   rs_ro_ratio - Rs divided by Ro
         gas_id      - target gas type
Output:  ppm of the target gas
Remarks: This function passes different curves to the MQGetPercentage function which 
         calculates the ppm (parts per million) of the target gas.
************************************************************************************/ 
long MQGetGasPercentage(float rs_ro_ratio, int gas_id)
{
  if ( gas_id == GAS_NH3 ) {
     return MQGetPercentage(rs_ro_ratio,NH3Curve);
  } else if ( gas_id == GAS_CO2 ) {
     return MQGetPercentage(rs_ro_ratio,CO2Curve);
  } else if ( gas_id == GAS_CH4 ) {
     return MQGetPercentage(rs_ro_ratio,CH4Curve);
  } 

  return 0;
}
 
/*****************************  MQGetPercentage **********************************
Input:   rs_ro_ratio - Rs divided by Ro
         pcurve      - pointer to the curve of the target gas
Output:  ppm of the target gas
Remarks: By using the slope and a point of the line. The x(logarithmic value of ppm) 
         of the line could be derived if y(rs_ro_ratio) is provided. As it is a 
         logarithmic coordinate, power of 10 is used to convert the result to non-logarithmic 
         value.
************************************************************************************/ 
long  MQGetPercentage(float rs_ro_ratio, float *pcurve)
{
  return (pow(10,( ((log(rs_ro_ratio)-pcurve[1])/pcurve[2]) + pcurve[0])));
}
