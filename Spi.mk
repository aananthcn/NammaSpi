INCDIRS  += -I ${SPI_PATH}/src \
	    -I ${SPI_PATH}/api	\
	    -I ${SPI_PATH}/cfg \
	    -I ${SPI_PATH}/src/bsp/

LDFLAGS  += -g
CFLAGS   += -Werror ${INCDIRS} -g
ASFLAGS  += ${INCDIRS} -g

$(info compiling Spi source files)


SPI_OBJS := \
	${SPI_PATH}/src/Spi.o \
	${SPI_PATH}/cfg/Spi_cfg.o \
	${SPI_PATH}/src/SchM_Spi.o \
	${SPI_PATH}/src/bsp/rp2040/bsp_spi.o

