// for GROVE MIDI Unit and M5Atom

#include <M5Atom.h>
#include <MIDI.h>
#include <FastLED.h>
MIDI_CREATE_INSTANCE(HardwareSerial, Serial2, MIDI);

void setup() {
  Serial.begin(115200);
  Serial2.begin(31250, SERIAL_8N1, 32, 26);
  M5.begin(true, false, true);
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

/** 送信側タスク */
void loop() {
  MIDI.sendNoteOn(40, 64, 1);
  Serial.println("Send CH1,Noteon,Note 40,Vel 64");
  delay(1000);
  MIDI.sendNoteOff(40, 64, 1);
  Serial.println("Send CH1,Noteoff,Note 40,Vel 64");
  delay(1000);
}

/** 受信側タスク */
void performanceTask(void *pvParameters) {
  while (1) {
    if (MIDI.read(1)) {
      if (MIDI.getType() == midi::NoteOn && MIDI.getData1() == 40 && MIDI.getData2() == 64) {
        Serial.println("Receive CH1,Noteon,Note 40,Vel 64");
        M5.dis.drawpix(0, 0x0f0f00);
      }
      if (MIDI.getType() == midi::NoteOff && MIDI.getData1() == 40 && MIDI.getData2() == 64) {
        Serial.println("Receive CH1,Noteoff,Note 40,Vel 64");
        M5.dis.drawpix(0, 0x000000);
      }
    }
    vTaskDelay(1);
  }
}
