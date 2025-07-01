#include <Adafruit_BMP085.h>
#include <Wire.h>
#include <TinyGPS++.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BNO055.h>
#include <utility/imumaths.h>
#include <SPI.h>
#include <SD.h>
#include <EEPROM.h>
#include <mavlink.h>


#define SD_CS_PIN SS
#define team_no 56727

File telemetryFile;
Adafruit_BMP085 bmp;
Adafruit_BNO055 bno = Adafruit_BNO055(-1, 0x28);
TinyGPSPlus gps;

static const uint32_t gps_baud = 9600;
static const uint32_t xbee_baud = 9600;
static const uint32_t serial_baud = 9600;
static const uint32_t mavlink_baud = 57600;
const int buzzer = 2, ayrilma = 4, acilma = 5; // Pin numbers
unsigned int packet_number=0;

String time_telemetry,telemetry,header;

int eeAddress_1 = 0, eeAddress_2 = 0, eeAddress_3 = 0;

int payload_stage=1, stage_count=0;
String stage_output = "";
float h0=0,altitude_new=0,altitude_old=0,h_diff,time_initial=0,time_last=0,time_difference=0,velocity=0;
boolean firs_data_control = true;

int stage_control = 0;
int ascent_or_descent = 0, descent_count=0, ascent_count=0;

float temperature=0,pressure=0;
float pitch=0,roll=0,yaw=0;
int gpsSatellites=0, gpsYear=0, gpsMonth=0, gpsDay=0, gpsHour=0, gpsMinute=0, gpsSecond=0, gpsSats=0;
float gpsLatitude, gpsLongitude=0, gpsAltitude=0;

float old_yaw = 0, yaw_difference = 0;
boolean direction_control, direction_control2, single_input = true;
int revolution = 0, number_control = 1;

int analog_reading;
float battery_voltage;

boolean seperation_control1 = true, seperation_control2 = false, seperation_control3 = false, seperation_control4 = false;

String gcs_data_reading="";
String video_transmission ="Hayir";

int interval = 1000;  // ms
unsigned long last_trans,last_trans_seperation,last_trans_final;


void setup() {
  Serial.begin(serial_baud);
  Serial1.begin(xbee_baud); //RX TX 1 UART CONNECTİON FOR XBEE
  Serial2.begin(gps_baud);   //RX TX 2 UART CONNECTİON FOR GPS
  Serial3.begin(mavlink_baud);
  
  bmp.begin();
  bno.begin();
  
  SD.begin(SD_CS_PIN);
  
  pinMode(ayrilma,OUTPUT);
  pinMode(acilma,OUTPUT);
  pinMode(buzzer,OUTPUT);
  
  digitalWrite(ayrilma,LOW);
  digitalWrite(acilma,LOW);
  digitalWrite(buzzer,LOW);

 request_datastream();
 
 header = String("TAKIM NO") + ',' + String("PAKET NUMARASI") + ',' + String("GONDERME SAATI")+ ',' + String("BASINC") + ',' + String("YUKSEKLIK") + ',' + \
              String("INIS HIZI") + ',' + String("SICAKLIK") + ',' + String("PIL GERILIMI")+ ',' + String("GPS LATITUDE") + ',' + String("GPS LONGITUDE") + ',' + \
              String("GPS ALTITUDE") + ',' + String("UYDU STATUSU") + ',' + String("PITCH") + ',' + String("ROLL") + ',' + String("YAW") + ',' + \
              String("DONUS SAYISI") + ',' + String("VIDEO AKTARIM BILGISI");
 telemetryFile = SD.open("TMUY2021_56727_TLM.csv", FILE_WRITE);
 
 telemetryFile.println(header);
 telemetryFile.close();
     ////////////////////////////// READ LAST DATA //////////////////////////////
  
  EEPROM.get(eeAddress_1,packet_number);         // Read last packet number
  eeAddress_2 = eeAddress_1 + sizeof(float);
  EEPROM.get(eeAddress_2,altitude_new);             // Read last altitude
  eeAddress_3 = eeAddress_2 + sizeof(int);
  EEPROM.get(eeAddress_3,payload_stage); // Read last stage
  
}

