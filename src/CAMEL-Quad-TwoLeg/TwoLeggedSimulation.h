//
// Created by jaehoon on 22. 3. 31..
//

#ifndef RAISIM_SIMPLEPENDULUMSIMULATION_H
#define RAISIM_SIMPLEPENDULUMSIMULATION_H

#include "include/CAMEL/Simulation.h"
#include "TwoLeggedRobot.h"
#include "TwoLeggedPDController.h"

<<<<<<< HEAD
class TwoLeggedSimulation : public Simulation {

public:
    TwoLeggedSimulation(raisim::World *world, double dT) : Simulation(world, dT) { ; }
=======
class SingleLeggedSimulation : public Simulation {

public:
    SingleLeggedSimulation(raisim::World *world, double dT) : Simulation(world, dT) { ; }
>>>>>>> initialize standup branch

private:

};


#endif //RAISIM_SIMPLEPENDULUMSIMULATION_H
