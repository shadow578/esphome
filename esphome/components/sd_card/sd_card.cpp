#include "sd_card.h"

#ifdef USE_ARDUINO

#include "esphome/core/log.h"

#include "vfs_api.h"
#include "FS.h"

namespace esphome {
namespace sd_card {
static const char *const TAG = "sd_card";

void SDCardComponent::setup() {
  ESP_LOGI(TAG, "SD Card Setup");
  this->spi_setup();

  auto vfs = new VFSImpl();
  this->fs_ = make_unique<fs::SDFS>(FSImplPtr(vfs));

  if (mount()) {
    ls();
  }
}

void SDCardComponent::dump_config() {
  LOG_PIN("  CS Pin: ", this->cs_);
}

bool SDCardComponent::mount() {
  auto spi = this->parent_->get_interface();
  if (spi == nullptr) {
    ESP_LOGE(TAG, "Failed to get SPI interface");
    return false;
  }

  this->is_mounted_ = this->fs_->begin(static_cast<InternalGPIOPin*>(this->cs_)->get_pin(), *spi);

  if (this->is_mounted_) {
    ESP_LOGI(TAG, "SD Card mounted");
  } else {
    ESP_LOGW(TAG, "Failed to mount SD Card");
  }

  return this->is_mounted_;
}

void SDCardComponent::ls_(std::string path, const bool recursive, int indent) {
  if (!this->is_mounted_) {
    return;
  }

  File root = this->fs_->open(path.c_str());
  if (!root) {
    ESP_LOGE(TAG, "Failed to open directory %s", path.c_str());
    return;
  }

  ESP_LOGI(TAG, "%*s%s", indent, "", path.c_str());
  if (!root.isDirectory()) {
    // can only list files within a directory, so stop here
    return;
  }
  
  indent++;
  File child = root.openNextFile();
  while (child) {
    ESP_LOGI(TAG, "%*s%s", indent, "", child.name());

    // ls child directories
    if (recursive && child.isDirectory()) {
      ls_(child.name(), recursive, indent);
    }

    // next file
    child = root.openNextFile();
  }
}

} // namespace sd_card
} // namespace esphome

#endif  // USE_ARDUINO
