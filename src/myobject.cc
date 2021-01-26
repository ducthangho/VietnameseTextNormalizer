#include "myobject.h"
#include <assert.h>

using namespace Napi;

vnTextNormalizer::vnTextNormalizer(const Napi::CallbackInfo& info) : ObjectWrap(info){

}


#define DECLARE_NAPI_METHOD(name, func)                                        \
  { name, 0, func, 0, 0, 0, napi_default, 0 }


static void	ConvertUtf8toUnicode(const unsigned char* utf8str, int utf8strlength, qwchar* ucs2buffer)
{
	if (utf8str && ucs2buffer)
	{
		for (int ichar = 0; ichar < utf8strlength && utf8str[0] != 0; ichar++)
		{
			if ((utf8str[0] & 0xE0) == 0xC0 && (utf8str[1] & 0xC0) == 0x80)
			{
				/* 2 bytes UTF-8 Character.*/
				*ucs2buffer = (((qwchar)(utf8str[0] & 0x001F)) << 6) | (utf8str[1] & 0x3F);
				ucs2buffer++;
				utf8str += 2;
				ichar++;
			}
			else if ((utf8str[0] & 0xF0) == 0xE0 && (utf8str[1] & 0xC0) == 0x80 && (utf8str[2] & 0xC0) == 0x80)
			{
				/* 3bytes UTF-8 Character.*/
				*ucs2buffer = (((qwchar)(utf8str[0] & 0x000F)) << 12) | (((qwchar)(utf8str[1] & 0x3F)) << 6) | ((qwchar)(utf8str[2] & 0x3F));
				ucs2buffer++;
				utf8str += 3;
				ichar += 2;
			}
			else
			{
				/* 1 byte UTF-8 Character.*/
				*ucs2buffer = *utf8str;
				ucs2buffer++;
				utf8str++;
			}
		}
	}
}
static int	ConvertUnicodetoUtf8(const qwchar* ucs2str, int ucs2length, unsigned char* utf8buffer)
{
	if (ucs2str && ucs2length > 0 && utf8buffer)
	{
		int				length = 0;
		unsigned char* out = utf8buffer;
		qwchar			temp_ucs2str_index = *ucs2str;
		for (int counter = 0, step = 0, end = ucs2length; counter < end; counter++)
		{
			if (0x0080 > * ucs2str)
			{
				/* 1 byte UTF-8 Character.*/
				*out = (unsigned char)(temp_ucs2str_index);
				length++;
				step = 1;
				ucs2str++;
				temp_ucs2str_index = *(ucs2str);
				out += step;
			}
			else if (0x0800 > * ucs2str)
			{
				if (*(out - 1) == 20)
				{
					*out = (unsigned char)(temp_ucs2str_index);
					length++;
					step = 1;
					ucs2str++;
					temp_ucs2str_index = *(ucs2str);
					out += step;
					continue;
				}

				/*2 bytes UTF-8 Character.*/
				*out = ((unsigned char)(temp_ucs2str_index >> 6)) | 0xc0;
				*(out + 1) = ((unsigned char)(temp_ucs2str_index & 0x003F)) | 0x80;
				length += 2;
				step = 2;
				ucs2str++;
				temp_ucs2str_index = *(ucs2str);
				out += step;
			}
			else
			{
				/* 3 bytes UTF-8 Character .*/
				*out = ((unsigned char)(temp_ucs2str_index >> 12)) | 0xE0;
				*(out + 1) = ((unsigned char)((temp_ucs2str_index & 0x0FC0) >> 6)) | 0x80;
				*(out + 2) = ((unsigned char)(temp_ucs2str_index & 0x003F)) | 0x80;
				length += 3;
				step = 3;
				ucs2str++;
				temp_ucs2str_index = *(ucs2str);
				out += step;
			}
		}
		*out = 0;
		return length;
	}
	return 0;
}


