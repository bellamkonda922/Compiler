/*
                Assignment-02
Name : Bellamkonda Kartheek
Rollno : 190101023
Environment : Windows g++/9.2.0
Running Code:
1. g++ 190101023_linker-loader.cpp
2. ./a.exe
*/
#include <iostream>
#include <fstream>
#include <string.h>
#include <vector>
#include <map>
#include <sstream>

using namespace std;

void pass1(); // pass1 of linker loader
void pass2(); // pass2 of linker loader
// Used to convert integer to a hexadecimal string
string Hex(int num)
{
    map<int, char> m;
    m[0] = '0';
    m[1] = '1';
    m[2] = '2';
    m[3] = '3';
    m[4] = '4';
    m[5] = '5';
    m[6] = '6';
    m[7] = '7';
    m[8] = '8';
    m[9] = '9';
    m[10] = 'A';
    m[11] = 'B';
    m[12] = 'C';
    m[13] = 'D';
    m[14] = 'E';
    m[15] = 'F';
    string res = "";
    int cnt = 0;
    if (num==0)
    {
        return "0";
    }

    if (num > 0)
    {
        while (num)
        {
            res = m[num % 16] + res;
            cnt++;
            num /= 16;
        }
    }

    else
    {

        unsigned int n = num;

        while (n)
        {
            res = m[n % 16] + res;
            cnt++;
            n /= 16;
        }
    }

    return res;
}

int d2hex(string operand);                                                     // Given a string in hexidecimal format converts it to
void get_details(string data, string &progname, string &addr, string &length); // used to get details from header
string make6(string s)
{
    while (s.size() < 4)
    {
        s = "0" + s;
    }
    return s;
}
vector<pair<string, string>> getrecords(string data); // get all details from modification records
long int neg = (long int)0xFFFFFFFF000000;
int progaddr = 0, csaddr = 0, cslength = 0, cursection = -1, error = 0, execaddr = 0;
string progname = "";
// used to store address of external symbols
struct exttable
{
    string symbol;
    int controlsection;
    int address;
};
vector<exttable> esttab;

// used to store objcode at a location
map<int, string> objcode;

// used to store details of a modification record
struct modrecord
{
    int value;
    int length;
    string label;
    string op;
    int controlsection;
};
// convert to hexadecimal string given a integer
string converttohex(int locctr)
{
    string ans = "";
    while (locctr)
    {
        int hex = locctr % 16;
        if (hex < 10)
        {
            char c = '0';
            c += hex;
            ans.push_back(c);
        }
        else
        {
            char c = 'A';
            c += hex - 10;
            ans.push_back(c);
        }
        locctr = locctr / 16;
    }
    string temp = "";
    int len = ans.size();
    for (int i = len - 1; i >= 0; i--)
    {
        temp.push_back(ans[i]);
    }
    while (temp.size() < 4)
    {
        temp = "0" + temp;
    }
    return temp;
}
vector<modrecord> modifyrecord;
void getmodrecord(string data, modrecord &mod); // Given line get all modification records
int main()
{
    objcode.clear();
    pass1();
    pass2();
    // for(auto obj:objcode){
    //     cout<<obj.first<<" "<<obj.second<<endl;
    // }
}

