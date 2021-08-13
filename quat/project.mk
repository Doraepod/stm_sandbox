# C source files
PROJECT_CSRC 	= src/main.c \
                  src/i2c.c  \
                  src/lld_gyroscope.c \
                  src/lld_accelerometer.c \
                  src/quaternions.c \
                  src/debug.c \
                  src/magwick.c
# C++ source files
PROJECT_CPPSRC 	= 
# Directories to search headers in
PROJECT_INCDIR	= headers
# Additional libraries
PROJECT_LIBS	=
# Compiler options
PROJECT_OPT     =

# Additional .mk files are included here
