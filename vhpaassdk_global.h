#pragma once

#include <QtCore/qglobal.h>

#ifndef BUILD_STATIC
# if defined(VHPAASSDK_LIB)
#  define VHPAASSDK_EXPORT Q_DECL_EXPORT
# else
#  define VHPAASSDK_EXPORT Q_DECL_IMPORT
# endif
#else
# define VHPAASSDK_EXPORT
#endif
