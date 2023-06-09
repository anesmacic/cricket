#include "CricketDriver.h"
#include <iostream>
#include <fcntl.h>
#include <termios.h>
#include <unistd.h>

void CricketDriver::setSerialAttributes() {
    struct termios tty;
    memset(&tty, 0, sizeof tty);

    if (tcgetattr(fd, &tty) != 0) {
        std::cerr << "Error: Unable to get serial port attributes\n";
        exit(EXIT_FAILURE);
    }

    cfsetospeed(&tty, BAUDRATE);
    cfsetispeed(&tty, BAUDRATE);

    tty.c_cflag &= ~CSIZE;
    tty.c_cflag |= DATABITS;
    tty.c_cflag &= ~CSTOPB;
    tty.c_cflag &= ~PARENB;
    tty.c_cc[VMIN] = 1;
    tty.c_cc[VTIME] = 5;

    if (tcsetattr(fd, TCSANOW, &tty) != 0) {
        std::cerr << "Error: Unable to set serial port attributes\n";
        exit(EXIT_FAILURE);
    }
}

CricketDriver::CricketDriver(const std::string& portName) : PORTNAME(portName) {
    // Open the serial port
    fd = open(portName.c_str(), O_RDWR | O_NOCTTY | O_SYNC);
    if (fd == -1) {
        std::cerr << "Error: Unable to open serial port\n";
        exit(EXIT_FAILURE);
    }

    // Set serial port attributes
    setSerialAttributes();
}

void CricketDriver::generateTriggers(int triggerCount, int triggerFrequency) {
    // Send triggerCount and triggerFrequency to the microcontroller
    char command[10];
    snprintf(command, sizeof(command), "t,%d,%d\r\n", triggerCount, triggerFrequency);
    write(fd, command, sizeof(command));
}

CricketDriver::~CricketDriver() {
    // Close the serial port
    close(fd);
}