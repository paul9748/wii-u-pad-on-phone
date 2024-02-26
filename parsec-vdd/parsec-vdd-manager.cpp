#include <stdio.h>
#include <thread>
#include <chrono>
#include <vector>
#include <iostream>
#include <algorithm>
#include <Windows.h>
#include "./parsec-vdd.h"

using namespace std::chrono_literals;
using namespace parsec_vdd;

DEVMODE devMode;

std::vector<std::string> initialDisplayList;

void saveInitialDisplayList() {
    DISPLAY_DEVICE dd;
    ZeroMemory(&dd, sizeof(dd));
    dd.cb = sizeof(dd);
    for (int i = 0; EnumDisplayDevices(NULL, i, &dd, 0); i++) {
        initialDisplayList.push_back(std::string(dd.DeviceName));
    }
}

std::vector<std::string> findNewDisplays() {
    std::vector<std::string> newDisplays;
    DISPLAY_DEVICE dd;
    ZeroMemory(&dd, sizeof(dd));
    dd.cb = sizeof(dd);
    for (int i = 0; EnumDisplayDevices(NULL, i, &dd, 0); i++) {
        std::string currentDisplay(dd.DeviceName);
        if (std::find(initialDisplayList.begin(), initialDisplayList.end(), currentDisplay) == initialDisplayList.end()) {
            newDisplays.push_back(currentDisplay);
        }
    }
    return newDisplays;
}



int main() {
    saveInitialDisplayList();
    DeviceStatus status = QueryDeviceStatus(&VDD_CLASS_GUID, VDD_HARDWARE_ID);
    if (status != DEVICE_OK) {
        printf("Parsec VDD device is not OK, got status %d.\n", status);
        return 1;
    }

    HANDLE vdd = OpenDeviceHandle(&VDD_ADAPTER_GUID);
    if (vdd == NULL || vdd == INVALID_HANDLE_VALUE) {
        printf("Failed to obtain the device handle.\n");
        return 1;
    }

    bool running = true;
    std::vector<int> displays;

    std::thread updater([&running, vdd] {
        while (running) {
            VddUpdate(vdd);
            std::this_thread::sleep_for(100ms);
        }
        });
    updater.detach();


    Sleep(200);
    if (displays.size() < VDD_MAX_DISPLAYS) {
        int index = VddAddDisplay(vdd);
        displays.push_back(index);
        printf("Added a new virtual display, index: %d.\n", index);
    }
    else {
        printf("Limit exceeded (%d), could not add more virtual displays.\n", VDD_MAX_DISPLAYS);
    }
    Sleep(1000);

    // 새로운 디스플레이를 찾습니다.
    std::vector<std::string> newDisplays = findNewDisplays();
    for (const auto& display : newDisplays) {
        std::cout << "New display detected: " << display << std::endl;

        // 새로운 디스플레이에서 사용 가능한 해상도를 찾습니다.
        DEVMODE dm;
        ZeroMemory(&dm, sizeof(dm));
        dm.dmSize = sizeof(dm);
        for (int i = 0; EnumDisplaySettings(display.c_str(), i, &dm); i++) {
            std::cout << "  Mode " << i << ": " << dm.dmPelsWidth << "x" << dm.dmPelsHeight << std::endl;
        }

        // 사용자로부터 해상도를 선택하도록 요청합니다.
        int modeIndex;
        std::cout << "Enter mode index for display " << display << ": ";
        std::cin >> modeIndex;

        // 선택한 해상도를 가져옵니다.
        EnumDisplaySettings(display.c_str(), modeIndex, &dm);

        // 디스플레이 설정을 변경합니다.
        LONG result = ChangeDisplaySettingsEx(display.c_str(), &dm, NULL, 0, NULL);
        if (result == DISP_CHANGE_SUCCESSFUL) {
            std::cout << "Display settings changed successfully for display " << display << std::endl;
        }
        else {
            std::cout << "Failed to change display settings for display " << display << ". Error code: " << result << std::endl;
        }
    }



    while (running) {

    }


}