Napi::Function vnTextNormalizer::GetClass(Napi::Env env) {
	return DefineClass(env, "VietnameseTextNormalizer", {
		vnTextNormalizer::InstanceMethod("Normalize", &vnTextNormalizer::Normalize),
		vnTextNormalizer::InstanceMethod("NormalizeForTTS", &vnTextNormalizer::NormalizeForTTS)
		});
}


Napi::Value vnTextNormalizer::Normalize(const Napi::CallbackInfo& info) {
	Napi::Env env = info.Env();
	if (info.Length() < 1) {
		Napi::TypeError::New(env, "Wrong number of arguments")
			.ThrowAsJavaScriptException();
		return env.Null();
	}

	if (!info[0].IsString()) {
		Napi::TypeError::New(env, "Wrong argument type")
			.ThrowAsJavaScriptException();
		return env.Null();
	}

	std::string utf8input = info[0].As<Napi::String>().Utf8Value();	
	std::string	utf8Result = utf8input;
		
	qwchar* ucs2buffer = (qwchar*)qcalloc(utf8Result.size() + 100/*safe*/, sizeof(qwchar));
	if (ucs2buffer)
	{
		ConvertUtf8toUnicode((const unsigned char*)(utf8Result.c_str()), utf8Result.size(), ucs2buffer);
		vn_normalizer.flagStandardTextForTTS = false;
		vn_normalizer.Input(ucs2buffer);
		vn_normalizer.Normalize();
		vn_normalizer.GenStandardText();
		if (vn_normalizer.standardText && vn_normalizer.standardTextChange > 0)
		{
			printf("Normalization : %d change(s) - Utf8 mode\n", vn_normalizer.standardTextChange);
			char* bufferUtf8 = (char*)calloc(vn_normalizer.standardTextLength * 4 + 100/*safe*/, sizeof(char));
			if (bufferUtf8)
			{
				ConvertUnicodetoUtf8(vn_normalizer.standardText, vn_normalizer.standardTextLength, (unsigned char*)bufferUtf8);
				utf8Result = bufferUtf8;
				qfree(bufferUtf8);
			}
		}
		qfree(ucs2buffer);
	}
	
	return Napi::String::New(env, utf8Result);
  
}


Napi::Value vnTextNormalizer::NormalizeForTTS(const Napi::CallbackInfo& info) {
	Napi::Env env = info.Env();
	if (info.Length() < 1) {
		Napi::TypeError::New(env, "Wrong number of arguments")
			.ThrowAsJavaScriptException();
		return env.Null();
	}

	if (!info[0].IsString()) {
		Napi::TypeError::New(env, "Wrong argument type")
			.ThrowAsJavaScriptException();
		return env.Null();
	}

	std::string utf8input = info[0].As<Napi::String>().Utf8Value();
	std::string	utf8Result = utf8input;

	qwchar* ucs2buffer = (qwchar*)qcalloc(utf8Result.size() + 100/*safe*/, sizeof(qwchar));
	if (ucs2buffer)
	{
		ConvertUtf8toUnicode((const unsigned char*)(utf8Result.c_str()), utf8Result.size(), ucs2buffer);
		vn_normalizer.flagStandardTextForTTS = true;
		vn_normalizer.Input(ucs2buffer);
		vn_normalizer.Normalize();
		vn_normalizer.GenStandardText();
		if (vn_normalizer.standardText && vn_normalizer.standardTextChange > 0)
		{
			printf("Normalization : %d change(s) - Utf8 mode\n", vn_normalizer.standardTextChange);
			char* bufferUtf8 = (char*)calloc(vn_normalizer.standardTextLength * 4 + 100/*safe*/, sizeof(char));
			if (bufferUtf8)
			{
				ConvertUnicodetoUtf8(vn_normalizer.standardText, vn_normalizer.standardTextLength, (unsigned char*)bufferUtf8);
				utf8Result = bufferUtf8;
				qfree(bufferUtf8);
			}
		}
		qfree(ucs2buffer);
	}

	return Napi::String::New(env, utf8Result);

}
