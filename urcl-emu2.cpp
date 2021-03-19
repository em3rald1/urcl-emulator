#include <iostream>
#include <fstream>
#include <iterator>
#include <vector>

#ifdef __unix__
#include <termios.h>
#include <unistd.h>
#include <stdio.h>

int getch(void)
{
    struct termios oldattr, newattr;
    int ch;
    tcgetattr(STDIN_FILENO, &oldattr);
    newattr = oldattr;
    newattr.c_lflag &= ~(ICANON | ECHO);
    tcsetattr(STDIN_FILENO, TCSANOW, &newattr);
    ch = getchar();
    tcsetattr(STDIN_FILENO, TCSANOW, &oldattr);
    return ch;
}

#elif defined(_WIN32) || defined(WIN32)
#include <conio.h>
#endif

using std::cin;
using std::cout;
using std::vector;

typedef unsigned char u8;
typedef unsigned short u16;
typedef bool u1; // unsigned 1 bit

#define H16B 0b1000000000000000

#define addrr 0
#define addir 1
#define addri 2
#define subrr 3
#define subir 4
#define subii 5
#define subri 100
#define rshr 6
#define rshl 7
#define lshr 8
#define lshl 9
#define incr 10
#define incl 11
#define decr 12
#define decl 13
#define xorrr 14
#define xorlr 15
#define xorll 16
#define orrr 17
#define orlr 18
#define orll 19
#define andrr 20
#define andlr 21
#define andll 22
#define notr 23
#define notl 24
#define mov 25
#define imm 26
#define lodr 27
#define lodl 28
#define stral 29
#define strar 30
#define strrl 31
#define strrr 32
#define brar 33
#define bral 34
#define brcr 35
#define brcl 36
#define bncr 37
#define bncl 38
#define brzr 39
#define brzl 40
#define bnzr 41
#define bnzl 42
#define brnl 56
#define brnr 57
#define brpl 58
#define brpr 59
#define nop 43
#define hlt 44
#define pshl 45
#define pshr 46
#define popi 47
#define calr 48
#define call 49
#define ret 50
#define sav 51
#define rsr 52
#define in 53
#define outr 54
#define outl 55
#define mltrr 60
#define mltlr 61
#define mltll 62
#define divrr 63
#define divlr 64
#define divrl 65
#define divll 66
#define modrr 67
#define modlr 68
#define modrl 69
#define modll 70

#define brllrr 71
#define brllrl 72
#define brlllr 73
#define brlrrr 74
#define brlrrl 75
#define brlrlr 76

#define brglrr 77
#define brglrl 78
#define brgllr 79
#define brgrrr 80
#define brgrrl 81
#define brgrlr 82

#define brelrr 83
#define brelrl 84
#define brerrr 85
#define brerrl 86

#define bnelrr 87
#define bnelrl 88
#define bnerrr 89
#define bnerrl 90

#define bodl 91
#define bodr 92

#define bevl 93
#define bevr 94

class EMU
{
public:
    vector<u16> memory;
    vector<u16> regs;
    vector<u16> ports;
    vector<u16> drive;
    u16 ip;
    u1 z, c, n;
    EMU()
    {
        memory.reserve(0xffff);
        drive.reserve(0xffff);
        regs.reserve(0xff);
        ports.reserve(0xff);
        ip = 0;
        for (int i = 0; i < 0xffff; i++)
        {
            memory[i] = 0;
            drive[i] = 0;
        }
        for (int i = 0; i < 0xff; i++)
        {
            regs[i] = 0;
            ports[i] = 0;
        }
        regs[0xff - 3] = 0x7fff;
        regs[0xff - 2] = 0x8fff;
        regs[0xff - 1] = 0x9fff;
    }
    u16 fetch() { return memory[ip++]; }
    void uf(int x) // update flags
    {
        z = x == 0;
        c = x > 65535;
        n = (x & H16B) != 0;
    }
    void push(u16 d)
    {
        memory[regs[0xff - 3]] = d;
        regs[0xff - 3]--;
    }
    u16 pop()
    {
        regs[0xff - 3]++;
        return memory[regs[0xff - 3]];
    }

    void _call(u16 d)
    {
        memory[regs[0xff - 2]] = ip;
        regs[0xff - 2]--;
        ip = d;
    }
    void _ret()
    {
        regs[0xff - 2]++;
        ip = memory[regs[0xff - 2]];
    }

