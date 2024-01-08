# 
# Created on Thu Dec 22 2022 10:23:34 PM
# 
# The MIT License (MIT)
# Copyright (c) 2022 Aananth C N
# 
# Permission is hereby granted, free of charge, to any person obtaining a copy of this software
# and associated documentation files (the "Software"), to deal in the Software without restriction,
# including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense,
# and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so,
# subject to the following conditions:
# 
# The above copyright notice and this permission notice shall be included in all copies or substantial
# portions of the Software.
# 
# THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED
# TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
# THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,
# TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
# 
CC=${COMPILER}gcc
LD=${COMPILER}ld
AS=${COMPILER}as
AR=${COMPILER}ar
RANLIB=${COMPILER}ranlib
OBJCOPY=${COMPILER}objcopy


include ${CAR_OS_PATH}/path_defs.mk


INCDIRS  += -I ${SPI_PATH}/src \
	    -I ${SPI_PATH}/api	\
	    -I ${SPI_PATH}/cfg \
	    -I ${SPI_PATH}/src/bsp \
	    -I ${MCU_PATH}/src \
	    -I ${DIO_PATH}/api \
	    -I ${CAR_OS_INC_PATH}/autosar \
	    -I ${CAR_OS_INC_PATH}/car_os \
	    -I ${CAR_OS_BOARDSOC_PATH} \
	    -I ${OS_PATH}/include


$(info  )
$(info compiling Spi source files)


SPI_OBJS := \
	${SPI_PATH}/src/Spi.o \
	${SPI_PATH}/cfg/Spi_cfg.o \
	${SPI_PATH}/src/SchM_Spi.o \
	${SPI_PATH}/src/bsp/rp2040/bsp_spi.o


# LDFLAGS := -g -relocatable
# CFLAGS  := -Werror ${INCDIRS} -g
# ASFLAGS := ${INCDIRS} -g
TARGET 	:= libSpi.a
# include c_l_flags.mk to add more definitions specific to micro-controller
include ${CAR_OS_PATH}/c_l_flags.mk

%.o: %.c
	$(CC) -c ${CFLAGS} ${INCDIRS} $< -o $@


all: $(TARGET)

LIB_OBJS := $(SPI_OBJS)

$(TARGET): $(LIB_OBJS)
	$(AR) -rcs ${TARGET} ${LIB_OBJS}
#	$(LD) ${LDFLAGS} -o $@ $^

clean:
	$(RM) $(LIB_OBJS) $(TARGET)
