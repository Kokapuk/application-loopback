#include <windows.h>

#include "loopback-capture-wrapper.h"

Napi::Value StartLoopbackCapture(const Napi::CallbackInfo &info)
{
	Napi::Env env = info.Env();

	if (info.Length() < 2 || !info[0].IsNumber() || !info[1].IsFunction())
	{
		Napi::TypeError::New(env, "Expected (number, function)").ThrowAsJavaScriptException();
		return env.Undefined();
	}

	if (hasActiveProcess)
	{
		Napi::TypeError::New(env, "Already capturing, call \"stopLoopbackCapture\" first").ThrowAsJavaScriptException();
		return env.Undefined();
	}

	DWORD processId = info[0].As<Napi::Number>().Uint32Value();
	Napi::Function callback = info[1].As<Napi::Function>();

	loopbackCapture.StartCaptureAsync(processId, true, env, callback);
	hasActiveProcess = true;

	return env.Undefined();
}

Napi::Value StopLoopbackCapture(const Napi::CallbackInfo &info)
{
	Napi::Env env = info.Env();

	loopbackCapture.StopCaptureAsync();
	hasActiveProcess = false;

	return env.Undefined();
}