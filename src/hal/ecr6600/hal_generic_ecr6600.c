#ifdef PLATFORM_ECR6600

#include "../hal_generic.h"

void HAL_RebootModule()
{
	drv_wdt_chip_reset();
}

#endif // PLATFORM_ECR6600
