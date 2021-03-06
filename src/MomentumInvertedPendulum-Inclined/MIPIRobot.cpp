//
// Created by user on 22. 6. 9.
//

#include "MIPIRobot.h"
// [0] : incline-roll
// [1] : base-roll
// [2] : motor-revolute
void MIPIRobot::initialize() {
    Eigen::VectorXd initialJointPosition(robot->getGeneralizedCoordinateDim());
    initialJointPosition.setZero();
    initialJointPosition[0] = 0.05; //0.1 for configuration figure
    initialJointPosition[1] = 0.0; //0.1 for configuration figure
    robot->setGeneralizedCoordinate(initialJointPosition);
}