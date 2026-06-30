MAKE=gmake
HEADER?=docs/header.txt
PREFIX=/usr/local
EFIND=find -regextype posix-extended
BIN=./bin/ychat
all:    build modules base
	@echo "Now edit the ychat.conf and run ychat!"
	@echo "The config file is searched in the following order:"
	@echo " ./ychat.conf " 
	@echo " ~/.ychat/ychat.conf "
	@echo " ./etc/ychat.conf "
	@echo " /etc/ychat.conf "
	@echo " $(PREFIX)/etc/ychat.conf "
	@echo WARNING! This software is EXPERIMENTAL!
install: all 
	@echo "===> Installing to ${PREFIX}"
	@for i in bin share/ychat lib/ychat; do \
		if ! [ -d ${PREFIX}/$$i ]; then \
			mkdir -p ${PREFIX}/$$i; \
		fi; \
	 done;
	@cp -fR ./mods ${PREFIX}/share/ychat/mods;
	@cp -fR ./etc ${PREFIX}/share/ychat/etc; 
	@cp -fR ./html ${PREFIX}/share/ychat/html; 
	@cp -f ./README ./COPYING ${PREFIX}/share/ychat;
	@cp -f ./bin/ychat ${PREFIX}/bin; 
deinstall: uninstall
uninstall:
	@echo "===> Uninstalling from ${PREFIX}"
	@for i in ${PREFIX}/bin/ychat ${PREFIX}/share/ychat \
		${PREFIX}/lib/ychat; do \
		rm -Rf $$i; \
	 done
strip:	all
	@strip ${BIN}
	@echo "===> Stripped binary size:      `du -hs ${BIN} | \
		sed 's/\.\/bin\///'`"	
base:	
	@if test -f bin/ychat; then echo "==> Backing up old binary"; \
		if test -f bin/ychat.old; then \
			rm -f bin/ychat.old; \
		fi; \
		mv bin/ychat bin/ychat.old; \
	 fi
	@${MAKE} -C ./src 
modules:
	@if test -d ./src/mods; then \
		${MAKE} -C ./src/mods; \
	 fi 
stats:
	@perl scripts/stats.pl
run_loop: 
	@while (true); do ./bin/ychat; done
run: 
	./bin/ychat
base_start: base 
	./bin/ychat
license:
	@more COPYING
#//<<*
yhttpdbase:
	@perl scripts/makeyhttpd.pl || echo "You need to have perl to do this!"
	@echo yhttpd code base has been generated in ../yhttpd	
#//*>>
clean:	clean_base clean_modules
clean_base:
	@if [ -f ./src/Makefile ]; then ${MAKE} -C ./src clean; fi
clean_modules:
	@if [ -d ./src/mods ]; then ${MAKE} -C ./src/mods clean; fi
help:
	@echo "You may run ${MAKE} with the following parameters:"
	@grep "^ ${MAKE} " README 
	@echo "For more questions read the README file or contact mail@ychat.org!"
mrproper: clean	
	@if [ -f ./src/Makefile ]; then ${MAKE} -C ./src mrproper; fi 
	@if [ -d src/mods ]; then ${MAKE} -C ./src/mods mrproper; fi
	@if [ -d ./bin ]; then rm -Rf ./bin; fi
	@if [ -d ./obj ]; then rm -Rf ./obj; fi
	@find ./ -name "*core*" -exec rm -f {} \; 
	@find ./log/ -type f | grep -v CVS | xargs rm -f 
	@rm -f Makefile
version:
	@./scripts/version.sh
debug:
	@gdb bin/ychat ychat.core
confdebug: 
	@CXXFLAGS='-g3 -ggdb' ./configure
dist:
	@./scripts/makedist.sh
ssltest:
	openssl genrsa -des3 -out privkey.pem 2048
	openssl req -new -x509 -key privkey.pem -out cert.pem -days 1095
	@mv -f privkey.pem cert.pem etc
headers:
	@${EFIND} ./ -regex '\./src/.*\.(h|(cpp)|(tmpl))' -exec \
		sh -c 'export FILE={}; ${MAKE} header' \;
header:
	@echo "===> Processing ${FILE}"
	@sed -n '/*:/d; w .tmp' ${FILE} 
	@header=`sed 's/\(.*\)/ echo " \*: \1"/' ${HEADER}`; \
		echo '/*:*' > ${FILE}; eval "$$header" >> ${FILE}; \
		echo ' *:*/' >> ${FILE}; cat .tmp >> ${FILE}; rm -f .tmp
replace:
	@${EFIND} ./ -regex '\./src/.*\.(h|(cpp)|(tmpl))' -exec \
		sh -c 'sed -n "s/$(FROM)/$(INTO)/g; \
		w .tmp" {} && mv -f .tmp {}' \;
build:
	@awk '{ if ($$2 == "BUILDNR") print $$1,$$2,$$3+1; else print }' \
        	src/build.h  >.tmp && mv -f .tmp src/build.h
touch:
	find ./ -type f -exec touch {} \;
