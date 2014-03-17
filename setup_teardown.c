#include "setup_teardown.h"
#include "platforms.h"
#include "devices.h"
#include "error.h"

ret_code SCOW_Set_Up()
{
	ret_code ret = CL_SUCCESS;

	ret = Collect_Platforms_List();
	OCL_DIE_ON_ERROR(ret, CL_SUCCESS, SCOW_Tear_Down(), ret);

	ret = Collect_Devices_List();
	OCL_DIE_ON_ERROR(ret, CL_SUCCESS, SCOW_Tear_Down(), ret);

	return ret;
}

ret_code SCOW_Tear_Down()
{
	ret_code ret = CL_SUCCESS;

	ret = Erase_Devices_List();
	ret = Erase_Platforms_List();

	return ret;
}