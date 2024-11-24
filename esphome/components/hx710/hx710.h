#pragma once

#include "esphome/core/component.h"
#include "esphome/core/hal.h"
#include "esphome/components/sensor/sensor.h"

#include <cinttypes>

namespace esphome {
namespace hx710 {

enum HX710Mode { HX710_DIFFERENTIAL_INPUT_10HZ = 1, HX710_OTHER_INPUT_40HZ = 2, HX710_DIFFERENTIAL_INPUT_40HZ = 3 };

class HX710Sensor : public sensor::Sensor, public PollingComponent {
 public:
  void set_dout_pin(GPIOPin *dout_pin) { dout_pin_ = dout_pin; }
  void set_sck_pin(GPIOPin *sck_pin) { sck_pin_ = sck_pin; }
  void set_gain(HX710Mode gain) { gain_ = gain; }

  void setup() override;
  void dump_config() override;
  float get_setup_priority() const override;
  void update() override;

 protected:
  bool read_sensor_(uint32_t *result);

  GPIOPin *dout_pin_;
  GPIOPin *sck_pin_;
  HX710Mode gain_{HX710_DIFFERENTIAL_INPUT_10HZ};
};

}  // namespace hx710
}  // namespace esphome
