# pH_Meter_forM5stack

## 概要
このリポジトリは、M5stack用のArduinoプログラムで、pHセンサーの読み取り値をM5stackの画面で作成されたアナログメーターに表示します。

## 動作確認条件
- M5stack Basic
- Arduino IDE 2.3.2
- ESPボードマネージャ 2.0.16 by Espressif
- M5stackライブラリ 0.4.6<br>
ArduinoIDE、ボードマネージャ、ライブラリは2024年5月19日現在最新のもので動作を確認しています。

## 使用方法
1. ESPのボードマネージャとM5stackのライブラリがインストールされていない場合はインストールしてください。
2. プログラムをダウンロードします。zipファイルの場合は解凍してください。
3. ダウンロードしたフォルダに入っているinoファイル、ヘッダーファイル、cppファイルが同じフォルダに入っていることを確認してください。
4. USB経由でM5stackをコンピュータに接続します。
5. Arduino IDEで.inoファイルを開きます。
6. Arduino IDEで正しいボードとポートを選択します。(ボードマネージャ：ESP32 Dev Module、ボーレート：115200)
7. プログラムをM5stackに書き込みます。

## ドキュメント
関連するドキュメントは以下のurlを参照してください。（pdfファイルがダウンロードされます。）<br>
[pHセンサーの検証資料](http://b-and-b-lab.jp/web/wp-content/uploads/2021/11/pHSns_Doc.pdf)

