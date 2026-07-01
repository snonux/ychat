dnl configure.ac Cypyright (2005, 2006, 2007) by the yChat Project

AC_INIT(main.cpp, [], [ychat at dev dot buetow dot org])
AC_CONFIG_HEADER(config.h)
AC_PROG_CXX
AC_PREFIX_DEFAULT(/usr/local)

AC_ARG_ENABLE(readline, AC_HELP_STRING([--disable-readline], [Disables readline support (default=yes)]), [], enable_readline=yes)
AC_ARG_ENABLE(readline, AC_HELP_STRING([--disable-readline], [Disables readline support (default=yes)]))
AC_ARG_ENABLE(ssl, AC_HELP_STRING([--enable-ssl], [Enable OpenSSL support (default=no)]))
AC_ARG_ENABLE(mysqlclient, AC_HELP_STRING([--enable-mysql], [Enable MySQL support (default=no)]))
AC_ARG_ENABLE(sqlite, AC_HELP_STRING([--enable-sqlite], [Enable embedded SQLite support (default=no)]))

header_error() AC_MSG_ERROR([Could not find required header, please check the installation of the required header])
lib_error() AC_MSG_ERROR([Library test failed, please check the installation of the required library])

echo "===> Checking for dependencies"

AC_CHECK_HEADERS(dlfcn.h netinet/in.h time.h pthread.h event.h, [], [header_error])
AC_CHECK_LIB(pthread, pthread_create, [], [lib_error])
AC_CHECK_LIB(event, event_init, [], [lib_error])

echo -n "===> Configuring with SSL "
if test -z $enable_ssl || test $enable_ssl != "yes"; then
  echo disabled
else
  echo enabled
  AC_CHECK_HEADERS(openssl/ssl.h, [], [header_error])
  AC_CHECK_LIB(ssl, SSL_write, [], [lib_error])
fi

echo -n "===> Configuring with MySQL "
if test -z $enable_mysql || test $enable_mysql != "yes"; then
  echo disabled
else
  echo enabled
  AC_CHECK_HEADERS(mysql/mysql.h, [], [header_error])
  AC_CHECK_LIB(mysqlclient, mysql_init, [], [lib_error])
fi

echo -n "===> Configuring with SQLite "
if test -z $enable_sqlite || test $enable_sqlite != "yes"; then
  echo disabled
else
  echo enabled
  AC_CHECK_HEADERS(sqlite3.h, [], [header_error])
  AC_CHECK_LIB(sqlite3, sqlite3_open, [], [lib_error])
fi

echo -n "===> Configuring with readline "
if test -z $enable_readline || test $enable_readline != "yes"; then
  echo disabled
else
  echo enabled
  AC_CHECK_HEADERS(readline/readline.h, [], [header_error])
  AC_CHECK_LIB(readline, readline, [], [lib_error])
fi

echo "===> Checking for find with extended regexp "

AC_SUBST([efind])
if find -E ./configure >/dev/null; then
 efind='find -E'
else
 efind='find -regextype posix-extended'
fi

if test `uname` = "Linux"; then 
  echo "===> Configuring with -ldl (Linux)"
  AC_CHECK_LIB(dl, dlopen, [], [lib_error])
fi


AC_OUTPUT(Makefile)
AC_OUTPUT(../Makefile)

echo "===> Posttasking Makefile"

SRCS=`find ./ -type f -name '*.cpp' | grep -v ./mods`
OBJS=''

echo > .Makefile || exit 1
echo > .Makefile.deps || exit 1

for src in $SRCS; do
  obj=`echo $src | sed 's/\(.*\)\.cpp/\.\.\/obj\/\1\.o/'`
  OBJS="$OBJS $obj"
  echo "$obj: $src" >> .Makefile.deps
done 
  
echo SRCS=$SRCS >> .Makefile.tmp
echo OBJS=$OBJS >> .Makefile.tmp
cat Makefile >> .Makefile.tmp
cat .Makefile.deps >> .Makefile.tmp
mv -f .Makefile.tmp Makefile || exit 1
rm -f .Makefile.deps

echo "===> Posttasking config.h"
cat << END >> config.h

/* Posttasking has been done by ./src/configure.
   Please edit ./src/configure.ac and run autoconf if you
   want to modify all values below this comment!
 */

/* Program prefix. */
#define PREFIX "$prefix"
END 

echo You are ready to run GNU Make now!
