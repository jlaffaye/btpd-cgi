# btpd-cgi
# See COPYRIGHT file for copyright and license details.

include config.mk

CC=	gcc
CFLAGS=	-Wall -pipe -std=c99 -fPIC ${INCLUDE} ${DEFINE}
LDFLAGS=${LIBDIR} -L./btpd/misc
LIBS=	-lneo_cgi -lneo_cs -lneo_utl -lz -lmisc -lm -lcrypto
INCLUDE=${INCDIR} ${INCCSDIR} -I./btpd/misc
DEFINE=-DETCDIR=\"${ETCDIR}\" -DSHAREDIR=\"${SHAREDIR}\"
EXEC=	btpd.cgi
OBJ=	${SRC:.c=.o}
SRC=	btpd.c \
	btpdcgi.c \
	add.c \
	del.c \
	kill.c \
	list.c \
	start.c \
	stat.c \
	stop.c
all: ${EXEC}

${EXEC}: libmisc ${OBJ}
	@echo "LINK	$@"
	@${CC} ${LDFLAGS} ${CFLAGS} ${OBJ} -o $@ ${LIBS}

.c.o:
	@echo "CC	$<"
	@${CC} -o $@ -c $< ${CFLAGS}

libmisc:
	@${MAKE} -C ./btpd

clean:
	@echo "RM	${EXEC} *.o *.core"
	@rm -f ${EXEC} *.o *.core

install:
	@echo "INSTALL	${EXEC}"
	@chmod 755 ${EXEC}
	@cp ${EXEC} ${BINDIR}
	@mkdir ${SHAREDIR}/btpdcgi
	@cp -R templates ${SHAREDIR}/btpdcgi

deinstall:
	@echo "DEINSTALL	${EXEC}"
	@rm -f ${BINDIR}/${EXEC}
	@rm -rf ${SHAREDIR}/btpdcgi
