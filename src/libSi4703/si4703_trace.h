#ifndef SI4703_TRACE_H
#define SI4703_TRACE_H

#include <cstdio>

#define SI4703_DEBUG_ON

#ifdef SI4703_DEBUG_ON
#define DEBUG_LOG(format, ...) printf ("%s: " format, __FUNCTION__, ##__VA_ARGS__)
#else
#define DEBUG_LOG(format, ...) 
#endif

#endif //SI4703_TRACE_H