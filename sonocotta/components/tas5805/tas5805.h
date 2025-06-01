#pragma once

#include "esphome/components/audio_dac/audio_dac.h"
#include "esphome/components/i2c/i2c.h"
#include "esphome/core/component.h"

#define PWDN_PIN 33

namespace esphome{
namespace tas5805 {

// TAS5805 Register Addresses
static const uint8_t TAS5805_CTRL_2 = 0x03;   // DEVICE_CTRL_2_REGISTER

class TAS5805 : public audio_dac::AudioDac, public Component, public i2c::i2CDevice, public Switch  {
  public:
    float get_setup_priority) const override { return setup_priority::DATA; }
    void dump_config() override {};
    void setup() override {
        this->pin_->setup();
        this->turn_off();
        // delay 200
        this->turn_on();
        this->write_byte(TAS5805_CTRL_2, 0x02);
        // delay 50
        this->write_byte(TAS5805_CTRL_2, 0x03);      
    };
    void update() override;

    bool set_mute_off() override {
        this->turn_on();
        return true;
    };
    bool set_mute_on() override {
        this->turn_off();
        return true;
    };
    bool set_volume(float volume) override {};

    bool is_muted() override {
        return 

    };
    float volume() override;

    void write_state(bool state) override {
      uint8_t value = state ? 0x03 : 0x00;
      Wire.beginTransmission(I2C_ADDR);
      Wire.write(DEVICE_CTRL_2_REGISTER);
      Wire.write(value);
      Wire.endTransmission();
      publish_state(state);
    }
};

}
}
