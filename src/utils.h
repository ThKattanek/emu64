// by Felix Palmen <Zirias>

#ifndef UTILS_H
#define UTILS_H

#ifdef _WIN32
#include <windows.h>
#include <QRegularExpression>
#endif

#define wstr_int(s) L ## s
#define wstr(s) wstr_int(s)

#ifdef _WIN32
#define qfopen(n,m) _wfopen(\
	reinterpret_cast<const wchar_t *>((n).utf16()), wstr(m))
#define qfname(n) ((n).toUtf8().data())
#define qfnsan(n) ((n).replace(QRegularExpression("[|.<>/\\\":]"), "_"))
#else
#define qfopen(n,m) fopen((n).toLocal8Bit().data(), m)
#define qfname(n) ((n).toLocal8Bit().data())
#define qfnsan(n) ((n).replace('/', '_'))
#endif

#endif // UTILS_H
