from distutils.core import setup, Extension
from distutils.sysconfig import get_config_vars
import sys

cfg_vars = get_config_vars()

# distutils compiles .cc with -Wstrict-prototypes, which is not
# valid for C++ compilation. The lines below remove that option
# in order to suppress the warning.
for key, value in cfg_vars.items():
    if type(value) == str:
        cfg_vars[key] = value.replace('-Wstrict-prototypes','')

setup(
    ext_modules=[
        Extension(
            'xmtools', ["xmtools.cc"],
            extra_compile_args=['-std=c++0x']
        )
    ]
)
