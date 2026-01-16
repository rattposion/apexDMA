#include "include.h"
#include "KmBox.h"
#include "../Makcu/MyMakcu.h"
#include <random>

//#define KMBXBOX_B_API __declspec(dllimport)
//KMBXBOX_B_API std::string find_port(const std::string& targetDescription);
//KMBXBOX_B_API bool open_port(HANDLE& hSerial, const char* portName, DWORD baudRate);
//KMBXBOX_B_API void send_command(HANDLE hSerial, const std::string& command);
//
//class KMBXBOX_B_API KmBoxBManager {
//private:
//    HANDLE hSerial;
//
//public:
//
//    int init();
//    void km_move(int X, int Y);
//    int km_getpos(int& x, int& y);
//    void km_move_auto(int X, int Y, int runtime);
//    void km_click();
//    bool km_right(bool down);
//    void lock_mx();
//    void lock_my();
//    void lock_mr();
//    void unlock_mx();
//    void unlock_mr();
//    void unlock_my();
//
//
//    HANDLE getSerialHandle() const;
//
//};
//
//KMBXBOX_B_API KmBoxBManager kmBoxBMgr;

void KmboxController::InitializeFirst() {
    // 尝试加载配置
    if (LoadKmboxConfig()) {
        SetRememberConfig(true);
    }

    // 初始化设备
    Initialize();
}

void KmboxController::Initialize() {
    if (kmboxConfig.type == "MAKCU")
    {
        std::cout << "[#] Init MAKCU on COM" << kmboxConfig.comPort << "..." << std::endl;

        // Initialize MAKCU using Windows Serial API (single method)
        MyMakcu::SetComPort(kmboxConfig.comPort);
        if (!MyMakcu::Initialize()) {
            std::cout << "[MAKCU] MAKCU initialization failed - port may be in use by another application" << std::endl;
            std::cout << "[MAKCU] Close all programs using COM ports (Arduino IDE, PuTTY, etc.)" << std::endl;
            std::cout << "[MAKCU] Try running as administrator" << std::endl;
            kmboxConfig.initialized = false;
            return;
        }

        std::cout << "[MAKCU] MAKCU initialized successfully on " << kmboxConfig.comPort << std::endl;
        kmboxConfig.initialized = true;

        if (GetRememberConfig()) {
            SaveKmboxConfig();
        }
    }
}

void KmboxController::renderKmboxSettings() {
    // 标题
    ImGui::Text("%s", t_("KMBOX SETTINGS"));
    ImGui::Separator();
    ImGui::Spacing();

    // 设备类型选择 - только MAKCU
    ImGui::Text("Device Type: MAKCU");
    kmboxConfig.type = "MAKCU";

    // Настройка COM порта для MAKCU
    char tempBuffer[256] = { 0 };
    strcpy(tempBuffer, kmboxConfig.comPort.c_str());
    if (ImGui::InputText(t_("ComPort"), tempBuffer, 256)) {
        kmboxConfig.comPort = tempBuffer;
    }

    ImGui::Text("Click Init below");
    ImGui::Spacing();

    // 添加记住配置选项
    bool remember = GetRememberConfig();
    if (ImGui::Checkbox(t_("RememberKmboxInfo"), &remember)) {
        SetRememberConfig(remember);
    }

    ImGui::Spacing();

    // 初始化按钮
    if (ImGui::Button(t_("Initialize"))) {
        Initialize();
    }

    ImGui::SameLine();

    // Move test button
    if (ImGui::Button("Move Test")) {
        moveTest();
    }

    // 状态显示
    if (kmboxConfig.initialized) {
        ImGui::TextColored(ImVec4(0, 1, 0, 0.5), "%s", t_("Success"));
    }
    else {
        ImGui::TextColored(ImVec4(1, 0, 0, 0.5), "%s", t_("Failed or Timeout"));
    }
}

