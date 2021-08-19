# C source files
PROJECT_CSRC 	= src/main.c \
                  src/i2c.c  \
                  src/lld_gyro_axel.c \
                  src/lld_magnetometer.c \
                  src/quaternions.c \
                  src/debug.c \
                  src/magwick.c \
                  Fusion-master/Fusion/FusionAhrs.c \
                  
# C++ source files
PROJECT_CPPSRC 	= 
# Directories to search headers in
PROJECT_INCDIR	= headers \
                  Fusion-master/Fusion
# Additional libraries
PROJECT_LIBS	=
# Compiler options
PROJECT_OPT     = -lm

# Additional .mk files are included here
