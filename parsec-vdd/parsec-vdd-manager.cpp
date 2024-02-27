//#include <stdio.h>
//#include <thread>
//#include <chrono>
//#include <vector>
//#include <iostream>
//#include <algorithm>
//#include <Windows.h>
//#include "./parsec-vdd.h"
//
//using namespace std::chrono_literals;
//using namespace parsec_vdd;
//
//DEVMODE devMode;
//
//std::vector<std::string> initialDisplayList;
//
//void saveInitialDisplayList() {
//    DISPLAY_DEVICE dd;
//    ZeroMemory(&dd, sizeof(dd));
//    dd.cb = sizeof(dd);
//    for (int i = 0; EnumDisplayDevices(NULL, i, &dd, 0); i++) {
//        initialDisplayList.push_back(std::string(dd.DeviceName));
//    }
//}
//
//std::vector<std::string> findNewDisplays() {
//    std::vector<std::string> newDisplays;
//    DISPLAY_DEVICE dd;
//    ZeroMemory(&dd, sizeof(dd));
//    dd.cb = sizeof(dd);
//    for (int i = 0; EnumDisplayDevices(NULL, i, &dd, 0); i++) {
//        std::string currentDisplay(dd.DeviceName);
//        if (std::find(initialDisplayList.begin(), initialDisplayList.end(), currentDisplay) == initialDisplayList.end()) {
//            newDisplays.push_back(currentDisplay);
//        }
//    }
//    return newDisplays;
//}
//
//
//
//int main() {
//    saveInitialDisplayList();
//    DeviceStatus status = QueryDeviceStatus(&VDD_CLASS_GUID, VDD_HARDWARE_ID);
//    if (status != DEVICE_OK) {
//        printf("Parsec VDD device is not OK, got status %d.\n", status);
//        return 1;
//    }
//
//    HANDLE vdd = OpenDeviceHandle(&VDD_ADAPTER_GUID);
//    if (vdd == NULL || vdd == INVALID_HANDLE_VALUE) {
//        printf("Failed to obtain the device handle.\n");
//        return 1;
//    }
//
//    bool running = true;
//    std::vector<int> displays;
//
//    std::thread updater([&running, vdd] {
//        while (running) {
//            VddUpdate(vdd);
//            std::this_thread::sleep_for(100ms);
//        }
//        });
//    updater.detach();
//
//
//    Sleep(200);
//    if (displays.size() < VDD_MAX_DISPLAYS) {
//        int index = VddAddDisplay(vdd);
//        displays.push_back(index);
//        printf("Added a new virtual display, index: %d.\n", index);
//    }
//    else {
//        printf("Limit exceeded (%d), could not add more virtual displays.\n", VDD_MAX_DISPLAYS);
//    }
//    Sleep(1000);
//
//    // 새로운 디스플레이를 찾습니다.
//    std::vector<std::string> newDisplays = findNewDisplays();
//    for (const auto& display : newDisplays) {
//        std::cout << "New display detected: " << display << std::endl;
//
//        // 새로운 디스플레이에서 사용 가능한 해상도를 찾습니다.
//        DEVMODE dm;
//        ZeroMemory(&dm, sizeof(dm));
//        dm.dmSize = sizeof(dm);
//        for (int i = 0; EnumDisplaySettings(display.c_str(), i, &dm); i++) {
//            std::cout << "  Mode " << i << ": " << dm.dmPelsWidth << "x" << dm.dmPelsHeight << std::endl;
//        }
//
//        // 사용자로부터 해상도를 선택하도록 요청합니다.
//        int modeIndex;
//        std::cout << "Enter mode index for display " << display << ": ";
//        std::cin >> modeIndex;
//
//        // 선택한 해상도를 가져옵니다.
//        EnumDisplaySettings(display.c_str(), modeIndex, &dm);
//
//        // 디스플레이 설정을 변경합니다.
//        LONG result = ChangeDisplaySettingsEx(display.c_str(), &dm, NULL, 0, NULL);
//        if (result == DISP_CHANGE_SUCCESSFUL) {
//            std::cout << "Display settings changed successfully for display " << display << std::endl;
//        }
//        else {
//            std::cout << "Failed to change display settings for display " << display << ". Error code: " << result << std::endl;
//        }
//    }
//
//
//
//    while (running) {
//
//    }
//
//
//}

