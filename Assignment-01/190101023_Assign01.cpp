/*
                Assignment-01
Name : Bellamkonda Kartheek
Rollno : 190101023
Environment : Windows g++/9.2.0
Running Code:
1. g++ 190101023_Assign01.cpp
2. ./a.exe
*/
#include <iostream>
#include <fstream>
#include <string.h>
#include <vector>
#include <map>
#include <sstream>

using namespace std;

void get_details(string &data, string &label, string &opcode, string &operand);              // used to get label opcode and operand separatley from each line in pass1
void get_strings(string &data, string &loc, string &label, string &opcode, string &operand); // used to get loc label opcode and operand separatley from each line in pass1
void pass1();
void pass2();
int d2hex(string operand);            // used to get decimal value frm hexa format
string get_machcode(string &operand); // to get machcode for operand
int lenofconst(string &operand);      // returns the length of constant when encountered a BYTE opcode
string converttohex(int locctr);      // used to convert integer to hexadecimal string

int LOCCTR, programlength, startaddr; // To store Location counter , programlength and startaddr
bool comment = 0;                     // to check whether line is a comment or not
map<string, string> optab;            // operation table to store opcodes
map<string, int> symtab;              // symbol table to store label locctr
string curloc, programname;

int main()
{
    optab["LDA"] = "00";
    optab["LDX"] = "04";
    optab["LDL"] = "08";
    optab["STA"] = "0C";
    optab["STX"] = "10";
    optab["STL"] = "14";
    optab["LDCH"] = "50";
    optab["STCH"] = "54";
    optab["ADD"] = "18";
    optab["SUB"] = "1C";
    optab["MUL"] = "20";
    optab["DIV"] = "24";
    optab["COMP"] = "28";
    optab["J"] = "3C";
    optab["JLT"] = "38";
    optab["JEQ"] = "30";
    optab["JGT"] = "34";
    optab["JSUB"] = "48";
    optab["RSUB"] = "4C";
    optab["TIX"] = "2C";
    optab["TD"] = "E0";
    optab["RD"] = "D8";
    optab["WD"] = "DC";
    pass1();
    pass2();
}

void pass1()
{
    /*
    Adding locctr to the instruction in pass1 and generating middlefile to be used in pass2
    */
    ifstream inputfile;
    inputfile.open("input.txt");

    ofstream middlefile;
    middlefile.open("middle.txt");

    string data;
    string label, opcode, operand;
    getline(inputfile, data);
    get_details(data, label, opcode, operand);
    curloc = operand;
    programname = label;
    if (opcode == "START")
    {
        LOCCTR = d2hex(operand);
        startaddr = LOCCTR;
        data = operand + "    " + data;
        middlefile << data << endl;
    }
    else
    {
        LOCCTR = 0;
    }

    while (getline(inputfile, data))
    {
        label = "";
        opcode = "";
        operand = "";
        get_details(data, label, opcode, operand);
        if (opcode != "END")
        {
            if (comment)
            {
                middlefile << data << endl;
                comment = 0;
            }
            else
            {
                if (label != "")
                {
                    if (symtab.find(label) != symtab.end())
                    {
                        cout << "MULTIPLE WITH SAME LABELS FOUND" << endl;
                    }
                    else
                    {
                        symtab.insert({label, LOCCTR});
                    }
                }
                string loc;
                loc = converttohex(LOCCTR);
                middlefile << loc << "    " << data << endl;
                if (optab.find(opcode) != optab.end())
                {
                    LOCCTR += 3;
                }
                else if (opcode == "WORD")
                {
                    LOCCTR += 3;
                }
                else if (opcode == "RESW")
                {
                    LOCCTR += 3 * stoi(operand);
                }
                else if (opcode == "RESB")
                {
                    LOCCTR += stoi(operand);
                }
                else if (opcode == "BYTE")
                {
                    LOCCTR += lenofconst(operand);
                }
            }
        }
        else
        {
            middlefile << "    "
                       << "    " << data << endl;
        }
    }
    programlength = LOCCTR - startaddr;
    inputfile.close();
    middlefile.close();
    cout << "Pass1 Completed succesfully" << endl;
}

