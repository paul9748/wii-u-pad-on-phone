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
//    // ���ο� ���÷��̸� ã���ϴ�.
//    std::vector<std::string> newDisplays = findNewDisplays();
//    for (const auto& display : newDisplays) {
//        std::cout << "New display detected: " << display << std::endl;
//
//        // ���ο� ���÷��̿��� ��� ������ �ػ󵵸� ã���ϴ�.
//        DEVMODE dm;
//        ZeroMemory(&dm, sizeof(dm));
//        dm.dmSize = sizeof(dm);
//        for (int i = 0; EnumDisplaySettings(display.c_str(), i, &dm); i++) {
//            std::cout << "  Mode " << i << ": " << dm.dmPelsWidth << "x" << dm.dmPelsHeight << std::endl;
//        }
//
//        // ����ڷκ��� �ػ󵵸� �����ϵ��� ��û�մϴ�.
//        int modeIndex;
//        std::cout << "Enter mode index for display " << display << ": ";
//        std::cin >> modeIndex;
//
//        // ������ �ػ󵵸� �����ɴϴ�.
//        EnumDisplaySettings(display.c_str(), modeIndex, &dm);
//
//        // ���÷��� ������ �����մϴ�.
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

// ����ü�� ����Ͽ� ���÷��� ���� ����
struct DisplayInfo {
    int index;
    std::string deviceName;
};

// ���÷����� �ػ󵵸� �����ϴ� ����ü
struct Resolution {
    LONG width;
    LONG height;
};

// ���� Ȱ��ȭ�� ���÷��� ����� ��ȸ�ϴ� �Լ�
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

// ���÷����� ��� ������ �ػ� ����� ��ȸ�ϴ� �Լ�
std::vector<Resolution> GetDisplayResolutions(const std::string& deviceName) {
    std::vector<Resolution> resolutions;
    DEVMODE devMode = { 0 };
    devMode.dmSize = sizeof(DEVMODE);

    for (int i = 0; EnumDisplaySettings(deviceName.c_str(), i, &devMode); ++i) {
        resolutions.push_back({ static_cast<LONG>(devMode.dmPelsWidth), static_cast<LONG>(devMode.dmPelsHeight) });
    }

    return resolutions;
}

// �� Ȱ��ȭ�� ���÷��� ����� ���ϰ� ���ο� ���÷��̸� ����ϴ� �Լ�
void CompareAndPrintActiveDisplays(const std::vector<DisplayInfo>& oldList, const std::vector<DisplayInfo>& newList) {
    std::set<std::string> oldDisplaySet;
    for (const auto& display : oldList) {
        oldDisplaySet.insert(display.deviceName);
    }

    std::cout << "New Active Displays:\n";
    for (const auto& display : newList) {
        if (oldDisplaySet.find(display.deviceName) == oldDisplaySet.end()) {
            std::cout << "Index: " << display.index << ", Device Name: " << display.deviceName << '\n';

            // ���ο� ���÷��̿��� ��� ������ �ػ� ��� ��ȸ
            std::vector<Resolution> resolutions = GetDisplayResolutions(display.deviceName);

            // 1920x1080 �ػ󵵰� ������ �ش� �ػ󵵷� ���÷��� ����
            for (const auto& resolution : resolutions) {
                if (resolution.width == 1920 && resolution.height == 1080) {
                    std::cout << "Changing resolution to 1920x1080...\n";
                    DEVMODE devMode = { 0 };
                    devMode.dmSize = sizeof(DEVMODE);
                    devMode.dmPelsWidth = resolution.width;
                    devMode.dmPelsHeight = resolution.height;
                    devMode.dmFields = DM_PELSWIDTH | DM_PELSHEIGHT;

                    // ���÷��� ���� ����
                    LONG result = ChangeDisplaySettingsEx(display.deviceName.c_str(), &devMode, nullptr, CDS_UPDATEREGISTRY, nullptr);
                    if (result == DISP_CHANGE_SUCCESSFUL) {
                        std::cout << "Resolution changed successfully.\n";
                    }
                    else {
                        std::cout << "Failed to change resolution.\n";
                    }

                    break;  // ù ��° 1920x1080 �ػ󵵸� ã������ �ݺ��� ����
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
            initialActiveDisplayList = currentActiveDisplayList; // Ȱ��ȭ�� ���÷��� ����� �ֽ�ȭ
        }
        else {
            break;
        }
    }

    return 0;
}
