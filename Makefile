
CXXFLAGS = -std=c++98 -fwrapv -pthread -g -O3 -Wall -Wextra
#LDFLAGS = -lX11 -lm -lrt
LDFLAGS = -lm -lrt

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
    eigen \

all: $(PROGRAMS)

% : %.cc $(INCLUDES)
	@/bin/echo Compiling $@
	@$(CXX) $(CXXFLAGS) $< -o $@ $(LDFLAGS)

clean:
	/bin/rm -rf $(PROGRAMS) *.dSYM *.so *.pyc

