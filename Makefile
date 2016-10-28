
CC = g++ -std=c++98 -fwrapv -pthread -g -O3 -Wall

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
    xmbase \
    xmhalf \
    xmlfsr \
    xmgps \
    xmfirpm \
    covar \

all: $(PROGRAMS)

% : %.cc $(INCLUDES)
	@/bin/echo Compiling $@
	@$(CC) $< -o $@ -lX11 -lm -lrt

clean:
	/bin/rm -rf $(PROGRAMS) *.dSYM *.so *.pyc