void pass2()
{
    /*
    Taking the generated middlefile and generating final output and object code and readable object code with carets in pass2
    */
    ifstream middlefile;
    middlefile.open("middle.txt");
    string readable;
    readable = " ^     ^     ^";

    ofstream outputfile, objectfile, readableobjfile;
    outputfile.open("output.txt");
    objectfile.open("object.o");
    readableobjfile.open("objectwithcaret.txt");

    string data, codeobj;
    string loc, label, opcode, operand;
    string header;
    header += "H";
    string tempy = programname;
    while (tempy.size() < 6)
    {
        tempy = tempy + " ";
    }
    header += tempy;
    tempy = converttohex(startaddr);
    while (tempy.size() < 6)
    {
        tempy = "0" + tempy;
    }
    header += tempy;
    tempy = converttohex(programlength);
    while (tempy.size() < 6)
    {
        tempy = "0" + tempy;
    }
    header += tempy;
    objectfile << header << endl;
    readableobjfile << header << endl;
    readableobjfile << readable << endl;
    readable = " ^     ^ ";
    while (getline(middlefile, data))
    {
        loc = "";
        label = "";
        opcode = "";
        operand = "";
        get_strings(data, loc, label, opcode, operand);
        // cout<<loc<<"\t"<<label<<"\t"<<opcode<<"\t"<<operand<<endl;
        if (comment)
        {
            outputfile << data << endl;
            comment = 0;
        }
        else
        {
            if (opcode == "START" || opcode == "END")
            {
                outputfile << data << endl;
                continue;
            }
            string machcode = "";
            int machoperand = 0;
            string machopcode = "";
            if (optab.find(opcode) != optab.end())
            {
                machopcode = optab[opcode];
                if (operand != "")
                {
                    if (symtab.find(operand) != symtab.end())
                    {
                        machoperand = symtab[operand];
                        machcode = machopcode + converttohex(machoperand);
                    }
                    else if (operand[operand.size() - 1] == 'X' && operand[operand.size() - 2] == ',')
                    {
                        operand.pop_back();
                        operand.pop_back();
                        machoperand = symtab[operand];
                        machcode = machopcode + converttohex(machoperand + d2hex("8000"));
                    }
                }
                else
                {
                    machcode = machopcode + "0000";
                }
            }
            else if (opcode == "BYTE" || opcode == "WORD")
            {
                machcode = get_machcode(operand);
            }
            while (data.size() <= 36)
            {
                data += " ";
            }
            outputfile << data << machcode << endl;
            int a = d2hex(loc);
            int b = d2hex(curloc);
            /* Text Record starts with T next 6 columns are starting LOCCTR then 2 ccolumns are length of text record
                then we can have object code upto 60 length if it exceeds we open a new text record
            */
            if (codeobj.size() + machcode.size() > 60 || a - b > 27)
            {
                string temp = "";
                temp += "T";
                while (curloc.size() < 6)
                {
                    curloc = "0" + curloc;
                }
                temp += curloc;
                int llen = codeobj.size();
                temp += converttohex(llen / 2);
                temp += codeobj;
                objectfile << temp << endl;
                readableobjfile << temp << endl;
                readableobjfile << readable << endl;
                readable = " ^     ^ ";
                codeobj = "";
                codeobj += machcode;
                int lengthy = machcode.size() - 1;
                readable += "^";
                while (lengthy--)
                {
                    readable += " ";
                }
                curloc = loc;
            }
            else
            {
                codeobj += machcode;
                if (machcode != "")
                {
                    int lengthy = machcode.size() - 1;
                    readable += "^";
                    while (lengthy--)
                    {
                        readable += " ";
                    }
                }
            }
        }
    }
    string temp = "";
    temp += "T";
    while (curloc.size() < 6)
    {
        curloc = "0" + curloc;
    }
    temp += curloc;
    int llen = codeobj.size();
    tempy = converttohex(llen / 2);
    while (tempy.size() < 2)
    {
        tempy = "0" + tempy;
    }
    temp += tempy;
    temp += codeobj;
    objectfile << temp << endl;
    readableobjfile << temp << endl;
    readableobjfile << readable << endl;
    string footer = "";
    footer += "E";
    tempy = converttohex(startaddr);
    while (tempy.size() < 6)
    {
        tempy = "0" + tempy;
    }
    footer += tempy;
    readable = " ^";
    objectfile << footer << endl;
    readableobjfile << footer << endl;
    readableobjfile << readable << endl;
    middlefile.close();
    outputfile.close();
    objectfile.close();
    cout << "Pass2 Completed succesfully" << endl;
}

