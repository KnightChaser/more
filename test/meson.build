# test/meson.build

test_exe = executable(
  'test_utils',
  'test_utils.c',
  '../src/utils.c',
  include_directories: '../src',
)

test('utils functions test', test_exe)