void loop() {
    
  time_telemetry ="";
  telemetry = "";
 
  ///////////////////BMP180///////////////////
  
  temperature=bmp.readTemperature();
  pressure=bmp.readPressure();
    
    ////////////////////////////// BNO55 //////////////////////////////
  imu::Vector<3> euler = bno.getVector(Adafruit_BNO055::VECTOR_EULER);
  pitch = euler.y();
  roll =  euler.z();
  yaw =   euler.x();
  
  
  ////////////////////////////// ADAFRUIT GPS //////////////////////////////
  while (Serial2.available() > 0){
  gps.encode(Serial2.read());
  
  gpsYear = gps.date.year();
  gpsMonth = gps.date.month();
  gpsDay = gps.date.day();
  gpsHour = gps.time.hour()+3;
  gpsMinute = gps.time.minute();
  gpsSecond = gps.time.second();
    
  gpsLatitude = gps.location.lat();
  gpsLongitude = gps.location.lng();
  gpsAltitude = gps.altitude.meters();
  }
 
  ////////////////////////////// İNİŞ HIZI HESABI //////////////////////////////
  MavLink_receive();
  
  ////////////////////////////// DÖNÜŞ SAYISI HESABI //////////////////////////////
  yaw_difference = yaw - old_yaw;
  old_yaw = yaw;
  
    if(yaw > 176 && yaw < 184 && single_input){
    if((number_control%2)){
      if(yaw_difference > 0){
        direction_control = true;
      }
      else{
        direction_control = false;
      }
    }
    else{
      if(yaw_difference > 0){
        direction_control2 = true;
      }
      else{
        direction_control2 = false;
      }
    }
    if((direction_control && direction_control2) || !(direction_control || direction_control2) ){
      revolution++;
    }
    number_control++;
single_input = false;
  }
  if(yaw < 176 || yaw > 184){
    single_input = true;
  }


  
  ////////////////////////////// CİRCUİT BATTERY VOLTAGE (TEENSY 4.0 ANALOG PİN) //////////////////////////////
  analog_reading = analogRead(A0);
  battery_voltage = -(0.002209*(analog_reading)*(analog_reading)) + 0.4888*(analog_reading) - 22.95;

  
  ////////////////////////////// SEPERATION //////////////////////////////
  
    if(payload_stage == 5 && seperation_control1){
  seperation:  
  Serial.println("ayrilma");
    digitalWrite(ayrilma,HIGH);
    seperation_control1 = false;
    seperation_control2 = true;
    last_trans_seperation = millis();
    }
  
  if (millis()>last_trans_seperation+interval && seperation_control2){
  Serial.println("ayrilma bitti");
  digitalWrite(ayrilma,LOW);
  seperation_control2 = false;
  seperation_control3 = true;
  last_trans_seperation = millis();
  }
  

 ///////// AÇILMA /////////

   if (millis()>last_trans_seperation+200 && seperation_control3){
   Serial.println("acilma");
   digitalWrite(acilma,HIGH);
   seperation_control3 = false;
   seperation_control4 = true;
   last_trans_seperation = millis();
   
  }
   if (millis()>last_trans_seperation+interval && seperation_control4){
    Serial.println("acilma bitti");
   digitalWrite(acilma,LOW);
   seperation_control4 = false;
  }
  ///////////////////////////// GCS_DATA_READİNG /////////////////////////////////////////
   if(Serial1.available()){
    gcs_data_reading = Serial1.read();
   } 
    
  ////////////////////////////// VIDEO TRANSMISSION CONTROL //////////////////////////////
  if(gcs_data_reading == "e"){
    video_transmission = "Evet";
    gcs_data_reading = "";
  }

  ////////////////////////////// SEPERATION WITH TELECOMMAND /////////////////////////////
   if(gcs_data_reading == "a"){
    gcs_data_reading = "";
    goto seperation;
  }
     ////////////////////////////////////////////// FLIGHT SOFTWARE STATES //////////////////////////////////////////////
   // 1-Bekleme 2-Yukselme 3-Zirve 4-Model Uydu Inis 5- Ayrilma 6-Gorev Yuku Inis 7-Irtifa Sabitleme 8-Kurtarma
   
  h_diff = altitude_new - altitude_old;
  
  if(h_diff >= 3){               
    ascent_count = ascent_count + 1;
  }else{
    ascent_count = 0;
  }
  if(h_diff <= -3){              
    descent_count = descent_count + 1;
  }else{
    descent_count = 0;
  }
  if(ascent_count >= 2){          // if ascending counter number is 2 or more
    ascent_or_descent = 1;        // ascent
  }else if(descent_count >= 2){   // if descending counter number is 2 or more
    ascent_or_descent = 2;        // descent
  }else{
    ascent_or_descent = 0;        // hover
  }   
  
  if(altitude_new < 10 && stage_control == 0 && ascent_or_descent == 0 ) {
    payload_stage = 1;
    stage_output = "Ucus Oncesi";
  }
  
  else if(altitude_new > 10 && altitude_new < 500 && stage_control == 0 && ascent_or_descent == 1) {
    payload_stage = 2;
    stage_output = "Yukselme";
  }
  
  else if(altitude_new >500  && ascent_or_descent == 1) {
    payload_stage = 3;
    stage_output = "Zirve";
    stage_control = 1;
  }
  
  else if(altitude_new > 420 && stage_control == 1 && ascent_or_descent == 2) {
    payload_stage = 4;
    stage_output = "Model Uydu Inis";
  }
  else if(altitude_new > 380 && altitude_new < 420 && stage_control == 1 && ascent_or_descent == 2) {
    payload_stage = 5;
    stage_output = "Ayrilma";
  }
  else if(altitude_new > 20 && altitude_new < 380 && stage_control == 1 && ascent_or_descent == 2) {
    payload_stage = 6;
    stage_output = "Gorev Yuku Inis";
    last_trans_final =  millis();
  }
  else if(altitude_new > 150 && altitude_new < 250  && stage_control == 1 && ascent_or_descent == 0 ) {
    payload_stage = 7;
    stage_output = "Irtifa Sabitleme";
  }
  else if(altitude_new < 20  && stage_control == 1  && ascent_or_descent == 2) {
    payload_stage = 8;
    stage_output = "Kurtarma";
  }

  
  //////////////////////////1 HZ TELEMETRY////////////////////////////////////
  if (millis()>last_trans+interval){
  altitude_old = altitude_new;
  
    packet_number ++;
    telemetryFile = SD.open("TMUY2021_56727_TLM.csv", FILE_WRITE);
    
    time_telemetry = String(gpsDay) + '/' + String(gpsMonth) + '/' + String(gpsYear) + '-' + String(gpsHour) + ':' + String(gpsMinute) + ':' + String(gpsSecond);
    
    telemetry = String(team_no) + ',' + String(packet_number) + ',' + String(time_telemetry)+ ',' + String(pressure) + ',' + String(altitude_new) + ',' + \
              String(velocity) + ',' + String(temperature) + ',' + String(battery_voltage)+ ',' + String(gpsLatitude,6) + ',' + String(gpsLongitude,6) + ',' + \
              String(gpsAltitude,6) + ',' + String(stage_output) + ',' + String(pitch) + ',' + String(roll) + ',' + String(yaw) + ',' + \
              String(revolution) + ',' + String(video_transmission);
    
    if(firs_data_control){
    Serial.println(telemetry);
    telemetryFile.println(telemetry);
    Serial1.println(telemetry);
    }
    last_trans = millis();
    telemetryFile.close();
    firs_data_control = true;
  }  
  
  ////////////////////////////// DATA STORAGE //////////////////////////////

  EEPROM.get(eeAddress_1,packet_number);         //Save packet number
  eeAddress_2 = eeAddress_1 + sizeof(float);
  EEPROM.get(eeAddress_2,altitude_new);             //Save altitude
  eeAddress_3 = eeAddress_2 + sizeof(int);
  EEPROM.get(eeAddress_3,payload_stage);                // Save stage 
  
  ////////////////////////////// KURTARMA //////////////////////////////
   if(payload_stage == 8){
    digitalWrite(buzzer,HIGH); 
    if(millis() > last_trans_final + 60000){
      while (true){
        Serial.println("loop"); 
      }
      
    }
    }
    
}

