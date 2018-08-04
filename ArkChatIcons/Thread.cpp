#include "Thread.h"

DWORD WINAPI ThreadProc(LPVOID lpParam) {
	UNREFERENCED_PARAMETER(lpParam);

	auto &plugin = Plugin::Get();
	HANDLE events[] = { plugin.tickMessageSendEvent, plugin.threadShutdownEvent };

	bool shouldExit = false;
	while (!shouldExit) {
		auto dwWaitResult = WaitForMultipleObjects(2, events, FALSE, INFINITE);
		switch (dwWaitResult) {
		case WAIT_OBJECT_0 + 0:
			if (ArkApi::GetApiUtils().GetWorld()) {
				//std::lock_guard<std::mutex> lock(plugin.new_message_available_mutex);
				//plugin.new_message_available = true;
			}
			break;
		case WAIT_OBJECT_0 + 1:
			shouldExit = true;
			break;
		case WAIT_TIMEOUT:
		default:
			Sleep(1000);
		}
	}

	CloseHandle(plugin.threadShutdownEvent);

	return 0;
}
