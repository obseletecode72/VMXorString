#include <iostream>
#include <string>
#include <cstring>
#include <vector>
#include "xor.hpp"

int g_pass = 0;
int g_fail = 0;
int g_total = 0;

void check(bool cond, int id, const char* result) {
    g_total++;
    if (cond) {
        g_pass++;
        std::cout << XORSTR("[PASS] Test ") << id << XORSTR(" -> ") << result << std::endl;
    } else {
        g_fail++;
        std::cout << XORSTR("[FAIL] Test ") << id << XORSTR(" -> ") << result << std::endl;
    }
}

void checkw(bool cond, int id, const wchar_t* result) {
    g_total++;
    std::cout.flush();
    if (cond) {
        g_pass++;
        std::wcout << XWOSTR(L"[PASS] Test ") << id << XWOSTR(L" -> ") << result << std::endl;
    } else {
        g_fail++;
        std::wcout << XWOSTR(L"[FAIL] Test ") << id << XWOSTR(L" -> ") << result << std::endl;
    }
    std::wcout.flush();
}

// if you dont want to global strings in memory, then define it using auto, if you define it using const char* will stay in memory
auto g_global1 = XORSTR("GlobalString");
auto g_global2 = XORSTR("AnotherGlobal");
auto g_globalW1 = XWOSTR(L"WideGlobal");
auto g_globalW2 = XWOSTR(L"WideGlobal2");