    void _sav(u16 r)
    {
        memory[regs[0xff - 1]] = regs[r];
        regs[0xff - 1]--;
    }
    void _rsr(u16 r)
    {
        regs[0xff - 1]++;
        regs[r] = memory[regs[0xff - 1]];
    }

    u1 step()
    {
        u1 res = 0;
        u16 instr = fetch();
        switch (instr)
        {
            // ADDITION

        case addrr: // R? = R? + R?
        {
            u16 &dest = regs[fetch()];
            u16 o1 = regs[fetch()];
            u16 o2 = regs[fetch()];
            dest = o1 + o2;
            uf(o1 + o2);
            break;
        }
        case addri: // R? = R? + INT
        {
            u16 &dest = regs[fetch()];
            u16 o1 = regs[fetch()];
            u16 o2 = fetch();
            dest = o1 + o2;
            uf(o1 + o2);
            break;
        }
        case addir: // R? = INT + R?
        {
            u16 &dest = regs[fetch()];
            u16 o1 = fetch();
            u16 o2 = regs[fetch()];
            dest = o1 + o2;
            uf(o1 + o2);
            break;
        }

            // SUBTRACTION

        case subrr: // R? = R? - R?
        {
            u16 &dest = regs[fetch()];
            u16 o1 = regs[fetch()];
            u16 o2 = regs[fetch()];
            dest = o1 - o2;
            uf(o1 - o2);
            break;
        }
        case subir: // R? = INT - R?
        {
            u16 &dest = regs[fetch()];
            u16 o1 = fetch();
            u16 o2 = regs[fetch()];
            dest = o1 - o2;
            uf(o1 - o2);
            break;
        }
        case subri: // R? = R? - INT
        {
            u16 &dest = regs[fetch()];
            u16 o1 = regs[fetch()];
            u16 o2 = fetch();
            dest = o1 - o2;
            uf(o1 - o2);
            break;
        }

            // XOR

        case xorrr: // R? = R? ^ R?
        {
            u16 &dest = regs[fetch()];
            u16 o1 = regs[fetch()];
            u16 o2 = regs[fetch()];
            dest = o1 ^ o2;
            uf(o1 ^ o2);
            break;
        }
        case xorlr: // R? = INT ^ R?
        {
            u16 &dest = regs[fetch()];
            u16 o1 = fetch();
            u16 o2 = regs[fetch()];
            dest = o1 ^ o2;
            uf(o1 ^ o2);
            break;
        }

            // AND

        case andrr: // R? = R? & R?
        {
            u16 &dest = regs[fetch()];
            u16 o1 = regs[fetch()];
            u16 o2 = regs[fetch()];
            dest = o1 & o2;
            uf(o1 & o2);
            break;
        }
        case andlr: // R? = INT & R?
        {
            u16 &dest = regs[fetch()];
            u16 o1 = fetch();
            u16 o2 = regs[fetch()];
            dest = o1 & o2;
            uf(o1 & o2);
            break;
        }

            // OR

        case orrr: // R? = R? | R?
        {
            u16 &dest = regs[fetch()];
            u16 o1 = regs[fetch()];
            u16 o2 = regs[fetch()];
            dest = o1 | o2;
            uf(o1 | o2);
            break;
        }
        case orlr: // R? = INT | R?
        {
            u16 &dest = regs[fetch()];
            u16 o1 = fetch();
            u16 o2 = regs[fetch()];
            dest = o1 | o2;
            uf(o1 | o2);
            break;
        }

            // RSH

        case rshl: // R? = INT >> 1
        {
            u16 &dest = regs[fetch()];
            u16 o1 = fetch();
            dest = o1 >> 1;
            uf(o1 >> 1);
            break;
        }
        case rshr: // R? = R? >> 1
        {
            u16 &dest = regs[fetch()];
            u16 o1 = regs[fetch()];
            dest = o1 >> 1;
            uf(o1 >> 1);
            break;
        }

            // LSH

        case lshl: // R? = INT << 1
        {
            u16 &dest = regs[fetch()];
            u16 o1 = fetch();
            dest = o1 << 1;
            uf(o1 << 1);
            break;
        }
        case lshr: // R? = R? << 1
        {
            u16 &dest = regs[fetch()];
            u16 o1 = regs[fetch()];
            dest = o1 << 1;
            uf(o1 << 1);
            break;
        }

            // NOT

        case notl: // R? = ~INT
        {
            u16 &dest = regs[fetch()];
            u16 o1 = fetch();
            dest = ~o1;
            uf(~o1);
            break;
        }
        case notr: // R? = ~R?
        {
            u16 &dest = regs[fetch()];
            u16 o1 = regs[fetch()];
            dest = ~o1;
            uf(~o1);
            break;
        }

            // INC/DEC

        case incr: // R? = R? + 1
        {
            u16 &dest = regs[fetch()];
            u16 o1 = regs[fetch()];
            dest = o1 + 1;
            uf(o1 + 1);
            break;
        }
        case decr: // R? = R? - 1
        {
            u16 &dest = regs[fetch()];
            u16 o1 = regs[fetch()];
            dest = o1 - 1;
            uf(o1 - 1);
            break;
        }

            // MOV/IMM

        case mov: // R? = R?
        {
            u16 &dest = regs[fetch()];
            dest = regs[fetch()];
            break;
        }
        case imm: // R? = INT
        {
            u16 &dest = regs[fetch()];
            dest = fetch();
            break;
        }

            // LOD

        case lodr: // R? = [R?]
        {
            u16 &dest = regs[fetch()];
            dest = memory[regs[fetch()]];
            break;
        }
        case lodl: // R? = [INT]
        {
            u16 &dest = regs[fetch()];
            dest = memory[fetch()];
            break;
        }

            // STR

        case stral: // [INT] = INT
        {
            u16 &dest = memory[fetch()];
            dest = fetch();
            break;
        }
        case strar: // [INT] = R?
        {
            u16 &dest = memory[fetch()];
            dest = regs[fetch()];
            break;
        }
        case strrl: // [R?] = INT
        {
            u16 &dest = memory[regs[fetch()]];
            dest = fetch();
            break;
        }
        case strrr: // [R?] = R?
        {
            u16 &dest = memory[regs[fetch()]];
            dest = regs[fetch()];
            break;
        }

            // JMP (aka BRA)

        case bral: // IP = INT
        {
            ip = fetch();
            break;
        }
        case brar: // IP = R?
        {
            ip = regs[fetch()];
            break;
        }

            // BRC (aka JMP if CARRY set)

        case brcl: // IF(CARRY) IP = INT
        {
            u16 d = fetch();
            if (c)
                ip = d;
            break;
        }
        case brcr: // IF(CARRY) IP = R?
        {
            u16 d = regs[fetch()];
            if (c)
                ip = d;
            break;
        }

            // BNC (aka JMP if CARRY not set)

        case bncl: // IF(!CARRY) IP = INT
        {
            u16 d = fetch();
            if (!c)
                ip = d;
            break;
        }
        case bncr: // IF(!CARRY) IP = R?
        {
            u16 d = regs[fetch()];
            if (!c)
                ip = d;
            break;
        }

            // BRZ (aka JMP if ZERO set)

        case brzl: // IF(CARRY) IP = INT
        {
            u16 d = fetch();
            if (z)
                ip = d;
            break;
        }
        case brzr: // IF(CARRY) IP = R?
        {
            u16 d = regs[fetch()];
            if (z)
                ip = d;
            break;
        }

            // BNZ (aka JMP if ZERO not set)

        case bnzl: // IF(!CARRY) IP = INT
        {
            u16 d = fetch();
            if (!z)
                ip = d;
            break;
        }
        case bnzr: // IF(!CARRY) IP = R?
        {
            u16 d = regs[fetch()];
            if (!z)
                ip = d;
            break;
        }

            // NOP/HLT

        case nop:
        {
            break;
        }
        case hlt: // STOP CLOCK
        {
            res = 1;
            break;
        }

            // PSH/POP

        case pshl: // [VSP] = INT, SP = SP - 1
        {
            push(fetch());
            break;
        }
        case pshr: // [VSP] = R?, SP = SP - 1
        {
            push(regs[fetch()]);
            break;
        }
        case popi: // SP = SP + 1, R? = [VSP]
        {
            regs[fetch()] = pop();
            break;
        }

            // CAL/RET

        case call: // [CSP] = IP, CSP = CSP - 1, IP = INT
        {
            _call(fetch());
            break;
        }
        case calr: // [CSP] = IP, CSP = CSP - 1, IP = R?
        {
            _call(regs[fetch()]);
            break;
        }
        case ret: // CSP = CSP + 1, IP = [CSP]
        {
            _ret();
            break;
        }

            // SAV/RSR

        case sav: // [SSP] = R?, SSP = SSP - 1
        {
            _sav(fetch());
            break;
        }
        case rsr: // SSP = SSP + 1, R? = [SSP]
        {
            _rsr(fetch());
            break;
        }

            // IN/OUT

        case in: // R? = PORTS[INT]
        {
            u16 p = fetch();
            u16 r = fetch();
            if (p == 78) // input from keyboard
            {
                ports[p] = getch();
            }
            else if (p == 122)
            {
                // read from drive
                ports[p] = drive[ports[120]];
            }
            regs[r] = ports[p];
            break;
        }
        case outl: // PORTS[INT] = INT
        {
            u16 p = fetch();
            u16 r = fetch();
            if (p == 78) // char display
            {
                cout << (char)r;
            }
            else if (p == 79) // int display
            {
                cout << (int)r;
            }
            else if (p == 122)
            {
                drive[ports[120]] = r;
            }
            ports[p] = r;
            break;
        }
        case outr: // PORTS[INT] = INT
        {
            u16 p = fetch();
            u16 r = regs[fetch()];
            if (p == 78) // char display
            {
                cout << (char)r;
            }
            else if (p == 79) // int display
            {
                cout << (int)r;
            }
            else if (p == 122)
            {
                drive[ports[120]] = r;
            }
            ports[p] = r;
            break;
        }

            // MLT/DIV/MOD

        case mltrr: // R? = R? * R?
        {
            u16 &dest = regs[fetch()];
            u16 o1 = regs[fetch()];
            u16 o2 = regs[fetch()];
            dest = o1 * o2;
            uf(o1 * o2);
            break;
        }
        case mltlr: // R? = INT * R?
        {
            u16 &dest = regs[fetch()];
            u16 o1 = fetch();
            u16 o2 = regs[fetch()];
            dest = o1 * o2;
            uf(o1 * o2);
            break;
        }

        case divrr: // R? = R? / R?
        {
            u16 &dest = regs[fetch()];
            u16 o1 = regs[fetch()];
            u16 o2 = regs[fetch()];
            dest = (int)o1 / o2;
            uf((int)o1 / o2);
            break;
        }
        case divlr: // R? = INT / R?
        {
            u16 &dest = regs[fetch()];
            u16 o1 = fetch();
            u16 o2 = regs[fetch()];
            dest = (int)o1 / o2;
            uf((int)o1 / o2);
            break;
        }
        case divrl: // R? = R? / INT
        {
            u16 &dest = regs[fetch()];
            u16 o1 = regs[fetch()];
            u16 o2 = fetch();
            dest = (int)o1 / o2;
            uf((int)o1 / o2);
            break;
        }

        case modrr: // R? = R? % R?
        {
            u16 &dest = regs[fetch()];
            u16 o1 = regs[fetch()];
            u16 o2 = regs[fetch()];
            dest = (int)o1 % o2;
            uf((int)o1 % o2);
            break;
        }
        case modlr: // R? = INT % R?
        {
            u16 &dest = regs[fetch()];
            u16 o1 = fetch();
            u16 o2 = regs[fetch()];
            dest = (int)o1 % o2;
            uf((int)o1 % o2);
            break;
        }
        case modrl: // R? = R? % INT
        {
            u16 &dest = regs[fetch()];
            u16 o1 = regs[fetch()];
            u16 o2 = fetch();
            dest = (int)o1 % o2;
            uf((int)o1 % o2);
            break;
        }

            // BRG/BRL/BRE/BNE

        case brglrr: // IF(R? > R?) IP = INT
        {
            u16 a = fetch();
            u16 o1 = regs[fetch()];
            u16 o2 = regs[fetch()];
            if (o1 > o2)
                ip = a;
            break;
        }
        case brglrl: // IF(R? > INT) IP = INT
        {
            u16 a = fetch();
            u16 o1 = regs[fetch()];
            u16 o2 = fetch();
            if (o1 > o2)
                ip = a;
            break;
        }
        case brgllr: // IF(INT > R?) IP = INT
        {
            u16 a = fetch();
            u16 o1 = fetch();
            u16 o2 = regs[fetch()];
            if (o1 > o2)
                ip = a;
            break;
        }

        case brgrrr: // IF(R? > R?) IP = R?
        {
            u16 a = regs[fetch()];
            u16 o1 = regs[fetch()];
            u16 o2 = regs[fetch()];
            if (o1 > o2)
                ip = a;
            break;
        }
        case brgrrl: // IF(R? > INT) IP = R?
        {
            u16 a = regs[fetch()];
            u16 o1 = regs[fetch()];
            u16 o2 = fetch();
            if (o1 > o2)
                ip = a;
            break;
        }
        case brgrlr: // IF(INT > R?) IP = R?
        {
            u16 a = regs[fetch()];
            u16 o1 = fetch();
            u16 o2 = regs[fetch()];
            if (o1 > o2)
                ip = a;
            break;
        }

        case brllrr: // IF(R? < R?) IP = INT
        {
            u16 a = fetch();
            u16 o1 = regs[fetch()];
            u16 o2 = regs[fetch()];
            if (o1 < o2)
                ip = a;
            break;
        }
        case brllrl: // IF(R? < INT) IP = INT
        {
            u16 a = fetch();
            u16 o1 = regs[fetch()];
            u16 o2 = fetch();
            if (o1 < o2)
                ip = a;
            break;
        }
        case brlllr: // IF(INT < R?) IP = INT
        {
            u16 a = fetch();
            u16 o1 = fetch();
            u16 o2 = regs[fetch()];
            if (o1 < o2)
                ip = a;
            break;
        }

        case brlrrr: // IF(R? < R?) IP = R?
        {
            u16 a = regs[fetch()];
            u16 o1 = regs[fetch()];
            u16 o2 = regs[fetch()];
            if (o1 < o2)
                ip = a;
            break;
        }
        case brlrrl: // IF(R? < INT) IP = R?
        {
            u16 a = regs[fetch()];
            u16 o1 = regs[fetch()];
            u16 o2 = fetch();
            if (o1 < o2)
                ip = a;
            break;
        }
        case brlrlr: // IF(INT < R?) IP = R?
        {
            u16 a = regs[fetch()];
            u16 o1 = fetch();
            u16 o2 = regs[fetch()];
            if (o1 < o2)
                ip = a;
            break;
        }

        case brelrr: // IF(R? == R?) IP = INT
        {
            u16 d = fetch();
            u16 o1 = regs[fetch()];
            u16 o2 = regs[fetch()];
            if (o1 == o2)
                ip = d;
            break;
        }
        case brelrl: // IF(R? == R?) IP = INT
        {
            u16 d = fetch();
            u16 o1 = regs[fetch()];
            u16 o2 = fetch();
            if (o1 == o2)
                ip = d;
            break;
        }
        case brerrr: // IF(R? == R?) IP = INT
        {
            u16 d = regs[fetch()];
            u16 o1 = regs[fetch()];
            u16 o2 = regs[fetch()];
            if (o1 == o2)
                ip = d;
            break;
        }
        case brerrl: // IF(R? == R?) IP = INT
        {
            u16 d = regs[fetch()];
            u16 o1 = regs[fetch()];
            u16 o2 = fetch();
            if (o1 == o2)
                ip = d;
            break;
        }

        case bnelrr: // IF(R? == R?) IP = INT
        {
            u16 d = fetch();
            u16 o1 = regs[fetch()];
            u16 o2 = regs[fetch()];
            if (o1 != o2)
                ip = d;
            break;
        }
        case bnelrl: // IF(R? == R?) IP = INT
        {
            u16 d = fetch();
            u16 o1 = regs[fetch()];
            u16 o2 = fetch();
            if (o1 != o2)
                ip = d;
            break;
        }
        case bnerrr: // IF(R? == R?) IP = INT
        {
            u16 d = regs[fetch()];
            u16 o1 = regs[fetch()];
            u16 o2 = regs[fetch()];
            if (o1 != o2)
                ip = d;
            break;
        }
        case bnerrl: // IF(R? == R?) IP = INT
        {
            u16 d = regs[fetch()];
            u16 o1 = regs[fetch()];
            u16 o2 = fetch();
            if (o1 != o2)
                ip = d;
            break;
        }
        case bodl: // IF(R? odd) IP = INT
        {
            u16 d = fetch();
            u16 r = regs[fetch()];
            if (r % 2 == 0)
                ip = d;
            break;
        }
        case bodr: // IF(R? odd) IP = R?
        {
            u16 d = regs[fetch()];
            u16 r = regs[fetch()];
            if (r % 2 == 0)
                ip = d;
            break;
        }

        case bevl: // IF(R? even) IP = INT
        {
            u16 d = fetch();
            u16 r = regs[fetch()];
            if (r % 2 == 1)
                ip = d;
            break;
        }
        case bevr: // IF(R? even) IP = R?
        {
            u16 d = regs[fetch()];
            u16 r = regs[fetch()];
            if (r % 2 == 1)
                ip = d;
            break;
        }
        }
        return res;
    }
    void loadRAM(char *fn)
    {
        std::ifstream data(fn, std::ios::binary);
        vector<u8> readData(std::istreambuf_iterator<char>(data), {});
        u16 *dataPtr = reinterpret_cast<u16 *>(readData.data());
        std::vector<u16> s(dataPtr, dataPtr + readData.size() / 2);

        for (int i = 0; i < s.size(); i++)
        {
            memory[i] = s[i];
        }
        for (int i = s.size() + 1; i < 0xffff; i++)
            memory[i] = 0;
    }

