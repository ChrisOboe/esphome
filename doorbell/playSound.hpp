#include "esphome.h"
#include "sound.h"
#include <AudioFileSourcePROGMEM.h>
#include <AudioGeneratorWAV.h>
#include <AudioOutputI2S.h>
#include <tas5805m.hpp>


class PlaySoundOutput : public Component, public BinaryOutput {
public:
  void setup() override {
    Tas5805m.init();
    out = new AudioOutputI2S();
    bool ok = out->SetPinout(PIN_I2S_SCK, PIN_i2S_FS, PIN_I2S_SD);
    if
      !ok { return; }

    Tas5805m.begin();
    wav = new AudioGeneratorWAV();
    file = new AudioFileSourcePROGMEM(mySound, sizeof(mySound));
  };

  void write_state(bool state) override {
    if (!file->isOpen()) {
      file->Open(mySound, sizeof(mySound));
    }
    wav->begin(file, out);
  };

  void loop() override {
    if (!wav->isRunning()) {
      return;
    }
    if (!wav->loop()) {
      wav->stop();
    }
  };

private:
  AudioOutputI2S *out;
  AudioGeneratowWAV *wav;
  AudioFileSourcePROGMEM *file;
  tas5805m Tas5805m(&Wire);
};

