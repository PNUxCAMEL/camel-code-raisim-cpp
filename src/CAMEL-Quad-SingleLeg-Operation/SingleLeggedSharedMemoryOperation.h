//
// Created by jaehoon on 22. 7. 19.
//

#ifndef RAISIM_SINGLELEGGEDSHAREDMEMORYOPERATION_H
#define RAISIM_SINGLELEGGEDSHAREDMEMORYOPERATION_H

typedef struct _SHM_
{
    double time;
    double position_z;
    double desiredPosition_z;
    double velocity_z;
    double desiredVelocity_z;
    double jointPosition[2];
    double desiredJointPosition[2];
    double jointVelocity[2];
    double desiredJointVelocity[2];
    double jointTorque[2];
    double GRF;
}SHM, *pSHM;

#endif //RAISIM_SINGLELEGGEDSHAREDMEMORYOPERATION_H
