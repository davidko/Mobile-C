add_library(b64 SHARED cencode.c cdecode.c)
add_library(b64_static STATIC cencode.c cdecode.c)
set_target_properties(b64_static PROPERTIES COMPILE_FLAGS "-fPIC")
