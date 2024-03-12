#include "pch.h"
#include "DodgeOverlay.h"
#include <fstream>

using std::string;
using namespace DodgeOverlay;

const string MENU_NAME = "dodgeoverlay";
const string PLUGIN_NAME = "Dodge Overlay";


BAKKESMOD_PLUGIN(DodgeOverlayPlugin, PLUGIN_NAME.c_str(), "0.0.3", 0)

inline ImVec2 operator+(const ImVec2 v1, const ImVec2 v2) {
    return ImVec2{ v1.x + v2.x, v1.y + v2.y };
}

inline ImVec2 operator-(const ImVec2 v1, const ImVec2 v2) {
    return ImVec2{ v1.x - v2.x, v1.y - v2.y };
}

inline ImVec2 operator/(const ImVec2 v1, const int i) {
    return ImVec2{ v1.x / i, v1.y / i };
}

void DodgeOverlayPlugin::onLoad() {
    m_dodgeDeadzone = gameWrapper->GetSettings().GetGamepadSettings().DodgeInputThreshold;
    m_configurationFilePath = gameWrapper->GetBakkesModPath() / m_configurationFilePath;

    if(std::ifstream(m_configurationFilePath)) {
        cvarManager->loadCfg(m_configurationFilePath.string());
    }

#pragma region register cvars
#pragma region dodgeoverlayWinXPos
    CVarWrapper tempCvar = cvarManager->getCvar("dodgeoverlayWinXPos");
    if(tempCvar.IsNull()) {
        tempCvar = cvarManager->registerCvar("dodgeoverlayWinXPos", "0.0");
    }
    tempCvar.addOnValueChanged(
        [this](std::string old, CVarWrapper now) {
            m_windowPosition.x = now.getFloatValue();
            writeCfg();
        });
    m_localCvars["dodgeoverlayWinXPos"] = tempCvar;
#pragma endregion
#pragma region dodgeoverlayWinYPos
    if((tempCvar = cvarManager->getCvar("dodgeoverlayWinYPos")).IsNull()) {
        tempCvar = cvarManager->registerCvar("dodgeOverlayWinYPos", "0.0");
    }
    tempCvar.addOnValueChanged(
        [this](std::string old, CVarWrapper now) {
            m_windowPosition.y = now.getFloatValue();
            writeCfg();
        });
    m_localCvars["dodgeOverlayWinYPos"] = tempCvar;
#pragma endregion
#pragma region dodgeoverlayScale
    if((tempCvar = cvarManager->getCvar("dodgeoverlayScale")).IsNull()) {
        tempCvar = cvarManager->registerCvar("dodgeoverlayScale", "1.0");
    }
    tempCvar.addOnValueChanged(
        [this](std::string old, CVarWrapper now) {
            m_scale = now.getFloatValue();
            writeCfg();
        });
    m_localCvars["dodgeoverlayScale"] = tempCvar;
#pragma endregion
#pragma region dodgeoverlayShowNums
    if((tempCvar = cvarManager->getCvar("dodgeoverlayShowNums")).IsNull()) {
        tempCvar = cvarManager->registerCvar("dodgeoverlayShowNums", "1");
    }
    tempCvar.addOnValueChanged(
        [this](std::string old, CVarWrapper now) {
            m_fShowNums = now.getBoolValue();
            writeCfg();
        });
    m_localCvars["dodgeoverlayShowNums"] = tempCvar;
#pragma endregion
#pragma region dodgeoverlayStickBorderColor
    if((tempCvar = cvarManager->getCvar("dodgeoverlayStickBorderColor")).IsNull()) {
        tempCvar = cvarManager->registerCvar("dodgeoverlayStickBorderColor", "(1.0, 1.0, 1.0, 1.0)");
    }
    tempCvar.addOnValueChanged(
        [this](std::string old, CVarWrapper now) {
            LinearColor color = now.getColorValue();
            m_stickBorderColor = ImColor(color.R, color.G, color.B, color.A);
            writeCfg();
        });
    m_localCvars["dodgeoverlayStickBorderColor"] = tempCvar;
#pragma endregion
#pragma region dodgeoverlayStickLocationColor
    if((tempCvar = cvarManager->getCvar("dodgeoverlayStickLocationColor")).IsNull()) {
        tempCvar = cvarManager->registerCvar("dodgeoverlayStickLocationColor", "(1.0, 1.0, 1.0, 1.0)");
    }
    tempCvar.addOnValueChanged(
        [this](std::string old, CVarWrapper now) {
            LinearColor color = now.getColorValue();
            m_stickLocationColor = ImColor(color.R, color.G, color.B, color.A);
            writeCfg();
        });
    m_localCvars["dodgeoverlayStickLocationColor"] = tempCvar;
#pragma endregion 
#pragma region dodgeoverlayDodgeDeadzoneColor
    if((tempCvar = cvarManager->getCvar("dodgeoverlayDodgeDeadzoneColor")).IsNull()) {
        tempCvar = cvarManager->registerCvar("dodgeoverlayDodgeDeadzoneColor", "(1.0, 1.0, 1.0, 1.0)");
    }
    tempCvar.addOnValueChanged(
        [this](std::string old, CVarWrapper now) {
            LinearColor color = now.getColorValue();
            m_dodgeDeadzoneColor = ImColor(color.R, color.G, color.B, color.A);
            writeCfg();
        });
    m_localCvars["dodgeoverlayDodgeDeadzoneColor"] = tempCvar;
#pragma endregion
#pragma region dodgeoverlayStickLocationSize
    if((tempCvar = cvarManager->getCvar("dodgeoverlayStickLocationSize")).IsNull()) {
        tempCvar = cvarManager->registerCvar("dodgeoverlayStickLocationSize", "5.0");
    }
    tempCvar.addOnValueChanged(
        [this](std::string old, CVarWrapper now) {
            m_stickLocationSize = now.getFloatValue();
            writeCfg();
        });
    m_localCvars["dodgeoverlayStickLocationSize"] = tempCvar;
#pragma endregion
#pragma region dodgeoverlayShowDodgeDeadzoneBorder
    if((tempCvar = cvarManager->getCvar("dodgeoverlayShowDodgeDeadzoneBorder")).IsNull()) {
        tempCvar = cvarManager->registerCvar("dodgeoverlayShowDodgeDeadzoneBorder", "1");
    }
    tempCvar.addOnValueChanged(
        [this](std::string old, CVarWrapper now) {
            m_fShowDodgeDeadzoneBorder = now.getBoolValue();
            writeCfg();
        });
    m_localCvars["dodgeoverlayShowDodgeDeadzoneBorder"] = tempCvar;
#pragma endregion
#pragma region dodgeoverlayDodgeDeadzoneBorderThickness
    if((tempCvar = cvarManager->getCvar("dodgeoverlayDodgeDeadzoneBorderThickness")).IsNull()) {
        tempCvar = cvarManager->registerCvar("dodgeoverlayDodgeDeadzoneBorderThickness", "1.0");
    }
    tempCvar.addOnValueChanged(
        [this](std::string old, CVarWrapper now) {
            m_dodgeDeadzoneBorderThickness = now.getFloatValue();
            writeCfg();
        });
    m_localCvars["dodgeoverlayDodgeDeadzoneBorderThickness"] = tempCvar;
#pragma endregion
#pragma region dodgeoverlayDodgeDeadzoneCrossedAlpha
    if((tempCvar = cvarManager->getCvar("dodgeoverlayDodgeDeadzoneCrossedAlpha")).IsNull()) {
        tempCvar = cvarManager->registerCvar("dodgeoverlayDodgeDeadzoneCrossedAlpha", "0.1");
    }
    tempCvar.addOnValueChanged(
        [this](std::string old, CVarWrapper now) {
            m_dodgeDeadzoneCrossedAlpha = now.getFloatValue();
            writeCfg();
        });
    m_localCvars["dodgeoverlayDodgeDeadzoneCrossedAlpha"] = tempCvar;
#pragma endregion
#pragma endregion

    //TODO: try HookEvent<T*>
    gameWrapper->HookEvent("Function TAGame.PlayerInput_TA.PlayerInput",
        [this](std::string) {
        CarWrapper car = gameWrapper->GetLocalCar();
        if (car) {
            PlayerControllerWrapper pc = car.GetPlayerController();
            if (pc) {
                ControllerInput inputs = pc.GetVehicleInput();
                m_stickLocation.x = inputs.DodgeStrafe;
                m_stickLocation.y = inputs.DodgeForward;
                m_dodgeDeadzoneRoll = 0.0f;
                if (!m_stickLocation.x) {
                    m_stickLocation.x = inputs.Roll;
                }
                else if (fabs(inputs.Roll) >= 1.0f) {
                    m_stickLocation.x += inputs.Roll;
                    if (m_stickLocation.x >= 1.0f) m_stickLocation.x = 1.0f;
                    if (m_stickLocation.x <= -1.0f) m_stickLocation.x = -1.0f;
                    m_dodgeDeadzoneRoll = m_dodgeDeadzone;
                }
            }
        }
        });

    gameWrapper->HookEvent("Function TAGame.GFxData_Settings_TA.SetDodgeInputThreshold",
        [this](std::string) {
            m_dodgeDeadzone = gameWrapper->GetSettings().GetGamepadSettings().DodgeInputThreshold;
        });
    gameWrapper->Execute(
        [this](GameWrapper* gameWrapper) {
            cvarManager->executeCommand("openmenu dodgeoverlay;");
        });
}

