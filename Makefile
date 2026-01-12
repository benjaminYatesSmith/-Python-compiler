ifeq ($(shell uname -s),Linux)
DEBUG=gdb -q -x ./tests/.env.gdb --args
else # Assume the rest is Apple stuff (Darwin)
DEBUG=lldb -b -o run --
endif

ifeq ($(with), asan)
ifeq ($(shell uname -s),Linux)
CFLAGS+=-fno-omit-frame-pointer -fno-common -fsanitize=undefined,leak,address -fsanitize-recover=all
LDLIBS+=-lasan -lubsan
else # No leak option on Apple
CFLAGS+=-fno-omit-frame-pointer -fno-common -fsanitize=undefined,address -fsanitize-recover=all
LDLIBS+=-fsanitize=undefined,address
endif
else
ifeq ($(shell uname -s),Linux)
CHECK=valgrind --leak-check=full --show-leak-kinds=all
else # No Valgrind on Apple machines
CHECK=
endif
endif

CFLAGS+=-Wall -Wextra -Werror=uninitialized -Werror=implicit -ggdb3 -D_XOPEN_SOURCE=700 -D_XOPEN_SOURCE_EXTENDED -Iinclude
ifeq ($(shell uname -s),Linux)
LDLIBS+=-lm
else # Assume the rest is Apple stuff (Darwin)
LDLIBS+=-lm -ggdb3
endif

OBJ=$(patsubst %.c,%.o,$(wildcard src/*.c))

.PRECIOUS: %.exe

all : tests progs

progs : $(patsubst %.c,%.exe,$(wildcard prog/*.c))
tests : $(patsubst %.c,%.exe,$(wildcard tests/*.c))
runtests : $(patsubst %.c,%.exe@run,$(wildcard tests/*.c))
checks : $(patsubst %.c,%.exe@check,$(wildcard tests/*.c))
test-demo : $(patsubst %.c,%.exe,$(wildcard tests/howto/*.c))

%@debug : %
	$(DEBUG) $< -v -g

%@check : %
ifeq ($(shell uname -s),Linux)
	$(CHECK) $< -v
else
	@echo "Aborting: Valgrind is not available on Macos. Use AddressSanitizer instead (see README.md)"
	@false
endif

%@run : %
	$< -v

%.exe : %.o $(OBJ) $(wildcard include/*/*.h)
	$(CC) $(OBJ) $< $(LDFLAGS) $(LDLIBS) -o $@

clean :
	find . -name '*.o' -delete
	find . -name '*.exe' -delete
	find . -name '*~' -delete

deep-clean: clean
	find . -name "*.stdout" -delete
	find . -name "*.stderr" -delete

ASAN_STOP_ON_ERROR=halt_on_error=1:abort_on_error=1:print_legend=0
ASAN_CONT_ON_ERROR=halt_on_error=0:abort_on_error=0:print_legend=0
ASAN_RUNTIME=$(ASAN_CONT_ON_ERROR):detect_leaks=1:leak_check_at_exit=1:symbolize=1:exitcode=1

pimp-my-ride :
	@if [ ! -z "${GDB_STYLISH}" ] ; then echo '\n  Your GDB is already beautiful ;)\n' ; exit 1 ; fi ;
	@echo "\e[7m\e[33m Installing GDB Dashboard \e[m"
	@mkdir -p ~/.gdb/dashboard
	@wget --quiet https://git.io/.gdbinit -P ~/.gdb/dashboard
	@echo 'source ~/.gdb/dashboard/.gdbinit' >> ~/.gdbinit
	@echo 'dashboard -style discard_scrollback False' >> ~/.gdbinit
	@echo 'alias g = dashboard' >> ~/.gdbinit
	@echo 'define hookpost-up\n\tdashboard\nend\ndefine hookpost-down\n\tdashboard\nend' >> ~/.gdbinit
	@echo "\e[7m\e[33m Installing GDB Enhanced Features (GEF) \e[m"
	@mkdir -p ~/.gdb/gef
	@git clone https://github.com/hugsy/gef.git ~/.gdb/gef
	@echo 'source ~/.gdb/gef/gef.py' >> ~/.gdbinit
	@echo 'gef config context.enable False' >> ~/.gdbinit
	@echo 'export GDB_STYLISH=1' >> ~/.bash_aliases
	@echo "\e[7m\e[32m Your GDB has been pimped, enjoy! \e[m"
	@echo "\e[7m\e[33m Installing VS Code stuff for C dev \e[m"
	@code --install-extension ms-vscode.cpptools-extension-pack
	@code --install-extension ms-vscode.hexeditor
	@code --install-extension qiumingge.cpp-check-lint
	@if [ -z "${ASAN_OPTIONS}" ] ; then echo "export ASAN_OPTIONS=$(ASAN_RUNTIME)" >> ~/.bash_aliases ; echo "export UBSAN_OPTIONS=$(ASAN_CONT_ON_ERROR)" >> ~/.bash_aliases ; echo "\n\e[7m\e[32mASan runtime environment updated.\e[m\n\n Now either type 'source ~/.bashrc' and press Enter, or logout and get a new terminal.\n" ; fi ;

my-box-stylish :
	sudo wget https://www.uvolante.org/apt/uvolante.sources -O /etc/apt/sources.list.d/uvolante.sources
	wget "https://code.visualstudio.com/sha/download?build=stable&os=linux-deb-x64" -O vscode.deb
	sudo dpkg -i vscode.deb && rm vscode.deb
	sudo apt update
	sudo apt install git make clang valgrind gdb meld libasan8 libubsan1 python3-pygments pyc-objdump -y
