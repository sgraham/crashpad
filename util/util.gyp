# Copyright 2014 The Crashpad Authors. All rights reserved.
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#     http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.

{
  'targets': [
    {
      'target_name': 'util',
      'type': 'static_library',
      'dependencies': [
        '../compat/compat.gyp:compat',
        '../third_party/mini_chromium/mini_chromium/base/base.gyp:base',
      ],
      'include_dirs': [
        '..',
        '<(INTERMEDIATE_DIR)',
      ],
      'sources': [
        'file/fd_io.cc',
        'file/fd_io.h',
        'file/file_writer.cc',
        'file/file_writer.h',
        'file/string_file_writer.cc',
        'file/string_file_writer.h',
        'mac/checked_mach_address_range.cc',
        'mac/checked_mach_address_range.h',
        'mac/launchd.h',
        'mac/launchd.mm',
        'mac/mac_util.cc',
        'mac/mac_util.h',
        'mac/mach_o_image_reader.cc',
        'mac/mach_o_image_reader.h',
        'mac/mach_o_image_segment_reader.cc',
        'mac/mach_o_image_segment_reader.h',
        'mac/mach_o_image_symbol_table_reader.cc',
        'mac/mach_o_image_symbol_table_reader.h',
        'mac/service_management.cc',
        'mac/service_management.h',
        'mac/process_reader.cc',
        'mac/process_reader.h',
        'mac/process_types.cc',
        'mac/process_types.h',
        'mac/process_types/all.proctype',
        'mac/process_types/crashreporterclient.proctype',
        'mac/process_types/custom.cc',
        'mac/process_types/dyld_images.proctype',
        'mac/process_types/flavors.h',
        'mac/process_types/internal.h',
        'mac/process_types/loader.proctype',
        'mac/process_types/nlist.proctype',
        'mac/process_types/traits.h',
        'mach/exc_client_variants.cc',
        'mach/exc_client_variants.h',
        'mach/exc_server_variants.cc',
        'mach/exc_server_variants.h',
        'mach/exception_behaviors.cc',
        'mach/exception_behaviors.h',
        'mach/exception_ports.cc',
        'mach/exception_ports.h',
        'mach/mach_extensions.cc',
        'mach/mach_extensions.h',
        'mach/mach_message_server.cc',
        'mach/mach_message_server.h',
        'mach/symbolic_constants_mach.cc',
        'mach/symbolic_constants_mach.h',
        'mach/task_memory.cc',
        'mach/task_memory.h',
        'misc/clock.cc',
        'misc/clock.h',
        'misc/clock_mac.cc',
        'misc/initialization_state.h',
        'misc/initialization_state_dcheck.cc',
        'misc/initialization_state_dcheck.h',
        'misc/scoped_forbid_return.cc',
        'misc/scoped_forbid_return.h',
        'misc/symbolic_constants_common.h',
        'misc/uuid.cc',
        'misc/uuid.h',
        'numeric/checked_range.h',
        'numeric/in_range_cast.h',
        'numeric/int128.h',
        'numeric/safe_assignment.h',
        'posix/process_util.h',
        'posix/process_util_mac.cc',
        'posix/symbolic_constants_posix.cc',
        'posix/symbolic_constants_posix.h',
        'stdlib/cxx.h',
        'stdlib/objc.h',
        'stdlib/pointer_container.h',
        'stdlib/string_number_conversion.cc',
        'stdlib/string_number_conversion.h',
        'stdlib/strlcpy.cc',
        'stdlib/strlcpy.h',
        'stdlib/strnlen.cc',
        'stdlib/strnlen.h',
        'synchronization/semaphore.cc',
        'synchronization/semaphore.h',
      ],
      'conditions': [
        ['OS=="mac"', {
          'actions': [
            {
              'action_name': 'mig exc.defs',
              'inputs': [
                'mach/mig.py',
                '$(SDKROOT)/usr/include/mach/exc.defs',
              ],
              'outputs': [
                '<(INTERMEDIATE_DIR)/util/mach/excUser.c',
                '<(INTERMEDIATE_DIR)/util/mach/excServer.c',
                '<(INTERMEDIATE_DIR)/util/mach/exc.h',
                '<(INTERMEDIATE_DIR)/util/mach/excServer.h',
              ],
              'action': [
                'python', '<@(_inputs)', '<@(_outputs)'
              ],
              'process_outputs_as_sources': 1,
            },
            {
              'action_name': 'mig mach_exc.defs',
              'inputs': [
                'mach/mig.py',
                '$(SDKROOT)/usr/include/mach/mach_exc.defs',
              ],
              'outputs': [
                '<(INTERMEDIATE_DIR)/util/mach/mach_excUser.c',
                '<(INTERMEDIATE_DIR)/util/mach/mach_excServer.c',
                '<(INTERMEDIATE_DIR)/util/mach/mach_exc.h',
                '<(INTERMEDIATE_DIR)/util/mach/mach_excServer.h',
              ],
              'action': [
                'python', '<@(_inputs)', '<@(_outputs)'
              ],
              'process_outputs_as_sources': 1,
            },
          ],
          'link_settings': {
            'libraries': [
              '$(SDKROOT)/System/Library/Frameworks/CoreFoundation.framework',
              '$(SDKROOT)/System/Library/Frameworks/Foundation.framework',
              '$(SDKROOT)/System/Library/Frameworks/IOKit.framework',
            ],
          },
        }],
      ],
    },
    {
      'target_name': 'util_test_lib',
      'type': 'static_library',
      'dependencies': [
        '../compat/compat.gyp:compat',
        '../third_party/gtest/gtest.gyp:gtest',
        '../third_party/mini_chromium/mini_chromium/base/base.gyp:base',
        'util',
      ],
      'include_dirs': [
        '..',
      ],
      'sources': [
        'test/errors.cc',
        'test/errors.h',
        'test/executable_path.h',
        'test/executable_path_mac.cc',
        'test/mac/dyld.h',
        'test/mac/mach_errors.cc',
        'test/mac/mach_errors.h',
        'test/mac/mach_multiprocess.cc',
        'test/mac/mach_multiprocess.h',
        'test/multiprocess.cc',
        'test/multiprocess.h',
        'test/multiprocess_exec.cc',
        'test/multiprocess_exec.h',
        'test/posix/close_multiple.cc',
        'test/posix/close_multiple.h',
      ],
      'conditions': [
        ['OS=="mac"', {
          'link_settings': {
            'libraries': [
              '$(SDKROOT)/usr/lib/libbsm.dylib',
            ],
          },
        }],
      ],
    },
    {
      'target_name': 'util_test',
      'type': 'executable',
      'dependencies': [
        'util',
        'util_test_lib',
        'util_test_multiprocess_exec_test_child',
        '../compat/compat.gyp:compat',
        '../third_party/gmock/gmock.gyp:gmock',
        '../third_party/gtest/gtest.gyp:gtest',
        '../third_party/gtest/gtest.gyp:gtest_main',
        '../third_party/mini_chromium/mini_chromium/base/base.gyp:base',
      ],
      'include_dirs': [
        '..',
      ],
      'sources': [
        'file/string_file_writer_test.cc',
        'mac/checked_mach_address_range_test.cc',
        'mac/launchd_test.mm',
        'mac/mac_util_test.mm',
        'mac/mach_o_image_reader_test.cc',
        'mac/mach_o_image_segment_reader_test.cc',
        'mac/process_reader_test.cc',
        'mac/process_types_test.cc',
        'mac/service_management_test.mm',
        'mach/exc_client_variants_test.cc',
        'mach/exc_server_variants_test.cc',
        'mach/exception_behaviors_test.cc',
        'mach/exception_ports_test.cc',
        'mach/mach_extensions_test.cc',
        'mach/mach_message_server_test.cc',
        'mach/symbolic_constants_mach_test.cc',
        'mach/task_memory_test.cc',
        'misc/clock_test.cc',
        'misc/initialization_state_dcheck_test.cc',
        'misc/initialization_state_test.cc',
        'misc/scoped_forbid_return_test.cc',
        'misc/uuid_test.cc',
        'numeric/checked_range_test.cc',
        'numeric/in_range_cast_test.cc',
        'numeric/int128_test.cc',
        'posix/process_util_test.cc',
        'posix/symbolic_constants_posix_test.cc',
        'stdlib/string_number_conversion_test.cc',
        'stdlib/strlcpy_test.cc',
        'stdlib/strnlen_test.cc',
        'synchronization/semaphore_test.cc',
        'test/executable_path_test.cc',
        'test/mac/mach_multiprocess_test.cc',
        'test/multiprocess_exec_test.cc',
        'test/multiprocess_test.cc',
      ],
      'conditions': [
        ['OS=="mac"', {
          'link_settings': {
            'libraries': [
              '$(SDKROOT)/System/Library/Frameworks/Foundation.framework',
            ],
          },
        }],
      ],
    },
    {
      'target_name': 'util_test_multiprocess_exec_test_child',
      'type': 'executable',
      'sources': [
        'test/multiprocess_exec_test_child.cc',
      ],
    },
  ],
}
