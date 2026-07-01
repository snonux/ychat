MAKE=`tail -n 1 make.version`
PREFIX=`grep "define PREFIX" src/glob.h | cut -d'"' -f2`
all:    version base 
	@echo If you want to help the ycurses project please run gmake mail
	@echo so that the developers receive an email about the platform
	@echo being used.
	@echo WARNING! This software is EXPERIMENTAL!
mail:
	@echo "VERSION:" > mail.tmp
	@${MAKE} version >> mail.tmp
	@echo >> mail.tmp
	@echo "UNAME:" >> mail.tmp
	@uname -a >> mail.tmp
	@echo >> mail.tmp
	@echo "DATE:" >> mail.tmp
	@date >> mail.tmp
	@echo >> mail.tmp
	@echo "COMPILER AND MAKE:" >> mail.tmp
	@cat g++.version make.version >> mail.tmp
	@cat mail.tmp | mail -s "Successfull build of ycurses" successfullbuild@yhttpd.org
	@rm -f mail.tmp 
install: deinstall
uninstall: deinstall
deinstall:
	@echo Install/deinstall is not supported!
	@echo Start ycurses with ./bin/ycurses instead! 
	@exit 1
base:	
	@if test -f bin/ycurses; then echo "Backing up old binary";if test -f bin/ycurses.old; then rm -f bin/ycurses.old; fi; mv bin/ycurses bin/ycurses.old; fi
	@perl -I. ./scripts/buildnr.pl
	@perl -I. ./scripts/setglobvals.pl
	@${MAKE} -C ./src
clean_base:
	@${MAKE} -C ./src clean
stats:
	@perl -I. scripts/stats.pl
run: 
	./bin/ycurses
base_start: base 
	./bin/ycurses
gpl:
	@more COPYING
clean:	clean_base 
help:
	@echo "You may run ${MAKE} with the following parameters:"
	@grep "^ ${MAKE} " README 
	@echo "For more questions read the README file or contact mail@ycurses.org!"
setup:
	@./configure
	@${MAKE}
config:
	@sh -c "scripts/config.sh"
	@echo If you run ${MAKE} config from the command line then you may need 
	@echo to rerun ./configure and recompile all now!
	
mrproper: clean	
	@if test -f src/glob.h.org; then mv -f src/glob.h.org src/glob.h;fi
	@if test -f g++.version; then rm -f g++.version; fi 
	@if test -f make.version; then rm -f make.version; fi 
	@if test -f src/Makefile; then rm -f src/Makefile; fi 
	@if test -f bin/ycurses; then find bin/ -name "ycurses*" | xargs rm -f; fi
	@find . -name "*.add" | xargs rm -f
	@ls | grep core | xargs rm -f
version:
	@./scripts/version.sh
debug:
	@gdb bin/ycurses ycurses.core
confdebug:
	./configure -g3 -ggdb
