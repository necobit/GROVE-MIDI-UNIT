// for GROVE MIDI Unit and M5Atom
//"M5.dis.drawpix"と"M5.begin(true, false, true);"のコメントアウトを外すと
//M5Atom本体のLEDをインジケーターにできますが、M5Atomの本体ライブラリの制御の関係で
//繋いだ全てのLEDの明るさが20/255になります。

#include <M5Atom.h>
#include <MIDI.h>
#include <FastLED.h>
MIDI_CREATE_INSTANCE(HardwareSerial, Serial2, MIDI); 

#define PIN_LEDS 19
#define NUM_LEDS 24
#define MAX_BRIGHTNESS 255
#define DEF_SATURATION 64
#define MAX_HUE 360
#define MAX_SATURATION 255
#define MAX_PERFORMANCE_STEP 20

//MIDI CCアサイン
#define MIDI_CC_DURATION 2 // 光減衰早さ
#define MIDI_CC_COLOR_HUE 3 // 色相
#define MIDI_CC_COLOR_SATURATION 4 // 彩度

#define MIDI_RXCH 16 //受信MIDIチャンネル

#define MIDI_VALUE_MAX 127

//-------------------------------
uint8_t brightness = MAX_BRIGHTNESS;
CRGB leds[NUM_LEDS];
int hue;
uint8_t saturation = DEF_SATURATION;
uint8_t duration = 100;
uint16_t latestNoteOnTimestamp = 0;
uint16_t currentNoteOnTimestamp = 0;
bool isPlaying = false;
uint8_t playIndex = 0;

uint8_t gamingHue = 0;

//Button p = Button(39, true, 10);
//-------------------------------

//void connected();
void onNoteOn(uint8_t channel, uint8_t note, uint8_t velocity, uint16_t timestamp);
void onNoteOff(uint8_t channel, uint8_t note, uint8_t velocity, uint16_t timestamp);
void onControlChange(uint8_t channel, uint8_t controller, uint8_t value, uint16_t timestamp);

void performanceTask(void *pvParameters);

//-------------------------------


void setup() {
  Serial.begin(115200);
  Serial2.begin(31250, SERIAL_8N1, 32, 26);
  //  M5.begin(true, false, true);
  MIDI.turnThruOn();
  // LED初期化
  FastLED.addLeds<NEOPIXEL, PIN_LEDS>(leds, NUM_LEDS);
  FastLED.clear();  // clear all pixel data
  FastLED.show();

  // 演出(LED制御)用のタスクを起動
  xTaskCreatePinnedToCore(
    performanceTask,
    "performanceTask",
    8192,
    NULL,
    1,
    NULL,
    0
  );
}

void loop() {
  if (MIDI.read(MIDI_RXCH))
  {
    int data1, data2;
    uint16_t timestamp;
    data1 = MIDI.getData1();
    data2 = MIDI.getData2();
    timestamp = millis();
    if (MIDI.getType() == midi::NoteOn)
    {
      onNoteOn(MIDI_RXCH, data1, data2, timestamp);
    }
    if (MIDI.getType() == midi::ControlChange)
    {
      onControlChange(MIDI_RXCH, data1, data2, timestamp);
    }
  }
}


void onNoteOn(uint8_t channel, uint8_t note, uint8_t velocity, uint16_t timestamp) {
  brightness = (uint8_t)((float)velocity * (float)MAX_BRIGHTNESS / (float)MIDI_VALUE_MAX);
  //  Serial.printf("Received note on : channel %d, note %d, velocity %d, bright %d (timestamp %dms)\n", channel, note, velocity, brightness, timestamp);
  latestNoteOnTimestamp = timestamp;
  isPlaying = true; // 演出の再生開始をセット
}


void onControlChange(uint8_t channel, uint8_t controller, uint8_t value, uint16_t timestamp)
{
  //  Serial.printf("Received control change : channel %d, controller %d, value %d (timestamp %dms)\n", channel, controller, value, timestamp);
  switch (controller) {
    case MIDI_CC_DURATION: // 1演出あたりの長さを設定
      duration = ((uint8_t)value * 100.f / (float)MIDI_VALUE_MAX) / 2 + 10;
      break;
    case MIDI_CC_COLOR_HUE: // LEDの色を設定
      hue = (uint8_t)value * (float)MAX_HUE / (float)MIDI_VALUE_MAX;
      break;
    case MIDI_CC_COLOR_SATURATION: // LEDの彩度を設定
      saturation = (uint8_t)value * (float)MAX_SATURATION / (float)MIDI_VALUE_MAX;
      break;
  }
}


uint8_t gamingHueIndex = 0;


/** 演出処理のタスク */
void performanceTask(void *pvParameters) {
  while (1) {
    if (isPlaying) {
      if (currentNoteOnTimestamp != latestNoteOnTimestamp) { // 新着noteOn有り = 今の演出をキャンセル
        playIndex = 0;
        currentNoteOnTimestamp = latestNoteOnTimestamp;
        //        M5.dis.drawpix(0, 0x303000);
      }
      // playIndexに応じて光をフェードアウトする
      uint8_t v = (int)((float)brightness * (1.f - (float)( (float)playIndex / (float)MAX_PERFORMANCE_STEP)));
      //      Serial.print(v);
      //      Serial.print(", ");
      //      Serial.println(playIndex);

      for (int i = 0; i < NUM_LEDS; i++)
      {
        leds[i] = CHSV(hue, saturation, v);
      }

      playIndex++;

      if (MAX_PERFORMANCE_STEP <= playIndex) { // 最後のstep = 演出終了
        isPlaying = false;
        FastLED.clear();

      }
      FastLED.show();
      vTaskDelay(duration);
      //      M5.dis.drawpix(0, 0x000000);
    }
    else {
      FastLED.clear();
      FastLED.show();
      vTaskDelay(10);
    }
  }
}
