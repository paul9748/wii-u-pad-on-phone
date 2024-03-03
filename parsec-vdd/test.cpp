//#include <stdio.h>
//#include <thread>
//#include <chrono>
//#include <vector>
//#include <iostream>
//#include <windows.h>
//#include "./parsec-vdd.h"
//
//using namespace std::chrono_literals;
//using namespace parsec_vdd;
//
//devmode devmode;
//
//void changeresolution(const char* devicename, int width, int height) {
//    devmode.dmsize = sizeof(devmode);
//    devmode.dmpelswidth = width;
//    devmode.dmpelsheight = height;
//    devmode.dmfields = dm_pelswidth | dm_pelsheight;
//
//    long result = changedisplaysettingsex(devicename, &devmode, null, cds_updateregistry, null);
//    if (result != disp_change_successful) {
//        std::cerr << "failed to change resolution" << std::endl;
//    }
//}
//
//void printactivedisplays() {
//    display_device displaydevice;
//    displaydevice.cb = sizeof(displaydevice);
//
//    std::cout << "list of active displays:" << std::endl;
//
//    for (int displayindex = 0; enumdisplaydevices(null, displayindex, &displaydevice, 0); ++displayindex) {
//        if (!(displaydevice.stateflags & display_device_active))
//            continue;
//
//        std::cout << displayindex + 1 << ". " << displaydevice.devicestring << std::endl;
//    }
//}
//
//void printavailableresolutions(const char* devicename) {
//    int modeindex = 0;
//    enumdisplaysettings(devicename, modeindex, &devmode);
//    std::cout << "list of available resolutions for the selected display:" << std::endl;
//
//    while (enumdisplaysettings(devicename, modeindex, &devmode)) {
//        std::cout << modeindex + 1 << ". width: " << devmode.dmpelswidth << " pixels, height: " << devmode.dmpelsheight << " pixels" << std::endl;
//        ++modeindex;
//    }
//}
//
//int selectdisplayandresolution() {
//    printactivedisplays();
//
//    int selecteddisplay;
//    std::cout << "enter the number of the desired display: ";
//    std::cin >> selecteddisplay;
//
//    display_device displaydevice;
//    zeromemory(&displaydevice, sizeof(displaydevice));
//    displaydevice.cb = sizeof(displaydevice);
//    enumdisplaydevices(null, selecteddisplay - 1, &displaydevice, 0);
//
//    printavailableresolutions(displaydevice.devicename);
//
//    int selectedresolution;
//    std::cout << "enter the number of the desired resolution: ";
//    std::cin >> selectedresolution;
//
//    int modeindex = selectedresolution - 1;
//    enumdisplaysettings(displaydevice.devicename, modeindex, &devmode);
//    changeresolution(displaydevice.devicename, devmode.dmpelswidth, devmode.dmpelsheight);
//
//    return 0;
//}
//
//int main() {
//    devicestatus status = querydevicestatus(&vdd_class_guid, vdd_hardware_id);
//    if (status != device_ok) {
//        printf("parsec vdd device is not ok, got status %d.\n", status);
//        return 1;
//    }
//
//    handle vdd = opendevicehandle(&vdd_adapter_guid);
//    if (vdd == null || vdd == invalid_handle_value) {
//        printf("failed to obtain the device handle.\n");
//        return 1;
//    }
//
//    bool running = true;
//    std::vector<int> displays;
//
//    std::thread updater([&running, vdd] {
//        while (running) {
//            vddupdate(vdd);
//            std::this_thread::sleep_for(100ms);
//        }
//        });
//    updater.detach();
//
//    printf("press a to add a virtual display.\n");
//    printf("press r to remove the last added.\n");
//    printf("press s to show available displays and change resolution.\n");
//    printf("press q to quit (then unplug all).\n\n");
//
//
//
//    while (running) {
//        int key = getchar();  // use getchar() instead of _getch()
//
//        switch (key) {
//        case 'q':
//            running = false;
//            break;
//        case 'a':
//            if (displays.size() < vdd_max_displays) {
//                int index = vddadddisplay(vdd);
//                displays.push_back(index);
//                printf("added a new virtual display, index: %d.\n", index);
//            }
//            else {
//                printf("limit exceeded (%d), could not add more virtual displays.\n", vdd_max_displays);
//            }
//            break;
//        case 'r':
//            if (displays.size() > 0) {
//                int index = displays.back();
//                vddremovedisplay(vdd, index);
//                displays.pop_back();
//                printf("removed the last virtual display, index: %d.\n", index);
//            }
//            else {
//                printf("no added virtual displays.\n");
//            }
//            break;
//        case 's':
//            selectdisplayandresolution();
//            break;
//        }
//    }
//
//    for (int index : displays) {
//        vddremovedisplay(vdd, index);
//    }
//
//    if (updater.joinable()) {
//        updater.join();
//    }
//
//    closedevicehandle(vdd);
//
//    return 0;
//}
