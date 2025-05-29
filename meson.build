# C Minilib Sip UA
# ******************************************************************************
# *    Project
# ******************************************************************************
project('c_minilib_sip_ua', 'c',
         version: '0.0.1',
	 meson_version: '>=1.0.1',
	 default_options: [
            'werror=true',
            'c_std=c11',
         ],
)

# ******************************************************************************
# *    App
# ******************************************************************************
if get_option('backtrace')
  add_project_arguments('-DCME_ENABLE_BACKTRACE', language: 'c')
endif
if get_option('buildtype').startswith('debug')
  add_project_arguments(['-fno-inline', '-fno-inline-functions'], language : 'c')  
endif

# We need this for socket99
add_project_arguments('-D_POSIX_C_SOURCE=200112L', language: 'c')

subdir('src')

c_minilib_error_dep = dependency('c_minilib_error',
  fallback: ['c_minilib_error', 'c_minilib_error_dep'],
  required: true,
)

c_minilib_mock_dep = dependency('c_minilib_mock',
  fallback: ['c_minilib_mock', 'c_minilib_mock_dep'],
  required: true,
)

c_minilib_init_dep = dependency('c_minilib_init',
  fallback: ['c_minilib_init', 'c_minilib_init_dep'],
  required: true,
)

c_minilib_config_dep = dependency('c_minilib_config',
  fallback: ['c_minilib_config', 'c_minilib_config_dep'],
  required: true,
)

c_minilib_sip_codec_dep = dependency('c_minilib_sip_codec',
  fallback: ['c_minilib_sip_codec', 'c_minilib_sip_codec_dep'],
  required: true,
)

stc_dep = dependency('stc',
  fallback: ['stc', 'stc_dep'],
  default_options: ['checkscoped=disabled'],
  required: true,
)

socket99_dep = dependency(
  'socket99',
  fallback: ['socket99', 'socket99_dep'],
  required: true
)

c_minilib_sip_ua_deps = [c_minilib_error_dep,
                         c_minilib_mock_dep,
			 c_minilib_init_dep,
			 c_minilib_config_dep,
			 c_minilib_sip_codec_dep,			 
			 stc_dep,
			 socket99_dep,
			 ]
c_minilib_sip_ua_inc = include_directories('include', 'src')

c_minilib_sip_ua_lib = library('c_minilib_sip_ua',
  sources,
  include_directories: c_minilib_sip_ua_inc,
  dependencies: c_minilib_sip_ua_deps,
  c_args: ['-DENABLE_MOCKS'], # This should be disabled when tests are disabled
  
)

c_minilib_sip_ua_dep = declare_dependency(
  link_with: c_minilib_sip_ua_lib,
  include_directories: c_minilib_sip_ua_inc,
  dependencies: c_minilib_sip_ua_deps,
)


# ******************************************************************************
# *    Executable
# ******************************************************************************
executable('sipua_demo',
  sources,
  include_directories: c_minilib_sip_ua_inc,
  dependencies: c_minilib_sip_ua_dep,  
  c_args: ['-DENABLE_MOCKS']
)

# ******************************************************************************
# *    Tests
# ******************************************************************************
if get_option('tests')
   subdir('test')
endif

# ******************************************************************************
# *    Examples
# ******************************************************************************
if get_option('examples')
   subdir('example')
endif
