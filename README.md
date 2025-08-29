# SNES_CPM-65_keyboard

スーパーファミコン(SNES)の実機をCP/M-65マシンにしましょう。  

## 方針
スーパーファミコンのコントローラー入力は16bitのシリアルデータでボタン状態を取得します。8bit入力してUSBキーボードからASCIIコードを取得できるようにします。

## 必要な部品
- Arduino UNO R4 Minima
- USB Host Shield
- コントローラーコネクタ付きケーブル
- 起動用カートリッジ
- USBキーボード

## 接続方法
スーパーファミコンのコントローラー2を使用します。
```
SFC            Arduino
Pin1  +5V
Pin2  Clock    D3
Pin3  P/S      D2
Pin4  Data     D4
Pin7  Ground
```

## スーパーファミコン用CP/M-65ファイルの入手
こちらの How? にある Download a binaly release 先にあるファイル snes.smc を取得します。  
https://github.com/davidgiven/cpm65

## ファイルの修正
バイナリーエディターを用いて修正していきます。  
右側がオリジナルです。

!["Arduino and USB Host Shield"](https://github.com/satoshiokue/SNES_CPM-65_keyboard/blob/main/hex-diff.jpg)
### init_fdサブルーチンの無効化
init_fdサブルーチンが$830Aから始まります。先頭の$08を$60 (rts) に書き換えてフロッピーディスクの初期化動作をキャンセルします。

### get_current_keyサブルーチンの変更
キー入力ルーチンを変更します。$8195から$81B8を書き換えます。  

```
get_current_key:
;   Return A = 0, No key
;
;
.block
    ; $8189
    phb                 ; $8B               Push Data Bank Register on Stack
    php                 ; $08               Push Processor Status Register on Stack
    a8i8                ; $E2,$30           A reg 8bit / X,Y 8bit
                        ; SEP #$30

    .databank $7f
    lda #$7f            ; $A9,$7F
    pha                 ; $48               Push Accumulator on Stack
    plb                 ; $AB               Pull Data Bank Register from Stack

    lda pending_key     ; $AD,$03,$00       Load Accumulator from Memory
    bne exit            ; $D0,$1C

; P/S pulse
    lda #$01            ; $A9,$01           LATCH=High
    sta $4016           ; $8F,$16,$40,$00
    lda #$00            ; $A9,$00           LATCH=Low
    sta $4016           ; $8F,$16,$40,$00

    ldx #$08            ; $A2,$08
ReadLoop:
    lda $4017           ; $AF,$17,$40,$00
    lsr a               ; $4A
    rol pending_key     ; $2E,$03,$00
    dex                 ; $CA
    bne ReadLoop        ; $D0,$F5

    lda pending_key     ; $AD,$03,$00       Load Accumulator from Memory

exit:
    plp                 ; $28               Pull Processor Status Register from Stack
    plb                 ; $AB               Pull Data Bank Register from Stack
    rts                 ; $60
```

このファイルを**何らか**のカートリッジを用いてスーパーファミコンで実行します。

!["Arduino and USB Host Shield"](https://github.com/satoshiokue/SNES_CPM-65_keyboard/blob/main/IMG_6772.jpeg)

## Arduinoのプログラム
SNES_KBD.inoを書き込みます。  
USキーボード用なのでJIS配列などでは修正が必要です。

https://github.com/satoshiokue/SNES_CPM-65_keyboard/blob/main/IMG_67440.mov
