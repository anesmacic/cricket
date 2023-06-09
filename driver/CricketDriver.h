#pragma once

#include <string>


// DEV ID tty.usbserial-AB6VBW3K
class CricketDriver {
private:
    const std::string PORTNAME;
    const int BAUDRATE = B9600; // default baud rate for UART communication
    const int DATABITS = 8;
    const int STOPBITS = 1;
    const int PARITY = 0; // no parity
    int fd; // file descriptor for the serial port

    void setSerialAttributes();

public:
    CricketDriver(const std::string& portName);
    void generateTriggers(int triggerCount, int triggerFrequency);
    ~CricketDriver();
};