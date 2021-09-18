LIBS:= cpr
OBJS:= login.o
EXES:= login

CC:= g++

all: login

clean:
	rm -f ${EXE} ${OBJS}

login.o: src/login.cpp
	${CC} -c $@ $<

login: login.o
	${CC} -o $@ $< -l${LIBS}

.PHONY: all clean