    void loadDrive(char *fn)
    {
        std::ifstream data(fn, std::ios::binary);
        vector<u8> readData(std::istreambuf_iterator<char>(data), {});
        u16 *dataPtr = reinterpret_cast<u16 *>(readData.data());
        std::vector<u16> s(dataPtr, dataPtr + readData.size() / 2);

        for (int i = 0; i < s.size(); i++)
        {
            drive[i] = s[i];
        }
    }

    vector<u8> unloadDrive()
    {
        u8 *drivePtr = reinterpret_cast<u8 *>(drive.data());
        std::vector<u8> driveData(drivePtr, drivePtr + drive.size() * 2);
        return driveData;
    }
    void debug() // prints cpu dump info
    {
        cout << "Registers:\n";
        for (int i = 0; i < 0xff; i++)
        {
            if (regs[i] == 0)
                continue;
            if (i == (0xff - 3))
            {
                cout << "VSP: " << regs[i] << "\n";
            }
            else if (i == (0xff - 2))
            {
                cout << "CSP: " << regs[i] << "\n";
            }
            else if (i == (0xff - 1))
            {
                cout << "SSP: " << regs[i] << "\n";
            }
            else
            {
                cout << "R" << i + 1 << ": " << regs[i] << "\n";
            }
        }
        cout << "Ports:\n";
        for (int i = 0; i < 0xff; i++)
        {
            if (ports[i] == 0)
                continue;
            cout << "%" << i << ": " << ports[i] << "\n";
        }
        cout << "IP: " << ip << "\n";
    }
    void start()
    {
        volatile u1 r = false;
        while (!r)
        {
            r = step();
            if (r)
                break;
        }
    }
};

