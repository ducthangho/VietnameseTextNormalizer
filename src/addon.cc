#include "myobject.h"

Napi::Object Init(Napi::Env env, Napi::Object exports) {
	Napi::String name = Napi::String::New(env, "VietnameseTextNormalizer");
	exports.Set(name, vnTextNormalizer::GetClass(env));
	return exports;
}

//NAPI_MODULE(NODE_GYP_MODULE_NAME, Init)
NODE_API_MODULE(addon, Init)
