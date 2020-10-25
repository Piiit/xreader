gepub_sources = [
  'gepub-archive.c',
  'gepub-doc.c',
  'gepub-text-chunk.c',
  'gepub-utils.c',
  'gepub-widget.c'
]

gepub_deps = [
  config_h,
  glib,
  webkit,
  dependency('libsoup-2.4'),
  dependency('gobject-2.0'),
  dependency('gio-2.0'),
  dependency('libxml-2.0'),
  dependency('libarchive')
]

libgepub = static_library(
  'libgepub',
  gepub_sources,
  include_directories: include_root,
  dependencies: gepub_deps,
)
