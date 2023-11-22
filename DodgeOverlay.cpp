#include "pch.h"
#include "DodgeOverlay.h"
using std::string;
using namespace DodgeOverlay;

BAKKESMOD_PLUGIN(DodgeOverlayPlugin, "Dodge Overlay", "0.0.3", 0)

void DodgeOverlayPlugin::onLoad() {
	dodgeDeadzone = gameWrapper->GetSettings().GetGamepadSettings().DodgeInputThreshold;
	configurationFilePath = gameWrapper->GetBakkesModPath() / configurationFilePath;

#pragma region register cvars
	cvarManager->registerCvar("dodgeoverlayWinXPos", "0.0").addOnValueChanged([this](std::string old, CVarWrapper now) {
		windowPosition.x = now.getFloatValue();
		writeCfg();
	});
	cvarManager->registerCvar("dodgeoverlayWinYPos", "0.0").addOnValueChanged([this](std::string old, CVarWrapper now) {
		windowPosition.y = now.getFloatValue();
		writeCfg();
	});
	cvarManager->registerCvar("dodgeoverlayScale", "1.0").addOnValueChanged([this](std::string old, CVarWrapper now) {
		scale = now.getFloatValue();
		writeCfg();
	});
	cvarManager->registerCvar("dodgeoverlayShowNums", "1").addOnValueChanged([this](std::string old, CVarWrapper now) {
		showNums = now.getBoolValue();
		writeCfg();
	});
	cvarManager->registerCvar("dodgeoverlayStickBorderColor", "(1.0, 1.0, 1.0, 1.0)").addOnValueChanged([this](std::string old, CVarWrapper now) {
		LinearColor color = now.getColorValue();
		stickBorderColor = ImColor(color.R, color.G, color.B, color.A);
		writeCfg();
	});
	cvarManager->registerCvar("dodgeoverlayStickLocationColor", "(1.0, 1.0, 1.0, 1.0)").addOnValueChanged([this](std::string old, CVarWrapper now) {
		LinearColor color = now.getColorValue();
		stickLocationColor = ImColor(color.R, color.G, color.B, color.A);
		writeCfg();
	});
	cvarManager->registerCvar("dodgeoverlayDodgeDeadzoneColor", "(1.0, 1.0, 1.0, 1.0)").addOnValueChanged([this](std::string old, CVarWrapper now) {
		LinearColor color = now.getColorValue();
		dodgeDeadzoneColor = ImColor(color.R, color.G, color.B, color.A);
		writeCfg();
	});
	cvarManager->registerCvar("dodgeoverlayStickLocationSize", "5.0").addOnValueChanged([this](std::string old, CVarWrapper now) {
		stickLocationSize = now.getFloatValue();
		writeCfg();
	});
	cvarManager->registerCvar("dodgeoverlayShowDodgeDeadzoneBorder", "1").addOnValueChanged([this](std::string old, CVarWrapper now) {
		showDodgeDeadzoneBorder = now.getBoolValue();
		writeCfg();
	});
	cvarManager->registerCvar("dodgeoverlayDodgeDeadzoneBorderThickness", "1.0").addOnValueChanged([this](std::string old, CVarWrapper now) {
		dodgeDeadzoneBorderThickness = now.getFloatValue();
		writeCfg();
	});
	cvarManager->registerCvar("dodgeoverlayDodgeDeadzoneCrossedAlpha", "0.1").addOnValueChanged([this](std::string old, CVarWrapper now) {
		dodgeDeadzoneCrossedAlpha = now.getFloatValue();
		writeCfg();
	});
#pragma endregion


	if (std::ifstream(configurationFilePath)) {
		cvarManager->loadCfg(configurationFilePath.string());
	}

	gameWrapper->HookEvent("Function TAGame.PlayerInput_TA.PlayerInput", [this](std::string) {
		CarWrapper car = gameWrapper->GetLocalCar();
		if (car) {
			PlayerControllerWrapper pc = car.GetPlayerController();
			if (pc) {
				ControllerInput inputs = pc.GetVehicleInput();
				stickLocation.x = inputs.DodgeStrafe;
				stickLocation.y = inputs.DodgeForward;
				if (fabs(stickLocation.x - 0.0) <= 1e-6)
					stickLocation.x = inputs.Roll;
			}
		}
	});
	gameWrapper->HookEvent("Function TAGame.GFxData_Settings_TA.SetDodgeInputThreshold", [this](std::string) {
		dodgeDeadzone = gameWrapper->GetSettings().GetGamepadSettings().DodgeInputThreshold;
	});

	gameWrapper->Execute([this](GameWrapper* gameWrapper) {
		cvarManager->executeCommand("openmenu dodgeoverlay;");
	});
}

