SRCS=./chat/chat.cpp ./chat/gcol.cpp ./chat/perm.cpp ./chat/room.cpp ./chat/sess.cpp ./chat/sman.cpp ./chat/user.cpp ./cli/cli.cpp ./conf/conf.cpp ./contrib/crypt/md5.cpp ./contrib/crypt/md5crypt.cpp ./contrib/xml/tinyxml.cpp ./contrib/xml/tinyxmlerror.cpp ./contrib/xml/tinyxmlparser.cpp ./data/con.cpp ./data/con_base.cpp ./data/data.cpp ./data/data_base.cpp ./html.cpp ./logd.cpp ./main.cpp ./modl.cpp ./monitor/dump.cpp ./monitor/stats.cpp ./name.cpp ./reqp.cpp ./sign.cpp ./sock/context.cpp ./sock/sock.cpp ./sock/sslsock.cpp ./time/timo.cpp ./time/timr.cpp ./tool/dir.cpp ./tool/tool.cpp ./wrap.cpp
OBJS= ../obj/./chat/chat.o ../obj/./chat/gcol.o ../obj/./chat/perm.o ../obj/./chat/room.o ../obj/./chat/sess.o ../obj/./chat/sman.o ../obj/./chat/user.o ../obj/./cli/cli.o ../obj/./conf/conf.o ../obj/./contrib/crypt/md5.o ../obj/./contrib/crypt/md5crypt.o ../obj/./contrib/xml/tinyxml.o ../obj/./contrib/xml/tinyxmlerror.o ../obj/./contrib/xml/tinyxmlparser.o ../obj/./data/con.o ../obj/./data/con_base.o ../obj/./data/data.o ../obj/./data/data_base.o ../obj/./html.o ../obj/./logd.o ../obj/./main.o ../obj/./modl.o ../obj/./monitor/dump.o ../obj/./monitor/stats.o ../obj/./name.o ../obj/./reqp.o ../obj/./sign.o ../obj/./sock/context.o ../obj/./sock/sock.o ../obj/./sock/sslsock.o ../obj/./time/timo.o ../obj/./time/timr.o ../obj/./tool/dir.o ../obj/./tool/tool.o ../obj/./wrap.o
BIN=../bin/ychat
CXX=g++
#EFLAGS=-frepo
CXXFLAGS=-fno-inline -fno-default-inline -g -O2 ${EFLAGS} 
#LDADD+=-ldl -levent -lpthread -lstdc++ -D_THREAD_SAVE -export-dynamic
LDADD+=-ldl -levent -lpthread -lstdc++ -export-dynamic
PREFIX=/usr/local
all: message ${OBJS} 
	@dirname=`dirname ${BIN}`; if ! test -d $$dirname; then mkdir -p $$dirname; fi
	@echo; echo "===> Linking base sources"
	@${CXX} -o ${BIN} ${OBJS} ${LDADD}
	@echo "===> LDADD	        :      ${LDADD}"	
	@echo "===> CXXFLAGS	        :      ${CXXFLAGS}"	
	@echo "===> CXX		:      ${CXX}"	
	@echo "===> Binary size        :      `du -hs ${BIN} | sed 's/\.\.\/bin\///'`"	
	@echo "===> `cd ..; ./scripts/version.sh`"
	@echo 
${OBJS}:
	@dirname=`dirname $@`; if ! test -d $$dirname; then mkdir -p $$dirname; fi
	@${CXX} ${CXXFLAGS} -c -o $@ `echo $@ | \
		sed 's/\.\.\/obj\/\.\/\(.*\)\.o/\.\/\1.cpp/'`
	@echo "base:`echo $@ | sed 's/\.\.\/obj\/\.\/\(.*\)\.o$$/\1/'` (`du \
		-hs $@ | awk '{ print $$1 }'`) "
message:
	@echo "===> Building base sources"
clean:
	@echo "===> Cleaning base"
	@find ../obj -name '*.o' | grep -v mods | xargs rm -f 
	@if [ -f ${BIN} ]; then rm -f ${BIN}; fi
mrproper: clean
	@for i in autom4te.cache; \
		do if [ -d $$i ]; then rm -Rf $$i; fi; done
	@for i in Makefile config.h config.log config.status; \
		do if [ -f $$i ]; then rm -f $$i; fi; done

../obj/./chat/chat.o: ./chat/chat.cpp
../obj/./chat/gcol.o: ./chat/gcol.cpp
../obj/./chat/perm.o: ./chat/perm.cpp
../obj/./chat/room.o: ./chat/room.cpp
../obj/./chat/sess.o: ./chat/sess.cpp
../obj/./chat/sman.o: ./chat/sman.cpp
../obj/./chat/user.o: ./chat/user.cpp
../obj/./cli/cli.o: ./cli/cli.cpp
../obj/./conf/conf.o: ./conf/conf.cpp
../obj/./contrib/crypt/md5.o: ./contrib/crypt/md5.cpp
../obj/./contrib/crypt/md5crypt.o: ./contrib/crypt/md5crypt.cpp
../obj/./contrib/xml/tinyxml.o: ./contrib/xml/tinyxml.cpp
../obj/./contrib/xml/tinyxmlerror.o: ./contrib/xml/tinyxmlerror.cpp
../obj/./contrib/xml/tinyxmlparser.o: ./contrib/xml/tinyxmlparser.cpp
../obj/./data/con.o: ./data/con.cpp
../obj/./data/con_base.o: ./data/con_base.cpp
../obj/./data/data.o: ./data/data.cpp
../obj/./data/data_base.o: ./data/data_base.cpp
../obj/./html.o: ./html.cpp
../obj/./logd.o: ./logd.cpp
../obj/./main.o: ./main.cpp
../obj/./modl.o: ./modl.cpp
../obj/./monitor/dump.o: ./monitor/dump.cpp
../obj/./monitor/stats.o: ./monitor/stats.cpp
../obj/./name.o: ./name.cpp
../obj/./reqp.o: ./reqp.cpp
../obj/./sign.o: ./sign.cpp
../obj/./sock/context.o: ./sock/context.cpp
../obj/./sock/sock.o: ./sock/sock.cpp
../obj/./sock/sslsock.o: ./sock/sslsock.cpp
../obj/./time/timo.o: ./time/timo.cpp
../obj/./time/timr.o: ./time/timr.cpp
../obj/./tool/dir.o: ./tool/dir.cpp
../obj/./tool/tool.o: ./tool/tool.cpp
../obj/./wrap.o: ./wrap.cpp