void DodgeOverlayPlugin::RenderSettings() {
    using namespace ImGui;
    if (DragFloat("Window position x", &m_windowPosition.x, 1.0f, 50.0f * m_finalScale, m_displaySize.x - m_windowSize.x / 4, "%.1f")) {
        m_localCvars["dodgeoverlayWinXPos"].setValue(m_windowPosition.x);
    }
    if (DragFloat("Window position y", &m_windowPosition.y, 1.0f, (50.0f) * m_finalScale, m_displaySize.y - m_windowSize.x / 4 - m_fShowNums * 12.0f * m_finalScale, "%.1f")) {
        m_localCvars["dodgeoverlayWinYPos"].setValue(m_windowPosition.y);
    };

    if (DragFloat("Scale", &m_scale, 0.1f, 1.0f, 10.0f, "%.1f")) {
        m_localCvars["dodgeoverlayScale"].setValue(m_scale);
    };
    Text(("Final scale: " + std::to_string(m_finalScale)).c_str());
    if (DragFloat("Stick location size", &m_stickLocationSize, 0.1f, 1.0f, 100.0f, "%.1f")) {
        m_localCvars["dodgeoverlayStickLocationSize"].setValue(m_stickLocationSize);
    };
    if (Checkbox("Show only dodge deadzone border", &m_fShowDodgeDeadzoneBorder)) {
        m_localCvars["dodgeoverlayShowDodgeDeadzoneBorder"].setValue(m_fShowDodgeDeadzoneBorder);
    };
    if (m_fShowDodgeDeadzoneBorder) {
        if (DragFloat("Dodge deadzone border thickness", &m_dodgeDeadzoneBorderThickness, 0.1f, 0.1f, 10.0f, "%.1f")) {
            m_localCvars["dodgeoverlayDodgeDeadzoneBorderThickness"].setValue(m_dodgeDeadzoneBorderThickness);
        };
    }
    if (Checkbox("Show outputs nums", &m_fShowNums)) {
        m_localCvars["dodgeoverlayShowNums"].setValue(m_fShowNums);
    };
    if (DragFloat("Background color alpha when deadzone has been crossed", &m_dodgeDeadzoneCrossedAlpha, 0.01f, 0.0f, 1.0f, "%.2f")) {
        m_localCvars["dodgeoverlayDodgeDeadzoneCrossedAlpha"].setValue(m_dodgeDeadzoneCrossedAlpha);
    };
    {
        float* colors[4] = { &m_stickBorderColor.Value.x, &m_stickBorderColor.Value.y, &m_stickBorderColor.Value.z, &m_stickBorderColor.Value.w };
        if (ColorEdit4("Stick border color", *colors, ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_AlphaBar)) {
            LinearColor color = LinearColor();
            color.R = m_stickBorderColor.Value.x;
            color.G = m_stickBorderColor.Value.y;
            color.B = m_stickBorderColor.Value.z;
            color.A = m_stickBorderColor.Value.w;
            m_localCvars["dodgeoverlayStickBorderColor"].setValue(color);
        };
    }
    {
        float* colors[4] = { &m_stickLocationColor.Value.x, &m_stickLocationColor.Value.y, &m_stickLocationColor.Value.z, &m_stickLocationColor.Value.w };
        if (ColorEdit4("Stick location color", *colors, ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_AlphaBar)) {
            LinearColor color = LinearColor();
            color.R = m_stickLocationColor.Value.x;
            color.G = m_stickLocationColor.Value.y;
            color.B = m_stickLocationColor.Value.z;
            color.A = m_stickLocationColor.Value.w;
            m_localCvars["dodgeoverlayStickLocationColor"].setValue(color);
        };
    }
    {
        float* colors[4] = { &m_dodgeDeadzoneColor.Value.x, &m_dodgeDeadzoneColor.Value.y, &m_dodgeDeadzoneColor.Value.z, &m_dodgeDeadzoneColor.Value.w };
        if (ColorEdit4("Dodge deadzone color", *colors, ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_AlphaBar)) {
            LinearColor color = LinearColor();
            color.R = m_dodgeDeadzoneColor.Value.x;
            color.G = m_dodgeDeadzoneColor.Value.y;
            color.B = m_dodgeDeadzoneColor.Value.z;
            color.A = m_dodgeDeadzoneColor.Value.w;
            m_localCvars["dodgeoverlayDodgeDeadzoneColor"].setValue(color);
        };
    }

    TextUnformatted("Dodge Overlay plugin settings");
}

