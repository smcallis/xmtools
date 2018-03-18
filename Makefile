
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

test:
	@for source in unit/*.cc; do \
            printf "  Running %-24s : " $$source; \
            $(CXX) $(CXXFLAGS) $$source -o ./doit $(LDFLAGS); \
            ./doit; \
            if test $$? = 0; \
                then echo Success; \
                else echo FAILURE; \
            fi; \
            rm -f ./doit; \
        done

clean:
	rm -rf $(PROGRAMS) *.dSYM *.so *.pyc