#include <iostream>
#include <windows.h>
#include <vector>
#include <set>

// 구조체를 사용하여 디스플레이 정보 저장
struct DisplayInfo {
    int index;
    std::string deviceName;
};

// 디스플레이의 해상도를 저장하는 구조체
struct Resolution {
    LONG width;
    LONG height;
};

// 현재 활성화된 디스플레이 목록을 조회하는 함수
std::vector<DisplayInfo> GetActiveDisplayList() {
    std::vector<DisplayInfo> displayList;
    DISPLAY_DEVICE displayDevice = { 0 };
    displayDevice.cb = sizeof(DISPLAY_DEVICE);

    for (int i = 0; EnumDisplayDevices(nullptr, i, &displayDevice, 0); ++i) {
        if ((displayDevice.StateFlags & DISPLAY_DEVICE_ACTIVE) != 0) {
            displayList.push_back({ i, displayDevice.DeviceName });
        }
    }

    return displayList;
}

// 디스플레이의 사용 가능한 해상도 목록을 조회하는 함수
std::vector<Resolution> GetDisplayResolutions(const std::string& deviceName) {
    std::vector<Resolution> resolutions;
    DEVMODE devMode = { 0 };
    devMode.dmSize = sizeof(DEVMODE);

    for (int i = 0; EnumDisplaySettings(deviceName.c_str(), i, &devMode); ++i) {
        resolutions.push_back({ static_cast<LONG>(devMode.dmPelsWidth), static_cast<LONG>(devMode.dmPelsHeight) });
    }

    return resolutions;
}

// 두 활성화된 디스플레이 목록을 비교하고 새로운 디스플레이를 출력하는 함수
void CompareAndPrintActiveDisplays(const std::vector<DisplayInfo>& oldList, const std::vector<DisplayInfo>& newList) {
    std::set<std::string> oldDisplaySet;
    for (const auto& display : oldList) {
        oldDisplaySet.insert(display.deviceName);
    }

    std::cout << "New Active Displays:\n";
    for (const auto& display : newList) {
        if (oldDisplaySet.find(display.deviceName) == oldDisplaySet.end()) {
            std::cout << "Index: " << display.index << ", Device Name: " << display.deviceName << '\n';

            // 새로운 디스플레이에서 사용 가능한 해상도 목록 조회
            std::vector<Resolution> resolutions = GetDisplayResolutions(display.deviceName);

            // 1920x1080 해상도가 있으면 해당 해상도로 디스플레이 설정
            for (const auto& resolution : resolutions) {
                if (resolution.width == 1920 && resolution.height == 1080) {
                    std::cout << "Changing resolution to 1920x1080...\n";
                    DEVMODE devMode = { 0 };
                    devMode.dmSize = sizeof(DEVMODE);
                    devMode.dmPelsWidth = resolution.width;
                    devMode.dmPelsHeight = resolution.height;
                    devMode.dmFields = DM_PELSWIDTH | DM_PELSHEIGHT;

                    // 디스플레이 설정 변경
                    LONG result = ChangeDisplaySettingsEx(display.deviceName.c_str(), &devMode, nullptr, CDS_UPDATEREGISTRY, nullptr);
                    if (result == DISP_CHANGE_SUCCESSFUL) {
                        std::cout << "Resolution changed successfully.\n";
                    }
                    else {
                        std::cout << "Failed to change resolution.\n";
                    }

                    break;  // 첫 번째 1920x1080 해상도를 찾았으면 반복문 종료
                }
            }
        }
    }
}

int main() {
    std::vector<DisplayInfo> initialActiveDisplayList = GetActiveDisplayList();

    while (true) {
        std::cout << "Press 'a' to refresh active display list or any other key to exit: ";
        char userInput;
        std::cin >> userInput;

        if (userInput == 'a' || userInput == 'A') {
            std::vector<DisplayInfo> currentActiveDisplayList = GetActiveDisplayList();
            CompareAndPrintActiveDisplays(initialActiveDisplayList, currentActiveDisplayList);
            initialActiveDisplayList = currentActiveDisplayList; // 활성화된 디스플레이 목록을 최신화
        }
        else {
            break;
        }
    }

    return 0;
}
