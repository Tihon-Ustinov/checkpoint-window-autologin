#include <windows.h>
#include <winuser.h>
#include <cassert>
#include <stdio.h>
#include <iostream>
#include <thread>
using namespace std;

#define PROG_TITLE "TrGUI"
#define PROG_CLASS "Qt5QWindowIcon"
#define PROG_CHILD "TrConnectDialogClassWindow"
#define PROG_PATH  "C:\\Program Files (x86)\\CheckPoint\\Endpoint Connect\\TrGUI.exe"

static BOOL CALLBACK enumChildWindows(HWND hwnd, LPARAM lParam);
static BOOL CALLBACK enumWindowsProc(HWND hwnd, LPARAM lParam);
static BOOL CALLBACK topLevelEnum(HWND hwnd, LPARAM lParam);
LPSTR convertString(string s) {
    return const_cast<char*>(s.c_str());
}
void toClipboard(HWND hwnd, const std::string& s) {
    OpenClipboard(hwnd);
    EmptyClipboard();
    HGLOBAL hg = GlobalAlloc(GMEM_MOVEABLE, s.size() + 1);
    if (!hg) {
        CloseClipboard();
        return;
    }
    memcpy(GlobalLock(hg), s.c_str(), s.size() + 1);
    GlobalUnlock(hg);
    SetClipboardData(CF_TEXT, hg);
    CloseClipboard();
    GlobalFree(hg);
}

void ctrlKey(int keyCode) {
    keybd_event(VK_CONTROL, 0x9d, 0, 0);
    keybd_event(keyCode, 0x9d, 0, 0);
    keybd_event(VK_CONTROL, 0x9d, KEYEVENTF_KEYUP, 0);
}


class CalcClicker
{
    private:
        HWND _childProc;
        HWND _winProc;
        string _title;
        string _childTitle;
        string _class;
        string _path;
        string LOGIN;
        string PASSWORD;
        friend static BOOL CALLBACK enumChildWindows(HWND hwnd, LPARAM lParam);
        friend static BOOL CALLBACK enumWindowsProc(HWND hwnd, LPARAM lParam);
        friend static BOOL CALLBACK topLevelEnum(HWND hwnd, LPARAM lParam);
        thread findlerThread;
    public:
        CalcClicker(string l, string pa, string t, string c, string p, string ct)
            : LOGIN(l)
            , PASSWORD(pa)
            ,_title(t)
            , _class(c)
            , _path(p)
            , _childTitle(ct)
            , _childProc(NULL)
        { 
           /* findlerThread = thread(&CalcClicker::startWaitForm, this);
            findlerThread.join();*/
        };

        void findChildWindow() {
            EnumWindows(enumWindowsProc, reinterpret_cast<LPARAM>(this));
            if (_winProc == NULL)
            {
                WinExec(convertString(_path), SW_NORMAL);
                EnumWindows(enumWindowsProc, reinterpret_cast<LPARAM>(this));
                if (_winProc == NULL)
                    return;
            }
            SetForegroundWindow(_winProc);
            EnumChildWindows(_winProc, enumChildWindows, reinterpret_cast<LPARAM>(this));
        }
        void runLogin() {
            INPUT input;
            input.type = INPUT_MOUSE;
            input.mi.dx = 0;
            input.mi.dy = 0;
            input.mi.dwFlags = MOUSEEVENTF_LEFTDOWN | MOUSEEVENTF_LEFTUP;
            input.mi.mouseData = 0;
            input.mi.dwExtraInfo = NULL;
            input.mi.time = 0;
            SetForegroundWindow(_childProc);
            WINDOWINFO info;
            GetWindowInfo(_childProc, &info);

            int x = info.rcWindow.left + 150, y = info.rcWindow.top + 228;
            SetCursorPos(x, y);
            input.mi.dx = x;
            input.mi.dy = y;
            SendInput(1, &input, sizeof(INPUT));
            // ctrl + A
            ctrlKey(65);
            // del
            keybd_event(46, 0x9d, 0, 0);
            // copy to buffer
            toClipboard(_childProc, LOGIN);
            // ctrl + v
            ctrlKey(86);

            Sleep(100);

            SetForegroundWindow(_childProc);
            x = info.rcWindow.left + 150, y = info.rcWindow.top + 248;
            SetCursorPos(x, y);
            input.mi.dx = x;
            input.mi.dy = y;
            SendInput(1, &input, sizeof(INPUT));
            // ctrl + A
            ctrlKey(65);
            // del
            keybd_event(46, 0x9d, 0, 0);
            // copy to buffer
            toClipboard(_childProc, PASSWORD);
            // ctrl + v
            ctrlKey(86);

            // tab
            keybd_event(9, 0x9d, 0, 0);
            // enter
            keybd_event(13, 0x9d, 0, 0);
        }

        bool checkFindChild() {
            return _childProc != NULL;
        }
        
        void startWaitForm() {
            while (!checkFindChild()) {
                findChildWindow();
                Sleep(5000);
            }
            if (!IsWindowVisible(_childProc)) {
                return;
            }
            runLogin();
            _childProc = NULL;
            _winProc = NULL;
        }
};
// ///////////////////////////////////////////////////////////////////////// //

static BOOL CALLBACK enumChildWindows(HWND hwnd, LPARAM lParam)
{
    CalcClicker* p = reinterpret_cast<CalcClicker*>(lParam);
    char wndCaption[256];
    GetWindowText(hwnd, wndCaption, sizeof(wndCaption));
    if (string(wndCaption) == p->_childTitle) {
        p->_childProc = hwnd;
        return FALSE;
    }
    return TRUE;
}

static BOOL CALLBACK enumWindowsProc(HWND hwnd, LPARAM lParam)
{
    CalcClicker* p = reinterpret_cast<CalcClicker*>(lParam);

    char szTextWin[255], szClassWin[255];
    if (GetWindowText(hwnd, szTextWin, sizeof(szTextWin)))
    {
        CharToOem(szTextWin, szTextWin);
        GetClassName(hwnd, szClassWin, sizeof(szClassWin));
        string nameWin(szTextWin);
        string classWin(szClassWin);
        if (nameWin == p->_title && classWin == p->_class) {
            p->_winProc = hwnd;
            szTextWin[0] = '\0';
            return false;
        }
    }
    szTextWin[0] = '\0';
    return TRUE;
}

// ///////////////////////////////////////////////////////////////////////// //
int main(int argc, char** argv)
{
    if (argc < 3) return 1;
    ::ShowWindow(::GetConsoleWindow(), SW_HIDE);
    CalcClicker cc = CalcClicker(argv[1], argv[2], PROG_TITLE, PROG_CLASS, PROG_PATH, PROG_CHILD);
    while(true) {
        cc.startWaitForm();
        Sleep(10000);
    }
    return 0;
}