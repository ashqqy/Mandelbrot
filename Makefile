# DEBUG MODE                                        | # RELEASE MODE
# make clean                                        | # make clean
# make CXXFLAGS=-DDEBUG                             | # make CXXFLAGS=-O3
# ./build/mandelbrot.x [slow|parallel|intrinsics]   | # ./build/mandelbrot.x [slow|parallel|intrinsics]

CXX_DEBUG_FLAGS ?= -D_DEBUG -ggdb3 -std=c++17 -O0 -Wall -Wextra -Weffc++ -Waggressive-loop-optimizations           \
                   -Wc++14-compat -Wmissing-declarations -Wdeprecated -Wcast-align -Wcast-qual -Wchar-subscripts   \
                   -Wconditionally-supported -Wconversion -Wctor-dtor-privacy -Wempty-body -Wfloat-equal           \
                   -Wformat-nonliteral -Wformat-security -Wformat-signedness -Wformat=2 -Winline -Wlogical-op      \
                   -Wnon-virtual-dtor -Wopenmp-simd -Woverloaded-virtual -Wpacked -Wpointer-arith -Winit-self      \
                   -Wredundant-decls -Wshadow -Wsign-conversion -Wsign-promo -Wstrict-null-sentinel                \
                   -Wstrict-overflow=2 -Wsuggest-attribute=noreturn -Wsuggest-final-methods -Wsuggest-final-types  \
                   -Wsuggest-override -Wswitch-default -Wsync-nand -Wundef -Wunreachable-code                      \
                   -Wunused -Wuseless-cast -Wvariadic-macros -Wno-literal-suffix -Wno-missing-field-initializers   \
                   -Wno-narrowing -Wno-old-style-cast -Wno-varargs -Wstack-protector -fcheck-new                   \
                   -fsized-deallocation -fstack-protector -fstrict-overflow -flto-odr-type-merging                 \
                   -fno-omit-frame-pointer -pie -fPIE -Werror=vla                                                  \
                   -fsanitize=address,alignment,bool,bounds,enum,float-cast-overflow,float-divide-by-zero,integer-divide-by-zero,leak,nonnull-attribute,null,object-size,return,returns-nonnull-attribute,shift,signed-integer-overflow,undefined,unreachable,vla-bound,vptr

SDL_FLAGS = `pkg-config --cflags --libs sdl3`

EXECUTABLE_BINARY = mandelbrot.x

INCLUDE_DIR = ./include
O_DIR ?= ./build

CSRC = src/ui.c src/render.c src/main.c
COBJ = $(addprefix $(O_DIR)/, $(CSRC:.c=.o))

#--------------------------------------------------------------------

override CXXFLAGS += -I$(INCLUDE_DIR)
override LDFLAGS += $(SDL_FLAGS)

ifneq (0, $(words $(findstring -DDEBUG, $(CXXFLAGS))))
override CXXFLAGS += $(CXX_DEBUG_FLAGS)
override LDFLAGS += $(CXX_DEBUG_FLAGS)
else
override CXXFLAGS += -DNDEBUG
endif

#--------------------------------------------------------------------

.PHONY: all
all: $(O_DIR)/$(EXECUTABLE_BINARY)

$(O_DIR)/$(EXECUTABLE_BINARY): $(COBJ)
	$(CXX) $(LDFLAGS) $^ -o $@

$(COBJ): $(O_DIR)/%.o: %.c
	@mkdir -p $(@D)
	$(CXX) $(CXXFLAGS) -c $^ -o $@

#--------------------------------------------------------------------

.PHONY: run
run:
	$(O_DIR)/mandelbrot.x parallel

#--------------------------------------------------------------------

.PHONY: clean
clean:
	rm -rf $(COBJ) $(O_DIR)/$(EXECUTABLE_BINARY)

#--------------------------------------------------------------------