int main() {
    {
        auto r = XORSTR("Hello").run();
        check(r == RUN_XOR("Hello"), 1, r.c_str());
    }
    {
        auto r = XORSTR("World").run();
        check(r == RUN_XOR("World"), 2, r.c_str());
    }
    {
        auto r = XORSTR("Test123").run();
        check(r == RUN_XOR("Test123"), 3, r.c_str());
    }
    {
        auto r = XORSTR("ABCDEFGHIJKLMNOPQRSTUVWXYZ").run();
        check(r == RUN_XOR("ABCDEFGHIJKLMNOPQRSTUVWXYZ"), 4, r.c_str());
    }
    {
        auto r = XORSTR("abcdefghijklmnopqrstuvwxyz").run();
        check(r == RUN_XOR("abcdefghijklmnopqrstuvwxyz"), 5, r.c_str());
    }
    {
        auto r = XORSTR("0123456789").run();
        check(r == RUN_XOR("0123456789"), 6, r.c_str());
    }
    {
        auto r = XORSTR("!@#$%^&*()").run();
        check(r == RUN_XOR("!@#$%^&*()"), 7, r.c_str());
    }
    {
        auto r = XORSTR("Hello World").run();
        check(r == RUN_XOR("Hello World"), 8, r.c_str());
    }
    {
        auto r = XORSTR("tab\there").run();
        check(r == RUN_XOR("tab\there"), 9, r.c_str());
    }
    {
        auto r = XORSTR("line\nbreak").run();
        check(r == RUN_XOR("line\nbreak"), 10, r.c_str());
    }
    {
        auto r = XORSTR("A").run();
        check(r == RUN_XOR("A"), 11, r.c_str());
    }
    {
        auto r = XORSTR("AB").run();
        check(r == RUN_XOR("AB"), 12, r.c_str());
    }
    {
        auto r = XORSTR("This is a longer string used for testing the mini-vm obfuscator engine").run();
        check(r == RUN_XOR("This is a longer string used for testing the mini-vm obfuscator engine"), 13, r.c_str());
    }
    {
        auto r = XORSTR("P@ssw0rd!2026#Sec").run();
        check(r == RUN_XOR("P@ssw0rd!2026#Sec"), 14, r.c_str());
    }
    {
        auto r = XORSTR("C:\\Windows\\System32\\drivers").run();
        check(r == RUN_XOR("C:\\Windows\\System32\\drivers"), 15, r.c_str());
    }
    {
        auto r = XORSTR("https://api.example.com/v1/auth").run();
        check(r == RUN_XOR("https://api.example.com/v1/auth"), 16, r.c_str());
    }
    {
        auto r = XORSTR("key=value&foo=bar").run();
        check(r == RUN_XOR("key=value&foo=bar"), 17, r.c_str());
    }
    {
        auto r = XORSTR("{\"name\":\"test\",\"id\":42}").run();
        check(r == RUN_XOR("{\"name\":\"test\",\"id\":42}"), 18, r.c_str());
    }
    {
        auto r = XORSTR("<xml><tag>data</tag></xml>").run();
        check(r == RUN_XOR("<xml><tag>data</tag></xml>"), 19, r.c_str());
    }
    {
        auto r = XORSTR("SELECT * FROM users WHERE id=1").run();
        check(r == RUN_XOR("SELECT * FROM users WHERE id=1"), 20, r.c_str());
    }
    {
        auto r = XORSTR("Hello").run();
        check(r != RUN_XOR("World"), 21, r.c_str());
    }
    {
        auto r = XORSTR("AAA").run();
        check(r != RUN_XOR("BBB"), 22, r.c_str());
    }
    {
        auto r = XORSTR("Same").run();
        check(!(r != RUN_XOR("Same")), 23, r.c_str());
    }
    {
        auto r = XORSTR("RunnerTest").run();
        check(r == RUN_XOR("RunnerTest"), 24, r.c_str());
    }
    {
        auto r = XORSTR("SizeTest").run();
        std::string info = std::string(r.c_str()) + XORSTR(" (size=") + std::to_string(r.size()) + XORSTR(")");
        check(r.size() == 8, 25, info.c_str());
    }
    {
        auto r = XORSTR("A").run();
        std::string info = std::string(r.c_str()) + XORSTR(" (size=") + std::to_string(r.size()) + XORSTR(")");
        check(r.size() == 1, 26, info.c_str());
    }
    {
        auto r = XORSTR("ABCDEF").run();
        std::string info = std::string(r.c_str()) + XORSTR(" (size=") + std::to_string(r.size()) + XORSTR(")");
        check(r.size() == 6, 27, info.c_str());
    }
    {
        auto r = XORSTR("CStrTest").run();
        check(std::strcmp(r.c_str(), RUN_XOR("CStrTest")) == 0, 28, r.c_str());
    }
    {
        auto r = XORSTR("RunMacro").run();
        check(std::strcmp(r.c_str(), RUN_XOR("RunMacro")) == 0, 29, r.c_str());
    }
    {
        auto r = XORSTR("P@ss!").run();
        check(std::strcmp(r.c_str(), RUN_XOR("P@ss!")) == 0, 30, r.c_str());
    }
    {
        std::string s(RUN_XOR("StdStringConvert"));
        check(s == RUN_XOR("StdStringConvert"), 31, s.c_str());
    }
    {
        std::string s1(RUN_XOR("Alpha"));
        std::string s2(RUN_XOR("Beta"));
        std::string both = s1 + XORSTR(", ") + s2;
        check(s1 == RUN_XOR("Alpha") && s2 == RUN_XOR("Beta"), 32, both.c_str());
    }
    {
        auto vm = XORSTR("CastTest");
        const char* p = (const char*)vm;
        check(std::strcmp(p, RUN_XOR("CastTest")) == 0, 33, p);
    }
    {
        auto vm = XORSTR("DecryptedTest");
        const char* d = vm.decrypted();
        check(std::strcmp(d, RUN_XOR("DecryptedTest")) == 0, 34, d);
    }
    {
        auto r = g_global1.run();
        check(g_global1 == RUN_XOR("GlobalString"), 35, r.c_str());
    }
    {
        auto r = g_global2.run();
        check(g_global2 == RUN_XOR("AnotherGlobal"), 36, r.c_str());
    }
    {
        auto r = g_global1.run();
        check(g_global1 != RUN_XOR("AnotherGlobal"), 37, r.c_str());
    }
    {
        auto r = g_global2.run();
        check(g_global2 != RUN_XOR("GlobalString"), 38, r.c_str());
    }
    {
        auto r1 = XORSTR("Consistency").run();
        auto r2 = XORSTR("Consistency").run();
        check(std::strcmp(r1.c_str(), r2.c_str()) == 0, 39, r1.c_str());
    }
    {
        auto r = XWOSTR(L"WideHello").run();
        checkw(r == RUN_XWO(L"WideHello"), 40, r.c_str());
    }
    {
        auto r = XWOSTR(L"WideWorld").run();
        checkw(r.size() == 9, 41, r.c_str());
    }
    {
        auto r = XWOSTR(L"WideTest").run();
        checkw(r == RUN_XWO(L"WideTest"), 42, r.c_str());
    }
    {
        auto r = XWOSTR(L"WideABC").run();
        checkw(r != RUN_XWO(L"WideDEF"), 43, r.c_str());
    }
    {
        auto r = XWOSTR(L"RunWide").run();
        checkw(r == RUN_XWO(L"RunWide"), 44, r.c_str());
    }
    {
        std::wstring ws(RUN_XWO(L"WideStdString"));
        checkw(ws == RUN_XWO(L"WideStdString"), 45, ws.c_str());
    }
    {
        auto r = g_globalW1.run();
        checkw(g_globalW1 == RUN_XWO(L"WideGlobal"), 46, r.c_str());
    }
    {
        auto r = g_globalW2.run();
        checkw(g_globalW2 == RUN_XWO(L"WideGlobal2"), 47, r.c_str());
    }
    {
        auto r = g_globalW1.run();
        checkw(g_globalW1 != RUN_XWO(L"WideGlobal2"), 48, r.c_str());
    }
    {
        auto wvm = XWOSTR(L"WideDecrypt");
        const wchar_t* d = wvm.decrypted();
        checkw(std::wcscmp(d, RUN_XWO(L"WideDecrypt")) == 0, 49, d);
    }
    {
        std::string arr[5];
        arr[0] = RUN_XOR("Elem0");
        arr[1] = RUN_XOR("Elem1");
        arr[2] = RUN_XOR("Elem2");
        arr[3] = RUN_XOR("Elem3");
        arr[4] = RUN_XOR("Elem4");
        bool ok = (arr[0] == RUN_XOR("Elem0"))
               && (arr[1] == RUN_XOR("Elem1"))
               && (arr[2] == RUN_XOR("Elem2"))
               && (arr[3] == RUN_XOR("Elem3"))
               && (arr[4] == RUN_XOR("Elem4"));
        std::string all = arr[0];
        for (int i = 1; i < 5; i++) {
            all += XORSTR(", ");
            all += arr[i];
        }
        check(ok, 50, all.c_str());
    }
    {
        std::vector<std::string> vec;
        vec.push_back(RUN_XOR("Vec0"));
        vec.push_back(RUN_XOR("Vec1"));
        vec.push_back(RUN_XOR("Vec2"));
        std::string all = vec[0] + XORSTR(", ") + vec[1] + XORSTR(", ") + vec[2];
        check(vec[0] == RUN_XOR("Vec0") && vec[1] == RUN_XOR("Vec1") && vec[2] == RUN_XOR("Vec2"), 51, all.c_str());
    }
    {
        auto r = XORSTR("LoopA").run();
        check((r == RUN_XOR("LoopA")) && (r != RUN_XOR("LoopB")), 52, r.c_str());
    }
    {
        bool all_ok = true;
        auto r = XORSTR("RepeatStability").run();
        for (int i = 0; i < 10; i++) {
            auto ri = XORSTR("RepeatStability").run();
            if (std::strcmp(ri.c_str(), RUN_XOR("RepeatStability")) != 0)
                all_ok = false;
        }
        check(all_ok, 53, r.c_str());
    }
    {
        struct Data {
            std::string field;
        };
        Data d;
        d.field = RUN_XOR("StructField");
        check(d.field == RUN_XOR("StructField"), 54, d.field.c_str());
    }
    {
        struct WData {
            std::wstring field;
        };
        WData wd;
        wd.field = RUN_XWO(L"WideStructField");
        checkw(wd.field == RUN_XWO(L"WideStructField"), 55, wd.field.c_str());
    }
    {
        auto r = XORSTR("   spaces   ").run();
        check(std::strcmp(r.c_str(), RUN_XOR("   spaces   ")) == 0, 56, r.c_str());
    }
    {
        auto r = XORSTR("~`[]{}|;:',.<>?/").run();
        check(r == RUN_XOR("~`[]{}|;:',.<>?/"), 57, r.c_str());
    }
    {
        auto r = XORSTR("MiXeD_CaSe_123!").run();
        check(r == RUN_XOR("MiXeD_CaSe_123!"), 58, r.c_str());
    }
    {
        auto r = XORSTR("\x01\x02\x03\x7F").run();
        check(r.c_str()[0] == '\x01' && r.c_str()[1] == '\x02' && r.c_str()[2] == '\x03' && r.c_str()[3] == '\x7F', 59, XORSTR("ctrl ok"));
    }
    {
        auto r = XORSTR("NtCreateFile").run();
        check(r == RUN_XOR("NtCreateFile"), 60, r.c_str());
    }
    {
        auto r = XORSTR("ZwQueryInformationProcess").run();
        check(r == RUN_XOR("ZwQueryInformationProcess"), 61, r.c_str());
    }
    {
        auto r = XORSTR("NtReadVirtualMemory").run();
        check(r == RUN_XOR("NtReadVirtualMemory"), 62, r.c_str());
    }
    {
        auto r = XORSTR("kernel32.dll").run();
        check(r == RUN_XOR("kernel32.dll"), 63, r.c_str());
    }
    {
        auto r = XORSTR("ntdll.dll").run();
        check(r == RUN_XOR("ntdll.dll"), 64, r.c_str());
    }
    {
        auto r = XORSTR("VirtualAllocEx").run();
        check(r == RUN_XOR("VirtualAllocEx"), 65, r.c_str());
    }
    {
        auto r = XORSTR("WriteProcessMemory").run();
        check(r == RUN_XOR("WriteProcessMemory"), 66, r.c_str());
    }
    {
        auto r = XORSTR("CreateRemoteThread").run();
        check(r == RUN_XOR("CreateRemoteThread"), 67, r.c_str());
    }
    {
        auto r = XWOSTR(L"NtCreateFile").run();
        checkw(r == RUN_XWO(L"NtCreateFile"), 68, r.c_str());
    }
    {
        auto r = XWOSTR(L"kernel32.dll").run();
        checkw(r == RUN_XWO(L"kernel32.dll"), 69, r.c_str());
    }
    {
        auto r = XWOSTR(L"\\Device\\HarddiskVolume1").run();
        checkw(r == RUN_XWO(L"\\Device\\HarddiskVolume1"), 70, r.c_str());
    }
    {
        std::string p(RUN_XOR("C:\\Users\\Admin\\Desktop\\payload.exe"));
        check(p == RUN_XOR("C:\\Users\\Admin\\Desktop\\payload.exe"), 71, p.c_str());
    }
    {
        std::wstring wp(RUN_XWO(L"\\\\?\\C:\\Windows\\System32\\ntdll.dll"));
        checkw(wp == RUN_XWO(L"\\\\?\\C:\\Windows\\System32\\ntdll.dll"), 72, wp.c_str());
    }
    {
        auto v1 = XORSTR("SameString");
        auto v2 = XORSTR("SameString");
        const char* p1 = (const char*)v1;
        const char* p2 = (const char*)v2;
        std::string both = std::string(p1) + XORSTR(" == ") + p2;
        check(std::strcmp(p1, p2) == 0, 73, both.c_str());
    }
    {
        auto r1 = XORSTR("Twelve_chars").run();
        auto r2 = XORSTR("Twelve_chars").run();
        std::string info = std::string(r1.c_str()) + XORSTR(" (size=") + std::to_string(r1.size()) + XORSTR("==") + std::to_string(r2.size()) + XORSTR(")");
        check(r1.size() == r2.size(), 74, info.c_str());
    }
    {
        auto r = XWOSTR(L"ABCDE").run();
        checkw(r.size() == 5, 75, r.c_str());
    }
    {
        auto r = XWOSTR(L"A").run();
        checkw(r.size() == 1, 76, r.c_str());
    }
    {
        auto r = XORSTR("NullCheck").run();
        check(r.c_str()[r.size()] == '\0', 77, r.c_str());
    }
    {
        auto vm = XORSTR("ProxyBuffer1");
        auto vm2 = XORSTR("ProxyBuffer2");
        const char* p1 = (const char*)vm;
        const char* p2 = (const char*)vm2;
        std::string both = std::string(p1) + XORSTR(", ") + p2;
        check(std::strcmp(p1, RUN_XOR("ProxyBuffer1")) == 0 && std::strcmp(p2, RUN_XOR("ProxyBuffer2")) == 0, 78, both.c_str());
    }
    {
        std::string r0(RUN_XOR("Multi0"));
        std::string r1(RUN_XOR("Multi1"));
        std::string r2(RUN_XOR("Multi2"));
        std::string r3(RUN_XOR("Multi3"));
        std::string all = r0 + XORSTR(", ") + r1 + XORSTR(", ") + r2 + XORSTR(", ") + r3;
        check(r0 == RUN_XOR("Multi0") && r3 == RUN_XOR("Multi3"), 79, all.c_str());
    }
    {
        auto r = XORSTR("AAAAAAAAAA").run();
        check(r == RUN_XOR("AAAAAAAAAA"), 80, r.c_str());
    }
    {
        auto r = XORSTR("\xFF").run();
        check(r.size() >= 1, 81, XORSTR("0xFF ok"));
    }
    {
        auto r = XORSTR("line1\nline2\nline3").run();
        check(r == RUN_XOR("line1\nline2\nline3"), 82, XORSTR("multiline ok"));
    }
    {
        auto r = XORSTR("col1\tcol2\tcol3").run();
        check(r == RUN_XOR("col1\tcol2\tcol3"), 83, XORSTR("tabs ok"));
    }
    {
        auto r = XORSTR("has space at end ").run();
        check(r == RUN_XOR("has space at end "), 84, r.c_str());
    }
    {
        auto r = XORSTR(" leading space").run();
        check(r == RUN_XOR(" leading space"), 85, r.c_str());
    }
    {
        auto vm = XORSTR("CompareReverse");
        auto r = vm.run();
        check(RUN_XOR("CompareReverse") == vm, 86, r.c_str());
    }
    {
        auto vm = XORSTR("NotEqualReverse");
        auto r = vm.run();
        check(RUN_XOR("SomethingElse") != vm, 87, r.c_str());
    }
    {
        auto wvm = XWOSTR(L"WideReverse");
        auto r = wvm.run();
        checkw(RUN_XWO(L"WideReverse") == wvm, 88, r.c_str());
    }
    {
        auto wvm = XWOSTR(L"WideReverse");
        auto r = wvm.run();
        checkw(RUN_XWO(L"Other") != wvm, 89, r.c_str());
    }
    {
        std::string concat;
        concat += RUN_XOR("Part1");
        concat += RUN_XOR("Part2");
        concat += RUN_XOR("Part3");
        check(concat == RUN_XOR("Part1Part2Part3"), 90, concat.c_str());
    }
    {
        std::wstring wc;
        wc += RUN_XWO(L"WPart1");
        wc += RUN_XWO(L"WPart2");
        checkw(wc == RUN_XWO(L"WPart1WPart2"), 91, wc.c_str());
    }
    {
        int count = 0;
        for (int i = 0; i < 100; i++) {
            auto r = XORSTR("StressTest").run();
            if (std::strcmp(r.c_str(), RUN_XOR("StressTest")) == 0)
                count++;
        }
        std::string info = std::to_string(count) + XORSTR("/100");
        check(count == 100, 92, info.c_str());
    }
    {
        int count = 0;
        for (int i = 0; i < 50; i++) {
            auto r = XWOSTR(L"WStress").run();
            if (r == RUN_XWO(L"WStress"))
                count++;
        }
        std::string info = std::to_string(count) + XORSTR("/50");
        check(count == 50, 93, info.c_str());
    }
    {
        auto r = XORSTR("SizeAfterRun").run();
        std::string info = std::string(r.c_str()) + XORSTR(" (size=") + std::to_string(r.size()) + XORSTR(")");
        check(r.size() == 12, 94, info.c_str());
    }
    {
        auto r = XWOSTR(L"WideSizeCheck").run();
        checkw(r.size() == 13, 95, r.c_str());
    }
    {
        std::string a0(RUN_XOR("idx0"));
        std::string a1(RUN_XOR("idx1"));
        std::string a2(RUN_XOR("idx2"));
        std::string all = a0 + XORSTR(", ") + a1 + XORSTR(", ") + a2;
        check(a0 == RUN_XOR("idx0") && a1 == RUN_XOR("idx1") && a2 == RUN_XOR("idx2"), 96, all.c_str());
    }
    {
        auto r = XORSTR("+-*/=<>!&|^~%").run();
        check(r == RUN_XOR("+-*/=<>!&|^~%"), 97, r.c_str());
    }
    {
        auto r = XORSTR("backslash\\test").run();
        check(r == RUN_XOR("backslash\\test"), 98, r.c_str());
    }
    {
        auto r = XORSTR("quote\"inside").run();
        check(r == RUN_XOR("quote\"inside"), 99, r.c_str());
    }
    {
        auto r = XORSTR("null\0skip").run();
        check(r == RUN_XOR("null"), 100, r.c_str());
    }

    std::cout << std::endl;
    std::cout << XORSTR("========================================") << std::endl;
    std::cout << XORSTR("  Total:  ") << g_total << std::endl;
    std::cout << XORSTR("  Passed: ") << g_pass << std::endl;
    std::cout << XORSTR("  Failed: ") << g_fail << std::endl;
    std::cout << XORSTR("========================================") << std::endl;

    system(RUN_XOR("pause"));
    return g_fail;
}