void DodgeOverlayPlugin::RenderSettings() {
	using namespace ImGui;
	if (DragFloat("Window position x", &windowPosition.x, 1.0f, 50.0f * finalScale, displaySize.x - windowSize.x / 4, "%.1f")) {
		cvarManager->getCvar("dodgeoverlayWinXPos").setValue(windowPosition.x);
	}
	if(DragFloat("Window position y", &windowPosition.y, 1.0f, (50.0f) * finalScale, displaySize.y - windowSize.x / 4 - showNums * 12.0f * finalScale, "%.1f")) {
		cvarManager->getCvar("dodgeoverlayWinYPos").setValue(windowPosition.y);
	};
	
	if(DragFloat("Scale", &scale, 0.1f, 1.0f, 10.0f, "%.1f")) {
		cvarManager->getCvar("dodgeoverlayScale").setValue(scale);
	};
	Text(("Final scale: " + std::to_string(finalScale)).c_str());
	if(DragFloat("Stick location size", &stickLocationSize, 0.1f, 1.0f, 100.0f, "%.1f")) {
		cvarManager->getCvar("dodgeoverlayStickLocationSize").setValue(stickLocationSize);
	};
	if(Checkbox("Show only dodge deadzone border", &showDodgeDeadzoneBorder)) {
		cvarManager->getCvar("dodgeoverlayShowDodgeDeadzoneBorder").setValue(showDodgeDeadzoneBorder);
	};
	if (showDodgeDeadzoneBorder) {
		if(DragFloat("Dodge deadzone border thickness", &dodgeDeadzoneBorderThickness, 0.1f, 0.1f, 10.0f, "%.1f")) {
			cvarManager->getCvar("dodgeoverlayDodgeDeadzoneBorderThickness").setValue(dodgeDeadzoneBorderThickness);
		};
	}
	if(Checkbox("Show outputs nums", &showNums)) {
		cvarManager->getCvar("dodgeoverlayShowNums").setValue(showNums);
	};
	if (DragFloat("Background color alpha when deadzone has been crossed", &dodgeDeadzoneCrossedAlpha, 0.01f, 0.0f, 1.0f, "%.2f")) {
		cvarManager->getCvar("dodgeoverlayDodgeDeadzoneCrossedAlpha").setValue(dodgeDeadzoneCrossedAlpha);
	};
	{
		float* colors[4] = { &stickBorderColor.Value.x, &stickBorderColor.Value.y, &stickBorderColor.Value.z, &stickBorderColor.Value.w };
		if(ColorEdit4("Stick border color", *colors, ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_AlphaBar)) {
			LinearColor color = LinearColor();
			color.R = stickBorderColor.Value.x;
			color.G = stickBorderColor.Value.y;
			color.B = stickBorderColor.Value.z;
			color.A = stickBorderColor.Value.w;
			cvarManager->getCvar("dodgeoverlayStickBorderColor").setValue(color);
		};
	}
	{
		float* colors[4] = { &stickLocationColor.Value.x, &stickLocationColor.Value.y, &stickLocationColor.Value.z, &stickLocationColor.Value.w };
		if(ColorEdit4("Stick location color", *colors, ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_AlphaBar)) {
			LinearColor color = LinearColor();
			color.R = stickLocationColor.Value.x;
			color.G = stickLocationColor.Value.y;
			color.B = stickLocationColor.Value.z;
			color.A = stickLocationColor.Value.w;
			cvarManager->getCvar("dodgeoverlayStickLocationColor").setValue(color);
		};
	}
	{
		float* colors[4] = { &dodgeDeadzoneColor.Value.x, &dodgeDeadzoneColor.Value.y, &dodgeDeadzoneColor.Value.z, &dodgeDeadzoneColor.Value.w };
		if(ColorEdit4("Dodge deadzone color", *colors, ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_AlphaBar)) {
			LinearColor color = LinearColor();
			color.R = dodgeDeadzoneColor.Value.x;
			color.G = dodgeDeadzoneColor.Value.y;
			color.B = dodgeDeadzoneColor.Value.z;
			color.A = dodgeDeadzoneColor.Value.w;
			cvarManager->getCvar("dodgeoverlayDodgeDeadzoneColor").setValue(color);
		};
	}

	TextUnformatted("Dodge overlay plugin settings");
}

