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

subdir('src')

# c_minilib_error_sub = subproject('c_minilib_error', default_options: ['tests=false'])
# c_minilib_error_dep = dependency('c_minilib_error',
#   fallback: ['c_minilib_error', 'c_minilib_error_dep'],
#   required: true,
# )

c_minilib_init_dep = dependency('c_minilib_init',
  fallback: ['c_minilib_init', 'c_minilib_init_dep'],
  required: true,
)

c_minilib_config_dep = dependency('c_minilib_config',
  fallback: ['c_minilib_config', 'c_minilib_config_dep'],
  required: true,
)

c_minilib_sip_ua_deps = [c_minilib_init_dep, c_minilib_config_dep]
c_minilib_sip_ua_inc = include_directories('include', 'src')
c_minilib_sip_ua_lib = library('c_minilib_sip_ua',
                         sources,
			 include_directories: c_minilib_sip_ua_inc,
                         dependencies: c_minilib_sip_ua_deps,
)

c_minilib_sip_ua_dep = declare_dependency(
  link_with: c_minilib_sip_ua_lib,
  include_directories: c_minilib_sip_ua_inc,
  dependencies: c_minilib_sip_ua_deps,  
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
