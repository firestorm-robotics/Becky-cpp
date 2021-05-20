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

#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h> // FRC ROBOTS SUPPORT THE LINUX KERNEL COMMANDS

#include <frc/Joystick.h>
#include <stdio.h>
#include <unistd.h>

#include "ModularRobot.hpp"
#include "c_str_man.hpp"

enum ServerCommand{

};

enum ServerResponse{

};

class SingleClientCommandServer : public Module{ // Skeleton class that does not handle data. This one only accepts a single client at a time!
    struct sockaddr_in serveraddress; // Address of the server
    int serverfd; // File descriptor of the server
    struct sockaddr clientaddress; // Guess!
    int clientfd;
    ServerResponse (*GotCommandCallback)(ServerCommand);
public:
    SingleClientCommandServer(int port, ServerResponse (*gotCommandCallback)(ServerCommand)){ // All servers bind to every address via INADDR_ANY, so no need for an address field. The second thing is a function pointer - Google It!
        GotCommandCallback = gotCommandCallback;
        memset(&serveraddress, 0, sizeof(serveraddress)); // Memset is a system-call that writes a value repetitively to memory. Because memory is often full of random bits (for some reason), use this to keep life the universe and everything from imploding.
        memset(&clientaddress, 0, sizeof(clientaddress)); // Not necessary but I'm a perfectionist

        serveraddress.sin_family = AF_INET; // IPv4
        serveraddress.sin_addr.s_addr = INADDR_ANY; // Every bindable address
        serveraddress.sin_port = htons(port);

        serverfd = socket(AF_INET, SOCK_STREAM, 0); // New socket following the AF_INET internet protocol, the SOCK_STREAM (TCP) transfer protocol, and with 0 flags
        bind(serverfd, (struct sockaddr*) &serveraddress, sizeof(serveraddress)); // Bind the server socket to the server address
        listen(serverfd, 25); // The socket will keep 25 incoming connections in queue until it tosses them out. This is a bad coding practice for a single-client-at-a-time server, but I want to be safe.
    }
    void run(){ // Receive data

    }
};


class Robot : public ModularRobot{
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
        //HAL_SendConsoleLine("This is a console line");
        setData("Becky", "Firestorm Robotics", 6341);
    }

    void Init(){
        DriveLeftSlave.Follow(DriveLeft);
        DriveRightSlave.Follow(DriveRight);
    }

    void TeleopLoop(){
        HAL_SendConsoleLine(inttostring(Controls.GetX()));
        usleep(2000000);
    }

    void BeginTest(){
        DriveLeft.Set(0.2);
        DriveRight.Set(0.2);
    }

    void CleanUpTest(){
        DriveLeft.Set(0);
        DriveRight.Set(0);
    }
};

#ifndef RUNNING_FRC_TESTS
int main() {
  return frc::StartRobot<Robot>();
}
#endif
