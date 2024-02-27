#include <node.h>
#include <windows.h>

void MousePress(const v8::FunctionCallbackInfo<v8::Value> &args)
{
    v8::Isolate *isolate = args.GetIsolate();
    int buttonCode = args[0]->Int32Value(isolate->GetCurrentContext()).ToChecked();
    int x = args[1]->Int32Value(isolate->GetCurrentContext()).ToChecked();
    int y = args[2]->Int32Value(isolate->GetCurrentContext()).ToChecked();

    SetCursorPos(x, y);
    mouse_event(MOUSEEVENTF_LEFTDOWN, 0, 0, 0, 0);

    args.GetReturnValue().Set(v8::Boolean::New(isolate, true));
}

void MouseMove(const v8::FunctionCallbackInfo<v8::Value> &args)
{
    v8::Isolate *isolate = args.GetIsolate();
    int x = args[0]->Int32Value(isolate->GetCurrentContext()).ToChecked();
    int y = args[1]->Int32Value(isolate->GetCurrentContext()).ToChecked();

    SetCursorPos(x, y);

    args.GetReturnValue().Set(v8::Boolean::New(isolate, true));
}

void MouseRelease(const v8::FunctionCallbackInfo<v8::Value> &args)
{
    v8::Isolate *isolate = args.GetIsolate();
    int buttonCode = args[0]->Int32Value(isolate->GetCurrentContext()).ToChecked();
    int x = args[1]->Int32Value(isolate->GetCurrentContext()).ToChecked();
    int y = args[2]->Int32Value(isolate->GetCurrentContext()).ToChecked();

    SetCursorPos(x, y);
    mouse_event(MOUSEEVENTF_LEFTUP, 0, 0, 0, 0);

    args.GetReturnValue().Set(v8::Boolean::New(isolate, true));
}

void Init(v8::Local<v8::Object> exports, v8::Local<v8::Object> module)
{
    NODE_SET_METHOD(exports, "mousePress", MousePress);
    NODE_SET_METHOD(exports, "mouseMove", MouseMove);
    NODE_SET_METHOD(exports, "mouseRelease", MouseRelease);
}

NODE_MODULE(mouseaddon, Init)
