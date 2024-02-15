#include <node.h>
#include <thread>
#include <chrono>
#include <vector>
#include <nan.h>
#include "parsec-vdd.h"

using namespace v8;
using namespace std::chrono_literals;
using namespace parsec_vdd;

// Global variables
bool running = true;
std::vector<int> displays;
HANDLE vdd = NULL;

// Function to add a virtual display
void AddVirtualDisplay(const FunctionCallbackInfo<Value> &args)
{
    Isolate *isolate = args.GetIsolate();

    // Check if the maximum number of displays has been reached
    if (displays.size() >= VDD_MAX_DISPLAYS)
    {
        isolate->ThrowException(Exception::TypeError(
            String::NewFromUtf8Literal(isolate, "Limit exceeded, could not add more virtual displays.")));
        return;
    }

    // Add a new virtual display
    int index = VddAddDisplay(vdd);
    displays.push_back(index);

    args.GetReturnValue().Set(index);
}

// Function to remove the last added virtual display
void RemoveLastVirtualDisplay(const FunctionCallbackInfo<Value> &args)
{
    Isolate *isolate = args.GetIsolate();

    // Check if there are any displays to remove
    if (displays.empty())
    {
        isolate->ThrowException(Exception::TypeError(
            String::NewFromUtf8Literal(isolate, "No added virtual displays.")));
        return;
    }

    // Remove the last virtual display
    int index = displays.back();
    VddRemoveDisplay(vdd, index);
    displays.pop_back();

    args.GetReturnValue().Set(index);
}

// Function to stop the program and clean up resources
void StopProgram(const FunctionCallbackInfo<Value> &args)
{
    // Stop the main loop
    running = false;

    // Remove all virtual displays before exiting
    for (int index : displays)
    {
        VddRemoveDisplay(vdd, index);
    }

    // Close the device handle
    if (vdd != NULL && vdd != INVALID_HANDLE_VALUE)
    {
        CloseDeviceHandle(vdd);
        vdd = NULL;
    }
}

// Function for the side thread to update vdd
void UpdateVdd()
{
    while (running)
    {
        VddUpdate(vdd);
        std::this_thread::sleep_for(100ms);
    }
}

// Function to initialize the native addon
void InitializeAddon(Local<Object> exports)
{
    // Check driver status

    DeviceStatus status = QueryDeviceStatus(&VDD_CLASS_GUID, VDD_HARDWARE_ID);
    if (status != DEVICE_OK)
    {
        Nan::ThrowError("Parsec VDD device is not OK.");
        return;
    }

    // Obtain device handle
    vdd = OpenDeviceHandle(&VDD_ADAPTER_GUID);
    if (vdd == NULL || vdd == INVALID_HANDLE_VALUE)
    {
        Nan::ThrowError("Failed to obtain the device handle.");
        return;
    }

    // Create a new thread for updating vdd
    std::thread updater(UpdateVdd);
    updater.detach();

    // Define the functions to be exported
    NODE_SET_METHOD(exports, "addVirtualDisplay", AddVirtualDisplay);
    NODE_SET_METHOD(exports, "removeLastVirtualDisplay", RemoveLastVirtualDisplay);
    NODE_SET_METHOD(exports, "stopProgram", StopProgram);
}

// Register the addon initialization function
NODE_MODULE(NativeAddon, InitializeAddon)
