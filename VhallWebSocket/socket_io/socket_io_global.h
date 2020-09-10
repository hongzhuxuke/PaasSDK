#pragma once

//#include <QtCore/qglobal.h>

#ifndef BUILD_STATIC
# if defined(SOCKET_IO_LIB)
#  define SOCKET_IO_EXPORT Q_DECL_EXPORT
# else
#  define SOCKET_IO_EXPORT Q_DECL_IMPORT
# endif
#else
# define SOCKET_IO_EXPORT
#endif
