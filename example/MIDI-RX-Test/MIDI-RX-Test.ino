// for GROVE MIDI Unit and M5Atom

#include <M5Atom.h>
#include <MIDI.h>
//#include <FastLED.h>
MIDI_CREATE_INSTANCE(HardwareSerial, Serial2, MIDI);

CRGB dispColor(uint8_t r, uint8_t g, uint8_t b) {
  return (CRGB)((r << 16) | (g << 8) | b);
}

int NoteCharData[325] = {

  0, 1, 1, 0, 0,
  1, 0, 0, 1, 0,
  1, 0, 0, 0, 0,
  1, 0, 0, 1, 0,
  0, 1, 1, 0, 0,

  0, 1, 1, 0, 2,
  1, 0, 0, 1, 2,
  1, 0, 0, 0, 2,
  1, 0, 0, 1, 0,
  0, 1, 1, 0, 0,

  1, 1, 1, 0, 0,
  1, 0, 0, 1, 0,
  1, 0, 0, 1, 0,
  1, 0, 0, 1, 0,
  1, 1, 1, 0, 0,

  1, 1, 1, 0, 2,
  1, 0, 0, 1, 2,
  1, 0, 0, 1, 2,
  1, 0, 0, 1, 0,
  1, 1, 1, 0, 0,

  1, 1, 1, 1, 0,
  1, 0, 0, 0, 0,
  1, 1, 1, 0, 0,
  1, 0, 0, 0, 0,
  1, 1, 1, 1, 0,

  1, 1, 1, 1, 0,
  1, 0, 0, 0, 0,
  1, 1, 1, 0, 0,
  1, 0, 0, 0, 0,
  1, 0, 0, 0, 0,

  1, 1, 1, 1, 2,
  1, 0, 0, 0, 2,
  1, 1, 1, 0, 2,
  1, 0, 0, 0, 0,
  1, 0, 0, 0, 0,

  0, 1, 1, 0, 0,
  1, 0, 0, 0, 0,
  1, 0, 1, 1, 0,
  1, 0, 0, 1, 0,
  0, 1, 1, 0, 0,

  0, 1, 1, 0, 2,
  1, 0, 0, 0, 2,
  1, 0, 1, 1, 2,
  1, 0, 0, 1, 0,
  0, 1, 1, 0, 0,

  0, 1, 1, 0, 0,
  1, 0, 0, 1, 0,
  1, 1, 1, 1, 0,
  1, 0, 0, 1, 0,
  1, 0, 0, 1, 0,

  0, 1, 1, 0, 2,
  1, 0, 0, 1, 2,
  1, 1, 1, 1, 2,
  1, 0, 0, 1, 0,
  1, 0, 0, 1, 0,


  1, 1, 1, 0, 0,
  1, 0, 0, 1, 0,
  1, 1, 1, 0, 0,
  1, 0, 0, 1, 0,
  1, 1, 1, 0, 0,

  0, 0, 0, 0, 0,
  0, 0, 0, 0, 0,
  0, 0, 0, 0, 0,
  0, 0, 0, 0, 0,
  0, 0, 0, 0, 0
};



void setup() {
  Serial.begin(115200);
  Serial2.begin(31250, SERIAL_8N1, 32, 26);
  M5.begin(true, false, true);
  delay(50);
  M5.dis.drawpix(0, 0x000000);
  MIDI.turnThruOff();

  // MIDI受信用のタスクを起動
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

int noteNO = 254;
int lastnoteNO;
int note12;

void loop() {
  while (noteNO != 255) {
    
    if (noteNO == 254) note12 = 12;
    else note12 = (noteNO + 12) % 12;
    for (int i = 0; i < 25; i++) {
      int num = NoteCharData[note12 * 25 + i];
      int col[3] = {num * 255, num * 255, num * 255};
      if (num == 2) {
        col[0] = 255;
        col[1] = 0;
        col[2] = 0;
      }
      M5.dis.drawpix(i, dispColor(col[0], col[1], col[2]));
    }
    noteNO = 255;
  }
  delay(1);
}

/** 受信側タスク */
void performanceTask(void *pvParameters) {
  while (1) {
    if (MIDI.read()) {
      if (MIDI.getType() == midi::NoteOn) {
        noteNO = MIDI.getData1();
        lastnoteNO = noteNO;
        Serial.print("Receive NoteON,Note NO:");
        Serial.println(noteNO);
      }
      if (MIDI.getType() == midi::NoteOff) {
        if (lastnoteNO == MIDI.getData1()){
        noteNO = 254;
        }
        Serial.print("Receive NoteOFF,Note NO:");
        Serial.println(MIDI.getData1());
      }
    }
    vTaskDelay(1);  
  }
}
