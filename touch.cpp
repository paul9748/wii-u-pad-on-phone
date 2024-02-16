#include <node.h>
#include "parsec-vdd.h"

namespace ParsecAddon
{

    // Wrapper for DeviceStatus enum
    enum DeviceStatus
    {
        DEVICE_OK = parsec_vdd::DEVICE_OK,
        DEVICE_INACCESSIBLE = parsec_vdd::DEVICE_INACCESSIBLE,
        DEVICE_UNKNOW = parsec_vdd::DEVICE_UNKNOW,
        DEVICE_UNKNOW_PROBLEM = parsec_vdd::DEVICE_UNKNOW_PROBLEM,
        DEVICE_DISABLED = parsec_vdd::DEVICE_DISABLED,
        DEVICE_DRIVER_ERROR = parsec_vdd::DEVICE_DRIVER_ERROR,
        DEVICE_RESTART_REQUIRED = parsec_vdd::DEVICE_RESTART_REQUIRED,
        DEVICE_DISABLED_SERVICE = parsec_vdd::DEVICE_DISABLED_SERVICE,
        DEVICE_NOT_INSTALLED = parsec_vdd::DEVICE_NOT_INSTALLED
    };

    // Wrapper for VddCtlCode enum
    enum VddCtlCode
    {
        VDD_IOCTL_ADD = parsec_vdd::VDD_IOCTL_ADD,
        VDD_IOCTL_REMOVE = parsec_vdd::VDD_IOCTL_REMOVE,
        VDD_IOCTL_UPDATE = parsec_vdd::VDD_IOCTL_UPDATE,
        VDD_IOCTL_VERSION = parsec_vdd::VDD_IOCTL_VERSION
    };

    // Wrapper for VddIoControl function
    uint32_t VddIoControl(HANDLE vdd, VddCtlCode code, const void *data, size_t size)
    {
        return parsec_vdd::VddIoControl(vdd, static_cast<parsec_vdd::VddCtlCode>(code), data, size);
    }

    // Wrapper for VddVersion function
    int VddVersion(HANDLE vdd)
    {
        return parsec_vdd::VddVersion(vdd);
    }

    // Wrapper for VddUpdate function
    void VddUpdate(HANDLE vdd)
    {
        parsec_vdd::VddUpdate(vdd);
    }

    // Wrapper for VddAddDisplay function
    int VddAddDisplay(HANDLE vdd)
    {
        return parsec_vdd::VddAddDisplay(vdd);
    }

    // Wrapper for VddRemoveDisplay function
    void VddRemoveDisplay(HANDLE vdd, int index)
    {
        parsec_vdd::VddRemoveDisplay(vdd, index);
    }

    // Wrapper for QueryDeviceStatus function
    DeviceStatus QueryDeviceStatus(const char *deviceId)
    {
        return static_cast<DeviceStatus>(parsec_vdd::QueryDeviceStatus(&parsec_vdd::VDD_CLASS_GUID, deviceId));
    }

    // Wrapper for OpenDeviceHandle function
    HANDLE OpenDeviceHandle()
    {
        return parsec_vdd::OpenDeviceHandle(&parsec_vdd::VDD_ADAPTER_GUID);
    }

    // Wrapper for CloseDeviceHandle function
    void CloseDeviceHandle(HANDLE vdd)
    {
        parsec_vdd::CloseDeviceHandle(vdd);
    }

    // Wrapper for removing virtual displays up to numMonitors
    void RemoveAllVirtualDisplays(HANDLE vdd, int numMonitors)
    {
        // Check if numMonitors is within valid range
        if (numMonitors <= 0 || numMonitors > parsec_vdd::VDD_MAX_DISPLAYS)
        {
            // Handle the error as needed
            printf("Invalid number of monitors specified.\n");
            return;
        }

        // Logic to remove virtual displays up to numMonitors
        std::vector<int> displaysToRemove;

        // Get the current number of displays
        int currentNumDisplays = VddIoControl(vdd, VDD_IOCTL_VERSION, nullptr, 0);

        // Determine the number of displays to remove
        int numDisplaysToRemove = std::min(numMonitors, currentNumDisplays);

        // Collect the indices of displays to be removed
        for (int i = 0; i < numDisplaysToRemove; ++i)
        {
            int index = VddIoControl(vdd, VDD_IOCTL_REMOVE, nullptr, 0);
            displaysToRemove.push_back(index);
        }

        // Print the removed display indices
        printf("Removed virtual displays up to index: ");
        for (int index : displaysToRemove)
        {
            printf("%d, ", index);
        }
        printf("\n");

        // Perform any additional cleanup or logging as needed
    }

    // Exported functions to Node.js
    void Init(v8::Local<v8::Object> exports)
    {
        NODE_SET_METHOD(exports, "queryDeviceStatus", QueryDeviceStatus);
        NODE_SET_METHOD(exports, "openDeviceHandle", OpenDeviceHandle);
        NODE_SET_METHOD(exports, "closeDeviceHandle", CloseDeviceHandle);
        NODE_SET_METHOD(exports, "vddIoControl", VddIoControl);
        NODE_SET_METHOD(exports, "vddVersion", VddVersion);
        NODE_SET_METHOD(exports, "vddUpdate", VddUpdate);
        NODE_SET_METHOD(exports, "vddAddDisplay", VddAddDisplay);
        NODE_SET_METHOD(exports, "vddRemoveDisplay", VddRemoveDisplay);
        NODE_SET_METHOD(exports, "removeAllVirtualDisplays", RemoveAllVirtualDisplays);
    }

    NODE_MODULE(NODE_GYP_MODULE_NAME, Init)

} // namespace ParsecAddon
