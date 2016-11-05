
CC = g++ -std=c++98 -fwrapv -pthread -g -O3 -Wall

INCLUDES = \
    xmtools.h \
    kissfft.h \
    xmkwds.h \

PROGRAMS = \
    xmbase \
    xmcat \
    xmcut \
    xmfirpm \
    xmgps \
    xmhalf \
    xmkwds \
    xmkwds \
    xmlfsr \
    xmnoise \
    xmrate \
    xmspec \
    xmstat \
    xmtest \
    xmtone \
    covar \

all: $(PROGRAMS)

% : %.cc $(INCLUDES)
	@/bin/echo Compiling $@
	@$(CC) $< -o $@ -lX11 -lm -lrt

clean:
	/bin/rm -rf $(PROGRAMS) *.dSYM *.so *.pyc

