#include "esphome.h"

// docs: https://www.ti.com/lit/ds/symlink/tas5805m.pdf
// implementation: https://github.com/sonocotta/esp32-tas5805m-dac
// implementation: https://github.com/mariosgit/TAS58xx

const uint16_t I2C_ADDRESS = 0x2D;
const uint16_t RESET_CTRL = 0x01;
const uint16_t DEVICE_CTRL_1 = 0x02;
const uint16_t DEVICE_CTRL_2 = 0x03;
const uint16_t SIG_CH_CTRL = 0x28;
const uint16_t SAP_CTRL1 = 0x33;
const uint16_t SAP_CTRL2 = 0x34;
const uint16_t SAP_CTRL3 = 0x35;
const uint16_t FS_MON = 0x37;
const uint16_t DIG_VOL_CTL = 0x4c;
const uint16_t ANA_CTRL = 0x53;
const uint16_t AGAIN = 0x54; 
const uint16_t BQ_WR_CTRL1 = 0x5c;
const uint16_t ADR_PIN_CTRL = 0x60;
const uint16_t ADR_PIN_CONFIG = 0x61;
const uint16_t DSP_MISC = 0x66;
const uint16_t AUTOMUTE_STATE = 0x69;
const uint16_t CHAN_FAULT = 0x70;
const uint16_t GLOBAL_FAULT1 = 0x71;
const uint16_t GLOBAL_FAULT2 = 0x72;
const uint16_t OT_WARNING = 0x73;
const uint16_t PIN_CONTROL1 = 0x74;
const uint16_t FAULT_CLEAR = 0x78;

class Tas5805m : public Component {
  public:
      void setup() override{

        // from the docs:
        // 7.5.3.1 Startup Procedures
        // 1. Configure ADR/ FAULT pin with proper settings for I2C device address.
        // 2. Bring up power supplies (it does not matter if PVDD or DVDD comes up first).
        // 3. Once power supplies are stable, bring up PDN to High and wait 5ms at least, then start SCLK, LRCLK.
        // 4. Once I2S clocks are stable, set the device into HiZ state and enable DSP via the I2C control port.
        // 5. Wait 5ms at least. Then initialize the DSP Coefficient, then set the device to Play state.
        // 6. The device is now in normal operation.

        // Configure FAULT pin
		//write(ADR_PIN_CTRL, 1); // adr pin  output
		//delay(5);
		// ADR_PIN_CONFIG
        // 00000: off (low)
        // 00011: Auto mute flag (asserted when both L and R channels are auto muted)
        // 00100: Auto mute flag for left channel 0101: Auto mute flag for right channel
        // 00110: Clock invalid flag (clock error or clock missing) 00111: Reserved
        // 01001: Reserved
        // 01011: ADR as FAULTZ output
        //write(ADR_PIN_CONFIG, 0b01011);

        // bring up i2s
        //i2s_mode_t mode = I2S_PHILIPS_MODE; 
        //I2S.begin(mode, 8000, 16);


        // bring up PDN
        pinMode(33,OUTPUT);
        digitalWrite(33,LOW);
        delay(20);
        digitalWrite(33,HIGH);
        delay(500);

        //write(DEVICE_CTRL_2, 0x1A); // DSP reset + HiZ + Mute
        //delay(100);
        //write(DEVICE_CTRL_2, 0x0A); // DSP run + HiZ + Mute

        //setBookPage(0,0);

        //write(DEVICE_CTRL_2, 0x08); // mute
        //write(DEVICE_CTRL_2, 0x1A); // DSP reset + HiZ + Mute
        //write(RESET_CTRL, 0x11); // reset DSP and CTL

        //write(SAP_CTRL1, 0x02); //0x02 = 24bit I2S, 0x00 = 16 bit i2s
        //write(DSP_MISC, 0x08); //decouple BQ coefs for LR
        //delay(5);
		////write(DEVICE_CTRL_2, 0x0B);  // Play + Mute
        //write(DEVICE_CTRL_2, 0x03); // play + unmute


        //readStatus();
      }

      void loop() override {
        if (!init) {
            begin();
            init = true;
        };
        //pinMode(33,OUTPUT);
        //digitalWrite(33,HIGH);
        //if (!ok) {
        //    write(FAULT_CLEAR, 1);
        //};
        //readCtrl2();
      }
  private: 
      bool init = false;
      void begin() {
        delay(1000);
        // bring up PDN
        digitalWrite(33,HIGH);
        delay(100);
		write(DEVICE_CTRL_2, 0x02); // HI Z
        delay(100);
		write(DEVICE_CTRL_2, 0x03); // Play
        delay(100);
        readStatus(); 
      };

