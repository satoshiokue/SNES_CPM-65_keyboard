# SNES_CPM-65_keyboard

スーパーファミコン(SNES)の実機でCP/M-65を動かしてみます。  

## 必要な部品
- Arduino UNO R4 Minima
- USB Host Shield
- コントローラーコネクタ付きケーブル
- 起動用カートリッジ
- USBキーボード

## ファイルの入手
こちらの How? にある Download a binaly release 先にあるファイル snes.smc を取得します。  
https://github.com/davidgiven/cpm65

## ファイルの修正
バイナリーエディターを用いて修正していきます。

### init_fdサブルーチンの無効化

このファイルを**何らか**のカートリッジを用いてスーパーファミコンで実行します。

!["Arduino and USB Host Shield"](https://github.com/satoshiokue/SNES_CPM-65_keyboard/blob/main/IMG_6772.jpeg)

https://github.com/satoshiokue/SNES_CPM-65_keyboard/blob/main/IMG_67440.mov
