project('cpuid2cpuflags', 'c',
        license: 'BSD-2-Clause',
        version: '11',
        )
package_string = meson.project_name() + '-' + meson.project_version()

executable('cpuid2cpuflags', [
             'src/main.c',
             'src/arm.c',
             'src/hwcap.c',
             'src/ppc.c',
             'src/x86.c',
             'src/x86-lib.c',
           ],
           c_args: ['-DPACKAGE_STRING="' + package_string + '"'],
           install: true,
           )

# utilities
alias_target('utils', [
  executable('hwcap-dump', [
               'src/hwcap-dump.c',
               'src/hwcap.c',
             ],
             build_by_default: false,
             c_args: ['-DCPUID_ARM', '-DCPUID_MOCK'],
             ),
  executable('x86-dump', [
               'src/x86-dump.c',
               'src/x86-lib.c',
             ],
             build_by_default: false,
             ),
])

tests = {
  'arm': [
    'aarch64-aws-a1.txt',
    'aarch64-cavium-thunderx.txt',
    'aarch64-cavium-thunderx2.txt',
    'aarch64-dragonboard.txt',
    'aarch64-hp-moonshot-xgene-1.txt',
    'aarch64-odroid-n2.txt',
    'aarch64-raspberry-4.txt',
    'armv5tel-dreamplug.txt',
    'armv6l-raspberry-pi1b.txt',
    'armv7l-beaglebone.txt',
    'armv7l-compulab-utilite-pro.txt',
    'armv7l-jhcloos-cheap.txt',
    'armv7l-odroid-c1.txt',
    'armv7l-raspberry-pi3.txt',
    'armv7l-rk3288.txt',
    'armv7l-scaleway-c1.txt',
    'armv7l-solidrun-cubox.txt',
  ],
  'ppc': [
    'power8.txt',
    'power9.txt',
  ],
  'x86': [
    'amd-colfax.txt',
    'amd-zen2.txt',
    'athlon64-windsor.txt',
    'i3-ivybridge.txt',
    'opteron-6272.txt',
    'xeon-e-2176g.txt',
    'xeon-silver-4410.txt',
  ],
}

sh = find_program('sh',
                  required: true)
meson_exe = find_program('meson',
                         required: true)

mocks = []
foreach target: tests.keys()
  mocks += [executable(target + '-mock', [
                         'src/mock.c',
                         'src/' + target + '.c',
                       ],
                       build_by_default: false,
                       c_args: ['-DCPUID_' + target.to_upper(), '-DCPUID_MOCK'],
                       )]

  foreach t: tests[target]
    test(t, sh,
         args: [meson.current_source_dir() / 'tests/test.sh',
                meson.current_build_dir(),
                meson.current_source_dir() / 'tests' / target / t,
               ],
         suite: target,
         )
  endforeach
endforeach

alias_target('mocks', mocks)

test('build-mocks', meson_exe,
     args: ['compile', '-v', 'mocks'],
     is_parallel: false,
     priority: 1000,
     verbose: true,
     )
