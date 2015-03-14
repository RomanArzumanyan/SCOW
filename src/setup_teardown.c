/*
* @file setup_teardown.c
* @brief implementation for functions, that maintain SCOW setup & tear-down
*
* @see setup_teardown.h
*
* Copyright 2014 Roman Arzumanyan (roman.arzum@gmail.com)
*
* Licensed under the Apache License, Version 2.0 (the "License");
* You may obtain a copy of the License at
*     http://www.apache.org/licenses/LICENSE-2.0
* Unless required by applicable law or agreed to in writing, software
* distributed under the License is distributed on an "AS IS" BASIS,
* WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
* See the License for the specific language governing permissions and
* limitations under the License. */

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