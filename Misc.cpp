#include "include.h"
#include "Misc.h"
#include "Menu.h"

Misc::Misc() : AutoConfigurable("Misc")
{
    // Use Controller removed - always use keyboard
}
//附加到主线程
void Misc::run()
{
    // All memory write functions have been disabled
    // Super Glide, Tap Strafe, Bunny Hop, and Auto Grapple are no longer functional
    std::this_thread::sleep_for(std::chrono::milliseconds(1));
}

void Misc::renderMenu()
{
    // Super Glide, Tap Strafe, Bunny Hop, and Auto Grapple removed from project

    // 参数切换器部分
    ImGui::Separator();
    ImGui::Text(t_("Config Switcher"));
    ImGui::Checkbox(t_("Enable Switcher"), &cfgSwitcher.enable);

    if (cfgSwitcher.enable)
    {

        auto& renderer = ImGuiRenderer::getInstance();

        // Controller support removed - always use keyboard
        {
            renderer.renderKeyBinding(t_("Previous Config"), &cfgSwitcher.switchKeyUp);
            renderer.renderKeyBinding(t_("Next Config"), &cfgSwitcher.switchKeyNext);
        }

        // 参数列表
        ImGui::Separator();
        ImGui::Text(t_("Config List"));

        // 显示当前参数列表及删除按钮
        for (int i = 0; i < cfgSwitcher.Lists.size(); i++)
        {
            ImGui::PushID(i);
            ImGui::Text("%s", cfgSwitcher.Lists[i].c_str());
            ImGui::SameLine();
            if (ImGui::Button("X"))
            {
                cfgSwitcher.Lists.erase(cfgSwitcher.Lists.begin() + i);
                if (cfgSwitcher.activeIndex >= cfgSwitcher.Lists.size() && !cfgSwitcher.Lists.empty())
                {
                    cfgSwitcher.activeIndex = cfgSwitcher.Lists.size() - 1;
                }
                i--;
            }
            ImGui::PopID();
        }

        // 添加参数按钮
        if (ImGui::Button(t_("Add Config")))
        {
            ImGui::OpenPopup("Select Config");
        }

        // 参数选择弹窗
        if (ImGui::BeginPopup("Select Config"))
        {
            ImGui::Text(t_("Select a Config to add:"));
            for (auto cfgName : Menu::availableConfigs)
                if (ImGui::Selectable(cfgName.c_str()))
                {
                    // 检查是否已存在
                    bool exists = false;
                    for (const auto& existing : cfgSwitcher.Lists)
                    {
                        if (existing == cfgName)
                        {
                            exists = true;
                            break;
                        }
                    }

                    if (!exists)
                    {
                        cfgSwitcher.Lists.push_back(cfgName);
                    }

                    ImGui::CloseCurrentPopup();
                }
            ImGui::EndPopup();
        }
    }
}

void Misc::updateKeys() {
    static bool init = false;
    if (!init) {
        std::unordered_map<std::string, int> espKeys = {
            {"nextKey", cfgSwitcher.switchKeyNext},
            {"upKey", cfgSwitcher.switchKeyUp},
            {"nextKeyController", cfgSwitcher.switchKeyNextController},
            {"upKeyController", cfgSwitcher.switchKeyUpController},
        };

        keyDetector->registerKeysForContext("Misc", espKeys);
        init = true;
    }

    keyDetector->updateKeyCode("Misc", "nextKey", cfgSwitcher.switchKeyNext);
    keyDetector->updateKeyCode("Misc", "upKey", cfgSwitcher.switchKeyUp);
    keyDetector->updateKeyCode("Misc", "nextKeyController", cfgSwitcher.switchKeyNextController);
    keyDetector->updateKeyCode("Misc", "upKeyController", cfgSwitcher.switchKeyUpController);
}

bool Misc::isKeyHeld(const std::string& keyName) {
    return IS_KEY_HELD("Misc", keyName);
}

bool Misc::isKeyToggled(const std::string& keyName) {
    if (IS_KEY_TRIGGERED("Misc", keyName))
    {
        CLEAR_KEY_TOGGLE_STATE("Misc", keyName);
        return true;
    }
    return false;
}

void Misc::renderSwitcher()
{
    if (!cfgSwitcher.enable || cfgSwitcher.Lists.empty())
        return;

    updateKeys();

    if (isKeyToggled("nextKey"))
    {
        cfgSwitcher.activeIndex = (cfgSwitcher.activeIndex + 1) % cfgSwitcher.Lists.size();
        ConfigManager::loadConfigFromFile(cfgSwitcher.Lists[cfgSwitcher.activeIndex]);
    }
    else if (isKeyToggled("upKey"))
    {
        cfgSwitcher.activeIndex = (cfgSwitcher.activeIndex - 1 + cfgSwitcher.Lists.size()) % cfgSwitcher.Lists.size();
        ConfigManager::loadConfigFromFile(cfgSwitcher.Lists[cfgSwitcher.activeIndex]);
    }

    ImGui::PushFont(ImGui::GetIO().Fonts->Fonts[1]);
    ImGui::SetNextWindowSize(ImVec2(200, 100), ImGuiCond_Once);
    ImGui::Begin(t_("Configs"), nullptr, ImGuiWindowFlags_AlwaysAutoResize);
    ImGui::Text(t_("Config List:"));
    for (int i = 0; i < cfgSwitcher.Lists.size(); i++)
    {
        ImGui::TextColored(i == cfgSwitcher.activeIndex ? ImVec4(1.0f, 1.0f, 0.0f, 1.0f) : ImVec4(1.0f, 1.0f, 1.0f, 1.0f), (cfgSwitcher.Lists[i] + ".cfg").c_str());
    }
    ImGui::End();
    ImGui::PopFont();
}