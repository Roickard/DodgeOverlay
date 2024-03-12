#ifndef DODGEOVERLAY_H
#define DODGEOVERLAY_H

namespace DodgeOverlay {
    class DodgeOverlayPlugin : 
        public BakkesMod::Plugin::BakkesModPlugin,
        public BakkesMod::Plugin::PluginSettingsWindow,
        public BakkesMod::Plugin::PluginWindow 
    {
    private:
        bool m_fWindowOpen = false;
        ImVec2 m_windowPosition = ImVec2();
        ImVec2 m_windowSize = ImVec2();
        ImVec2 m_displaySize = ImVec2(100.0f, 100.0f);
        float m_scale = 1.0f;
        float m_finalScale = 1.0f;

        float m_radius = 1.0f;
        int m_circleSegments = 4;
        bool m_fShowNums = true;
        ImColor m_stickBorderColor = ImColor(1.0f, 1.0f, 1.0f, 1.0f);
        ImColor m_stickLocationColor = ImColor(1.0f, 1.0f, 1.0f, 1.0f);
        ImColor m_dodgeDeadzoneColor = ImColor(1.0f, 1.0f, 1.0f, 1.0f);
        ImVec2 m_stickLocation = ImVec2();
        float m_stickLocationSize = 5.0f;
        float m_dodgeDeadzone = 0.0f;
        float m_dodgeDeadzoneRoll = 0.0f;
        bool m_fShowDodgeDeadzoneBorder = true;
        float m_dodgeDeadzoneBorderThickness = 1.0f;
        float m_dodgeDeadzoneCrossedAlpha = 0.1f;

        std::filesystem::path m_configurationFilePath = "cfg/dodgeoverlay.cfg";

        std::unordered_map<std::string, CVarWrapper> m_localCvars;

    public:
        virtual void onLoad();
        virtual void RenderSettings() override;
        virtual std::string GetPluginName() override;
        virtual void SetImGuiContext(uintptr_t ctx) override;
        virtual void Render() override;
        void RenderImGui();
        virtual std::string GetMenuName() override;
        virtual std::string GetMenuTitle() override;
        virtual bool ShouldBlockInput() override;
        virtual bool IsActiveOverlay() override;
        virtual void OnOpen() override;
        virtual void OnClose() override;
        void writeCfg();
        virtual void onUnload();
    };
}

#endif // DODGEOVERLAY_H