#include "hx710.h"
#include "esphome/core/log.h"
#include "esphome/core/helpers.h"

namespace esphome {
namespace hx710 {

static const char *const TAG = "hx710";

void HX710Sensor::setup() {
  ESP_LOGCONFIG(TAG, "Setting up HX710 '%s'...", this->name_.c_str());
  this->sck_pin_->setup();
  this->dout_pin_->setup();
  this->sck_pin_->digital_write(false);

  // Read sensor once without publishing to set the gain
  this->read_sensor_(nullptr);
}

void HX710Sensor::dump_config() {
  LOG_SENSOR("", "HX710", this);
  LOG_PIN("  DOUT Pin: ", this->dout_pin_);
  LOG_PIN("  SCK Pin: ", this->sck_pin_);
  LOG_UPDATE_INTERVAL(this);
}
float HX710Sensor::get_setup_priority() const { return setup_priority::DATA; }
void HX710Sensor::update() {
  uint32_t result;
  if (this->read_sensor_(&result)) {
    int32_t value = static_cast<int32_t>(result);
    ESP_LOGD(TAG, "'%s': Got value %" PRId32, this->name_.c_str(), value);
    this->publish_state(value);
  }
}
bool HX710Sensor::read_sensor_(uint32_t *result) {
  if (this->dout_pin_->digital_read()) {
    ESP_LOGW(TAG, "HX710 is not ready for new measurements yet!");
    this->status_set_warning();
    return false;
  }

  uint32_t data = 0;
  bool final_dout;

  {
    InterruptLock lock;
    for (uint8_t i = 0; i < 24; i++) {
      this->sck_pin_->digital_write(true);
      delayMicroseconds(1);
      data |= uint32_t(this->dout_pin_->digital_read()) << (23 - i);
      this->sck_pin_->digital_write(false);
      delayMicroseconds(1);
    }

    // Cycle clock pin for gain setting
    for (uint8_t i = 0; i < this->gain_; i++) {
      this->sck_pin_->digital_write(true);
      delayMicroseconds(1);
      this->sck_pin_->digital_write(false);
      delayMicroseconds(1);
    }
    final_dout = this->dout_pin_->digital_read();
  }

  if (!final_dout) {
    ESP_LOGW(TAG, "HX710 DOUT pin not high after reading (data 0x%" PRIx32 ")!", data);
    this->status_set_warning();
    return false;
  }

  this->status_clear_warning();

  if (data & 0x800000ULL) {
    data |= 0xFF000000ULL;
  }

  if (result != nullptr)
    *result = data;
  return true;
}

}  // namespace hx710
}  // namespace esphome
