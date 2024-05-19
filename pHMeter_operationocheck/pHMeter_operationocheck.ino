// M5stackでpHセンサの動作確認を行うためのプログラムです
// 使用したライブラリ https://github.com/m5stack/m5stack
// adcについてはこちらのプログラムを参考にしました https://github.com/uChip/MCP342X
// メータの表示についてはこちらのプログラムを参考にしました https://github.com/Bodmer/TFT_eSPI
// 温度は#define TEMPの値を書き換えてください
// キャリブレーション機能については未実装です。（近日対応予定）
// 動作確認
// Arduino 2.3.2 ESP board manager esp32 by Espressif 2.0.16 M5stack ライブラリ 0.4.6
// macでのビルドも確認
// esp32のボードマネージャは最新のものにしておかないとエラーが出ることがある（再現確認はしていない）
// 2024-05-19 S.Nakamura


#include <M5Stack.h>
#include "M5_pHsensor.h"


//温度補正用の温度 
#define TEMP 20.0 


// メーターの設定------------------------------------------------------------------------
#define M_SIZE 1.3333       // メータの大きさの設定
float ltx = 0;              // Saved x coord of bottom of needle
uint16_t osx = M_SIZE*120;  // Saved x & y coords
uint16_t osy = M_SIZE*120; 
float old_analog =  999;    // Value last displayed

//プロトタイプ宣言
void ReadSensor(float Temp);
void analogMeter_pH();
void LCDStartDisp();
void LCDsensornotfound();
void plotNeedle_pH(float value);


MCP342X pH_ADC(MCP342X_DEFAULT_ADDRESS+0);     // ADD0 品を実装

void setup(){
  
  M5.begin();
  Wire.begin(21, 22);

  Serial.println("pH sensor start");
  LCDStartDisp();   // 起動画面の表示
  delay(2000);      // 起動画面の表示用

  // センサの接続確認
  while(pH_ADC.testConnection() != 1){

    Serial.println("sensor not found");
    LCDsensornotfound();
    delay(1000);
  }
  
  // メータ画面の作成
  M5.Lcd.fillScreen(TFT_BLACK);
  M5.Lcd.setBrightness(200);
  analogMeter_pH();           // メータ画面の出力
}

void loop(){

  ReadSensor(TEMP);
  delay(100);
} // loopend



// pHを計測する関数
void ReadSensor(float Temp){
  
  int16_t PH_Hex;   // pH (コード値)
  float pH;         // pH（換算値）
  
  MeasurePH_Hex( &PH_Hex );                   // コード値取得
  pH = ConvertPH_FromCode( PH_Hex, Temp  );   // pH 値を算出（校正されていない） 

  // 確認用にシリアルとメータで表示する
  Serial.printf("pH : %f", pH);
  plotNeedle_pH(pH);                          // pHの値をメーターに反映
}


// M5stackの起動画面
void LCDStartDisp(){

  M5.Lcd.fillScreen(TFT_BLACK);
  M5.Lcd.setCursor(0, 10);
  M5.Lcd.setTextColor(TFT_RED, TFT_BLACK);
  M5.Lcd.setTextSize(3); //文字の大きさを設定（1～7）
  M5.Lcd.print("B&B pH monitor");
  M5.Lcd.print("\n");
  M5.Lcd.print("\n");
  M5.Lcd.print("\n");
  M5.Lcd.setTextColor(TFT_WHITE, TFT_BLACK);
  M5.Lcd.print("B&B Lab.inc");
}

// センサが見つからなかったときの表示
void LCDsensornotfound(){
  
  M5.Lcd.fillScreen(TFT_BLACK);
  M5.Lcd.setCursor(0, 50);
  M5.Lcd.setTextColor(TFT_WHITE, TFT_BLACK);
  M5.Lcd.setTextSize(2);
  M5.Lcd.print("sensor not found");
  M5.Lcd.println("");
  M5.Lcd.println("");
  M5.Lcd.print("please check the sensor");
}



