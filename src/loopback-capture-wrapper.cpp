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

	HRESULT result = loopbackCapture.StartCaptureAsync(processId, true, env, callback);

	if (result == E_INVALIDARG)
	{
		Napi::TypeError::New(env, "Process with that id does not exist").ThrowAsJavaScriptException();
		return env.Undefined();
	} else if (result != S_OK) {
		Napi::TypeError::New(env, "Unknown error").ThrowAsJavaScriptException();
		return env.Undefined();
	}

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