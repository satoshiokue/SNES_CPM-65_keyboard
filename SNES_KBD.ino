// Arduino UNO R4 Minima + USB Host Shield
//
// D4 Output P103 Data
// D3 Input  P104 Clock
// D2 Input  P105 P/S

#include <hidboot.h>
#include <usbhub.h>
#include <SPI.h>

#define BUFFER_SIZE 32
volatile uint8_t ringBuffer[BUFFER_SIZE];
volatile uint8_t head = 0;
volatile uint8_t tail = 0;
volatile uint8_t currentByte = 0;

uint8_t HIDtoAscii(uint8_t hidKey, MODIFIERKEYS &mod);

class KbdRptParser : public KeyboardReportParser
{
  protected:
    void OnKeyDown(uint8_t m, uint8_t key) {
    MODIFIERKEYS mod;
   *((uint8_t*)&mod) = m;

    // 1. HID Usage ID を ASCII に変換
    uint8_t ascii = HIDtoAscii(key, mod);

    // 2. Ctrl が押されているなら変換
    if (mod.bmLeftCtrl || mod.bmRightCtrl) {
      // 'A'〜'Z' を 0x01〜0x1A に変換
      if (ascii >= 'A' && ascii <= 'Z') {
        ascii = ascii - 'A' + 1;
      // 'a'〜'z' を 0x01〜0x1A に変換
      } else if (ascii >= 'a' && ascii <= 'z') {
        ascii = ascii - 'a' + 1;
      }
    }
    enqueue(ascii);

    Serial.print("Result code: ");
    PrintHex<uint8_t>(ascii, 0x80);

    Serial.print(" ASCII code: ");
    PrintHex<uint8_t>(key, 0x80);

    Serial.println();
  }

  void enqueue(uint8_t val) {
    uint8_t next = (head + 1) % BUFFER_SIZE;
    if (next != tail) { // バッファフルでないとき
       ringBuffer[head] = val;
      head = next;  
    }
  }
};

USB     Usb;
//USBHub     Hub(&Usb);
HIDBoot<USB_HID_PROTOCOL_KEYBOARD>    HidKeyboard(&Usb);

KbdRptParser Prs;

// HID Usage ID → ASCII (USキーボード簡易版)
uint8_t HIDtoAscii(uint8_t hidKey, MODIFIERKEYS &mod) {
  bool shift = (mod.bmLeftShift || mod.bmRightShift);

  switch (hidKey) {
    // 文字キー A〜Z
    case 0x04: return shift ? 'A' : 'a'; // A
    case 0x05: return shift ? 'B' : 'b'; // B
    case 0x06: return shift ? 'C' : 'c'; // C
    case 0x07: return shift ? 'D' : 'd';
    case 0x08: return shift ? 'E' : 'e';
    case 0x09: return shift ? 'F' : 'f';
    case 0x0A: return shift ? 'G' : 'g';
    case 0x0B: return shift ? 'H' : 'h';
    case 0x0C: return shift ? 'I' : 'i';
    case 0x0D: return shift ? 'J' : 'j';
    case 0x0E: return shift ? 'K' : 'k';
    case 0x0F: return shift ? 'L' : 'l';
    case 0x10: return shift ? 'M' : 'm';
    case 0x11: return shift ? 'N' : 'n';
    case 0x12: return shift ? 'O' : 'o';
    case 0x13: return shift ? 'P' : 'p';
    case 0x14: return shift ? 'Q' : 'q';
    case 0x15: return shift ? 'R' : 'r';
    case 0x16: return shift ? 'S' : 's';
    case 0x17: return shift ? 'T' : 't';
    case 0x18: return shift ? 'U' : 'u';
    case 0x19: return shift ? 'V' : 'v';
    case 0x1A: return shift ? 'W' : 'w';
    case 0x1B: return shift ? 'X' : 'x';
    case 0x1C: return shift ? 'Y' : 'y';
    case 0x1D: return shift ? 'Z' : 'z';

    // 数字キー 1〜0
    case 0x1E: return shift ? '!' : '1';
    case 0x1F: return shift ? '@' : '2';
    case 0x20: return shift ? '#' : '3';
    case 0x21: return shift ? '$' : '4';
    case 0x22: return shift ? '%' : '5';
    case 0x23: return shift ? '^' : '6';
    case 0x24: return shift ? '&' : '7';
    case 0x25: return shift ? '*' : '8';
    case 0x26: return shift ? '(' : '9';
    case 0x27: return shift ? ')' : '0';

    // 記号・その他
    case 0x28: return 0x0D;           // Enter
    case 0x29: return 0x1B;           // ESC
    case 0x2A: return 0x08;           // Backspace
    case 0x2B: return 0x09;           // TAB
    case 0x2C: return ' ';            // Space
    case 0x2D: return shift ? '_' : '-';
    case 0x2E: return shift ? '+' : '=';
    case 0x2F: return shift ? '{' : '[';
    case 0x30: return shift ? '}' : ']';
    case 0x31: return shift ? '|' : '\\';
    case 0x33: return shift ? ':' : ';';
    case 0x34: return shift ? '"' : '\'';
    case 0x35: return shift ? '~' : '`';
    case 0x36: return shift ? '<' : ',';
    case 0x37: return shift ? '>' : '.';
    case 0x38: return shift ? '?' : '/';
    case 0x49: return 0x12;           // Insert
    case 0x4A: return 0x0C;           // HOME
    case 0x4B: return 0x13;           // Page UP
    case 0x4C: return 0x7F;           // DEL
    case 0x4D: return 0x0E;           // End
    case 0x4E: return 0x14;           // Page Down
    case 0x4F: return 0x89;           // Right Arrow
    case 0x50: return 0x88;           // Left Arrow
    case 0x51: return 0x8A;           // Down Arrow
    case 0x52: return 0x8B;           // Up Arrow
  }
  return 0; // 未対応キーは 0 を返す
}

