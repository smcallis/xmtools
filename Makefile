
CXXFLAGS = -std=c++98 -fwrapv -pthread -g -O3 -Wall -Wextra -Iinc
LDFLAGS = -lm -lrt

INCLUDES = \
    inc/xmtools.h \
    inc/xm/* \

PROGRAMS = \
    bin/xmcat \
    bin/xmcut \
    bin/xmgps \
    bin/xmkwds \
    bin/xmnoise \
    bin/xmrate \
    bin/xmstat \
    bin/xmtone \

all: $(PROGRAMS)

bin/% : src/%.cc $(INCLUDES)
	@echo Compiling $@
	@$(CXX) $(CXXFLAGS) $< -o $@ $(LDFLAGS)

clean:
	rm -rf $(PROGRAMS) *.dSYM *.so *.pyc

