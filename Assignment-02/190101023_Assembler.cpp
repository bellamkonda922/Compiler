/*
                Assignment-02
Name : Bellamkonda Kartheek
Rollno : 190101023
Environment : Windows g++/9.2.0
Running Code:
1. g++ 190101023_Assembler.cpp
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
string getval(string k);
string modreadable = " ^     ^ ^";
int format(string opcode);                   // to get the format of opcode
vector<string> parseStrings(string operand); // to get all values in a string in modrecords
int evalliteral(string str);                 // used to evaluate literal
string converttohex(int loc);                // convert into hex string
void evaluateexpress(string operand, int &value, bool &isabsolute, bool &referenced);
int d2hex(string operand);            // used to get decimal value frm hexa format
string get_machcode(string &operand); // to get machcode for operand
int lenofconst(string &operand);      // returns the length of constant when encountered a BYTE opcode
string converttohex(int locctr);
string make6(string s);                     // make string size to 6
void searchext(string operand, bool isdef); // search in external table
map<string, int> format3, format4;
bool comment = 0, isextended = 0, isliteral = 0; // to check whether line is a comment or not
bool symtabfound = 0;
bool littabfound = 0;
bool optabfound = 0;
map<string, string> optab; // operation table to store opcodes
map<string, int> reg;
int cursection = 0;

struct symbol_table
{
    string symbol;
    int addr;
    bool type;
    int controlsection;
};

vector<symbol_table> symtab;

struct extsymbtab
{
    string symbol;
    int controlsection;
    bool type;
};

vector<extsymbtab> exttab;

struct literal
{
    string name;
    int val;
    int len;
    int addr;
};

vector<literal> littab;

struct locationtab
{
    string sectionname;
    int loc;
    int startaddr;
    int len;
};

vector<locationtab> loctab(20);

string generate(string operand, string loc)
{
    int sign = 1;
    int i = 0;
    string temp = "";
    string modrecord = "M";
    if (operand[i] == '-')
    {
        sign = -1;
        i++;
    }
    else if (operand[i] == '+')
    {
        i++;
    }
    while (i < operand.size())
    {
        if (operand[i] == '-' || operand[i] == '+')
        {
            for (auto sym : symtab)
            {
                if (sym.symbol == temp && sym.controlsection != cursection)
                {
                    modrecord += make6(loc);
                    modrecord += "06";
                    if (sign == -1)
                    {
                        modrecord += "-";
                    }
                    else
                    {
                        modrecord += "+";
                    }
                    modrecord += temp + "\n" + "M";
                }
            }
            temp = "";
            if (operand[i] == '-')
            {
                sign = -1;
            }
        }
        else
        {
            temp += operand[i];
        }
        i++;
    }

    for (auto sym : symtab)
    {
        if (sym.symbol == temp && sym.controlsection != cursection)
        {
            modrecord += make6(loc);
            modrecord += "06";
            if (sign == -1)
            {
                modrecord += "-";
            }
            else
            {
                modrecord += "+";
            }
            modrecord += temp;
        }
    }
    return modrecord;
}

string generatecaret(string operand, string loc)
{
    int sign = 1;
    int i = 0;
    string temp = "";
    string modrecord = "M";
    if (operand[i] == '-')
    {
        sign = -1;
        i++;
    }
    else if (operand[i] == '+')
    {
        i++;
    }
    while (i < operand.size())
    {
        if (operand[i] == '-' || operand[i] == '+')
        {
            for (auto sym : symtab)
            {
                if (sym.symbol == temp && sym.controlsection != cursection)
                {
                    modrecord += make6(loc);
                    modrecord += "06";
                    if (sign == -1)
                    {
                        modrecord += "-";
                    }
                    else
                    {
                        modrecord += "+";
                    }
                    modrecord += temp + "\n" + modreadable + "M";
                }
            }
            temp = "";
            if (operand[i] == '-')
            {
                sign = -1;
            }
        }
        else
        {
            temp += operand[i];
        }
        i++;
    }

    for (auto sym : symtab)
    {
        if (sym.symbol == temp && sym.controlsection != cursection)
        {
            modrecord += make6(loc);
            modrecord += "06";
            if (sign == -1)
            {
                modrecord += "-";
            }
            else
            {
                modrecord += "+";
            }
            modrecord += temp;
        }
    }
    return modrecord;
}

int main()
{

    reg["A"] = 0;
    reg["X"] = 1;
    reg["L"] = 2;
    reg["B"] = 3;
    reg["S"] = 4;
    reg["T"] = 5;
    reg["F"] = 6;

    optab["LDA"] = "00";
    optab["LDX"] = "04";
    optab["LDL"] = "08";
    optab["LDB"] = "68";
    optab["LDT"] = "74";
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
    optab["COMPR"] = "A0";
    optab["CLEAR"] = "B4";
    optab["J"] = "3C";
    optab["JLT"] = "38";
    optab["JEQ"] = "30";
    optab["JGT"] = "34";
    optab["JSUB"] = "48";
    optab["RSUB"] = "4C";
    optab["TIX"] = "2C";
    optab["TIXR"] = "B8";
    optab["TD"] = "E0";
    optab["RD"] = "D8";
    optab["WD"] = "DC";

    format3["n"] = 1 << 17;
    format3["i"] = 1 << 16;
    format3["x"] = 1 << 15;
    format3["b"] = 1 << 14;
    format3["p"] = 1 << 13;
    format3["e"] = 1 << 12;
    format3["neg"] = (1 << 12) - 1;
    format4["n"] = 1 << 25;
    format4["i"] = 1 << 24;
    format4["x"] = 1 << 23;
    format4["b"] = 1 << 22;
    format4["p"] = 1 << 21;
    format4["e"] = 1 << 20;
    format4["neg"] = (1 << 20) - 1;
    pass1();

    pass2();
}

void pass1()
{
    ifstream inputfile;
    inputfile.open("input.txt");
    ofstream middlefile;
    middlefile.open("middle.txt");

    string data;
    string label, opcode, operand;
    getline(inputfile, data);
    get_details(data, label, opcode, operand);

    if (opcode == "START")
    {
        loctab[cursection].startaddr = d2hex(operand);
        loctab[cursection].loc = loctab[cursection].startaddr;
        loctab[cursection].sectionname = label;
        data = converttohex(d2hex(operand)) + "    " + data;
        middlefile << data << endl;
    }
    else
    {
        loctab[cursection].startaddr = 0;
        loctab[cursection].loc = 0;
    }

    while (getline(inputfile, data))
    {
        label = "";
        opcode = "";
        operand = "";
        if (data == "")
        {
            middlefile << data << endl;
            continue;
        }
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
                if (opcode == "EQU")
                {
                    int value = 0;
                    bool referenced = false;
                    bool isabsolute = false;
                    evaluateexpress(operand, value, isabsolute, referenced);
                    symtabfound = 0;
                    for (auto sym : symtab)
                    {
                        if (sym.symbol == label && sym.controlsection == cursection)
                        {
                            symtabfound = 1;
                            break;
                        }
                    }
                    if (!symtabfound)
                    {
                        symbol_table sye;
                        sye.symbol = label;
                        sye.addr = loctab[cursection].loc;
                        sye.type = 0;
                        sye.controlsection = cursection;
                        symtab.push_back(sye);
                    }
                    symtab[symtab.size() - 1].addr = value;
                    symtab[symtab.size() - 1].type = isabsolute;
                    middlefile << converttohex(value) << "    " << data << endl;
                }
                else if (opcode == "CSECT")
                {

                    loctab[cursection].len = loctab[cursection].loc - loctab[cursection].startaddr;
                    cursection++;
                    loctab[cursection].sectionname = label;
                    loctab[cursection].loc = 0;
                    loctab[cursection].startaddr = 0;
                    middlefile << converttohex(loctab[cursection].loc) << "    " << data << endl;
                }
                else
                {
                    if (label != "")
                    {
                        symtabfound = 0;
                        for (auto sym : symtab)
                        {
                            if (sym.symbol == label && sym.controlsection == cursection)
                            {
                                symtabfound = 1;
                                break;
                            }
                        }
                        if (!symtabfound)
                        {
                            symbol_table sye;
                            sye.symbol = label;
                            sye.addr = loctab[cursection].loc;
                            sye.type = 0;
                            sye.controlsection = cursection;
                            symtab.push_back(sye);
                        }
                    }
                    if (opcode[0] == '+')
                    {
                        isextended = true;
                        int n = opcode.size();
                        opcode = opcode.substr(1, n);
                    }
                    else
                    {
                        isextended = false;
                    }
                    optabfound = (optab.find(opcode) != optab.end());
                    isliteral = false;
                    if (operand[0] == '=')
                    {
                        isliteral = true;
                        littabfound = 0;
                        for (auto lit : littab)
                        {
                            if (lit.name == operand)
                            {
                                littabfound = 1;
                                break;
                            }
                        }
                        if (littabfound == 0)
                        {
                            string str = operand.substr(1, operand.size());
                            literal lite;
                            lite.val = evalliteral(str);
                            lite.addr = -1;
                            lite.len = lenofconst(str);
                            lite.name = operand;
                            littab.push_back(lite);
                        }
                    }
                    if (opcode == "EXTDEF")
                    {
                        searchext(operand, true);
                        middlefile << "    "
                                   << "    " << data << endl;
                    }
                    else if (opcode == "EXTREF")
                    {
                        searchext(operand, false);
                        middlefile << "    "
                                   << "    " << data << endl;
                    }
                    else if (opcode == "LTORG")
                    {
                        middlefile << "\t\t" << data << endl;
                        // handle
                        for (int i = 0; i < littab.size(); i++)
                        {
                            if (littab[i].addr == -1)
                            {
                                littab[i].addr = loctab[cursection].loc;
                                middlefile << converttohex(loctab[cursection].loc) << "    "
                                           << "*"
                                           << "      " << littab[i].name << endl;
                                string str = littab[i].name.substr(1, littab[i].name.size());
                                loctab[cursection].loc += lenofconst(str);
                            }
                        }
                    }
                    else
                    {
                        middlefile << converttohex(loctab[cursection].loc) << "    " << data << endl;
                    }
                }
                // increase_loc
                if (optabfound && ((opcode == "COMPR") || (opcode == "CLEAR") || (opcode == "TIXR")))
                {
                    loctab[cursection].loc += 2;
                }
                else if (optabfound && isextended)
                {
                    loctab[cursection].loc += 4;
                }
                else if (optabfound && !isextended)
                {
                    loctab[cursection].loc += 3;
                }
                else if (opcode == "WORD")
                {
                    loctab[cursection].loc += 3;
                }
                else if (opcode == "RESW")
                {
                    loctab[cursection].loc += 3 * stoi(operand);
                }
                else if (opcode == "RESB")
                {
                    loctab[cursection].loc += stoi(operand);
                }
                else if (opcode == "BYTE")
                {
                    loctab[cursection].loc += lenofconst(operand);
                }
            }
        }
        else
        {
            middlefile << "    "
                       << "    " << data << endl;
        }
    }
    // handle
    for (int i = 0; i < littab.size(); i++)
    {
        if (littab[i].addr == -1)
        {
            littab[i].addr = loctab[cursection].loc;
            middlefile << converttohex(loctab[cursection].loc) << "    "
                       << "*"
                       << "      " << littab[i].name << endl;
            string str = littab[i].name.substr(1, littab[i].name.size());

            loctab[cursection].loc += lenofconst(str);
        }
    }
    loctab[cursection].len = loctab[cursection].loc - loctab[cursection].startaddr;
    inputfile.close();
    middlefile.close();
    cout << "Pass1 Completed succesfully" << endl;
}

string make6(string s)
{
    while (s.size() < 6)
    {
        s = "0" + s;
    }
    return s;
}
void pass2()
{
    ifstream middlefile;
    middlefile.open("middle.txt");
    cursection = 0;
    ofstream outputfile, objectfile, caretfile;
    outputfile.open("output.txt");
    objectfile.open("object.txt");
    caretfile.open("objectwithcaret.txt");

    string readable = "";

    modreadable += "\n";
    string data, codeobj;
    string globalmod = "";
    string globalmodcaret = "";
    ;
    string loc, label, opcode, operand;
    int startloc = 0;

    string header = "";
    header += "H";
    string headercaret = " ^     ^     ^     ";
    getline(middlefile, data);
    get_strings(data, loc, label, opcode, operand);
    startloc = d2hex(loc);
    if (opcode == "START")
    {
        header += loctab[cursection].sectionname;
        while (header.size() < 7)
        {
            header += " ";
        }
        header += make6(converttohex(loctab[cursection].startaddr));
        header += make6(converttohex(loctab[cursection].len));
        outputfile << data << endl;
        objectfile << header << endl;
        caretfile << header << endl;
        caretfile << headercaret << endl;
    }
    int objcode = 0;
    string objcodestr = "";
    string textrecord = "";
    cursection = 0;
    while (getline(middlefile, data))
    {
        loc = "";
        label = "";
        opcode = "";
        operand = "";
        objcode = 0;
        objcodestr = "";
        if (data == "")
        {
            outputfile << data << endl;
            continue;
        }
        get_strings(data, loc, label, opcode, operand);
        if (comment)
        {
            outputfile << data << endl;
            comment = 0;
        }
        else
        {
            if (label == "*")
            {
                bool littabfound = 0;
                for (auto lit : littab)
                {
                    if (lit.name == opcode)
                    {
                        littabfound = 1;
                        objcodestr = getval(lit.name);
                        break;
                    }
                }
                while (data.size() < 37)
                {
                    data += " ";
                }
                outputfile << data << "\t\t" << objcodestr << endl;
            }
            else if (format(opcode) == 2)
            {
                objcode = d2hex(optab[opcode]);
                objcode <<= 8;
                string s = "";
                s += operand[0];
                objcode = objcode + (reg[s] << 4);
                if (operand[1] == ',')
                {
                    s = "";
                    s += operand[2];
                    objcode = objcode + reg[s];
                }
                objcodestr = converttohex(objcode);
                while (data.size() < 37)
                {
                    data += " ";
                }
                outputfile << data << "\t\t" << objcodestr << endl;
            }
            else if (format(opcode) == 3)
            {
                objcode = d2hex(optab[opcode]);
                objcode <<= 16;
                if (operand == "")
                {
                    objcode += format3["i"] + format3["n"];
                }
                else
                {
                    if (operand[0] == '#')
                    {
                        operand = operand.substr(1, operand.size() - 1);
                        bool symtabfound = 0;
                        for (auto sym : symtab)
                        {
                            if (sym.symbol == operand && sym.controlsection == cursection)
                            {
                                symtabfound = 1;
                                objcode = sym.addr;
                                break;
                            }
                        }
                        if (symtabfound)
                        {
                            objcode = objcode - (d2hex(loc) + 3) + format3["i"] + format3["p"];
                        }
                        else
                        {
                            objcode += d2hex(operand) + format3["i"];
                        }
                    }
                    else if (operand[0] == '@')
                    {
                        operand = operand.substr(1, operand.size() - 1);
                        bool symtabfound = 0;
                        for (auto sym : symtab)
                        {
                            if (sym.symbol == operand && sym.controlsection == cursection)
                            {
                                symtabfound = 1;
                                objcode += (sym.addr - (d2hex(loc) + 3)) & format3["neg"];
                                break;
                            }
                        }
                        if (symtabfound)
                        {
                            objcode += format3["p"] + format3["n"];
                        }
                        else
                        {
                            objcode += d2hex(operand) + format3["n"];
                        }
                    }
                    else if (operand[0] == '=')
                    {
                        for (auto lit : littab)
                        {
                            if (lit.name == operand)
                            {
                                objcode += (lit.addr - (d2hex(loc) + 3)) & format3["neg"];
                                break;
                            }
                        }
                        objcode += format3["p"] + format3["n"] + format3["i"];
                    }
                    else if (operand[operand.size() - 1] == 'X' && operand[operand.size() - 2] == ',')
                    {
                        operand = operand.substr(0, operand.size() - 3);
                        bool symtabfound = 0;
                        for (auto sym : symtab)
                        {
                            if (sym.symbol == operand && sym.controlsection == cursection)
                            {
                                objcode += (sym.addr - (d2hex(loc) + 3)) & format3["neg"];
                                break;
                            }
                        }
                        objcode += format3["n"] + format3["i"] + format3["x"] + format3["p"];
                    }
                    else
                    {
                        bool symtabfound = 0;
                        for (auto sym : symtab)
                        {
                            if (sym.symbol == operand && sym.controlsection == cursection)
                            {
                                objcode += (sym.addr - (d2hex(loc) + 3)) & format3["neg"];
                                break;
                            }
                        }
                        objcode += format3["n"] + format3["i"] + format3["p"];
                    }
                }
                objcodestr = converttohex(objcode);
                while (objcodestr.size() < 6)
                {
                    objcodestr = "0" + objcodestr;
                }
                while (data.size() < 37)
                {
                    data += " ";
                }
                outputfile << data << "\t\t" << objcodestr << endl;
            }
            else if (format(opcode) == 4)
            {

                string modrecord = "";
                modrecord += "M";
                modrecord += make6(converttohex(d2hex(loc) + 1));
                modrecord += "05+";

                opcode = opcode.substr(1, opcode.size() - 1);

                objcode = d2hex(optab[opcode]);
                objcode <<= 24;
                objcode = objcode + format4["n"] + format4["i"] + format4["e"];
                if (operand != "")
                {
                    if (operand[operand.size() - 1] == 'X' && operand[operand.size() - 2] == ',')
                    {
                        objcode += format4["x"];
                        operand = operand.substr(0, operand.size() - 2);
                        for (auto sym : symtab)
                        {
                            if (sym.symbol == operand && sym.controlsection == cursection)
                            {
                                objcode = objcode + ((sym.addr - (d2hex(loc) + 3)) & format3["neg"]) & format4["neg"];
                                break;
                            }
                        }
                    }
                    else
                    {
                        bool symtabfound = 0;
                        for (auto sym : symtab)
                        {
                            if (sym.symbol == operand && sym.controlsection == cursection)
                            {
                                objcode += sym.addr;
                                symtabfound = 1;
                                break;
                            }
                        }
                    }
                }
                // objcode+=format4["neg"];
                objcodestr = converttohex(objcode);
                while (data.size() < 37)
                {
                    data += " ";
                }
                modrecord += operand;
                globalmod += modrecord + "\n";
                globalmodcaret += modrecord + "\n" + modreadable;

                outputfile << data << "\t\t" << objcodestr << endl;
            }
            else if (opcode == "CSECT")
            {
                if (textrecord != "")
                {
                    string len = converttohex(textrecord.size() / 2);
                    len = len.substr(2, len.size());
                    readable = " ^     ^ " + readable;
                    textrecord = "T" + make6(converttohex(startloc)) + len + textrecord;
                    objectfile << textrecord << endl;
                    caretfile << textrecord << endl;
                    caretfile << readable << endl;
                    textrecord = "";
                    readable = "";
                }
                globalmod = globalmod.substr(0, globalmod.size() - 1);
                globalmodcaret = globalmodcaret.substr(0, globalmodcaret.size() - 1);
                objectfile << globalmod << endl;
                caretfile << globalmodcaret << endl;
                globalmod = "";
                globalmodcaret = "";
                if (cursection == 0)
                {
                    string s = "E";
                    s += make6(converttohex(loctab[cursection].startaddr));
                    objectfile << s << endl;
                    objectfile << endl;
                    caretfile << s << endl;
                    caretfile << " ^" << endl;
                    caretfile << endl;
                }
                else
                {
                    objectfile << "E" << endl;
                    objectfile << endl;
                    caretfile << "E" << endl;
                    caretfile << endl;
                }
                for (auto loci : loctab)
                {
                    if (loci.sectionname == label)
                    {
                        outputfile << data << endl;
                        loc = loci.startaddr;
                        cursection++;
                    }
                }
                header = "H";
                header += loctab[cursection].sectionname;
                while (header.size() < 7)
                {
                    header += " ";
                }
                header += make6(converttohex(loctab[cursection].startaddr));
                header += make6(converttohex(loctab[cursection].len));
                objectfile << header << endl;
                caretfile << header << endl;
                caretfile << headercaret << endl;
                startloc = loctab[cursection].startaddr;
            }
            else if (opcode == "WORD")
            {
                // void evaluateexpress(string operand, int &value, bool &isabsolute, bool &referenced)
                int value = 0;
                bool isabsolute = false;
                bool referenced = false;
                evaluateexpress(operand, value, isabsolute, referenced);
                if (referenced)
                {
                    string modrecord = generate(operand, loc);
                    string modrecordcaret = generatecaret(operand, loc);
                    globalmod += modrecord + "\n";
                    globalmodcaret += modrecordcaret + "\n" + modreadable;
                }
                objcode += value;
                objcodestr = converttohex(objcode);
                while (objcodestr.size() < 6)
                {
                    objcodestr = "0" + objcodestr;
                }
                while (data.size() < 37)
                {
                    data += " ";
                }
                outputfile << data << "\t\t" << objcodestr << endl;
            }
            else if (opcode == "BYTE")
            {
                objcode = 0;
                if ((operand[0] == 'C' || operand[0] == 'c') && operand[1] == '\'')
                {
                    for (int i = 2; i <= operand.size() - 2; i++)
                    {
                        objcode += (int)operand[i];
                        objcode <<= 8;
                    }
                    objcode >>= 8;
                    objcodestr = converttohex(objcode);
                }
                else if ((operand[0] == 'X' || operand[0] == 'x') && operand[1] == '\'')
                {
                    operand = operand.substr(2, operand.size() - 3);
                    objcode += d2hex(operand);
                    objcodestr = operand;
                }
                while (data.size() < 37)
                {
                    data += " ";
                }
                outputfile << data << "\t\t" << objcodestr << endl;
            }
            else
            {
                if (opcode == "EXTDEF")
                {
                    outputfile << data << endl;
                    string define = "";
                    define += "D";
                    string definecaret = " ";
                    vector<string> vect = parseStrings(operand);
                    for (auto s : vect)
                    {
                        define += s;
                        definecaret += "^     ";
                        for (auto sym : symtab)
                        {
                            if (sym.symbol == s && sym.controlsection == cursection)
                            {
                                define += make6(converttohex(sym.addr));
                                definecaret += "^     ";
                            }
                        }
                    }
                    objectfile << define << endl;
                    caretfile << define << endl;
                    caretfile << definecaret << endl;
                    continue;
                }
                else if (opcode == "EXTREF")
                {
                    string refer = "";
                    string refercaret = " ";
                    refer += "R";
                    vector<string> vect = parseStrings(operand);
                    for (auto s : vect)
                    {
                        while (s.size() < 6)
                        {
                            s += " ";
                        }
                        refer += s;
                        refercaret += "^     ";
                    }
                    objectfile << refer << endl;
                    caretfile << refer << endl;
                    caretfile << refercaret << endl;
                    outputfile << data << endl;
                    continue;
                }
                else
                {
                    outputfile << data << endl;
                }
            }

            if (objcodestr.size() + textrecord.size() > 60 || d2hex(loc) - startloc > 27 || opcode == "LTORG")
            {
                // cout<<data<<(objcodestr.size() + textrecord.size() >60) <<"!!"<< (d2hex(loc) - startloc>27) <<"!!"<< (opcode=="LTORG")<<endl;
                string len = converttohex(textrecord.size() / 2);
                len = len.substr(2, len.size());
                if (textrecord != "")
                {
                    readable = " ^     ^ " + readable;
                    textrecord = "T" + make6(converttohex(startloc)) + len + textrecord;
                    objectfile << textrecord << endl;
                    caretfile << textrecord << endl;
                    caretfile << readable << endl;
                }
                startloc = d2hex(loc);
                readable = "";
                textrecord = objcodestr;
                int n = objcodestr.size() - 1;
                if (n > 0)
                {
                    readable += "^";
                }
                while (n > 0)
                {
                    readable += " ";
                    n--;
                }
            }
            else
            {
                textrecord += objcodestr;

                int n = objcodestr.size() - 1;
                if (n > 0)
                {
                    readable += "^";
                }
                while (n > 0)
                {
                    readable += " ";
                    n--;
                }
            }
        }
    }

    if (textrecord != "")
    {
        string len = converttohex(textrecord.size() / 2);
        len = len.substr(2, len.size());
        readable = " ^     ^ " + readable;
        textrecord = "T" + make6(converttohex(startloc)) + len + textrecord;
        objectfile << textrecord << endl;
        caretfile << textrecord << endl;
        caretfile << readable << endl;
        textrecord = "";
    }
    globalmod = globalmod.substr(0, globalmod.size() - 1);
    globalmodcaret = globalmodcaret.substr(0, globalmodcaret.size() - 1);
    objectfile << globalmod << endl;
    caretfile << globalmodcaret << endl;
    if (cursection == 0)
    {
        string s = "E";
        s += make6(converttohex(loctab[cursection].startaddr));
        objectfile << s << endl;
        objectfile << endl;
        caretfile << s << endl;
        caretfile << " ^" << endl;
        caretfile << endl;
    }
    else
    {
        objectfile << "E" << endl;
        objectfile << endl;
        caretfile << "E" << endl;
        caretfile << endl;
    }
    cout << "Pass2 Completed Succefully" << endl;
}
// saaketh
vector<string> parseStrings(string operand)
{
    vector<string> res;
    string value;
    for (int i = 0; i < operand.size(); ++i)
    {
        if (operand[i] == ',')
        {
            res.push_back(value);
            value = "";
        }
        else
        {
            value += operand[i];
        }
    }
    res.push_back(value);
    return res;
}

void searchext(string operand, bool type)
{
    bool extfound = 0;
    vector<string> records = parseStrings(operand);
    int n = loctab.size();
    for (string rec : records)
    {
        for (int i = 0; i < exttab.size(); ++i)
        {
            if (exttab[i].symbol == rec && exttab[i].controlsection == n)
            {
                extfound = 1;
            }
        }
    }
    if (extfound == 0)
    {
        extsymbtab ex;
        ex.controlsection = n;
        ex.type = type;
        exttab.push_back(ex);
    }
}
// saketh
int evalliteral(string str)
{
    int ans = 0;
    string tmp = "";
    if ((str[0] == 'C' || str[0] == 'c') && str[1] == '\'')
    {
        for (int i = 2; i <= (int)(str.size() - 2); i++)
        {
            ans = ans + (int)str[i];
            ans <<= 8;
        }
        ans >>= 8;
    }
    else if ((str[0] == 'X' || str[0] == 'x') && str[1] == '\'')
    {
        for (int i = 2; i <= (int)(str.size() - 2); i++)
        {
            tmp.push_back(str[i]);
        }
        ans = ans + d2hex(tmp);
    }
    return ans;
}

void evaluateexpress(string operand, int &value, bool &isabsolute, bool &referenced)
{
    int relative = 0;
    bool ok = false;
    string temp = "";
    int i = 0;
    if (operand == "*")
    {
        value = loctab[cursection].loc;
        relative++;
    }
    else
    {
        int sign = 1;
        if (operand[i] == '-')
        {
            sign = -1;
            i++;
        }
        else if (operand[i] == '+')
        {
            i++;
        }
        int n = operand.size();
        while (i < n)
        {
            if (operand[i] == '+' || operand[i] == '-')
            {
                ok = false;
                for (auto sym : symtab)
                {
                    if (sym.symbol == temp)
                    {

                        if (sym.controlsection != cursection)
                        {
                            referenced = true;
                            value = 0;
                            return;
                        }
                        if (sign == -1)
                        {
                            value = value - sym.addr;
                            relative--;
                        }
                        else
                        {
                            value = value + sym.addr;
                            relative++;
                        }
                        ok = true;
                        break;
                    }
                }
                if (!ok)
                {
                    // cout<<temp<<endl;
                    if (sign == -1)
                    {
                        value = value - stoi(temp);
                    }
                    else
                    {
                        value = value + stoi(temp);
                    }
                }
                temp = "";
                if (operand[i] == '-')
                {
                    sign = -1;
                }
            }
            else
            {
                temp.push_back(operand[i]);
            }
            i++;
        }
        ok = false;
        for (auto sym : symtab)
        {

            if (sym.symbol == temp)
            {
                if (sym.controlsection != cursection)
                {
                    referenced = true;
                    break;
                }
                if (sign == -1)
                {
                    value = value - sym.addr;
                    relative--;
                }
                else
                {
                    value = value + sym.addr;
                    relative++;
                }
                ok = true;
                break;
            }
        }
        if (!ok)
        {
            // cout<<temp<<endl;
            if (sign == -1)
            {
                value = value - stoi(temp);
            }
            else
            {
                value = value + stoi(temp);
            }
        }
    }
    if (relative == 0 && referenced == false)
    {
        isabsolute = true;
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
        for (i = 2; i < temp.size(); i++)
        {
            if (temp[i] == '\'')
            {
                ok = 1;
                break;
            }
        }
    }
    if (ok)
    {
        return i - 2;
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
    while (temp.size() < 4)
    {
        temp = "0" + temp;
    }
    return temp;
}

string getval(string k)
{
    k = k.substr(1, k.size() - 1);
    if (k[1] == '\'' && (k[0] == 'C' || k[0] == 'c'))
    {
        // cout<<k<<endl;
        // cout<<evalliteral(k)<<endl;
        return converttohex(evalliteral(k));
    }
    else if (k[1] == '\'' && (k[0] == 'X' || k[0] == 'x'))
    {
        return k.substr(2, k.size() - 3);
    }
    return "";
}
int format(string opcode)
{
    if (opcode == "CLEAR" || opcode == "COMPR" || opcode == "TIXR")
    {
        return 2;
    }
    else if (opcode[0] == '+')
    {
        return 4;
    }
    else
    {
        bool ok = (optab.find(opcode) != optab.end());
        if (ok)
        {
            return 3;
        }
    }
    return 0;
}