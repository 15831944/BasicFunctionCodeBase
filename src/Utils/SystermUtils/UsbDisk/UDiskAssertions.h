#ifndef _UDISKASSERTIONS_H_
#define _UDISKASSERTIONS_H_

#include "Assertions.h"

#ifdef __cplusplus

extern int gUDiskModuleLevel;

namespace Hippo {

#define LogUDiskError(args...)   LOG_ERROR(HLG_RUNNING, gUDiskModuleLevel, args)
#define LogUDiskWarn(args...)    LOG_WARNING(HLG_RUNNING, gUDiskModuleLevel, args)
#define LogUDiskDebug(args...)   LOG(HLG_RUNNING, gUDiskModuleLevel, args)

#define LogUDiskErrorOut(args...) \
do {  \
    LOG_ERROR(HLG_RUNNING, gUDiskModuleLevel, args);\
    goto Err; \
}while(0)

} // namespace Hippo

#endif //__cplusplus

#endif //_UDISKASSERTIONS_H_

