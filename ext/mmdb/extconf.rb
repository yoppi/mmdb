require 'mkmf'

if have_header("maxminddb.h") && have_library("maxminddb")
  if enable_config("debug")
    CONFIG["debugflags"] << " -g3 -O0"
  end
  create_makefile("mmdb")
end
