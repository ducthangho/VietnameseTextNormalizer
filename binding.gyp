{
  'targets': [
    {
      'target_name': 'vietnamesetextnormalizer',
      'sources': [ "src/addon.cc", "src/myobject.cc", "src/ContextSystem.cpp","src/SyllableSystem.cpp","src/VietnameseTextNormalizer.cpp","src/WordSystem.cpp" ],
      'include_dirs': ["<!@(node -p \"require('node-addon-api').include\")"],
      'dependencies': ["<!(node -p \"require('node-addon-api').gyp\")"],
      'cflags!': [ '-fno-exceptions','-O3' ],
      'cflags_cc!': [ '-fno-exceptions','-O3' ],
      'xcode_settings': {
        'GCC_ENABLE_CPP_EXCEPTIONS': 'YES',
        'CLANG_CXX_LIBRARY': 'libc++',
        'MACOSX_DEPLOYMENT_TARGET': '10.7'
      },
      'msvs_settings': {
        'VCCLCompilerTool': { 'ExceptionHandling': 1 },
      }
    }
  ]
}