// リングバッファ
int bufferGetChar() {
  if (head == tail) return -1;
  char c = ringBuffer[tail];
  tail = (tail + 1) % BUFFER_SIZE;
  return c;
}

static int currentChar = -1;

void setup()
{
  Serial.begin( 115200 );
  while (!Serial);
  Serial.println("Start");

  if (Usb.Init() == -1)
      Serial.println("OSC did not start.");
  delay( 200 );

  R_PORT1->PCNTR1 |= 1 << 3;        // P103(Data) Output
  R_PORT1->PODR &= ~(1 << 3);       // P103(Data) Low

  HidKeyboard.SetReportParser(0, &Prs);
}

void loop()
{
  Usb.Task();

  // --- LATCH立上りで転送準備 ---
  if (R_PORT1->PIDR >> 5 & 0x01) {    // P105(P/S)
    currentChar = bufferGetChar();
    if (currentChar < 0) {
      currentChar = 0x00; // バッファが空なら 0x00 を出力する 
    }

    if (currentChar & 0x80) {
      R_PORT1->PODR |= 1 << 3;        // P103(Data) High
    } else {
      R_PORT1->PODR &= ~(1 << 3);     // P103(Data) Low
    }

    while(R_PORT1->PIDR >> 4 & 0x01); // P104(Clock) 立ち下がり待ち

    if (currentChar & 0x40) {
      R_PORT1->PODR |= 1 << 3;
    } else {
      R_PORT1->PODR &= ~(1 << 3);
    }

    while(R_PORT1->PIDR >> 4 & 0x01);

    if (currentChar & 0x20) {
      R_PORT1->PODR |= 1 << 3;
    } else {
      R_PORT1->PODR &= ~(1 << 3);
    }

    while(R_PORT1->PIDR >> 4 & 0x01);

    if (currentChar & 0x10) {
      R_PORT1->PODR |= 1 << 3;
    } else {
      R_PORT1->PODR &= ~(1 << 3);
    }

    while(R_PORT1->PIDR >> 4 & 0x01);

    if (currentChar & 0x08) {
      R_PORT1->PODR |= 1 << 3;
    } else {
      R_PORT1->PODR &= ~(1 << 3);
    }

    while(R_PORT1->PIDR >> 4 & 0x01);

    if (currentChar & 0x04) {
      R_PORT1->PODR |= 1 << 3;
    } else {
      R_PORT1->PODR &= ~(1 << 3);
    }

    while(R_PORT1->PIDR >> 4 & 0x01);

    if (currentChar & 0x02) {
      R_PORT1->PODR |= 1 << 3;
    } else {
      R_PORT1->PODR &= ~(1 << 3);
    }

    while(R_PORT1->PIDR >> 4 & 0x01);

    if (currentChar & 0x01) {
      R_PORT1->PODR |= 1 << 3;
    } else {
      R_PORT1->PODR &= ~(1 << 3);
    }

    while(R_PORT1->PIDR >> 4 & 0x01);

    R_PORT1->PODR &= ~(1 << 3);     // P103(Data) Low
  }
}

