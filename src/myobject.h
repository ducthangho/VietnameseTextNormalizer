#ifndef TEST_ADDONS_NAPI_6_OBJECT_WRAP_MYOBJECT_H_
#define TEST_ADDONS_NAPI_6_OBJECT_WRAP_MYOBJECT_H_

#define NAPI_CPP_EXCEPTIONS

#include <napi.h>
#include <node_api.h>
#include "VietnameseTextNormalizer.h"



class vnTextNormalizer : public Napi::ObjectWrap < vnTextNormalizer>{
 public:

  vnTextNormalizer(const Napi::CallbackInfo&);
  //static Napi::Object Init(Napi::Env env, Napi::Object  exports);
  //static void Destructor(napi_env env, void* nativeObject, void* finalize_hint);
  Napi::Value Normalize(const Napi::CallbackInfo& info);
  Napi::Value NormalizeForTTS(const Napi::CallbackInfo& info);
  static Napi::Function GetClass(Napi::Env);

 private:
  explicit vnTextNormalizer();
  
  //static inline napi_value Constructor(napi_env env);

  VietnameseTextNormalizer vn_normalizer;  
};

#endif  // TEST_ADDONS_NAPI_6_OBJECT_WRAP_MYOBJECT_H_