string get_machcode(string &operand)
{
    string ans = "";
    if ((operand[0] == 'X' || operand[0] == 'x') && operand[1] == '\'')
    {
        int len = operand.size();
        for (int i = 2; i < len && operand[i] != '\''; i++)
        {
            ans.push_back(operand[i]);
        }
        return ans;
    }
    if ((operand[0] == 'C' || operand[0] == 'c') && operand[1] == '\'')
    {
        int len = operand.size();
        for (int i = 2; i < len && operand[i] != '\''; i++)
        {
            int temp = (int)operand[i];
            string s = "";
            s = converttohex(temp);
            ans += s;
        }
        return ans;
    }
    int temp = stoi(operand);
    ans = converttohex(temp);
    while (ans.size() < 6)
    {
        ans = "0" + ans;
    }
    return ans;
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

void get_details(string &data, string &label, string &opcode, string &operand)
{
    int len = data.length();
    int cur = 0;
    if (data[0] == '.')
    {
        comment = 1;
        return;
    }
    if (data[0] == ' ')
    {
        label = "";
    }
    else
    {
        while (cur < len && data[cur] != ' ')
        {
            label += data[cur];
            cur++;
        }
    }
    while (cur < len && data[cur] == ' ')
    {
        cur++;
    }
    while (cur < len && data[cur] != ' ')
    {
        opcode += data[cur];
        cur++;
    }
    while (cur < len && data[cur] == ' ')
    {
        cur++;
    }
    while (cur < len && cur != len)
    {
        operand += data[cur];
        cur++;
    }
    return;
}

void get_strings(string &data, string &loc, string &label, string &opcode, string &operand)
{
    int len = data.length();
    int cur = 0;
    if (data[0] == '.')
    {
        comment = 1;
        return;
    }
    while (cur < len && data[cur] != ' ')
    {
        loc += data[cur];
        cur++;
    }
    cur = 8;
    if (data[cur] == ' ')
    {
        label = "";
    }
    else
    {
        while (cur < len && data[cur] != ' ')
        {
            label += data[cur];
            cur++;
        }
    }
    while (cur < len && data[cur] == ' ')
    {
        cur++;
    }
    while (cur < len && data[cur] != ' ')
    {
        opcode += data[cur];
        cur++;
    }
    while (cur < len && data[cur] == ' ')
    {
        cur++;
    }
    while (cur < len && cur != len)
    {
        operand += data[cur];
        cur++;
    }
    return;
}

int lenofconst(string &operand)
{
    string temp;
    bool ok = 0;
    int i = 0;
    temp = operand;
    if ((temp[0] == 'C' || temp[0] == 'c') && temp[1] == '\'')
    {
        for (i = 2; i <temp.size(); i++)
        {
            if (temp[i] == '\'')
            {
                ok=1;
                break;
            }
        }
    }
    if(ok){
        return i-2;
    }
    if ((temp[0] == 'X' || temp[0] == 'x') && temp[1] == '\'')
    {
        return 1;
    }
    return i;
}

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
    return temp;
}