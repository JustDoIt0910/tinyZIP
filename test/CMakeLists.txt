add_executable(bar_test bar_test.c ../bar.c ../internal/str.c)
add_executable(lzw_test lzw_test.c ../internal/bitstream.c
        ../internal/str.c ../internal/3w_tire.c
        ../lzw.c ../bar.c)