project('octiron', ['cpp'], default_options : ['cpp_std=c++26'])

# Enable assertions in debug mode
if get_option('buildtype') == 'debug'
  add_project_arguments('-DENABLE_ASSERTIONS', language : 'cpp')
  message('Assertions enabled (Debug build)')
endif

# Gather source files for bootloader
boot_src_asm = files('src/bootloader/bootloader.S')
boot_src_cpp = files(
  'src/bootloader/bootloader.cpp',
  'src/devices/uart.cpp',
  'src/fmt/fmt.cpp',
  # 'src/libc_impl.cpp',
  'src/pmm.cpp',
  'src/tar.cpp',
  'src/memory.cpp'
)

# Convert the root.tar to an object file an embed it in the bootloader.elf
root_tar_obj = custom_target('root_tar_obj',
  input : 'root.tar',
  output : 'root.tar.o',
  command : [
    'riscv64-unknown-elf-objcopy', 
    '-I', 'binary', '-O', 'elf64-littleriscv', '-B', 'riscv',
    '@INPUT@', '@OUTPUT@'
  ]
)

boot_sources = boot_src_asm + boot_src_cpp + root_tar_obj
boot_include = include_directories('include')

# Add the object file to bootloader sources
# boot_sources += [hello_tar_obj]

# Apply compile options to the bootloader executable
boot = executable('bootloader.elf',
  boot_sources,
  include_directories : boot_include,
  # cpp_args : [
  #   '-Wall', '-Wextra', '-Werror',
  #   '-mcmodel=medany', '-ffreestanding',
  #   '-nostdlib', '-fno-exceptions',
  #   '-fno-stack-protector', '-fno-rtti'
  # ],
  link_args : [
    '-T' + meson.project_source_root() / 'scripts/bootloader_qemu_virt.ld',
    '-nostdlib', '-ffreestanding',
    '-Wl,-Map=bootloader.elf.map'
  ],
  install : false
)

# # Custom flags for assembly files can be set directly in the executable
# boot_src_asm_with_flags = []
# foreach asm_file : boot_src_asm
#   boot_src_asm_with_flags += asm_file
# endforeach

# Custom run target
run_target('run',
  command : [meson.project_source_root() / 'scripts/qemu.sh', '1', 'bootloader.elf'],
  depends : boot,
)

# Custom debug target
run_target('debug',
  command : [meson.project_source_root() / 'scripts/qemu.sh', '0', 'bootloader.elf'],
  depends : boot,
)