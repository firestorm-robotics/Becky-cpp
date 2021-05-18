// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

// This used to include Robot.h, now it doesn't because I prefer to write my own class.
// By the way, this is a modular robot program that can be extended quite easily.

#include <frc/DriverStation.h>
#include <frc/livewindow/LiveWindow.h>
#include <frc/shuffleboard/Shuffleboard.h>
#include <hal/DriverStation.h>
#include <networktables/NetworkTable.h>
#include <ctre/Phoenix.h>
#include "rev/CANSparkMax.h"
#include <atomic>

#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h> // FRC ROBOTS SUPPORT THE LINUX FRAMEWORK

#include <frc/Joystick.h>
#include <frc/RobotBase.h>
#include <unistd.h>

class SingleServer{ // Skeleton class that does not handle data. This one only accepts a single client at a time!
    struct sockaddr_in serveraddress;
    int serverfd;
    struct sockaddr clientaddress;
    int clientfd;
public:
    SingleServer(int port){ // All servers bind to every address via INADDR_ANY, so no need for an address field

    }
};

class Robot : public frc::RobotBase{
private:
    std::atomic<bool> m_exit{false}; // Multithreaded variable. This is why the code doesn't die!
    rev::CANSparkMax DriveLeft{11,rev::CANSparkMax::MotorType::kBrushless};
    rev::CANSparkMax DriveLeftSlave{12,rev::CANSparkMax::MotorType::kBrushless};
    rev::CANSparkMax DriveRight{13,rev::CANSparkMax::MotorType::kBrushless};
    rev::CANSparkMax DriveRightSlave{14,rev::CANSparkMax::MotorType::kBrushless};
    TalonSRX ArmMaster{15};
    TalonSRX ArmSlave{16};

    frc::Joystick Controls{5};
    int mode = 0; // 0 = disabled, 1 = autonomous, 2 = test, 3 = teleop

    // Below this line is server stuff. There may be multiple servers and in the case that there are we must make sure that they follow this port specification:
    // Command servers are from 8000 - 8099
    // Data broadcast servers are from 8100 - 8199 (For things like processor statistics and motor information etc)
    // Media streaming (video/audio) is from 8200 - 8299
    // General purpose servers (do any combination of the above) are from 8300 - 8399


public:
    Robot(){
    }

    void RobotInit() {
        DriveLeftSlave.Follow(DriveLeft);
        DriveRightSlave.Follow(DriveRight);
    }

    void Loop(){ // Mainloop

    }

    void DisabledMode() {}

    void AutonomousMode() {}

    void TeleopMode() {
        DriveRight.Set(0);
        DriveLeft.Set(0);
    }

    void TestMode() {
        DriveLeft.Set(0.2);
        DriveRight.Set(0.2);
    }

    void StartCompetition() {
        auto& lw = *frc::LiveWindow::GetInstance();

        /* A little socket server! This is basically to replace the horrible, awful, ugly, mean, stinky-poo, totally un-happy FRC shuffleboard.
        struct sockaddr_in serveraddress;
        int serversocket;
        struct sockaddr clientaddress;
        int clientsocket;

        memset(&serveraddress,0,sizeof(serveraddress));
        serveraddress.sin_family = AF_INET;
        serveraddress.sin_addr.s_addr = INADDR_ANY;
        serveraddress.sin_port = htons(8080);
        serversocket = socket(AF_INET, SOCK_STREAM, 0);
        bind(serversocket, (struct sockaddr*) &serveraddress, sizeof(serveraddress));
        listen(serversocket, 13);

        unsigned int size = sizeof(clientaddress);
        clientsocket = accept(serversocket, (struct sockaddr*) &clientaddress, &size);
        char data[1030];
        recv(clientsocket, data, 1024, 0);
        send(clientsocket, data, 1024, 0); // Echo server, runs before the RIO does anything.*/
        RobotInit();

        // Tell the DS that the robot is ready to be enabled
        HAL_ObserveUserProgramStarting();

        HAL_SendConsoleLine("Becky (FRC 6341) operational, begin mainloop");
        while (!m_exit){ // Restructured from the old uglies. This one gives easy-peasy mainlooping without our ugly-mugly turdy-purdy stinky-winky infinite while loop
            Loop(); // General mainloop
            if (IsDisabled() && !(mode == 0)){ // Disabled tasks
                HAL_ObserveUserProgramDisabled();
                HAL_SendConsoleLine("Running disabled mode tasks")
                DisabledMode();
                mode = 0;
            }
            else if (IsAutonomous() && !(mode == 1)){ // Autonomous tasks
                HAL_ObserveUserProgramAutonomous();
                HAL_SendConsoleLine("Running autonomous mode tasks")
                AutonomousMode();
                mode = 1;
            }
            else if (IsTest() && !(mode == 2)){ // Test tasks
                HAL_ObserveUserProgramTest();
                HAL_SendConsoleLine("Running test mode tasks")
                TestMode();
                mode = 2;
            }
            else if (!mode == 3){ // Teleop tasks
                HAL_ObserveUserProgramTeleop();
                HAL_SendConsoleLine("Running teleop mode tasks")
                TeleopMode();
                mode = 3;
            }
        }
    }

    void EndCompetition() {
        m_exit = true;
    }
};

#ifndef RUNNING_FRC_TESTS
int main() {
  return frc::StartRobot<Robot>();
}
#endif
