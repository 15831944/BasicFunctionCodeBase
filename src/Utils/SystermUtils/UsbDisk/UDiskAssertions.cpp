#include "UDiskAssertions.h"
#include "LogModule.h"

static int s_UDiskModuleFlag = 0;
int gUDiskModuleLevel = LOG_LEVEL_ERROR;

namespace Hippo {

static LogModule UDiskQuickInstallModule("udisk", s_UDiskModuleFlag, gUDiskModuleLevel);

} // namespace Hippo

