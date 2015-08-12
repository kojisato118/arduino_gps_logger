#include <SoftwareSerial.h>

//http://baticadila.dip.jp/arduino_105.html
//を参考

#define PIN_GPS_Rx 10
#define PIN_GPS_Tx 11

#define SERIAL_BAUDRATE 9600
#define GPS_BAUDRATE 9600

#define BUFFER_SIZE 256

SoftwareSerial sGps(PIN_GPS_Rx, PIN_GPS_Tx);
bool isValid;

void setup() {
  // put your setup code here, to run once:
  Serial.begin(SERIAL_BAUDRATE);
  Serial.println("GPS Logger Start!"); 
  
  sGps.begin(GPS_BAUDRATE);

  isValid = false;
}

void loop() {
  // put your main code here, to run repeatedly:
 char buf[BUFFER_SIZE];
 getLine(buf);
  
 analyzeData(buf);
}

/*****************************************************
 * bufにgpsから取得するデータ1行分(改行コード0x0Aまで)の文字列を格納する。 
 * ただし、最大でBUFFER_SIZEの大きさまで。
 */
void getLine(char *buf){
  if(buf == NULL)
    return;
    
  int count = 0;
  
  do {
    if (sGps.available()) {
      buf[count] = sGps.read();
      count++;
    }
    if (count >= BUFFER_SIZE) break;
  } while(buf[count - 1] != 0x0A);
  buf[count] = '\0';
}

/*************
 *gpsデータ1行分(buf)を読み込んで解析する。 
 *
 *gpsデータが有効状態のときに、センテンスID=$GPGAAのデータから位置情報を取得する。
 */
 
void analyzeData(char *buf){
  if(buf == NULL)
    return;
  
  checkValidity(buf);

  if(!isValid)
    return;
  
  char *gpsTime, *gpsLat, *gpsLatDir, *gpsLon, *gpsLonDir, *gpsTemp; 
  
  if (strncmp("$GPGGA", buf, 6) == 0) {
    strtok(buf, ",");
    gpsTime = strtok(NULL, ","); 
    gpsLat = strtok(NULL, ",");  
    gpsLatDir = strtok(NULL, ",");
    gpsLon = strtok(NULL, ","); 
    gpsLonDir = strtok(NULL, ",");
    
    Serial.print("Time = ");
    Serial.print(gpsTime);
    Serial.print(" : ");
    Serial.print("Latitude = ");
    Serial.print(gpsLatDir);
    Serial.print(" - ");
    Serial.print(gpsLat);
    Serial.print(" : ");
    Serial.print("Longitude = ");
    Serial.print(gpsLonDir);
    Serial.print(" - ");
    Serial.println(gpsLon);
  }
}

/**************
 * gps情報が有効かの判断をする。
 * 
 * センテンスID=$GPRMCの第２Fieldの値がAのとき、gps情報が有効であると判定する。
 * http://akizukidenshi.com/download/ds/canmore/GMS7-CR6_v1.0_ak.pdf
 */
void checkValidity(char *buf){
  if(buf == NULL){
    isValid = false;
    return;
  }
  
  char *stat;
  if (strncmp("$GPRMC", buf, 6) == 0) {
    char tmp[BUFFER_SIZE];
    strcpy(tmp, buf);
    
    stat = strtok(tmp, ","); 
    stat = strtok(NULL, ","); 
    stat = strtok(NULL, ","); 
    Serial.println(stat);
    
    isValid = (strncmp("A", stat, 1) == 0);
  }
}

