unity_subproject = subproject('unity')

unity_dependency = unity_subproject.get_variable('unity_dep')

test_dependencies = [unity_dependency, c_minilib_sip_ua_dep, c_minilib_mock_dep]

test_runner = unity_subproject.get_variable('gen_test_runner')

test_link_args = ['-fsanitize=memory,undefined']

test_include_dirs = [include_directories('.')]

subdir('test_unity.d')

c_minilib_mock_proj = subproject('c_minilib_mock')
mock_flags_gen_script = c_minilib_mock_proj.get_variable('wraps_gen_script')

# List of test source files (extend this list as needed)
test_files = [
  'test_event_loop.c',
  'test_event_loop_fd_vec.c',
  'test_event_loop_fdhelper_hmap.c',  
  'test_udp_socket.c',
  'test_sip_transport.c',    
  # 'test_pollfd_vec.c',
  # 'test_socket.c',
]

foreach test_file : test_files
  test_name = test_file.split('.')[0]  # e.g., "test_parser"
  test_exe = executable(test_name,
    sources: [
      test_file,
      test_runner.process(test_file),
    ],
    dependencies: test_dependencies,
    include_directories: test_include_dirs,
    c_args: ['-DENABLE_MOCKS'],
    link_args: run_command(mock_flags_gen_script, ['UdpSocket_create_fd'], check: true).stdout().strip().split(' ')	    
  )
  test(test_name, test_exe)
endforeach
