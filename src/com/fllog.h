#ifndef FLLOGHEADERH_INCLUDED
#define FLLOGHEADERH_INCLUDED

#include"flArray.h"

typedef flArray flLog;

#define fllogNew(logstr) flarrstrNew(logstr)

#define fllogNews(argc, ...) flarrstrNews(argc, __VA_ARGS__)

#define fllogPush(_fllog, strv) flarrstrPush(_fllog, strv)

#define fllogPushs(_fllog, argc, ...) flarrstrPushs(_fllog, argc, __VA_ARGS__)

#define fllogStr(_fllog) flarrstrCstr(_fllog)

#define fllogClear(_fllog) _flarrSetLength(_fllog, 0)

#define fllogFree(_fllog) flarrFree(_fllog)

#define fllogPfree(_fllogPP) flarrPfree(_fllogPP)

#endif