bool KmboxController::SaveKmboxConfig() {
    if (!rememberKmboxInfo) return false;

    try {
        json config;
        // 保存设备类型
        if (kmboxConfig.type == "BPro") config["type"] = 0;
        else if (kmboxConfig.type == "Net") config["type"] = 1;
        else if (kmboxConfig.type == "MAKCU") config["type"] = 2;

        // 根据设备类型保存对应配置
        if (kmboxConfig.type == "BPro") {
            config["comPort"] = kmboxConfig.comPort;
            config["baudRate"] = kmboxConfig.baudRate;
        }
        else if (kmboxConfig.type == "Net") {
            config["ip"] = kmboxConfig.ip;
            config["port"] = kmboxConfig.port;
            config["uuid"] = kmboxConfig.uuid;
        }
        // MAKCU не требует дополнительных настроек

        std::ofstream file(CONFIG_FILE);
        if (!file.is_open()) return false;

        file << config.dump(4);
        file.close();
        return true;
    }
    catch (const std::exception& e) {
        return false;
    }
}

bool KmboxController::LoadKmboxConfig() {
    std::ifstream file(CONFIG_FILE);
    if (!file.is_open()) return false;

    try {
        json config = json::parse(file);

        // 设置设备类型
        int type = config["type"].get<int>();
        if (type == 0) kmboxConfig.type = "BPro";
        else if (type == 1) kmboxConfig.type = "Net";
        else if (type == 2) kmboxConfig.type = "MAKCU";

        if (type == 0) { // BPro
            // 加载BPro配置
            kmboxConfig.comPort = config["comPort"].get<std::string>();
            kmboxConfig.baudRate = config["baudRate"].get<std::string>();
        }
        else if (type == 1) { // Net
            // 加载Net配置
            kmboxConfig.ip = config["ip"].get<std::string>();
            kmboxConfig.port = config["port"].get<std::string>();
            kmboxConfig.uuid = config["uuid"].get<std::string>();
        }
        // MAKCU (type == 2) не требует дополнительных настроек

        return true;
    }
    catch (const std::exception& e) {
        return false;
    }
}

void KmboxController::Move(int x, int y) {
    if (kmboxConfig.type == "MAKCU") {
        if (!kmboxConfig.initialized) {
            std::cout << "[MAKCU] Move failed: device not initialized" << std::endl;
            return;
        }

        // Use real MAKCU library
        MyMakcu::Move(x, y);
    }
}

void KmboxController::moveTest() {
    // Move mouse 50 pixels right and 50 pixels down through MAKCU
    if (kmboxConfig.type == "MAKCU" && kmboxConfig.initialized) {
        // Send move command to MAKCU via COM port
        char cmd[1024];
        sprintf_s(cmd, "km.move(%d, %d)\r\n", 50, 50);
        kmboxConfig._comPort.write(cmd);
        std::cout << "[MAKCU] Move Test executed" << std::endl;
    }
    else {
        std::cout << "[MAKCU] Move Test: device not initialized" << std::endl;
    }
}

float RandomFloat() {
    static std::random_device rd;
    static std::mt19937 gen(rd());
    static std::uniform_real_distribution<float> dis(0.0f, 1.0f);
    return dis(gen);
}

float RandomRange(float min, float max) {
    if (min > max) {
        std::swap(min, max);
    }
    return RandomFloat() * (max - min) + min;
}

void KmboxController::LeftClick() {
    // Memory write removed - using only MAKCU method
    if (kmboxConfig.type == "MAKCU") {
        if (!kmboxConfig.initialized) {
            std::cout << "[TriggerBot] MAKCU not initialized for LeftClick" << std::endl;
            return;
        }

        std::cout << "[TriggerBot] Sending click command to real MAKCU device..." << std::endl;

        // Use real MAKCU library
        MyMakcu::LeftClick();

        // Add delay for MAKCU processing
        Sleep(kmboxConfig.minDelay);

        std::cout << "[TriggerBot] Real MAKCU click command sent successfully" << std::endl;
    }
}