std::vector<uint8_t> networkSerialize(std::vector<uint16_t> input) {
    std::vector<uint8_t> output;
    output.reserve(input.size() * sizeof(uint16_t)); // Pre-allocate for sake of
                                                     // performance
    for(auto snumber : input) {
        output.push_back((snumber & 0xFF00) >> 8); // Extract the MSB
        output.push_back((snumber & 0xFF)); // Extract the LSB
    }
    return output;
}

void writeFileBytes(const char *filename, std::vector<u8> fileBytes)
{
    std::ofstream file(filename, std::ios::out | std::ios::binary);
    std::copy(fileBytes.cbegin(), fileBytes.cend(),
              std::ostream_iterator<unsigned char>(file));
}

int main(int argc, char **argv)
{
    if (argc < 3)
    {
        cout << "Usage: " << argv[0] << " <compiled code>\n";
        return 1;
    }
    if (argv[1][0] == '-')
    {
        if (argv[1][1] == 'r')
        {
            EMU urcl;
            urcl.loadRAM(argv[2]);
            //urcl.loadDrive(argv[3]);
            urcl.start();
            urcl.debug();
            //vector<u8> drive_unloaded = networkSerialize(urcl.drive);
            //cout << drive_unloaded.size() << "\n";
            //for(int i = 0; i < drive_unloaded.size(); i++) {
            //    cout << (int)drive_unloaded[i] << " ";
            //}
            //writeFileBytes(argv[3], drive_unloaded);
        }
    }
}