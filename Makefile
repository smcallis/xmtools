INCDIRS = \

LIBDIRS = \

CC = g++ -std=c++98 -pthread -g -O3 -Wall $(INCDIRS) $(LIBDIRS)

INCLUDES = \
    xmtools.h \
    kissfft.h \
    xmkwds.h \

PROGRAMS = \
    xmtest \
    xmstat \
    xmkwds \
    xmcut \
    xmcat \
    xmrate \
    xmtone \
    xmnoise \
    xmspec \

all: $(PROGRAMS)

% : %.cc $(INCLUDES)
	@/bin/echo Compiling $@
	@$(CC) $< -o $@ -lX11 -lm -lrt

clean:
	/bin/rm -rf $(PROGRAMS) *.dSYM *.so *.pyc

