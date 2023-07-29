#pragma once

#include <cstdint>

class Cricket {
public:
	Cricket();
	~Cricket();
	bool open(const char* portName);
	void close();
	bool isOpen() const;
	void startTriggers(uint32_t numberOfCPUCycles, uint32_t numberOfCPUCycles2);
	void stopTriggers();

private:
	void initialize();
	bool setPortParameters();
	void* mHandle;
};