// 測定値をsinglestatで表示する関数
void SinglestatDisp(float pH){

  M5.Lcd.setTextSize(1);
  M5.Lcd.setTextColor(TFT_WHITE, TFT_BLACK);  

  /*数字のクリア処理
  定期的なクリアが必要
  第一引数：x座標、第二引数：ｙ座標、第三引数：幅、第四引数：高さ、第五引数：塗りつぶす色*/
  M5.Lcd.fillRect(0, 169, 500, 70, BLACK);    //y座標についてはメーターから外れるように計算で出した
  M5.Lcd.drawString("pH", 50, 179, 4);   //単位の描画
  /*Float型の整数の描画
  第一引数：整数、第二引数：小数点以下、第三引数：ｘ座標、第四引数：y座標、第五引数：フォント*/
  M5.Lcd.drawFloat(pH, 1, 150, 179, 7); //数字の描画
}


// アナログメータの表示
void analogMeter_pH(){

  M5.Lcd.setTextSize(1);  
  // Meter outline
  M5.Lcd.fillRect(0, 0, M_SIZE*239, M_SIZE*126, TFT_DARKGREY);
  M5.Lcd.fillRect(5, 3, M_SIZE*230, M_SIZE*119, TFT_WHITE);

  M5.Lcd.setTextColor(TFT_BLACK);  // Text colour

  // Draw ticks every 5 degrees from -50 to +50 degrees (100 deg. FSD swing)
  for (int i = -50; i < 51; i += 5) {
    // Long scale tick length
    int tl = 15;

    //描画する1目盛りの座標
    float sx = cos((i - 90) * 0.0174532925);
    float sy = sin((i - 90) * 0.0174532925);
    uint16_t x0 = sx * (M_SIZE*100 + tl) + M_SIZE*120;
    uint16_t y0 = sy * (M_SIZE*100 + tl) + M_SIZE*140;
    uint16_t x1 = sx * M_SIZE*100 + M_SIZE*120;
    uint16_t y1 = sy * M_SIZE*100 + M_SIZE*140;

    // ゾーン塗りつぶしの時の次のメモリの座標
    float sx2 = cos((i + 5 - 90) * 0.0174532925);
    float sy2 = sin((i + 5 - 90) * 0.0174532925);
    int x2 = sx2 * (M_SIZE*100 + tl) + M_SIZE*120;
    int y2 = sy2 * (M_SIZE*100 + tl) + M_SIZE*140;
    int x3 = sx2 * M_SIZE*100 + M_SIZE*120;
    int y3 = sy2 * M_SIZE*100 + M_SIZE*140;

    // Orange zone limits
    if (i >= -50 && i < -25) {
     M5.Lcd.fillTriangle(x0, y0, x1, y1, x2, y2, TFT_ORANGE);
     M5.Lcd.fillTriangle(x1, y1, x2, y2, x3, y3, TFT_ORANGE);
    }

    // Orange zone limits
    if (i >= 25 && i < 50) {
      M5.Lcd.fillTriangle(x0, y0, x1, y1, x2, y2, TFT_ORANGE);
      M5.Lcd.fillTriangle(x1, y1, x2, y2, x3, y3, TFT_ORANGE);
    }

    // Short scale tick length
    if (i % 25 != 0) tl = 8;

    // Recalculate coords incase tick lenght changed
    x0 = sx * (M_SIZE*100 + tl) + M_SIZE*120;
    y0 = sy * (M_SIZE*100 + tl) + M_SIZE*140;
    x1 = sx * M_SIZE*100 + M_SIZE*120;
    y1 = sy * M_SIZE*100 + M_SIZE*140;

    // Draw tick
    M5.Lcd.drawLine(x0, y0, x1, y1, TFT_BLACK);

    // Check if labels should be drawn, with position tweaks
    if (i % 25 == 0) {
      // Calculate label positions
      //ラベルの位置の計算
      x0 = sx * (M_SIZE*100 + tl + 10) + M_SIZE*120;
      y0 = sy * (M_SIZE*100 + tl + 10) + M_SIZE*140;
      switch (i / 25) {
        
        //表示するメモリ
        case -2: M5.Lcd.drawCentreString("1", x0, y0 - 12, 1); break;
        case -1: M5.Lcd.drawCentreString("3.5", x0, y0 - 9, 1); break;
        case 0: M5.Lcd.drawCentreString("7", x0, y0 - 7, 1); break;
        case 1: M5.Lcd.drawCentreString("10.5", x0, y0 - 9, 1); break;
        case 2: M5.Lcd.drawCentreString("14", x0, y0 - 12, 1); break;
      }
    }

    // Now draw the arc of the scale
    sx = cos((i + 5 - 90) * 0.0174532925);
    sy = sin((i + 5 - 90) * 0.0174532925);
    x0 = sx * M_SIZE*100 + M_SIZE*120;
    y0 = sy * M_SIZE*100 + M_SIZE*140;
    // Draw scale arc, don't draw the last part
    if (i < 50) M5.Lcd.drawLine(x0, y0, x1, y1, TFT_BLACK);
  }

  M5.Lcd.drawString("pH", M_SIZE*(5 + 230 - 40), M_SIZE*(119 - 20), 1); // Units at bottom right
  M5.Lcd.drawRect(5, 3, M_SIZE*230, M_SIZE*119, TFT_BLACK); // Draw bezel line
}