      bool write(byte reg, byte data) {
        Wire.beginTransmission(I2C_ADDRESS);
        Wire.write(reg);
        Wire.write(data);
        byte retval = Wire.endTransmission();
        if (retval != 0) {
          ESP_LOGW("tas5805m", "Couln't write.");
        };
        return retval == 0;
      }
      bool read (byte reg) {
        Wire.beginTransmission(I2C_ADDRESS);
        Wire.write(reg);
        Wire.endTransmission();
        Wire.requestFrom(I2C_ADDRESS, (uint8_t)1);
        byte result = 0;
        while (Wire.available()) {
            result = Wire.read();
        }
        return result;
      }

      bool setBookPage(byte book, byte page) {
        // Go to Page 0
        bool ok = write(0,0);
        if (!ok) {
            ESP_LOGW("tas5805m", "Couln't go to page 0");
            return false;
        };
        // Change Book
        ok = write(0x7f, book);
        if (!ok) {
            ESP_LOGW("tas5805m", "Couln't change book");
            return false;
        };

        // Go to Page 
        ok = write(0, page);
        if (!ok) {
            ESP_LOGW("tas5805m", "Couln't change page");
            return false;
        };
        return true;
      }

      void readCtrl2() {
          enum DC2_BITS
          {
              CTRL_STATE_DEEPSLEEP = 0x0,
              CTRL_STATE_SLEEP     = 0x1,
              CTRL_STATE_HIZ       = 0x2,
              CTRL_STATE_PLAY      = 0x3,
              MUTE                 = 0x8
          };

          setBookPage(0,0);
          byte state = read (DEVICE_CTRL_2);
          ESP_LOGD("tas5805m", "DEVICE_CTRL_2: %#01x", state);

          if (state & MUTE) { ESP_LOGD("tas5805m", "Muted");}
          if (state & CTRL_STATE_PLAY) { ESP_LOGD("tas5805m", "State: play");}
          if (state & CTRL_STATE_HIZ) { ESP_LOGD("tas5805m", "State: hiz");}
          if (state & CTRL_STATE_SLEEP) { ESP_LOGD("tas5805m", "State: sleep");}
          if (state & CTRL_STATE_DEEPSLEEP) { ESP_LOGD("tas5805m", "State: deepsleep");}
      };

      bool readStatus() {
          enum GC_BITS
          {
              CH1_DC_1 = 0x08,
              CH2_DC_1 = 0x04,
              CH1_OC_I = 0x02,
              CH2_OC_I = 0x01,
              GC1_OTP_CRC_ERROR = 0x80,
              GC1_BQ_WR_ERROR = 0x40,
              GC1_CLK_FAULT_I = 0x04,
              GC1_PVDD_OV_I = 0x02,
              GC1_PVDD_UV_I = 0x01,
              GC2_OTSD_I = 0x01
          };

          setBookPage(0,0);

		  bool error = false;
          byte chf = read(CHAN_FAULT);
          ESP_LOGD("tas5805m", "CHAN_FAULT: %#01x", chf);

          byte gf1 = read(GLOBAL_FAULT1);
          ESP_LOGD("tas5805m", "GLOBAL_FAULT1: %#01x", gf1);

          byte gf2 = read(GLOBAL_FAULT2);
          ESP_LOGD("tas5805m", "GLOBAL_FAULT2: %#01x", gf2);

          if(chf&0xff || gf1&0xc7 || gf2&0x01)
          {
              if(chf & CH1_DC_1) { ESP_LOGW("tas5805m", "Left channel DC fault."); }
              if(chf & CH2_DC_1) { ESP_LOGW("tas5805m", "Right channel DC fault."); }
              if(chf & CH1_OC_I) { ESP_LOGW("tas5805m", "Left channel over current fault."); }
              if(chf & CH2_OC_I) { ESP_LOGW("tas5805m", "Right channel over current fault."); }
              if(gf1 & GC1_OTP_CRC_ERROR) { ESP_LOGW("tas5805m", "Indicate OTP CRC check error."); }
              if(gf1 & GC1_BQ_WR_ERROR) { ESP_LOGW("tas5805m", "The recent BQ is written failed."); }
              if(gf1 & GC1_CLK_FAULT_I) { ESP_LOGW("tas5805m", "Clock fault."); }
              if(gf1 & GC1_PVDD_OV_I) { ESP_LOGW("tas5805m", "PVDD OverVoltage fault."); }
              if(gf1 & GC1_PVDD_UV_I) { ESP_LOGW("tas5805m", "PVDD UnderVoltage fault."); }
              if(gf2 & GC2_OTSD_I) { ESP_LOGW("tas5805m", "Over temperature shut down fault."); }
              error = true;
          }

          byte otw = read(OT_WARNING);
          if (otw&0x04) {
              ESP_LOGW("tas5805m", "Over temperature warning.");
              error = true;
          }

          return error;
      }
};
