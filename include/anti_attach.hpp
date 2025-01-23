#pragma once
#include <Windows.h>
#include <iostream>
#include <cstdio>
#include <memory>
#include <algorithm>
#include <string>
#include <thread>

class Anti_Attaching {
public:
	void Initialize(int argc, char* argv[]);
private:
	void setup_ntdll();
	void Terminate_Main_Process(DWORD pid);
	void Protect_Main_Process(DWORD pid);
	void Create_Child_Process();


};