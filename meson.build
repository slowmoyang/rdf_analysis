project(
  'rdf_analysis',
  'cpp',
  license: 'MIT',
  version: '1.0.0',
)

root_config = find_program('root-config', required: true)
root_compile_args = run_command(root_config, '--cflags', check: true).stdout().strip().split(' ')
root_link_args = run_command(root_config, '--ldflags', '--libs', check: true).stdout().strip().split(' ')
root_incdir = run_command(root_config, '--incdir', check: true).stdout().strip().split(' ')
root_dep = declare_dependency(
    compile_args: root_compile_args,
    link_args: root_link_args,
    include_directories: root_incdir
)

delphes_tools_dep = declare_dependency(
  include_directories: include_directories('include'),
  sources: [
    'src/rdf_analysis.cc',
  ],
  dependencies: [
    root_dep,
  ]
)

meson.override_dependency(meson.project_name(), delphes_tools_dep)
