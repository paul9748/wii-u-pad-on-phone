#include <node.h>
#include <windows.h>
#include <string>

using namespace v8;

HWND FindWindowByTitle(const char *title)
{
    int unicode_length = MultiByteToWideChar(CP_UTF8, 0, title, -1, NULL, 0);
    wchar_t *unicode_title = new wchar_t[unicode_length];
    MultiByteToWideChar(CP_UTF8, 0, title, -1, unicode_title, unicode_length);

    HWND window_handle = FindWindowW(NULL, unicode_title);

    delete[] unicode_title;

    return window_handle;
}

std::string GetActiveWindowTitle()
{
    HWND hwnd = GetForegroundWindow();
    if (hwnd == NULL)
    {
        return "";
    }

    int length = GetWindowTextLengthW(hwnd);
    wchar_t *buffer = new wchar_t[length + 1];
    GetWindowTextW(hwnd, buffer, length + 1);
    std::wstring utf16Title(buffer);
    delete[] buffer;

    int utf8Length = WideCharToMultiByte(CP_UTF8, 0, utf16Title.c_str(), -1, NULL, 0, NULL, NULL);
    char *utf8Buffer = new char[utf8Length];
    WideCharToMultiByte(CP_UTF8, 0, utf16Title.c_str(), -1, utf8Buffer, utf8Length, NULL, NULL);
    std::string utf8Title(utf8Buffer);
    delete[] utf8Buffer;

    return utf8Title;
}

void resizeWindow(const FunctionCallbackInfo<Value> &args)
{
    Isolate *isolate = args.GetIsolate();
    HWND hwnd = FindWindowByTitle(*String::Utf8Value(isolate, args[0]->ToString(isolate->GetCurrentContext()).FromMaybe(v8::Local<v8::String>())));
    if (hwnd == NULL)
    {
        isolate->ThrowException(Exception::TypeError(String::NewFromUtf8(isolate, "Window not found:").ToLocalChecked()));
        return;
    }
    {
        RECT rect;
        GetWindowRect(hwnd, &rect);
        int titleBarHeight = GetSystemMetrics(SM_CYCAPTION);
        int newWidth = 854;
        int newHeight = 480 + titleBarHeight;
        SetWindowPos(hwnd, NULL, 0, 0, newWidth, newHeight, SWP_NOMOVE | SWP_NOZORDER);
        args.GetReturnValue().Set(titleBarHeight);
    }
}

void getWindowTitle(const FunctionCallbackInfo<Value> &args)
{
    Isolate *isolate = args.GetIsolate();
    std::string windowTitle = GetActiveWindowTitle();
    args.GetReturnValue().Set(String::NewFromUtf8(isolate, windowTitle.c_str()).ToLocalChecked());
    return;
}

void mouseAction(const FunctionCallbackInfo<Value> &args)
{
    Isolate *isolate = args.GetIsolate();

    v8::String::Utf8Value param1(isolate, args[0]->ToString(isolate->GetCurrentContext()).FromMaybe(v8::Local<v8::String>()));
    std::string action = std::string(*param1);
    int x = args[1]->Int32Value(isolate->GetCurrentContext()).FromJust();
    int y = args[2]->Int32Value(isolate->GetCurrentContext()).FromJust();

    HWND hwnd = FindWindowByTitle(*String::Utf8Value(isolate, args[3]->ToString(isolate->GetCurrentContext()).FromMaybe(v8::Local<v8::String>())));
    if (hwnd == NULL)
    {
        isolate->ThrowException(Exception::TypeError(String::NewFromUtf8(isolate, "Window not found").ToLocalChecked()));
        return;
    }

    if (action == "mousedown")
    {
        SendMessage(hwnd, WM_LBUTTONDOWN, MK_LBUTTON, MAKELPARAM(x, y));
    }
    else if (action == "mouseup")
    {
        SendMessage(hwnd, WM_LBUTTONUP, 0, MAKELPARAM(x, y));
    }
    else if (action == "move")
    {
        SetCursorPos(x, y);
    }
    else
    {
        isolate->ThrowException(Exception::TypeError(String::NewFromUtf8(isolate, "Invalid action").ToLocalChecked()));
        return;
    }
}

void Init(Local<Object> exports)
{
    NODE_SET_METHOD(exports, "mouseAction", mouseAction);
    NODE_SET_METHOD(exports, "resizeWindow", resizeWindow);
    NODE_SET_METHOD(exports, "getWindowTitle", getWindowTitle);
}

NODE_MODULE(touch, Init)
