//
// Created by jaehoon on 22. 3. 31.. dkswogns46@gmail.com
//

#include "SingleLeggedSimulation.h"
#include "include/SimulationUI/simulationMainwindow.h"
#include "include/RT/rb_utils.h"
#include <QApplication>
#include <cmath>

extern MainWindow *MainUI;
pthread_t thread_simulation;

std::string urdfPath = "\\home\\camel\\raisimLib\\camel-code-raisim-cpp\\rsc\\camel_single_leg\\camel_single_leg_no_meshes.urdf";
std::string name = "single_leg";
raisim::World world;
Eigen::MatrixXd validationData(7,101);
int validationData_idx = 0;
double simulationDuration = 0.5;
double dT = 0.005;
SingleLeggedSimulation sim = SingleLeggedSimulation(&world, dT);
SingleLeggedRobot robot = SingleLeggedRobot(&world, urdfPath, name);

//SingleLeggedPDController controller = SingleLeggedPDController(&robot);
SingleLeggedIDController controller = SingleLeggedIDController(&robot, dT);
double oneCycleSimTime = 0;
int divider = ceil(simulationDuration / dT / 200);
int iteration = 0;

void raisimSimulation() {
    if ((MainUI->button1) && (oneCycleSimTime < simulationDuration)) {
        oneCycleSimTime = iteration * dT;
        controller.doControl();
        world.integrate();
        validationData(0, validationData_idx) = robot.getQ()[1];
        validationData(1, validationData_idx) = robot.getQ()[2];
        validationData(2, validationData_idx) = robot.getQD()[1];
        validationData(3, validationData_idx) = robot.getQD()[2];
        validationData(4, validationData_idx) = controller.torque[1];
        validationData(5, validationData_idx) = controller.torque[2];
        validationData(6, validationData_idx) = robot.getGRFz();
        if(robot.getGRFz() == 0)
        {
            std::cout<<"zero GRF is sensored."<<std::endl;
        }

        if (iteration % divider == 0) {
            MainUI->data_x[MainUI->data_idx] = world.getWorldTime();
            MainUI->data_y1[MainUI->data_idx] = robot.getQ()[0];
            MainUI->data_y1_desired[MainUI->data_idx] = controller.desiredPosition;
            MainUI->data_y2[MainUI->data_idx] = robot.getQD()[0];
            MainUI->data_y2_desired[MainUI->data_idx] = controller.desiredVelocity;
            MainUI->data_y3_blue[MainUI->data_idx] = controller.torque[1];
            MainUI->data_y3_red[MainUI->data_idx] = controller.torque[2];
            MainUI->data_idx += 1;
        }
        validationData_idx++;
        iteration++;
    } else if (oneCycleSimTime >= simulationDuration) {
        std::cout<<validationData_idx<<std::endl;
        sim.writeToCSVfile("GRFValidation_IDControlled_Quintic_3.csv", validationData);
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

