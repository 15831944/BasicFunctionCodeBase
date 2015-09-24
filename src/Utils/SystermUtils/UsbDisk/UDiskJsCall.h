#ifndef __UDISKJSCALL_H_
#define __UDISKJSCALL_H_

#include "JsGroupCall.h"

#ifdef __cplusplus
namespace Hippo {
class UDiskJsCall : public JsGroupCall {
};
}
#endif // __cplusplus

extern "C" int UDiskJsCallInit(int type);

#endif
