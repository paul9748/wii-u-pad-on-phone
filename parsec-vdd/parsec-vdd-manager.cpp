#include <stdio.h>
#include <thread>
#include <chrono>
#include <vector>
#include <iostream>
#include <Windows.h>
#include "./parsec-vdd.h"

using namespace std::chrono_literals;
using namespace parsec_vdd;

DEVMODE devMode;

void changeResolution(const char* deviceName, int width, int height) {
    devMode.dmSize = sizeof(devMode);
    devMode.dmPelsWidth = width;
    devMode.dmPelsHeight = height;
    devMode.dmFields = DM_PELSWIDTH | DM_PELSHEIGHT;

    LONG result = ChangeDisplaySettingsEx(deviceName, &devMode, NULL, CDS_UPDATEREGISTRY, NULL);
    if (result != DISP_CHANGE_SUCCESSFUL) {
        std::cerr << "Failed to change resolution" << std::endl;
    }
}

void printActiveDisplays() {
    DISPLAY_DEVICE displayDevice;
    displayDevice.cb = sizeof(displayDevice);

    std::cout << "List of active displays:" << std::endl;

    for (int displayIndex = 0; EnumDisplayDevices(NULL, displayIndex, &displayDevice, 0); ++displayIndex) {
        if (!(displayDevice.StateFlags & DISPLAY_DEVICE_ACTIVE))
            continue;

        std::cout << displayIndex + 1 << ". " << displayDevice.DeviceString << std::endl;
    }
}

void printAvailableResolutions(const char* deviceName) {
    int modeIndex = 0;
    EnumDisplaySettings(deviceName, modeIndex, &devMode);
    std::cout << "List of available resolutions for the selected display:" << std::endl;

    while (EnumDisplaySettings(deviceName, modeIndex, &devMode)) {
        std::cout << modeIndex + 1 << ". Width: " << devMode.dmPelsWidth << " pixels, Height: " << devMode.dmPelsHeight << " pixels" << std::endl;
        ++modeIndex;
    }
}

int selectDisplayAndResolution() {
    printActiveDisplays();

    int selectedDisplay;
    std::cout << "Enter the number of the desired display: ";
    std::cin >> selectedDisplay;

    DISPLAY_DEVICE displayDevice;
    ZeroMemory(&displayDevice, sizeof(displayDevice));
    displayDevice.cb = sizeof(displayDevice);
    EnumDisplayDevices(NULL, selectedDisplay - 1, &displayDevice, 0);

    printAvailableResolutions(displayDevice.DeviceName);

    int selectedResolution;
    std::cout << "Enter the number of the desired resolution: ";
    std::cin >> selectedResolution;

    int modeIndex = selectedResolution - 1;
    EnumDisplaySettings(displayDevice.DeviceName, modeIndex, &devMode);
    changeResolution(displayDevice.DeviceName, devMode.dmPelsWidth, devMode.dmPelsHeight);

    return 0;
}

int main() {
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

    printf("Press A to add a virtual display.\n");
    printf("Press R to remove the last added.\n");
    printf("Press S to show available displays and change resolution.\n");
    printf("Press Q to quit (then unplug all).\n\n");

    
        
    while (running) {
        int key = getchar();  // Use getchar() instead of _getch()

        switch (key) {
        case 'q':
            running = false;
            break;
        case 'a':
            if (displays.size() < VDD_MAX_DISPLAYS) {
                int index = VddAddDisplay(vdd);
                displays.push_back(index);
                printf("Added a new virtual display, index: %d.\n", index);
            }
            else {
                printf("Limit exceeded (%d), could not add more virtual displays.\n", VDD_MAX_DISPLAYS);
            }
            break;
        case 'r':
            if (displays.size() > 0) {
                int index = displays.back();
                VddRemoveDisplay(vdd, index);
                displays.pop_back();
                printf("Removed the last virtual display, index: %d.\n", index);
            }
            else {
                printf("No added virtual displays.\n");
            }
            break;
        case 's':
            selectDisplayAndResolution();
            break;
        }
    }

    for (int index : displays) {
        VddRemoveDisplay(vdd, index);
    }

    if (updater.joinable()) {
        updater.join();
    }

    CloseDeviceHandle(vdd);

    return 0;
}
