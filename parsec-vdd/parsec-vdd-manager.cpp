#include <iostream>
#include <vector>
#include <set>
#include <thread>
#include <chrono>
#include <csignal>
#include <Windows.h>
#include "./parsec-vdd.h"

using namespace std::chrono_literals;
using namespace parsec_vdd;

HANDLE vdd = NULL;
std::vector<int> displays;
std::thread updater;

struct DisplayInfo {
    int index;
    std::string deviceName;
};

struct Resolution {
    LONG width;
    LONG height;
};

// Function declarations
std::vector<DisplayInfo> GetActiveDisplayList();
std::vector<Resolution> GetDisplayResolutions(const std::string& deviceName);
void CompareAndPrintActiveDisplays(const std::vector<DisplayInfo>& oldList, const std::vector<DisplayInfo>& newList);
void signalHandler(int signum);
void CleanupAndExit();

int main() {
    std::vector<DisplayInfo> initialActiveDisplayList = GetActiveDisplayList();

    std::signal(SIGINT, signalHandler);  // For Ctrl+C
    std::signal(SIGTERM, signalHandler); // For "terminate" command

    DeviceStatus status = QueryDeviceStatus(&VDD_CLASS_GUID, VDD_HARDWARE_ID);
    if (status != DEVICE_OK) {
        printf("Parsec VDD device is not OK, got status %d.\n", status);
        return 1;
    }

    vdd = OpenDeviceHandle(&VDD_ADAPTER_GUID);
    if (vdd == NULL || vdd == INVALID_HANDLE_VALUE) {
        printf("Failed to obtain the device handle.\n");
        return 1;
    }

    bool running = true;

    updater = std::thread([&running] {
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

    std::vector<DisplayInfo> currentActiveDisplayList = GetActiveDisplayList();
    CompareAndPrintActiveDisplays(initialActiveDisplayList, currentActiveDisplayList);
    initialActiveDisplayList = currentActiveDisplayList;

    while (running) {
        // Your main program loop here...
    }

    CleanupAndExit();
}

std::vector<DisplayInfo> GetActiveDisplayList() {
    std::vector<DisplayInfo> displayList;
    DISPLAY_DEVICE displayDevice = { 0 };
    displayDevice.cb = sizeof(DISPLAY_DEVICE);

    for (int i = 0; EnumDisplayDevices(nullptr, i, &displayDevice, 0); ++i) {
        if ((displayDevice.StateFlags & DISPLAY_DEVICE_ACTIVE) != 0) {
            displayList.push_back({ i, std::string(displayDevice.DeviceName) });
        }
    }

    return displayList;
}

std::vector<Resolution> GetDisplayResolutions(const std::string& deviceName) {
    std::vector<Resolution> resolutions;
    DEVMODE devMode = { 0 };
    devMode.dmSize = sizeof(DEVMODE);

    for (int i = 0; EnumDisplaySettings(deviceName.c_str(), i, &devMode); ++i) {
        resolutions.push_back({ static_cast<LONG>(devMode.dmPelsWidth), static_cast<LONG>(devMode.dmPelsHeight) });
    }

    return resolutions;
}

void CompareAndPrintActiveDisplays(const std::vector<DisplayInfo>& oldList, const std::vector<DisplayInfo>& newList) {
    std::set<std::string> oldDisplaySet;
    for (const auto& display : oldList) {
        oldDisplaySet.insert(display.deviceName);
    }

    std::cout << "New Active Displays:\n";
    for (const auto& display : newList) {
        if (oldDisplaySet.find(display.deviceName) == oldDisplaySet.end()) {
            std::cout << "Index: " << display.index << ", Device Name: " << display.deviceName << '\n';

            std::vector<Resolution> resolutions = GetDisplayResolutions(display.deviceName);
            bool resolutionFound = false;

            for (const auto& resolution : resolutions) {
                if (resolution.width == 852 && resolution.height == 480) {
                    resolutionFound = true;
                    std::cout << "Changing resolution to 852x480...\n";
                    DEVMODE devMode = { 0 };
                    devMode.dmSize = sizeof(DEVMODE);
                    devMode.dmPelsWidth = resolution.width;
                    devMode.dmPelsHeight = resolution.height;
                    devMode.dmFields = DM_PELSWIDTH | DM_PELSHEIGHT;

                    LONG result = ChangeDisplaySettingsEx(display.deviceName.c_str(), &devMode, nullptr, CDS_UPDATEREGISTRY, nullptr);
                    if (result == DISP_CHANGE_SUCCESSFUL) {
                        std::cout << "Resolution changed successfully.\n";
                    }
                    else {
                        std::cout << "Failed to change resolution.\n";
                    }

                    break;
                }
            }
        }
    }
}

void signalHandler(int signum) {
    CleanupAndExit();
    exit(signum);
}

void CleanupAndExit() {
    for (int index : displays) {
        VddRemoveDisplay(vdd, index);
    }

    if (updater.joinable()) {
        updater.join();
    }

    CloseDeviceHandle(vdd);
}
