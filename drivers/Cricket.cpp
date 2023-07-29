#include "Cricket.h"
#include <Windows.h>
#include <iostream>

Cricket::Cricket() {
	initialize();
}



Cricket::~Cricket() {
	close();
}

bool Cricket::open(const char* portName) {
	mHandle = CreateFileA(
		portName, GENERIC_READ | GENERIC_WRITE, 0, NULL, OPEN_EXISTING, 0, NULL
	);

	if (mHandle == INVALID_HANDLE_VALUE) {
		std::cerr << "Failed to open port " << portName << ". Error code: " << GetLastError() << std::endl;
		return false;
	}

	if (!setPortParameters()) {
		close();
		return false;
	}
	return true;
}

void Cricket::close() {
	if (mHandle != nullptr) {
		CloseHandle(mHandle);
		mHandle = nullptr;
	}
}

bool Cricket::isOpen() const {
	return mHandle != nullptr;
}

void Cricket::startTriggers(uint32_t numberOfCPUCycles, uint32_t numberOfCPUCyclesDAQ) {
	char buffer[8];
	sprintf_s(buffer, "S%03d%03d", numberOfCPUCyclesDAQ, numberOfCPUCycles);
	std::cout << "Cricket command:: " << buffer << std::endl;
	DWORD bytesWritten;
	if (!WriteFile(mHandle, buffer, sizeof(buffer), &bytesWritten, nullptr)) {
		std::cerr << "Failed to write to port. Error code: " << GetLastError() << std::endl;
	}
}


void Cricket::stopTriggers() {
	Cricket::startTriggers(0,0);
}


void Cricket::initialize() {
	mHandle = nullptr;
}

bool Cricket::setPortParameters() {
	DCB dcbSerialParams = { 0 };
	dcbSerialParams.DCBlength = sizeof(dcbSerialParams);
	if (!GetCommState(mHandle, &dcbSerialParams)) {
		std::cerr << "Failed to get port parameters. Error code: " << GetLastError() << std::endl;
		return false;
	}
	dcbSerialParams.BaudRate = CBR_9600;
	dcbSerialParams.ByteSize = 8;
	dcbSerialParams.StopBits = ONESTOPBIT;
	dcbSerialParams.Parity = NOPARITY;
	if (!SetCommState(mHandle, &dcbSerialParams)) {
		std::cerr << "Failed to set port parameters. Error code: " << GetLastError() << std::endl;
		return false;
	}
	COMMTIMEOUTS timeouts = { 0 };
	timeouts.ReadIntervalTimeout = 50;
	timeouts.ReadTotalTimeoutConstant = 50;
	timeouts.ReadTotalTimeoutMultiplier = 10;
	timeouts.WriteTotalTimeoutConstant = 50;
	timeouts.WriteTotalTimeoutMultiplier = 10;

	if (!SetCommTimeouts(mHandle, &timeouts)) {
		std::cerr << "Failed to set port timeouts. Error code: " << GetLastError() << std::endl;
		return false;
	}
	return true;
}