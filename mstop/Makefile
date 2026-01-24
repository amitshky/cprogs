proj_name := mstop

src_dir := src
build_dir := build
install_dir := /usr/local/bin
include_dir = -I$(src_dir)

src = $(shell find $(src_dir) -type f -wholename "$(src_dir)/*.c")
out_file = $(build_dir)/$(proj_name)

cflags := -Wall -Wextra -Werror
cver := -std=c23
libflags := -lm -lpthread
dbg_def := -D_DEBUG


default: config run

# generate compile_commands.json and build project
config:
	bear -- $(MAKE) build

# check if build directory exists, create if it doesn't
builddir:
	[ -e $(build_dir) ] || mkdir $(build_dir)

# debug build
build: builddir
	gcc -o $(out_file) $(src) $(cflags) $(libflags) $(include_dir) $(dbg_def) -g

dbg: build
	gdb -tui --args ./$(out_file)

# release build
release: builddir
	gcc -o $(out_file) $(src) $(cflags) $(libflags) $(include_dir) -O3

install: release
	cp $(out_file) $(install_dir)
	chmod 755 $(install_dir)/$(proj_name)

uninstall:
	rm -f $(install_dir)/$(proj_name)

run:
	./$(out_file)

clean:
	rm -rf $(build_dir)

clean_proj:
	rm -rf $(build_dir) .cache compile_commands.json
