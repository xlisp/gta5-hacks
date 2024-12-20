// ConsoleApplication1.cpp : 此文件包含 "main" 函数。程序执行将在此处开始并结束。
//

//#include <iostream>
//
//int main()
//{
//    std::cout << "Hello World!\n";
//}

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
        //return 1;
        std::cout << "no found pid!\n";
    }

    LPCVOID targetAddress = (LPCVOID)0x00007FF636AC859B; // 示例地址
    BYTE newData[] = { 0x48, 0x8B, 0x42, 0x08, 0xF6, 0x40, 0x38, 0x01 }; // 示例新数据

    if (WriteToMemory(processID, targetAddress, newData, sizeof(newData))) {
        std::wcout << L"内存修改成功！" << std::endl;
        std::cout << "update ok!\n";
    }

    std::cout << "finished!\n";
    //return 0;
}


// 运行程序: Ctrl + F5 或调试 >“开始执行(不调试)”菜单
// 调试程序: F5 或调试 >“开始调试”菜单

// 入门使用技巧: 
//   1. 使用解决方案资源管理器窗口添加/管理文件
//   2. 使用团队资源管理器窗口连接到源代码管理
//   3. 使用输出窗口查看生成输出和其他消息
//   4. 使用错误列表窗口查看错误
//   5. 转到“项目”>“添加新项”以创建新的代码文件，或转到“项目”>“添加现有项”以将现有代码文件添加到项目
//   6. 将来，若要再次打开此项目，请转到“文件”>“打开”>“项目”并选择 .sln 文件