void pass1()
{
    ifstream inputfile;
    inputfile.open("linker_loader_input.txt");
    string addr = "", length = "";
    cout << "ProgAddr has been set to 0x4000 at start of pass1" << endl;
    // cin>>progaddr;
    string programadress = "4000";
    progaddr = d2hex(programadress);
    csaddr = progaddr;
    string data = "";
    while (getline(inputfile, data))
    {
        // getline(inputfile,data);
        if (data[0] == 'H')
        {
            cursection++;
            progname = "";
            addr = "";
            length = "";
            get_details(data, progname, addr, length);
            cslength = d2hex(length);
            bool estabfound = 0;
            for (auto es : esttab)
            {
                if (es.symbol == progname)
                {
                    estabfound = 1;
                    break;
                }
            }
            if (estabfound)
            {
                error = 1;
                cout << "ERROR" << endl;
                return;
            }
            else
            {
                exttable es;
                es.symbol = progname;
                es.address = csaddr;
                es.controlsection = cursection;
                esttab.push_back(es);
            }
            while (data[0] != 'E')
            {
                getline(inputfile, data);
                // handling Define RECORDS
                if (data[0] == 'D')
                {
                    vector<pair<string, string>> records;
                    records = getrecords(data);
                    for (auto rec : records)
                    {
                        bool estabfound = 0;
                        for (auto es : esttab)
                        {
                            if (es.symbol == rec.first)
                            {
                                estabfound = 1;
                                break;
                            }
                        }
                        if (estabfound)
                        {
                            error = 1;
                            cout << "ERROR" << endl;
                            return;
                        }
                        else
                        {
                            exttable es;
                            es.symbol = rec.first;
                            es.address = csaddr + d2hex(rec.second);
                            es.controlsection = cursection;
                            esttab.push_back(es);
                        }
                    }
                }
            }
            csaddr += cslength;
        }
    }
    inputfile.close();

    cout << "Pass1 COmpleted Succesfully" << endl;
}

void pass2()
{
    ifstream inputfile;
    inputfile.open("linker_loader_input.txt");
    ofstream outputfile;
    outputfile.open("linker_loader_output.txt");
    csaddr = progaddr;
    execaddr = progaddr;
    string addr = "", length = "", data = "";
    int prevaddr = 0, curaddr = 0, startaddr = 0;
    cursection = -1;
    while (getline(inputfile, data))
    {
        // getline(inputfile,data);
        if (data[0] == 'H')
        {
            cursection++;
            progname = "";
            addr = "";
            length = "";
            get_details(data, progname, addr, length);
            cslength = d2hex(length);
            prevaddr = d2hex(addr) + csaddr;
            while (data[0] != 'E')
            {
                getline(inputfile, data);
                if (data[0] == 'T')
                {
                    string temp;
                    int reclength = 0;
                    temp = data.substr(1, 6);
                    startaddr = d2hex(temp) + csaddr;
                    curaddr = prevaddr;
                    temp = data.substr(7, 8);
                    reclength = d2hex(temp);
                    for (int i = 0; i < startaddr - prevaddr; i++)
                    {
                        objcode[curaddr] = "..";
                        curaddr++;
                    }
                    curaddr = startaddr;
                    int j = 9;
                    while (j < data.size() - 1)
                    {
                        string s = "";
                        s += data[j];
                        s += data[j + 1];
                        objcode[curaddr] = s;
                        curaddr++;
                        j += 2;
                    }
                    prevaddr = curaddr;
                }
                else if (data[0] == 'M')
                {
                    modrecord mod;
                    getmodrecord(data, mod);
                    modifyrecord.push_back(mod);
                }
            }
            if (data[0] == 'E' && data.size() > 1)
            {
                data = data.substr(1, data.size() - 1);
                execaddr = csaddr + d2hex(data);
            }
            csaddr += cslength;
        }
    }
    for (int i = 0; i < 29; i++)
    {
        objcode[curaddr] = "xx";
        curaddr++;
    }
    // handling modification records
    for (auto mod : modifyrecord)
    {

        int curloc = mod.value;
        int startloc = curloc;
        int val = 0;
        for (auto es : esttab)
        {
            if (es.symbol == mod.label)
            {
                val = es.address;
                break;
            }
        }
        string addr = "";
        long int newaddr = 0;
        for (int i = 0; i < mod.length; i += 2)
        {
            addr += objcode[curloc];
            curloc++;
        }
        if (addr[0] == 'F')
        {
            newaddr = neg;
        }
        if (mod.op == "-")
        {
            newaddr += (long int)d2hex(addr) - (long int)val;
        }
        else if (mod.op == "+")
        {
            newaddr += (long int)d2hex(addr) + (long int)val;
        }

        string s = Hex((int)newaddr);
        while (s.size() < 6)
        {
            s = "0" + s;
        }
        if (s.size() > 6 && s[0] == 'F' && s[1] == 'F')
        {
            addr = s.substr(2, s.size() - 2);
        }
        else
        {
            addr = s;
        }
        for (int i = 0; i < addr.size(); i += 2)
        {
            string s = "";
            s += addr[i];
            s += addr[i + 1];
            objcode[startloc] = s;
            startloc++;
        }
    }
    int recordaddr = progaddr;
    // printing into output file
    if (recordaddr >= 32)
    {
        outputfile << "0000"
                   << "\t";
        for (int i = 0; i < 4; i++)
        {
            for (int j = 0; j < 4; j++)
            {
                outputfile << "xx";
            }
            outputfile << "\t";
        }
        outputfile << endl;
        for (int i = 0; i < 4; i++)
        {
            outputfile << "  .  "
                       << "\t"
                       << "    .    "
                       << "\t"
                       << "    .    "
                       << "\t"
                       << "    .    "
                       << "\t"
                       << "    .    "
                       << "\t" << endl;
        }
    }
    if (recordaddr >= 16)
    {
        outputfile << make6(Hex(recordaddr - 16)) << "\t";
        for (int i = 0; i < 4; i++)
        {
            for (int j = 0; j < 4; j++)
            {
                outputfile << "xx";
            }
            outputfile << "\t";
        }
        outputfile << endl;
    }

    while (true)
    {
        if (objcode.find(recordaddr) == objcode.end())
        {
            break;
        }
        outputfile << make6(Hex(recordaddr)) << "\t";
        for (int i = 0; i < 4; i++)
        {
            for (int j = 0; j < 4; j++)
            {
                outputfile << objcode[recordaddr];
                recordaddr++;
            }
            outputfile << "\t";
        }
        outputfile << endl;
    }
    inputfile.close();
    outputfile.close();
    cout << "Pass 2 Completed Succesfully" << endl;
}

