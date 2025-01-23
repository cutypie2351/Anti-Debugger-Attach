#include "anti_attach.hpp"

using namespace std;

void Anti_Attaching::Initialize(int argc, char* argv[]) {
	setup_ntdll();

	if (argc == 1) {
		Create_Child_Process();
		return;
	}

	// If its has the pid arg, its the child process so its need to protect the main process.
	DWORD parent_pid = atoi(argv[1]);
	Protect_Main_Process(parent_pid);
}

void Anti_Attaching::setup_ntdll() {
	// get the module handle of ntdll
	HMODULE ntdll_handle = GetModuleHandleA("ntdll.dll");
	if (!ntdll_handle)
		throw runtime_error("Cannot get ntdll handle!");

	// load some functions that debuggers use.
	if (!GetProcAddress(ntdll_handle, "NtOpenThread")) 
		throw runtime_error("Cannot Get NtOpenThread Address!");
	if (!GetProcAddress(ntdll_handle, "NtQueryObject")) 
		throw runtime_error("Cannot Get NtQueryObject Address!");
	
	if (!GetProcAddress(ntdll_handle, "NtQuerySystemInformation"))
		throw runtime_error("Cannot Get NtQuerySystemInformation Address!");

}

void Anti_Attaching::Terminate_Main_Process(DWORD pid) {
	HANDLE main_proc = OpenProcess(PROCESS_TERMINATE, FALSE, pid);
	// if main process is protected its should be null.
	if (!main_proc)
		return;

	// else show this message box and kill the main process.
	MessageBoxA(NULL, "Debugger detected!", "Sparta Protector", MB_ICONWARNING);
	TerminateProcess(main_proc, 0);
	CloseHandle(main_proc);
	// exit the child process also.
	ExitProcess(0);
}

void Anti_Attaching::Create_Child_Process() {
	// corrent path var to create the process.
	char CorrentPath[MAX_PATH];
	GetModuleFileNameA(0, CorrentPath, MAX_PATH);

	// the command to run the child process with the same program as the main process.
	std::string command = "\"" + std::string(CorrentPath) + "\" " + std::to_string(GetCurrentProcessId());

	STARTUPINFOA startup_info { sizeof(startup_info) };
	PROCESS_INFORMATION process_info = { NULL };
	
	if (!CreateProcessA(nullptr, (LPSTR)command.c_str(),nullptr, nullptr, FALSE, 0, nullptr, nullptr, &startup_info, &process_info))
		throw runtime_error("Cannot Create the Child Process!");

	CloseHandle(process_info.hProcess);
	CloseHandle(process_info.hThread);

	// Wait for child to exit
	WaitForSingleObject(process_info.hProcess, INFINITE);
}

void Anti_Attaching::Protect_Main_Process(DWORD pid) {

	// try to debug the main process, if its failed then another debugger attached it so we terminate the main.
	HANDLE main_proc = OpenProcess(PROCESS_ALL_ACCESS, FALSE, pid);
	if (!main_proc || !DebugActiveProcess(pid)) {
		Terminate_Main_Process(pid);
		CloseHandle(main_proc);
		return;
	}

	// get debug handle
	HANDLE debug_handle = *(HANDLE*)((BYTE*)__readgsqword(0x30) + 0x16A8);

	// check if main process debugged.
	if (!debug_handle) {
		Terminate_Main_Process(pid);
		CloseHandle(main_proc);
		return;
	}


	// Try to duplicate handle
	HANDLE duplicate_handle;
	if (!DuplicateHandle(GetCurrentProcess(), debug_handle, main_proc, &duplicate_handle,
		0, FALSE, DUPLICATE_SAME_ACCESS)) {
		Terminate_Main_Process(pid);
		CloseHandle(main_proc);
		return;
	}


	// Monitor debug events
	DEBUG_EVENT debug_event;
	int timeout = 0;

	while (timeout < 10) {
		if (!WaitForDebugEvent(&debug_event, 100)) {
			timeout++;
			continue;
		}

		timeout = 0;
		ContinueDebugEvent(debug_event.dwProcessId,
			debug_event.dwThreadId,
			DBG_CONTINUE);
	}

	// Cleanup
	CloseHandle(duplicate_handle);
	CloseHandle(debug_handle);
	CloseHandle(main_proc);
	ExitThread(0);
}