void MavLink_receive()
  { 
  mavlink_message_t msg;
  mavlink_status_t status;

  while(Serial3.available())
  {
    uint8_t c= Serial3.read();

    //Get new message
    if(mavlink_parse_char(MAVLINK_COMM_0, c, &msg, &status))
    {

    //Handle new message from autopilot
      switch(msg.msgid)
      {

        case MAVLINK_MSG_ID_GLOBAL_POSITION_INT:
      {
        mavlink_global_position_int_t packet;
        mavlink_msg_global_position_int_decode(&msg, &packet);
        
       altitude_new= float(packet.relative_alt)/1000;
      }
        case MAVLINK_MSG_ID_VFR_HUD:
      {
        mavlink_vfr_hud_t packet;
        mavlink_msg_vfr_hud_decode(&msg, &packet);
        
        velocity = packet.groundspeed;

      }
  /*    
        case MAVLINK_MSG_ID_SCALED_PRESSURE:
      {
        mavlink_scaled_pressure_t packet;
        mavlink_msg_scaled_pressure_decode(&msg, &packet);
        
       temperature = ((packet.temperature)/100.0);
       pressure = (packet.press_abs*100);
      }
*/
      
      break;

      }
    }
  }
}
//function called by arduino to read any MAVlink messages sent by serial communication from flight controller to arduino

void request_datastream() {
//Request Data from Pixhawk
  uint8_t _system_id = 255; // id of computer which is sending the command (ground control software has id of 255)
  uint8_t _component_id = 2; // seems like it can be any # except the number of what Pixhawk sys_id is
  uint8_t _target_system = 1; // Id # of Pixhawk (should be 1)
  uint8_t _target_component = 0; // Target component, 0 = all (seems to work with 0 or 1
  uint8_t _req_stream_id = MAV_DATA_STREAM_ALL;
  uint16_t _req_message_rate = 0xA; //number of times per second to request the data in hex
  uint8_t _start_stop = 1; //1 = start, 0 = stop
 
  // Initialize the required buffers
  mavlink_message_t msg;
  uint8_t buf[MAVLINK_MAX_PACKET_LEN];
 
  // Pack the message
  mavlink_msg_request_data_stream_pack(_system_id, _component_id, &msg, _target_system, _target_component, _req_stream_id, _req_message_rate, _start_stop);
  uint16_t len = mavlink_msg_to_send_buffer(buf, &msg);  // Send the message (.write sends as bytes)
 
  Serial3.write(buf, len); //Write data to serial port
}