string DodgeOverlayPlugin::GetPluginName() {
    return PLUGIN_NAME;
}

void DodgeOverlayPlugin::SetImGuiContext(uintptr_t ctx) {
    ImGui::SetCurrentContext(reinterpret_cast<ImGuiContext*>(ctx));
}

void DodgeOverlayPlugin::Render() {
    ServerWrapper server = (gameWrapper->IsInOnlineGame()) ? (gameWrapper->GetOnlineGame()) : (gameWrapper->GetGameEventAsServer());
    if (!server.IsNull() && !server.GetbMatchEnded()) {
        RenderImGui();
    }
}

void DodgeOverlayPlugin::RenderImGui() {
    using namespace ImGui;
    SetNextWindowBgAlpha(0.0f);
    PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
    PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
    ImGuiWindowFlags windowFlags = ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoInputs;
    if (Begin(GetMenuTitle().c_str(), &m_fWindowOpen, windowFlags)) {
        m_displaySize = GetIO().DisplaySize;
        m_finalScale = m_scale * m_displaySize.y / 1000;
        m_windowSize = ImVec2(200.0f * m_finalScale, 200.0f * m_finalScale);
        m_radius = 50.0f * m_finalScale;
        ImVec2 finalWindowPosition = m_windowPosition - ImVec2(50.0f * m_finalScale, 50.0f * m_finalScale) - m_windowSize / 4;
        SetWindowSize(m_windowSize);
        SetWindowFontScale(m_finalScale);
        SetWindowPos(finalWindowPosition);

        ImDrawList* drawList = GetWindowDrawList();
        ImVec2 p = GetCursorScreenPos();

        ImVec2 stickCenter = p + ImVec2(m_windowSize.x / 2, m_windowSize.x / 2);    // Because I need only square center position excluding outputs nums height
        drawList->AddQuad(stickCenter + ImVec2(-m_radius, m_radius), stickCenter + ImVec2(m_radius, m_radius), stickCenter + ImVec2(m_radius, -m_radius), stickCenter + ImVec2(-m_radius, -m_radius), m_stickBorderColor);
        drawList->AddCircleFilled(stickCenter + ImVec2(m_stickLocation.x * m_radius, -m_stickLocation.y * m_radius), m_radius * m_stickLocationSize / 100, m_stickLocationColor, 0);
        float tempDodgeDeadzone = m_dodgeDeadzone - m_dodgeDeadzoneRoll;
        if (m_fShowDodgeDeadzoneBorder) {
            drawList->AddQuad(stickCenter + ImVec2(-tempDodgeDeadzone * m_radius, 0.0f), stickCenter + ImVec2(0.0f, tempDodgeDeadzone * m_radius), stickCenter + ImVec2(tempDodgeDeadzone * m_radius, 0.0f), stickCenter + ImVec2(0.0f, -tempDodgeDeadzone * m_radius), m_dodgeDeadzoneColor, m_dodgeDeadzoneBorderThickness * m_finalScale);
        }
        else {
            drawList->AddQuadFilled(stickCenter + ImVec2(-tempDodgeDeadzone * m_radius, 0.0f), stickCenter + ImVec2(0.0f, tempDodgeDeadzone * m_radius), stickCenter + ImVec2(tempDodgeDeadzone * m_radius, 0.0f), stickCenter + ImVec2(0.0f, -tempDodgeDeadzone * m_radius), m_dodgeDeadzoneColor);
        }
        ImColor color = (std::abs(m_stickLocation.x) + std::abs(m_stickLocation.y) < tempDodgeDeadzone) ? ImColor(1.0f, 0.0f, 0.0f, m_dodgeDeadzoneCrossedAlpha) : ImColor(0.0f, 1.0f, 0.0f, m_dodgeDeadzoneCrossedAlpha);
        drawList->AddQuadFilled(stickCenter + ImVec2(-m_radius + 1.0f, m_radius - 1.0f), stickCenter + ImVec2(m_radius - 1.0f, m_radius - 1.0f), stickCenter + ImVec2(m_radius - 1.0f, -m_radius + 1.0f), stickCenter + ImVec2(-m_radius + 1.0f, -m_radius + 1.0f), color);
        if (m_fShowNums) {
            drawList->AddText(stickCenter + ImVec2(-m_radius, m_radius), m_stickLocationColor, ("X: " + std::format("{:.2f}", m_stickLocation.x)).c_str());
            drawList->AddText(stickCenter + ImVec2(0, m_radius), m_stickLocationColor, ("Y: " + std::format("{:.2f}", m_stickLocation.y)).c_str());
        }

        PopStyleVar(2);
    }
    End();
}

string DodgeOverlayPlugin::GetMenuName() {
    return MENU_NAME;
}

string DodgeOverlayPlugin::GetMenuTitle() {
    return PLUGIN_NAME;
}

bool DodgeOverlayPlugin::ShouldBlockInput() {
    return false;
}

bool DodgeOverlayPlugin::IsActiveOverlay() {
    return false;
}

void DodgeOverlayPlugin::OnOpen() {
    m_fWindowOpen = true;
}

void DodgeOverlayPlugin::OnClose() {
    m_fWindowOpen = false;
}

void DodgeOverlayPlugin::writeCfg() {
    std::fstream configurationFile;

    configurationFile.open(m_configurationFilePath, std::ios::out);

    for(auto cvar : m_localCvars) {
        configurationFile << cvar.second.getCVarName() + " \"" + cvar.second.getStringValue() + "\"\n";
    }

    configurationFile.close();
}

void DodgeOverlayPlugin::onUnload() {
    writeCfg();
}
