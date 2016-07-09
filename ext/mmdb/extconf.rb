require 'mkmf'

if have_header("maxminddb.h") && have_library("maxminddb")
  create_makefile("mmdb")
end
