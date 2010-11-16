# Copyright (C) 2010 Peter Tworek<tworaz666@gmail.com>
#
# This program is free software: you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation, either version 3 of the License, or
# (at your option) any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program.  If not, see <http://www.gnu.org/licenses/>.

APP_NAME=term-app-launcher
OBJ=term-app-launcher.o

DESTDIR?=
PREFIX?=/usr
BINDIR?=${PREFIX}/bin

CC=gcc
LD=gcc
CFLAGS=-Wall -O2 -pipe
VTE_CFLAGS=$(shell pkg-config --cflags vte)
VTE_LIBS=$(shell pkg-config --libs vte)


all: ${APP_NAME}

${APP_NAME}: ${OBJ}
	${LD} ${VTE_LIBS} $< -o $@

%.o:%.c
	${CC} ${CFLAGS} ${VTE_CFLAGS} -c -o $@ $<

install: ${APP_NAME}
	install -g root -d ${DESTDIR}/${BINDIR}
	install -g root -m 0755 ${APP_NAME} ${DESTDIR}/${BINDIR}/${APP_NAME}

clean:
	rm -f ${OBJ} ${APP_NAME}

.PHONY: all install
