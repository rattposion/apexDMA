#pragma once
#include <string>
#include <iostream>
#include <Kmbox/KmboxB.h>
#include <Kmbox/KmboxNet.hpp>

// Extern declarations for missing MYKMBOX stub functions
extern void send_command(void* hSerial, const std::string& command);
extern std::string find_port(const std::string& targetDescription);
extern bool open_port(void*& hSerial, const char* portName, unsigned long baudRate);

// Extern declaration for KmBoxBManager (stub implementation)
class KmBoxBManager {
private:
    void* hSerial = nullptr;

public:
    int init() {
        std::cout << "[STUB] KmBoxBManager::init called" << std::endl;
        return 0; // Success
    }

    void km_move(int X, int Y) {
        std::cout << "[STUB] KmBoxBManager::km_move called with X: " << X << ", Y: " << Y << std::endl;
    }

    int km_getpos(int& x, int& y) {
        std::cout << "[STUB] KmBoxBManager::km_getpos called" << std::endl;
        x = 0;
        y = 0;
        return 0;
    }

    void km_move_auto(int X, int Y, int runtime) {
        std::cout << "[STUB] KmBoxBManager::km_move_auto called with X: " << X << ", Y: " << Y << ", runtime: " << runtime << std::endl;
    }

    void km_click() {
        std::cout << "[STUB] KmBoxBManager::km_click called" << std::endl;
    }

    bool km_right(bool down) {
        std::cout << "[STUB] KmBoxBManager::km_right called with down: " << down << std::endl;
        return true;
    }

    void lock_mx() { std::cout << "[STUB] KmBoxBManager::lock_mx called" << std::endl; }
    void lock_my() { std::cout << "[STUB] KmBoxBManager::lock_my called" << std::endl; }
    void lock_mr() { std::cout << "[STUB] KmBoxBManager::lock_mr called" << std::endl; }
    void unlock_mx() { std::cout << "[STUB] KmBoxBManager::unlock_mx called" << std::endl; }
    void unlock_my() { std::cout << "[STUB] KmBoxBManager::unlock_my called" << std::endl; }
    void unlock_mr() { std::cout << "[STUB] KmBoxBManager::unlock_mr called" << std::endl; }

    void* getSerialHandle() const {
        std::cout << "[STUB] KmBoxBManager::getSerialHandle called" << std::endl;
        return (void*)1; // Dummy handle
    }
};

extern KmBoxBManager kmBoxBMgr;

class KmboxController
{
public:
    KmboxController() {};

    bool rememberKmboxInfo = false;
    const std::string CONFIG_FILE = "kmbox_config.json";
    bool SaveKmboxConfig();
    // 加载配置文件
    bool LoadKmboxConfig();
    // 获取和设置是否记住配置
    bool GetRememberConfig() const { return rememberKmboxInfo; }
    void SetRememberConfig(bool remember) { rememberKmboxInfo = remember; }
    
    struct KMBoxConfig {
        std::string type = "MAKCU";  // Default to MAKCU
        std::string comPort = "3";   // Default COM port 3
        std::string baudRate = "115200";
        std::string ip = "192.168.2.188";
        std::string port = "9742";
        std::string uuid;
        int minDelay{ 1 };
        bool initialized{ false };
        _com _comPort; // 修正为ComPort类型
    } kmboxConfig;
    
    void InitializeFirst();
    void Initialize();
    void renderKmboxSettings();

    //Tools

    void Move(int x, int y);
    void LeftClick();
    void moveTest();
};