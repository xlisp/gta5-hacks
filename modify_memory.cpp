#include <windows.h>
#include <tlhelp32.h>
#include <iostream>

// 获取目标进程的 PID
DWORD GetProcessID(const wchar_t* processName) {
    HANDLE hSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
    if (hSnapshot == INVALID_HANDLE_VALUE) return 0;

    PROCESSENTRY32 pe;
    pe.dwSize = sizeof(PROCESSENTRY32);

    if (Process32First(hSnapshot, &pe)) {
        do {
            if (wcscmp(pe.szExeFile, processName) == 0) {
                CloseHandle(hSnapshot);
                return pe.th32ProcessID;
            }
        } while (Process32Next(hSnapshot, &pe));
    }

    CloseHandle(hSnapshot);
    return 0;
}

// 修改内存地址的数据
bool WriteToMemory(DWORD processID, LPCVOID address, const BYTE* data, SIZE_T size) {
    HANDLE hProcess = OpenProcess(PROCESS_ALL_ACCESS, FALSE, processID);
    if (!hProcess) {
        std::wcerr << L"无法打开进程。" << std::endl;
        return false;
    }

    SIZE_T bytesWritten;
    if (!WriteProcessMemory(hProcess, (LPVOID)address, data, size, &bytesWritten)) {
        std::wcerr << L"写入失败。" << std::endl;
        CloseHandle(hProcess);
        return false;
    }

    std::wcout << L"成功修改内存地址数据！" << std::endl;
    CloseHandle(hProcess);
    return true;
}

int main() {
    const wchar_t* targetProcess = L"GTA5.exe"; // 目标程序名
    DWORD processID = GetProcessID(targetProcess);

    if (!processID) {
        std::wcerr << L"未找到目标进程。" << std::endl;
        return 1;
    }

    LPCVOID targetAddress = (LPCVOID)0x00007FF636AC859B; // 示例地址
    BYTE newData[] = {0x48, 0x8B, 0x42, 0x08, 0xF6, 0x40, 0x38, 0x01}; // 示例新数据

    if (WriteToMemory(processID, targetAddress, newData, sizeof(newData))) {
        std::wcout << L"内存修改成功！" << std::endl;
    }

    return 0;
}

