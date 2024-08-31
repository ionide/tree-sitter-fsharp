#include <napi.h>

typedef struct TSLanguage TSLanguage;

extern "C" TSLanguage *tree_sitter_fsharp();
extern "C" TSLanguage *tree_sitter_fsharp_signature();

// "tree-sitter", "language" hashed with BLAKE2
const napi_type_tag LANGUAGE_TYPE_TAG = {0x8AF2E5212AD58ABF,
                                         0xD5006CAD83ABBA16};

Napi::Object Init(Napi::Env env, Napi::Object exports) {
  auto fsharp_exports = Napi::Object::New(env);
  fsharp_exports["name"] = Napi::String::New(env, "fsharp");
  auto fsharp_language =
      Napi::External<TSLanguage>::New(env, tree_sitter_fsharp());
  fsharp_language.TypeTag(&LANGUAGE_TYPE_TAG);
  fsharp_exports["language"] = fsharp_language;
  exports["fsharp"] = fsharp_exports;

  auto signature_exports = Napi::Object::New(env);
  signature_exports["name"] = Napi::String::New(env, "fsharp_signature");
  auto signature_language =
      Napi::External<TSLanguage>::New(env, tree_sitter_fsharp_signature());
  signature_language.TypeTag(&LANGUAGE_TYPE_TAG);
  signature_exports["language"] = signature_language;
  exports["signature"] = signature_exports;

  return exports;
}

NODE_API_MODULE(tree_sitter_fsharp_binding, Init)
