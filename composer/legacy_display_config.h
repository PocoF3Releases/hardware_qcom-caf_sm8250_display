/*
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef __LEGACY_DISPLAY_CONFIG_H__
#define __LEGACY_DISPLAY_CONFIG_H__

#include <config/client_interface.h>
#include <vendor/display/config/1.9/IDisplayConfig.h>

namespace sdm {

class HWCSession;

using LegacyIDisplayConfig = vendor::display::config::V1_9::IDisplayConfig;

class LegacyDisplayConfig : public LegacyIDisplayConfig {
 public:
  explicit LegacyDisplayConfig(HWCSession *hwc_session);
  ~LegacyDisplayConfig() override;

  bool IsReady() const { return config_intf_ != nullptr; }

  ::android::hardware::Return<void> isDisplayConnected(
      LegacyIDisplayConfig::DisplayType dpy, isDisplayConnected_cb _hidl_cb) override;
  ::android::hardware::Return<int32_t> setSecondayDisplayStatus(
      LegacyIDisplayConfig::DisplayType dpy,
      LegacyIDisplayConfig::DisplayExternalStatus status) override;
  ::android::hardware::Return<int32_t> configureDynRefeshRate(
      LegacyIDisplayConfig::DisplayDynRefreshRateOp op, uint32_t refresh_rate) override;
  ::android::hardware::Return<void> getConfigCount(
      LegacyIDisplayConfig::DisplayType dpy, getConfigCount_cb _hidl_cb) override;
  ::android::hardware::Return<void> getActiveConfig(
      LegacyIDisplayConfig::DisplayType dpy, getActiveConfig_cb _hidl_cb) override;
  ::android::hardware::Return<int32_t> setActiveConfig(
      LegacyIDisplayConfig::DisplayType dpy, uint32_t config) override;
  ::android::hardware::Return<void> getDisplayAttributes(
      uint32_t config_index, LegacyIDisplayConfig::DisplayType dpy,
      getDisplayAttributes_cb _hidl_cb) override;
  ::android::hardware::Return<int32_t> setPanelBrightness(uint32_t level) override;
  ::android::hardware::Return<void> getPanelBrightness(
      getPanelBrightness_cb _hidl_cb) override;
  ::android::hardware::Return<int32_t> minHdcpEncryptionLevelChanged(
      LegacyIDisplayConfig::DisplayType dpy, uint32_t min_enc_level) override;
  ::android::hardware::Return<int32_t> refreshScreen() override;
  ::android::hardware::Return<int32_t> controlPartialUpdate(
      LegacyIDisplayConfig::DisplayType dpy, bool enable) override;
  ::android::hardware::Return<int32_t> toggleScreenUpdate(bool on) override;
  ::android::hardware::Return<int32_t> setIdleTimeout(uint32_t value) override;
  ::android::hardware::Return<void> getHDRCapabilities(
      LegacyIDisplayConfig::DisplayType dpy, getHDRCapabilities_cb _hidl_cb) override;
  ::android::hardware::Return<int32_t> setCameraLaunchStatus(uint32_t on) override;
  ::android::hardware::Return<void> displayBWTransactionPending(
      displayBWTransactionPending_cb _hidl_cb) override;

  ::android::hardware::Return<int32_t> setDisplayAnimating(
      uint64_t display_id, bool animating) override;
  ::android::hardware::Return<int32_t> setDisplayIndex(
      LegacyIDisplayConfig::DisplayTypeExt disp_type, uint32_t base, uint32_t count) override;
  ::android::hardware::Return<int32_t> controlIdlePowerCollapse(
      bool enable, bool synchronous) override;
  ::android::hardware::Return<void> getWriteBackCapabilities(
      getWriteBackCapabilities_cb _hidl_cb) override;
  ::android::hardware::Return<int32_t> SetDisplayDppsAdROI(
      uint32_t display_id, uint32_t h_start, uint32_t h_end, uint32_t v_start,
      uint32_t v_end, uint32_t factor_in, uint32_t factor_out) override;
  ::android::hardware::Return<int32_t> updateVSyncSourceOnPowerModeOff() override;
  ::android::hardware::Return<int32_t> updateVSyncSourceOnPowerModeDoze() override;
  ::android::hardware::Return<int32_t> setPowerMode(
      uint32_t disp_id, LegacyIDisplayConfig::PowerMode power_mode) override;
  ::android::hardware::Return<bool> isPowerModeOverrideSupported(uint32_t disp_id) override;
  ::android::hardware::Return<bool> isHDRSupported(uint32_t disp_id) override;
  ::android::hardware::Return<bool> isWCGSupported(uint32_t disp_id) override;
  ::android::hardware::Return<int32_t> setLayerAsMask(
      uint32_t disp_id, uint64_t layer_id) override;
  ::android::hardware::Return<void> getDebugProperty(
      const ::android::hardware::hidl_string &prop_name,
      getDebugProperty_cb _hidl_cb) override;
  ::android::hardware::Return<void> getActiveBuiltinDisplayAttributes(
      getDisplayAttributes_cb _hidl_cb) override;
  ::android::hardware::Return<int32_t> setPanelLuminanceAttributes(
      uint32_t disp_id, float min_lum, float max_lum) override;
  ::android::hardware::Return<bool> isBuiltInDisplay(uint32_t disp_id) override;

 private:
  HWCSession *hwc_session_ = nullptr;
  DisplayConfig::ConfigInterface *config_intf_ = nullptr;
};

}  // namespace sdm

#endif  // __LEGACY_DISPLAY_CONFIG_H__
