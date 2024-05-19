#include "M5_pHsensor.h"

/*  static float  stepSizeTbl[] = {
    0.001,    // 12-bit, 1X Gain
    0.0005,   // 12-bit, 2X Gain
    0.00025,  // 12-bit, 4X Gain
    0.000125, // 12-bit, 8X Gain
    0.00025,  // 14-bit, 1X Gain
    0.000125, // 14-bit, 2X Gain
    0.0000625,  // 14-bit, 4X Gain
    0.00003125, // 14-bit, 8X Gain
    0.0000625,  // 16-bit, 1X Gain
    0.00003125, // 16-bit, 2X Gain
    0.000015625,  // 16-bit, 4X Gain
    0.0000078125, // 16-bit, 8X Gain
    0.000015625,  // 18-bit, 1X Gain
    0.0000078125, // 18-bit, 2X Gain
    0.00000390625,  // 18-bit, 4X Gain
    0.000001953125  // 18-bit, 8X Gain
    };
*/

/******************************************
 * Default constructor, uses default I2C address.
 * @see MCP342X_DEFAULT_ADDRESS
 */
MCP342X::MCP342X() {
    devAddr = MCP342X_DEFAULT_ADDRESS;
}

/******************************************
 * Specific address constructor.
 * @param address I2C address
 * @see MCP342X_DEFAULT_ADDRESS
 * @see MCP342X_A0GND_A1GND, etc.
 */
MCP342X::MCP342X(uint8_t address) {
    devAddr = address;
}

/******************************************
 * Verify the I2C connection.
 * Make sure the device is connected and responds as expected.
 * @return True if connection is valid, false otherwise
 */
bool MCP342X::testConnection() {
    Wire.beginTransmission(devAddr);
    return (Wire.endTransmission() == 0);
}


/******************************************
 * Set the configuration shadow register
 */
void MCP342X::configure(uint8_t configData) {
  configRegShdw = configData;
}

/******************************************
 * Get the configuration shadow register
 */
uint8_t MCP342X::getConfigRegShdw(void) {
  return configRegShdw;
}

/******************************************
 * Get the step size based on the configuration shadow register
 */
/*float MCP342X::getStepSize(void) {
  uint8_t select = configRegShdw & (MCP342X_SIZE_MASK | MCP342X_GAIN_MASK);
  return stepSizeTbl[select];
}*/

/******************************************
 * Start a conversion using configuration settings from
 *   the shadow configuration register
 */
bool MCP342X::startConversion(void) {
  Wire.beginTransmission(devAddr);
  Wire.write(configRegShdw | MCP342X_RDY);
  return (Wire.endTransmission() == 0);
}

 
/******************************************
 * Start a conversion using configuration settings from
 *   the shadow configuration register substituting the
 *   supplied channel
 */
bool MCP342X::startConversion(uint8_t channel) {
  Wire.beginTransmission(devAddr);
  configRegShdw = ((configRegShdw & ~MCP342X_CHANNEL_MASK) | 
         (channel & MCP342X_CHANNEL_MASK));
  Wire.write(configRegShdw | MCP342X_RDY);
  return (Wire.endTransmission() == 0);
}

 
/******************************************
 * Read the conversion value (12, 14 or 16 bit)
 *  Spins reading status until ready then
 *  fills in the supplied location with the 16-bit (two byte)
 *  conversion value and returns the status byte
 *  Note: status of -1 "0xFF' implies read error
 */
uint8_t MCP342X::getResult(int16_t *dataPtr) {
  uint8_t adcStatus;
  if((configRegShdw & MCP342X_SIZE_MASK) == MCP342X_SIZE_18BIT) {
    return 0xFF;
  }

  do {
     if(Wire.requestFrom(devAddr, (uint8_t) 3) == 3) {
       ((char*)dataPtr)[1] = Wire.read();
       ((char*)dataPtr)[0] = Wire.read();
       adcStatus = Wire.read();
     }
     else return 0xFF;
  } while((adcStatus & MCP342X_RDY) != 0x00);
  return adcStatus;
}

 
/******************************************
 * Check to see if the conversion value (12, 14 or 16 bit)
 *  is available.  If so, then
 *  fill in the supplied location with the 16-bit (two byte)
 *  conversion value and status the config byte
 *  Note: status of -1 "0xFF' implies read error
 */
uint8_t MCP342X::checkforResult(int16_t *dataPtr) {
  uint8_t adcStatus;
  if((configRegShdw & MCP342X_SIZE_MASK) == MCP342X_SIZE_18BIT) {
    return 0xFF;
  }

  if(Wire.requestFrom(devAddr, (uint8_t) 3) == 3) {
    ((char*)dataPtr)[1] = Wire.read();
    ((char*)dataPtr)[0] = Wire.read();
    adcStatus = Wire.read();
  }
  else return 0xFF;

  return adcStatus;
}

 
/******************************************
 * Read the conversion value (18 bit)
 *  Spins reading status until ready then
 *  fills in the supplied location (32 bit) with
 *  the 24-bit (three byte) conversion value
 *  and returns the status byte
 *  Note: status of -1 "0xFF' implies read error
 */