void getmodrecord(string data, modrecord &mod)
{
    data = data.substr(1, data.size() - 1);
    string temp = "";
    for (int i = 0; i < 6; i++)
    {
        temp += data[i];
    }

    mod.controlsection = cursection;
    mod.value = d2hex(temp) + csaddr;
    temp = "";
    for (int i = 6; i < 8; i++)
    {
        temp += data[i];
    }

    mod.length = d2hex(temp);
    mod.op = data[8];
    temp = "";
    for (int i = 9; i < data.size(); i++)
    {
        temp += data[i];
    }
    mod.label = temp;
}

vector<pair<string, string>> getrecords(string data)
{
    vector<pair<string, string>> ans;
    data = data.substr(1, data.size() - 1);
    int n = data.size();
    int outindex = 0;
    while (outindex < n)
    {
        string name = "", addr = "";
        for (int i = outindex; i < outindex + 6 && data[i] != ' '; i++)
        {
            name += data[i];
        }
        for (int i = outindex + 6; i < outindex + 12 && data[i] != ' '; i++)
        {
            addr += data[i];
        }
        ans.push_back({name, addr});
        outindex += 12;
    }
    return ans;
}

void get_details(string data, string &progname, string &addr, string &length)
{
    for (int i = 1; i < 7 && data[i] != ' '; i++)
    {
        progname += data[i];
    }
    for (int i = 7; i < 13; i++)
    {
        addr += data[i];
    }
    for (int i = 13; i < 19; i++)
    {
        length += data[i];
    }
}

int d2hex(string operand)
{
    int ans = 0;
    int len = operand.size();
    int cur = 0;
    while (cur < len)
    {
        if (operand[cur] >= '0' && operand[cur] <= '9')
        {
            ans = ans * 16 + (operand[cur] - '0');
        }
        else if (operand[cur] >= 'A' && operand[cur] <= 'Z')
        {
            ans = ans * 16 + 10 + (operand[cur] - 'A');
        }
        else if (operand[cur] >= 'a' && operand[cur] <= 'z')
        {
            ans = ans * 16 + 10 + (operand[cur] - 'a');
        }
        cur++;
    }
    return ans;
}