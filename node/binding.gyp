{
  'variables': {
    'WEBNN_NATIVE_DIR': '<@(module_root_dir)/../',
  },
  'conditions': [
    [ 'OS=="win"',  {
      'variables': { 
        'WEBNN_NATIVE_LIBRARY': '-lwebnn_native.dll.lib',
        'WEBNN_PROC_LIBRARY': '-lwebnn_proc.dll.lib',
      }}
    ],
    [ 'OS=="linux"', {
      'variables': { 
        'WEBNN_NATIVE_LIBRARY': '-lwebnn_native',
        'WEBNN_PROC_LIBRARY': '-lwebnn_proc',
      }}
    ]
  ],
  'targets': [
    {
      'target_name': 'webnn_node',
      'sources': [
        "<!@(node -p \"require('fs').readdirSync('./src').map(f=>'src/'+f).join(' ')\")",
        "<!@(node -p \"require('fs').readdirSync('./src/ops').map(f=>'src/ops/'+f).join(' ')\")"
      ],
      'cflags!': [ '-fno-exceptions', '-fno-rtti'],
      'cflags_cc!': [ '-fno-exceptions', '-fno-rtti'],
      'default_configuration': 'Release',
      'configurations': {
        'Debug': {
          'msvs_settings': {
            'VCCLCompilerTool': {
              'ExceptionHandling': 1,
              'RuntimeTypeInfo': 'true',
              'RuntimeLibrary': 3 # MultiThreadedDebugDLL (/MDd)
            },
          },
        },
        'Release': {
          'msvs_settings': {
            'VCCLCompilerTool': { 
              'ExceptionHandling': 1,
              'RuntimeTypeInfo': 'true',
              'RuntimeLibrary': 2 # MultiThreadedDLL (/MD)
            },
          },
        }
      },
      'include_dirs' : [
        '<!@(node -p "require(\'node-addon-api\').include")',
        '<(module_root_dir)/src',
        '<(WEBNN_NATIVE_DIR)/src/include',
        '<(webnn_native_lib_path)/gen/src/include',
      ],
      'library_dirs' : [
        '<(webnn_native_lib_path)',
      ],
      'libraries' : [
        '<(WEBNN_NATIVE_LIBRARY)',
        '<(WEBNN_PROC_LIBRARY)'
      ]
    }
  ]
}