uint8_t MCP342X::getResult(int32_t *dataPtr) {
  uint8_t adcStatus;
  if((configRegShdw & MCP342X_SIZE_MASK) != MCP342X_SIZE_18BIT) {
    return 0xFF;
  }

  do {
     if(Wire.requestFrom((uint8_t) devAddr, (uint8_t) 4) == 4) {
       ((char*)dataPtr)[3] = Wire.read();
       ((char*)dataPtr)[2] = Wire.read();
       ((char*)dataPtr)[1] = Wire.read();
       adcStatus = Wire.read();
     }
     else return 0xFF;
  } while((adcStatus & MCP342X_RDY) != 0x00);
  *dataPtr = (*dataPtr)>>8;
  return adcStatus;
}


/******************************************
 * Check to see if the conversion value (18 bit)
 *  is available.  If so, then
 *  fill in the supplied location (32 bit) with
 *  the 24-bit (three byte) conversion value
 *  and return the status byte
 *  Note: status of -1 "0xFF' implies read error
 */
uint8_t MCP342X::checkforResult(int32_t *dataPtr) {
  uint8_t adcStatus;
  if((configRegShdw & MCP342X_SIZE_MASK) != MCP342X_SIZE_18BIT) {
    return 0xFF;
  }

  if(Wire.requestFrom((uint8_t) devAddr, (uint8_t) 4) == 4) {
    ((char*)dataPtr)[3] = Wire.read();
    ((char*)dataPtr)[2] = Wire.read();
    ((char*)dataPtr)[1] = Wire.read();
    adcStatus = Wire.read();
  }
  else return 0xFF;

  *dataPtr = (*dataPtr)>>8;
  return adcStatus;
}


//pHについてのコード----------------------------------------------------
// ADCの設定
#define VREF_MCP3425      2.048         // range ±2.048V @ Gain x1
#define RES_MCP3425_16  (65536 / 2.0)   // 諧調


// クラスオブジェクトの宣言
// MCP342X pH_ADC(MCP342X_DEFAULT_ADDRESS+0);     // ADD0 品を実装
extern MCP342X pH_ADC;     // ADD0 品を実装
#define ADC_GAIN_SETTING_PH MCP342X_GAIN_8X    // 測定レンジ±2.048V / 8 = ±256mV
                                               // 　　pH 3(酢酸：やや強い酸性) - pH11(アンモニア水：やや強いアルカリ性)に測定レンジを制限
#define GAIN_SETTING_PH  8.0                   // ゲイン8


//pHセンサの電源を入れる
void pHsnsPwrOn(){
  // Enable power for MCP342x (needed for FL100 shield only)
  pinMode(2, OUTPUT);
  digitalWrite(2, HIGH);

  delay(100); //立ち上がるまで少し待つ
}

//pHの計算
// pH 測定（コード値）
void MeasurePH_Hex( int16_t *pH_Code ){  

  // ADC の初期化
  pH_ADC.configure(MCP342X_MODE_CONTINUOUS |
                   MCP342X_CHANNEL_1 |
                   MCP342X_SIZE_16BIT |
                   ADC_GAIN_SETTING_PH
                 );

  // 変換開始
  pH_ADC.startConversion();
  
  // 値の取得
  pH_ADC.getResult(pH_Code);
  Serial.print("adc value : ");
  Serial.println(*pH_Code);

  delay(10);
}

// ネルンストの式
// https://ja.wikipedia.org/wiki/%E3%83%8D%E3%83%AB%E3%83%B3%E3%82%B9%E3%83%88%E3%81%AE%E5%BC%8F
// よりの定数（参照資料より）

// 計算に必要な物理乗数
#define NERNST_EQ_CONST 2.3026                // pH センサの電極の電位を求める定数
#define FARADAY_CONST   (9.6485 * 10000.0)    // ファラデー定数  [C/mol]
#define R_CONST        8.3144                 // 気体定数 [J /(mol K) ]
#define TEMP_0_DEG      273.15                // 絶対温度 0℃ [K]
// 温度以外の pH 換算の係数（絶対温度を乗じる必要あり）
#define PH_FACTOR_RT_F ( NERNST_EQ_CONST * R_CONST / FARADAY_CONST )

// pH 換算関数
//   第一引数：pH コード値
//   第二引数：温度 [℃]
float ConvertPH_FromCode( int16_t pH_Code, float Temperture  ){
  
  float voltage = (float) pH_Code / RES_MCP3425_16 * VREF_MCP3425 / GAIN_SETTING_PH;  // 電圧換算
  
  return (- voltage / ( PH_FACTOR_RT_F * ( Temperture + TEMP_0_DEG )) + 7.0);         // pH 換算をして返す

}