string DodgeOverlayPlugin::GetPluginName() {
	return "Dodge Overlay";
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
	if (Begin(GetMenuTitle().c_str(), &isWindowOpen, windowFlags)) {
		displaySize = GetIO().DisplaySize;
		finalScale = scale * displaySize.y / 1000;
		windowSize = ImVec2(200.0f * finalScale, 200.0f * finalScale);
		radius = 50.0f * finalScale;
		ImVec2 finalWindowPosition = windowPosition - ImVec2(50.0f * finalScale, 50.0f * finalScale) - windowSize / 4;
		SetWindowSize(windowSize);
		SetWindowFontScale(finalScale);
		SetWindowPos(finalWindowPosition);

		ImDrawList* drawList = GetWindowDrawList();
		ImVec2 p = GetCursorScreenPos();

		ImVec2 stickCenter = p + ImVec2(windowSize.x / 2, windowSize.x / 2);	// Because I need only square center position excluding outputs nums height
		drawList->AddQuad(stickCenter + ImVec2(-radius, radius), stickCenter + ImVec2(radius, radius), stickCenter + ImVec2(radius, -radius), stickCenter + ImVec2(-radius, -radius), stickBorderColor);
		drawList->AddCircleFilled(stickCenter + ImVec2(stickLocation.x * radius, -stickLocation.y * radius), radius * stickLocationSize / 100, stickLocationColor, 0);
		if (showDodgeDeadzoneBorder) {
			drawList->AddQuad(stickCenter + ImVec2(-dodgeDeadzone * radius, 0.0f), stickCenter + ImVec2(0.0f, dodgeDeadzone * radius), stickCenter + ImVec2(dodgeDeadzone * radius, 0.0f), stickCenter + ImVec2(0.0f, -dodgeDeadzone * radius), dodgeDeadzoneColor, dodgeDeadzoneBorderThickness * finalScale);
		}
		else {
			drawList->AddQuadFilled(stickCenter + ImVec2(-dodgeDeadzone * radius, 0.0f), stickCenter + ImVec2(0.0f, dodgeDeadzone * radius), stickCenter + ImVec2(dodgeDeadzone * radius, 0.0f), stickCenter + ImVec2(0.0f, -dodgeDeadzone * radius), dodgeDeadzoneColor);
		}
		ImColor color = (std::abs(stickLocation.x) + std::abs(stickLocation.y) < dodgeDeadzone) ? ImColor(1.0f, 0.0f, 0.0f, dodgeDeadzoneCrossedAlpha) : ImColor(0.0f, 1.0f, 0.0f, dodgeDeadzoneCrossedAlpha);
		drawList->AddQuadFilled(stickCenter + ImVec2(-radius + 1.0f, radius - 1.0f), stickCenter + ImVec2(radius - 1.0f, radius - 1.0f), stickCenter + ImVec2(radius - 1.0f, -radius + 1.0f), stickCenter + ImVec2(-radius + 1.0f, -radius + 1.0f), color);
		if (showNums) {
			drawList->AddText(stickCenter + ImVec2(-radius, radius), stickLocationColor, ("X: " + std::format("{:.2f}", stickLocation.x)).c_str());
			drawList->AddText(stickCenter + ImVec2(0, radius), stickLocationColor, ("Y: " + std::format("{:.2f}", stickLocation.y)).c_str());
		}

		PopStyleVar(2);
	}
	End();
}

string DodgeOverlayPlugin::GetMenuName() {
	return "dodgeoverlay";
}

string DodgeOverlayPlugin::GetMenuTitle() {
	return "Dodge Overlay";
}

bool DodgeOverlayPlugin::ShouldBlockInput() {
	return false;
}

bool DodgeOverlayPlugin::IsActiveOverlay() {
	return false;
}

void DodgeOverlayPlugin::OnOpen() {
	isWindowOpen = true;
}

void DodgeOverlayPlugin::OnClose() {
	isWindowOpen = false;
}

void DodgeOverlayPlugin::writeCfg() {
	std::fstream configurationFile;

	configurationFile.open(configurationFilePath, std::ios::out);

	configurationFile << "dodgeoverlayWinXPos \"" + cvarManager->getCvar("dodgeoverlayWinXPos").getStringValue() + "\"\n";
	configurationFile << "dodgeoverlayWinYPos \"" + cvarManager->getCvar("dodgeoverlayWinYPos").getStringValue() + "\"\n";
	configurationFile << "dodgeoverlayScale \"" + cvarManager->getCvar("dodgeoverlayScale").getStringValue() + "\"\n";
	configurationFile << "dodgeoverlayShowNums \"" + cvarManager->getCvar("dodgeoverlayShowNums").getStringValue() + "\"\n";
	configurationFile << "dodgeoverlayStickBorderColor \"" + cvarManager->getCvar("dodgeoverlayStickBorderColor").getStringValue() + "\"\n";
	configurationFile << "dodgeoverlayStickLocationColor \"" + cvarManager->getCvar("dodgeoverlayStickLocationColor").getStringValue() + "\"\n";
	configurationFile << "dodgeoverlayDodgeDeadzoneColor \"" + cvarManager->getCvar("dodgeoverlayDodgeDeadzoneColor").getStringValue() + "\"\n";
	configurationFile << "dodgeoverlayStickLocationSize \"" + cvarManager->getCvar("dodgeoverlayStickLocationSize").getStringValue() + "\"\n";
	configurationFile << "dodgeoverlayShowDodgeDeadzoneBorder \"" + cvarManager->getCvar("dodgeoverlayShowDodgeDeadzoneBorder").getStringValue() + "\"\n";
	configurationFile << "dodgeoverlayDodgeDeadzoneBorderThickness \"" + cvarManager->getCvar("dodgeoverlayDodgeDeadzoneBorderThickness").getStringValue() + "\"\n";
	configurationFile << "dodgeoverlayDodgeDeadzoneCrossedAlpha \"" + cvarManager->getCvar("dodgeoverlayDodgeDeadzoneCrossedAlpha").getStringValue() + "\"\n";

	configurationFile.close();
}

void DodgeOverlayPlugin::onUnload() {
	writeCfg();
}
