#ifndef XMTOOLS_H_
#define XMTOOLS_H_

// This is a catch-all file which includes everything

//{{{ standard includes

#include <new>
//#include <stdexcept>
#include <math.h>
#include <time.h>
#include <ctype.h>
#include <errno.h>
#include <stdio.h>
#include <fcntl.h>
#include <endian.h>
#include <stdarg.h>
#define __STDC_LIMIT_MACROS
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <sys/time.h>
#include <sys/wait.h>
#include <regex.h>

//}}}

#include "xm/promote.h"
#include "xm/basics.h"
#include "xm/hash.h"
#include "xm/list.h"
#include "xm/dict.h"
#include "xm/shared.h"
#include "xm/tuple.h"
#include "xm/queue.h"
#include "xm/compare.h"
#include "xm/sort.h"
#include "xm/string.h"
#include "xm/complex.h"
#include "xm/vector.h"
#include "xm/matrix.h"
#include "xm/cholesky.h"
#include "xm/solvers.h"
#include "xm/svdecomp.h"
#include "xm/roots.h"
#include "xm/random.h"
#include "xm/shiftreg.h"
#include "xm/gpsgold.h"
#include "xm/bessel.h"
#include "xm/firwin.h"
#include "xm/rician.h"
#include "xm/mednoise.h"
#include "xm/fftshift.h"
#include "xm/blocktuner.h"
#include "xm/singleton.h"
#include "xm/polyphase.h"
#include "xm/cartesian.h"
#include "xm/geodetic.h"
#include "xm/timecode.h"
#include "xm/rawfile.h"
#include "xm/uniqueid.h"
#include "xm/statevec.h"
#include "xm/dted.h"
#include "xm/bluefiles.h"
#include "xm/ephemeris.h"
#include "xm/cmdline.h"
#include "xm/lighttime.h"

#endif // XMTOOLS_H_

