/*
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include "legacy_display_config.h"

#include <errno.h>

#include "hwc_session.h"

namespace sdm {
namespace {

DisplayConfig::DisplayType ToDisplayType(LegacyIDisplayConfig::DisplayType type) {
  switch (type) {
    case LegacyIDisplayConfig::DisplayType::DISPLAY_PRIMARY:
      return DisplayConfig::DisplayType::kPrimary;
    case LegacyIDisplayConfig::DisplayType::DISPLAY_EXTERNAL:
      return DisplayConfig::DisplayType::kExternal;
    case LegacyIDisplayConfig::DisplayType::DISPLAY_VIRTUAL:
      return DisplayConfig::DisplayType::kVirtual;
    default:
      return DisplayConfig::DisplayType::kInvalid;
  }
}

DisplayConfig::ExternalStatus ToExternalStatus(
    LegacyIDisplayConfig::DisplayExternalStatus status) {
  switch (status) {
    case LegacyIDisplayConfig::DisplayExternalStatus::EXTERNAL_OFFLINE:
      return DisplayConfig::ExternalStatus::kOffline;
    case LegacyIDisplayConfig::DisplayExternalStatus::EXTERNAL_ONLINE:
      return DisplayConfig::ExternalStatus::kOnline;
    case LegacyIDisplayConfig::DisplayExternalStatus::EXTERNAL_PAUSE:
      return DisplayConfig::ExternalStatus::kPause;
    case LegacyIDisplayConfig::DisplayExternalStatus::EXTERNAL_RESUME:
      return DisplayConfig::ExternalStatus::kResume;
    default:
      return DisplayConfig::ExternalStatus::kInvalid;
  }
}

DisplayConfig::DynRefreshRateOp ToDynRefreshRateOp(
    LegacyIDisplayConfig::DisplayDynRefreshRateOp op) {
  switch (op) {
    case LegacyIDisplayConfig::DisplayDynRefreshRateOp::DISABLE_METADATA_DYN_REFRESH_RATE:
      return DisplayConfig::DynRefreshRateOp::kDisableMetadata;
    case LegacyIDisplayConfig::DisplayDynRefreshRateOp::ENABLE_METADATA_DYN_REFRESH_RATE:
      return DisplayConfig::DynRefreshRateOp::kEnableMetadata;
    case LegacyIDisplayConfig::DisplayDynRefreshRateOp::SET_BINDER_DYN_REFRESH_RATE:
      return DisplayConfig::DynRefreshRateOp::kSetBinder;
    default:
      return DisplayConfig::DynRefreshRateOp::kInvalid;
  }
}

DisplayConfig::PowerMode ToPowerMode(LegacyIDisplayConfig::PowerMode mode) {
  switch (mode) {
    case LegacyIDisplayConfig::PowerMode::Off:
      return DisplayConfig::PowerMode::kOff;
    case LegacyIDisplayConfig::PowerMode::Doze:
      return DisplayConfig::PowerMode::kDoze;
    case LegacyIDisplayConfig::PowerMode::On:
      return DisplayConfig::PowerMode::kOn;
    case LegacyIDisplayConfig::PowerMode::DozeSuspend:
      return DisplayConfig::PowerMode::kDozeSuspend;
  }

  return DisplayConfig::PowerMode::kOff;
}

LegacyIDisplayConfig::DisplayPortType ToLegacyDisplayPortType(
    DisplayConfig::DisplayPortType type) {
  switch (type) {
    case DisplayConfig::DisplayPortType::kDefault:
      return LegacyIDisplayConfig::DisplayPortType::DISPLAY_PORT_DEFAULT;
    case DisplayConfig::DisplayPortType::kDsi:
      return LegacyIDisplayConfig::DisplayPortType::DISPLAY_PORT_DSI;
    case DisplayConfig::DisplayPortType::kDtv:
      return LegacyIDisplayConfig::DisplayPortType::DISPLAY_PORT_DTV;
    case DisplayConfig::DisplayPortType::kWriteback:
      return LegacyIDisplayConfig::DisplayPortType::DISPLAY_PORT_WRITEBACK;
    case DisplayConfig::DisplayPortType::kLvds:
      return LegacyIDisplayConfig::DisplayPortType::DISPLAY_PORT_LVDS;
    case DisplayConfig::DisplayPortType::kEdp:
      return LegacyIDisplayConfig::DisplayPortType::DISPLAY_PORT_EDP;
    case DisplayConfig::DisplayPortType::kDp:
      return LegacyIDisplayConfig::DisplayPortType::DISPLAY_PORT_DP;
    default:
      return LegacyIDisplayConfig::DisplayPortType::INVALID;
  }
}

LegacyIDisplayConfig::DisplayAttributes ToLegacyAttributes(
    const DisplayConfig::Attributes &attributes) {
  LegacyIDisplayConfig::DisplayAttributes legacy = {};
  legacy.vsyncPeriod = attributes.vsync_period;
  legacy.xRes = attributes.x_res;
  legacy.yRes = attributes.y_res;
  legacy.xDpi = attributes.x_dpi;
  legacy.yDpi = attributes.y_dpi;
  legacy.panelType = ToLegacyDisplayPortType(attributes.panel_type);
  legacy.isYuv = attributes.is_yuv;
  return legacy;
}

int NoService() {
  return -ENODEV;
}

}  // namespace

LegacyDisplayConfig::LegacyDisplayConfig(HWCSession *hwc_session)
    : hwc_session_(hwc_session) {
  if (hwc_session_) {
    hwc_session_->RegisterClientContext({}, &config_intf_);
  }
}

LegacyDisplayConfig::~LegacyDisplayConfig() {
  if (hwc_session_ && config_intf_) {
    hwc_session_->UnRegisterClientContext(config_intf_);
    config_intf_ = nullptr;
  }
}

::android::hardware::Return<void> LegacyDisplayConfig::isDisplayConnected(
    LegacyIDisplayConfig::DisplayType dpy, isDisplayConnected_cb _hidl_cb) {
  bool connected = false;
  int error = config_intf_ ? config_intf_->IsDisplayConnected(ToDisplayType(dpy), &connected)
                           : NoService();
  _hidl_cb(error, connected);
  return ::android::hardware::Void();
}

::android::hardware::Return<int32_t> LegacyDisplayConfig::setSecondayDisplayStatus(
    LegacyIDisplayConfig::DisplayType dpy,
    LegacyIDisplayConfig::DisplayExternalStatus status) {
  return config_intf_ ? config_intf_->SetDisplayStatus(ToDisplayType(dpy),
                                                       ToExternalStatus(status))
                      : NoService();
}

::android::hardware::Return<int32_t> LegacyDisplayConfig::configureDynRefeshRate(
    LegacyIDisplayConfig::DisplayDynRefreshRateOp op, uint32_t refresh_rate) {
  return config_intf_ ? config_intf_->ConfigureDynRefreshRate(ToDynRefreshRateOp(op),
                                                              refresh_rate)
                      : NoService();
}

::android::hardware::Return<void> LegacyDisplayConfig::getConfigCount(
    LegacyIDisplayConfig::DisplayType dpy, getConfigCount_cb _hidl_cb) {
  uint32_t count = 0;
  int error = config_intf_ ? config_intf_->GetConfigCount(ToDisplayType(dpy), &count)
                           : NoService();
  _hidl_cb(error, count);
  return ::android::hardware::Void();
}

::android::hardware::Return<void> LegacyDisplayConfig::getActiveConfig(
    LegacyIDisplayConfig::DisplayType dpy, getActiveConfig_cb _hidl_cb) {
  uint32_t config = 0;
  int error = config_intf_ ? config_intf_->GetActiveConfig(ToDisplayType(dpy), &config)
                           : NoService();
  _hidl_cb(error, config);
  return ::android::hardware::Void();
}

::android::hardware::Return<int32_t> LegacyDisplayConfig::setActiveConfig(
    LegacyIDisplayConfig::DisplayType dpy, uint32_t config) {
  return config_intf_ ? config_intf_->SetActiveConfig(ToDisplayType(dpy), config)
                      : NoService();
}

::android::hardware::Return<void> LegacyDisplayConfig::getDisplayAttributes(
    uint32_t config_index, LegacyIDisplayConfig::DisplayType dpy,
    getDisplayAttributes_cb _hidl_cb) {
  DisplayConfig::Attributes attributes = {};
  int error = config_intf_
                  ? config_intf_->GetDisplayAttributes(config_index, ToDisplayType(dpy),
                                                       &attributes)
                  : NoService();
  _hidl_cb(error, ToLegacyAttributes(attributes));
  return ::android::hardware::Void();
}

::android::hardware::Return<int32_t> LegacyDisplayConfig::setPanelBrightness(uint32_t level) {
  return config_intf_ ? config_intf_->SetPanelBrightness(level) : NoService();
}

::android::hardware::Return<void> LegacyDisplayConfig::getPanelBrightness(
    getPanelBrightness_cb _hidl_cb) {
  uint32_t level = 0;
  int error = config_intf_ ? config_intf_->GetPanelBrightness(&level) : NoService();
  _hidl_cb(error, level);
  return ::android::hardware::Void();
}

::android::hardware::Return<int32_t> LegacyDisplayConfig::minHdcpEncryptionLevelChanged(
    LegacyIDisplayConfig::DisplayType dpy, uint32_t min_enc_level) {
  return config_intf_
             ? config_intf_->MinHdcpEncryptionLevelChanged(ToDisplayType(dpy), min_enc_level)
             : NoService();
}

::android::hardware::Return<int32_t> LegacyDisplayConfig::refreshScreen() {
  return config_intf_ ? config_intf_->RefreshScreen() : NoService();
}

::android::hardware::Return<int32_t> LegacyDisplayConfig::controlPartialUpdate(
    LegacyIDisplayConfig::DisplayType dpy, bool enable) {
  return config_intf_ ? config_intf_->ControlPartialUpdate(ToDisplayType(dpy), enable)
                      : NoService();
}

::android::hardware::Return<int32_t> LegacyDisplayConfig::toggleScreenUpdate(bool on) {
  return config_intf_ ? config_intf_->ToggleScreenUpdate(on) : NoService();
}

::android::hardware::Return<int32_t> LegacyDisplayConfig::setIdleTimeout(uint32_t value) {
  return config_intf_ ? config_intf_->SetIdleTimeout(value) : NoService();
}

::android::hardware::Return<void> LegacyDisplayConfig::getHDRCapabilities(
    LegacyIDisplayConfig::DisplayType dpy, getHDRCapabilities_cb _hidl_cb) {
  DisplayConfig::HDRCapsParams caps = {};
  int error = config_intf_ ? config_intf_->GetHDRCapabilities(ToDisplayType(dpy), &caps)
                           : NoService();

  LegacyIDisplayConfig::DisplayHDRCapabilities legacy_caps = {};
  legacy_caps.supportedHdrTypes.resize(caps.supported_hdr_types.size());
  for (size_t i = 0; i < caps.supported_hdr_types.size(); i++) {
    legacy_caps.supportedHdrTypes[i] = caps.supported_hdr_types[i];
  }
  legacy_caps.maxLuminance = caps.max_luminance;
  legacy_caps.maxAvgLuminance = caps.max_avg_luminance;
  legacy_caps.minLuminance = caps.min_luminance;

  _hidl_cb(error, legacy_caps);
  return ::android::hardware::Void();
}

::android::hardware::Return<int32_t> LegacyDisplayConfig::setCameraLaunchStatus(uint32_t on) {
  return config_intf_ ? config_intf_->SetCameraLaunchStatus(on) : NoService();
}

::android::hardware::Return<void> LegacyDisplayConfig::displayBWTransactionPending(
    displayBWTransactionPending_cb _hidl_cb) {
  bool pending = false;
  int error = config_intf_ ? config_intf_->DisplayBWTransactionPending(&pending) : NoService();
  _hidl_cb(error, pending);
  return ::android::hardware::Void();
}

::android::hardware::Return<int32_t> LegacyDisplayConfig::setDisplayAnimating(
    uint64_t display_id, bool animating) {
  return config_intf_ ? config_intf_->SetDisplayAnimating(display_id, animating) : NoService();
}

::android::hardware::Return<int32_t> LegacyDisplayConfig::setDisplayIndex(
    LegacyIDisplayConfig::DisplayTypeExt disp_type, uint32_t base, uint32_t count) {
  (void)disp_type;
  (void)base;
  (void)count;
  // DisplayConfig 2.0 removed this legacy API. The old sm8250 implementation
  // treated it as a successful no-op, so preserve that behavior.
  return 0;
}

::android::hardware::Return<int32_t> LegacyDisplayConfig::controlIdlePowerCollapse(
    bool enable, bool synchronous) {
  return config_intf_ ? config_intf_->ControlIdlePowerCollapse(enable, synchronous)
                      : NoService();
}

::android::hardware::Return<void> LegacyDisplayConfig::getWriteBackCapabilities(
    getWriteBackCapabilities_cb _hidl_cb) {
  bool supported = false;
  int error = config_intf_ ? config_intf_->GetWriteBackCapabilities(&supported) : NoService();
  LegacyIDisplayConfig::WriteBackCapabilities caps = {};
  caps.isWbUbwcSupported = supported;
  _hidl_cb(error, caps);
  return ::android::hardware::Void();
}

::android::hardware::Return<int32_t> LegacyDisplayConfig::SetDisplayDppsAdROI(
    uint32_t display_id, uint32_t h_start, uint32_t h_end, uint32_t v_start,
    uint32_t v_end, uint32_t factor_in, uint32_t factor_out) {
  return config_intf_ ? config_intf_->SetDisplayDppsAdROI(display_id, h_start, h_end,
                                                          v_start, v_end, factor_in,
                                                          factor_out)
                      : NoService();
}

::android::hardware::Return<int32_t> LegacyDisplayConfig::updateVSyncSourceOnPowerModeOff() {
  return config_intf_ ? config_intf_->UpdateVSyncSourceOnPowerModeOff() : NoService();
}

::android::hardware::Return<int32_t> LegacyDisplayConfig::updateVSyncSourceOnPowerModeDoze() {
  return config_intf_ ? config_intf_->UpdateVSyncSourceOnPowerModeDoze() : NoService();
}

::android::hardware::Return<int32_t> LegacyDisplayConfig::setPowerMode(
    uint32_t disp_id, LegacyIDisplayConfig::PowerMode power_mode) {
  return config_intf_ ? config_intf_->SetPowerMode(disp_id, ToPowerMode(power_mode))
                      : NoService();
}

::android::hardware::Return<bool> LegacyDisplayConfig::isPowerModeOverrideSupported(
    uint32_t disp_id) {
  bool supported = false;
  if (config_intf_) {
    config_intf_->IsPowerModeOverrideSupported(disp_id, &supported);
  }
  return supported;
}

::android::hardware::Return<bool> LegacyDisplayConfig::isHDRSupported(uint32_t disp_id) {
  bool supported = false;
  if (config_intf_) {
    config_intf_->IsHDRSupported(disp_id, &supported);
  }
  return supported;
}

::android::hardware::Return<bool> LegacyDisplayConfig::isWCGSupported(uint32_t disp_id) {
  bool supported = false;
  if (config_intf_) {
    config_intf_->IsWCGSupported(disp_id, &supported);
  }
  return supported;
}

::android::hardware::Return<int32_t> LegacyDisplayConfig::setLayerAsMask(
    uint32_t disp_id, uint64_t layer_id) {
  return config_intf_ ? config_intf_->SetLayerAsMask(disp_id, layer_id) : NoService();
}

::android::hardware::Return<void> LegacyDisplayConfig::getDebugProperty(
    const ::android::hardware::hidl_string &prop_name, getDebugProperty_cb _hidl_cb) {
  std::string value;
  int error = config_intf_ ? config_intf_->GetDebugProperty(prop_name.c_str(), &value)
                           : NoService();
  _hidl_cb(value, error);
  return ::android::hardware::Void();
}

::android::hardware::Return<void> LegacyDisplayConfig::getActiveBuiltinDisplayAttributes(
    getDisplayAttributes_cb _hidl_cb) {
  DisplayConfig::Attributes attributes = {};
  int error = config_intf_ ? config_intf_->GetActiveBuiltinDisplayAttributes(&attributes)
                           : NoService();
  _hidl_cb(error, ToLegacyAttributes(attributes));
  return ::android::hardware::Void();
}

::android::hardware::Return<int32_t> LegacyDisplayConfig::setPanelLuminanceAttributes(
    uint32_t disp_id, float min_lum, float max_lum) {
  return config_intf_
             ? config_intf_->SetPanelLuminanceAttributes(disp_id, min_lum, max_lum)
             : NoService();
}

::android::hardware::Return<bool> LegacyDisplayConfig::isBuiltInDisplay(uint32_t disp_id) {
  bool is_builtin = false;
  if (config_intf_) {
    config_intf_->IsBuiltInDisplay(disp_id, &is_builtin);
  }
  return is_builtin;
}

}  // namespace sdm
