//
// Created by user on 22. 6. 18.
//

#include "MIPISimulation.h"
#include "include/SimulationUI/simulationMainwindow.h"
#include "include/RT/rb_utils.h"
#include "MIPILQRController.h"
#include <QApplication>
#include <cmath>

extern MainWindow *MainUI;
pthread_t thread_simulation;

std::string urdfPath = "\\home\\camel\\raisimLib\\camel-code-raisim-cpp\\rsc\\camel_momentum_inverted_pendulum_with_incline.urdf";
std::string name = "MIPI";
raisim::World world;

double simulationDuration = 4.0;
double dT = 0.005;
MIPISimulation sim = MIPISimulation(&world, dT);
MIPIRobot robot = MIPIRobot(&world, urdfPath, name);
MIPILQRController controller = MIPILQRController(&robot, &world, dT);


double oneCycleSimTime = 0;
int divider = ceil(simulationDuration / dT / 200);
int iteration = 0;

void raisimSimulation() {
    if ((MainUI->button1) && (oneCycleSimTime < simulationDuration)) {
        oneCycleSimTime = iteration * dT;
        controller.doControl();
        world.integrate();
        if (iteration % divider == 0) {
            MainUI->data_x[MainUI->data_idx] = world.getWorldTime();
            MainUI->data_y1[MainUI->data_idx] = robot.getQ()[0];
            MainUI->data_y1_desired[MainUI->data_idx] = controller.getDesiredInclinePosition();
            MainUI->data_y2[MainUI->data_idx] = robot.getQ()[1];
            MainUI->data_y2_desired[MainUI->data_idx] = 0;
            MainUI->data_y3_blue[MainUI->data_idx] = controller.getTorque()[2];
            MainUI->data_y3_red[MainUI->data_idx] = robot.getQD()[2];
            MainUI->data_idx += 1;
        }

        iteration++;
    } else if (oneCycleSimTime >= simulationDuration) {
        MainUI->button1 = false;
        iteration = 0;
        oneCycleSimTime = 0;
        MainUI->plotWidget1();
        MainUI->plotWidget2();
        MainUI->plotWidget3();
        MainUI->data_idx = 0;
    }
}

void *rt_simulation_thread(void *arg) {
    std::cout << "entered #rt_time_checker_thread" << std::endl;
    struct timespec TIME_NEXT;
    struct timespec TIME_NOW;
    const long PERIOD_US = long(dT * 1e6); // 200Hz ?????? ?????????

    clock_gettime(CLOCK_REALTIME, &TIME_NEXT);
    std::cout << "bf #while" << std::endl;
    std::cout << "control freq : " << 1 / double(PERIOD_US) * 1e6 << std::endl;
    while (true) {
        clock_gettime(CLOCK_REALTIME, &TIME_NOW); //?????? ?????? ??????
        timespec_add_us(&TIME_NEXT, PERIOD_US);   //?????? ?????? ??????

        raisimSimulation();

        clock_nanosleep(CLOCK_REALTIME, TIMER_ABSTIME, &TIME_NEXT, NULL); //?????????????????? ????????? (??????????????? ?????? ???????????? ????????? ?????? ????????? ???)
        if (timespec_cmp(&TIME_NOW, &TIME_NEXT) > 0) {  // ??????????????? ???????????? ?????? ???????????? ?????? ?????????
            std::cout << "RT Deadline Miss, Time Checker thread : " << timediff_us(&TIME_NEXT, &TIME_NOW) * 0.001
                      << " ms" << std::endl;
        }
    }
}

int main(int argc, char *argv[]) {
    QApplication a(argc, argv);
    MainWindow w;
    raisim::RaisimServer server(&world);
    server.launchServer(8080);
    int thread_id_timeChecker = generate_rt_thread(thread_simulation, rt_simulation_thread, "simulation_thread", 0, 99,
                                                   NULL);
    w.show();

    return a.exec();
}

