#pragma once

#ifdef USE_ESP32_FRAMEWORK_ARDUINO

#include "../i2s_audio.h"

#include <driver/i2s.h>

#include "esphome/components/media_player/media_player.h"
#include "esphome/core/component.h"
#include "esphome/core/gpio.h"
#include "esphome/core/helpers.h"

#include <Audio.h>

namespace esphome {
namespace i2s_audio {

enum I2SState : uint8_t {
  I2S_STATE_STOPPED = 0,
  I2S_STATE_STARTING,
  I2S_STATE_RUNNING,
  I2S_STATE_STOPPING,
};

class I2SAudioMediaPlayer : public Component, public Parented<I2SAudioComponent>, public media_player::MediaPlayer {
 public:
  void setup() override;
  float get_setup_priority() const override { return esphome::setup_priority::LATE; }

  void loop() override;

  void dump_config() override;

  void set_dout_pin(uint8_t pin) { this->dout_pin_ = pin; }
  void set_mute_pin(GPIOPin *mute_pin) { this->mute_pin_ = mute_pin; }
#if SOC_I2S_SUPPORTS_DAC
  void set_internal_dac_mode(i2s_dac_mode_t mode) { this->internal_dac_mode_ = mode; }
#endif
  void set_external_dac_channels(uint8_t channels) { this->external_dac_channels_ = channels; }

  void set_i2s_comm_fmt_lsb(bool lsb) { this->i2s_comm_fmt_lsb_ = lsb; }

  media_player::MediaPlayerTraits get_traits() override;

  bool is_muted() const override { return this->muted_; }

  void start();
  void stop();

 protected:
  void control(const media_player::MediaPlayerCall &call) override;

  void mute_();
  void unmute_();
  void set_volume_(float volume, bool publish = true);

  void start_();
  void stop_();
  void play_();

  bool connecttouri_(const std::string &uri);

  I2SState i2s_state_{I2S_STATE_STOPPED};
  std::unique_ptr<Audio> audio_;

  uint8_t dout_pin_{0};

  GPIOPin *mute_pin_{nullptr};
  bool muted_{false};
  float unmuted_volume_{0};

#if SOC_I2S_SUPPORTS_DAC
  i2s_dac_mode_t internal_dac_mode_{I2S_DAC_CHANNEL_DISABLE};
#endif
  uint8_t external_dac_channels_;

  bool i2s_comm_fmt_lsb_;

  HighFrequencyLoopRequester high_freq_;

  optional<std::string> current_url_{};
  bool is_announcement_{false};

 private:
  enum class audio_task_command : uint8_t {
    NONE,
    PLAY,
    STOP,
    PAUSE_RESUME,
    SET_VOLUME,
    PLAY_STATE_CHANGE,
  };

  struct audio_message {
    audio_task_command command;
    union {
      const std::string *uri;  // PLAY request
      bool is_running;         // PLAY, STOP, PAUSE_RESUME and PLAY_STATE_CHANGE response
      float volume;            // SET_VOLUME request
    };
  };

  TaskHandle_t audio_task_handle_;
  QueueHandle_t audio_request_queue_;
  QueueHandle_t audio_response_queue_;

  bool audio_is_running_;

  void create_audio_task_();

  void send_request_(const audio_message &message, const bool wait = false);

  void audio_pause_resume_(const bool wait = false);

  void audio_set_volume_(const float volume);

  audio_task_command process_response_queue_();

  static void audio_task_(void *pvParam);

  bool connecttouri_impl_(const std::string &uri);
};

}  // namespace i2s_audio
}  // namespace esphome

#endif  // USE_ESP32_FRAMEWORK_ARDUINO
