#ifndef DODGEOVERLAY_H
#define DODGEOVERLAY_H

#include "pch.h"

const std::string PluginName = "Dodge Overlay";

inline ImVec2 operator+(const ImVec2 v1, const ImVec2 v2) {
  return ImVec2{ v1.x + v2.x, v1.y + v2.y };
}

inline ImVec2 operator-(const ImVec2 v1, const ImVec2 v2) {
  return ImVec2{ v1.x - v2.x, v1.y - v2.y };
}

inline ImVec2 operator/(const ImVec2 v1, const int i) {
  return ImVec2{ v1.x / i, v1.y / i };
}

class DodgeOverlayPlugin : public BakkesMod::Plugin::BakkesModPlugin, public BakkesMod::Plugin::PluginSettingsWindow, public BakkesMod::Plugin::PluginWindow {
private:
  bool isWindowOpen = false;
  ImVec2 windowPosition = ImVec2();
  ImVec2 windowSize = ImVec2();
  ImVec2 displaySize = ImVec2(100.0f, 100.0f);
  float scale = 1.0f;
  float finalScale = 1.0f;

  bool showNums = true;
  ImColor stickBorderColor = ImColor(1.0f, 1.0f, 1.0f, 1.0f);
  ImColor stickLocationColor = ImColor(1.0f, 1.0f, 1.0f, 1.0f);
  ImColor dodgeDeadzoneColor = ImColor(1.0f, 1.0f, 1.0f, 1.0f);
  ImVec2 stickLocation = ImVec2();
  float stickLocationSize = 5.0f;
  float dodgeDeadzone = 0.0f;
  float dodgeDeadzoneRoll = 0.0f;
  bool showDodgeDeadzoneBorder = true;
  float dodgeDeadzoneBorderThickness = 1.0f;
  float dodgeDeadzoneCrossedAlpha = 0.1f;

  std::filesystem::path configurationFilePath = "cfg/dodgeoverlay.cfg";

public:
  void onLoad();
  void RenderSettings() override;
  std::string GetPluginName() override;
  void SetImGuiContext(uintptr_t ctx) override;
  void Render() override;
  void RenderImGui();
  std::string GetMenuName() override;
  std::string GetMenuTitle() override;
  bool ShouldBlockInput() override;
  bool IsActiveOverlay() override;
  void OnOpen() override;
  void OnClose() override;
  void writeCfg();
  void onUnload();
};

#endif // DODGEOVERLAY_H