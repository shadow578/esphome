#pragma once

#ifdef USE_ARDUINO

#include "esphome/core/component.h"
#include "esphome/components/spi/spi.h"

#include <SD.h>

namespace esphome {
namespace sd_card {

class SDCardComponent : public Component,
                        public spi::SPIDevice<spi::BIT_ORDER_MSB_FIRST, spi::CLOCK_POLARITY_LOW,
                                              spi::CLOCK_PHASE_LEADING, spi::DATA_RATE_4MHZ> {
 public:
  void setup() override;
  void dump_config() override;

  /**
   * @brief get the SDFS instance
   */
  fs::SDFS *get_fs() { return fs_.get(); }

  /**
   * @brief is the filesystem mounted?
   */
  bool is_mounted() { return is_mounted_; }

  /**
   * @brief attempt to mount the filesystem
   * @return true if successful
   */
  bool mount();

  /**
   * @brief list files and directories in the filesystem
   * @param path the path to list
   * @param recursive list subdirectories recursively?
   *
   * @note prints to ESP_LOGI
   */
  void ls(std::string path = "/", const bool recursive = true) { ls_(path, recursive); }

 private:
  void ls_(std::string path, const bool recursive, int indent = 0);

  std::unique_ptr<fs::SDFS> fs_;
  bool is_mounted_{false};
};

class SDCardDevice {
 public:
  void set_sd_card_parent(SDCardComponent *parent) { this->sd_card_parent_ = parent; }

 protected:
  SDCardComponent *sd_card_parent_;
};

}  // namespace sd_card
}  // namespace esphome

#endif  // USE_ARDUINO