// #########################################################################
// Update needle position
// #########################################################################
// 針の描画
// 第一引数：取得した値
// 第二引数：画面を更新する時間
void plotNeedle_pH(float value){

  M5.Lcd.setTextColor(TFT_BLACK, TFT_WHITE);
  char buf[8]; dtostrf(value, 4, 0, buf);

  if (value < 0) value = 0; // Limit value to emulate needle end stops
  if (value > 14) value = 14;

  // pHが変動したら値を書き換える
  if (abs(value - old_analog) >0.05){    //ちらつきを抑えるために値が大きく変動したときに書き換える（0.1では大きすぎる）
    
    old_analog = value;
    
    // 確認用の表示
    Serial.print("value :");
    Serial.println(value);
    int d_pH = (int )value *10;

    // 取得値を再マッピングさせる
    float sdeg = map(d_pH, -10, 150, 30, 150); // Map value to angle
    // Calcualte tip of needle coords
    //degree（角度）からラジアンへの変換 https://oshiete.goo.ne.jp/qa/5663715.html 
    float sx = -cos(sdeg * 0.0174532925);
    float sy = -sin(sdeg * 0.0174532925);

    // Calculate x delta of needle start (does not start at pivot point)
    float tx = tan((sdeg + 90) * 0.0174532925);
    
    // Erase old needle image
    M5.Lcd.drawLine(M_SIZE*(120 + 20 * ltx - 1), M_SIZE*(140 - 20), osx - 1, osy, TFT_WHITE);
    M5.Lcd.drawLine(M_SIZE*(120 + 20 * ltx), M_SIZE*(140 - 20), osx, osy, TFT_WHITE);
    M5.Lcd.drawLine(M_SIZE*(120 + 20 * ltx + 1), M_SIZE*(140 - 20), osx + 1, osy, TFT_WHITE);

    // Re-plot text under needle
    M5.Lcd.setTextColor(TFT_BLACK);
    M5.Lcd.drawCentreString("pH", M_SIZE*120, M_SIZE*70, 2); // // Comment out to avoid font 4

    // Store new needle end coords for next erase
    ltx = tx;
    osx = M_SIZE*(sx * 98 + 120);
    osy = M_SIZE*(sy * 98 + 140);

    // Draw the needle in the new postion, magenta makes needle a bit bolder
    // draws 3 lines to thicken needle
    M5.Lcd.drawLine(M_SIZE*(120 + 20 * ltx - 1), M_SIZE*(140 - 20), osx - 1, osy, TFT_RED);
    M5.Lcd.drawLine(M_SIZE*(120 + 20 * ltx), M_SIZE*(140 - 20), osx, osy, TFT_MAGENTA);
    M5.Lcd.drawLine(M_SIZE*(120 + 20 * ltx + 1), M_SIZE*(140 - 20), osx + 1, osy, TFT_RED);

    SinglestatDisp(value);
  }
}

