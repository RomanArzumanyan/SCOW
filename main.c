#include "scow.h"

int main(void)
{
    SCOW_Set_Up();

    scow_Steel_Thread *thread = Make_Steel_Thread(Pick_Device_By_Type(CL_DEVICE_TYPE_GPU));
    thread->device->Print_Info(thread->device, DEVICE_ALL_AVAILABLE);

    thread->Destroy(thread);

    SCOW_Tear_Down();
    return 0;
}

