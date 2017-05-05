#include <cstdlib>
#include <cstdio>
#include <vector>
#include <string>
#include <iostream>
#include <algorithm>
#include <string.h>
#include <map>
using namespace std;

struct label {
    label() {
        jumpto = 0;
    }

    label(int i) {
        jumpto = i;
    }

    int jumpto = 0;
};

string instruction_strings[] {
    "ADD",
    "SUB",
    "MUL",
    "DIV",
    "INC",
    "AND",
    "OR",
    "XOR",
    "NOT",
    "JMP",
    "JZ",
    "JO",
    "LD",
    "ST",
    "CLC",
    "CLZ",
    "CALL",
    "RET",
    "INT",
    "HLT",
    "ADC",
    "MOV",
    "PUSH",
    "POP",
    "SSP",
    "GSP",
    "CMP",
    "LIT",
    "CLF",
    "C2S",
    "C2A",
    "C2M",
    "C2D",
    "OUT",
    "IN",
    "DEC",
    "C2I",
    "CDD",
    "SBP",
    "GBP",
    "MOD",
    "CMD",
    "IRET",
    "GFR",
    "SFR",
    "DSI",
    "ENI",
    "LSL",
    "RSL",
    "DSP",
    "DBP",
    "SWBS",
    "POP8",
    "POP16",
    "PUSH8",
    "PUSH16"

    //maybe add a conditional move

};

enum instructions {
    ADD = 0,
    SUB,
    MUL,
    DIV,
    INC,
    AND,
    OR,
    XOR,
    NOT,
    JMP,
    JZ,
    JO,
    LD,
    ST,
    CLC,
    CLZ,
    CALL,
    RET,
    INT,
    HLT,
    ADC,
    MOV,
    PUSH,
    POP,
    SSP,
    GSP,
    CMP,
    LIT,
    CLF,
    C2S,
    C2A,
    C2M,
    C2D,
    OUT,
    IN,
    DEC,
    C2I,
    CDD,
    SBP,
    GBP,
    MOD,
    CMD,
    IRET,
    GFR,
    SFR,
    DSI,
    ENI,
    LSL,
    RSL,
    DSP,
    DBP,
    SWBS,
    POP8,
    POP16,
    PUSH8,
    PUSH16

    //maybe add a conditional move

};
void split_string(string const &k, string const &delim, vector<string> &output)
{
    // Due to the use of strpbrk here, this will stop copying at a null char. This is in fact desirable.
    char const *last_ptr = k.c_str(), *next_ptr;
    while ((next_ptr = strpbrk(last_ptr, delim.c_str())) != nullptr)
    {
        output.emplace_back(last_ptr, next_ptr - last_ptr);
        last_ptr = next_ptr + 1;
    }
    output.emplace_back(last_ptr);
}

struct opInfo {
    opInfo() {
        opstring = "";
    }

    opInfo(string opstyle) {
        opstring = opstyle;
    }

    std::string opstring;
};

struct dataMnemonic {
    dataMnemonic(unsigned long location, unsigned char value) : location(location), value(value) {}
    unsigned long location;
    unsigned char value;
};

int main(int argc, char *argv[])
{
    std::vector<dataMnemonic> mnemonics;
    std::map<int,opInfo> operationstyles;
    std::map<string,int> enum_lookup_table;
    std::map<string,label> label_lookup_table;
    for(int i = 0; i <= PUSH16; i++){
        operationstyles[i] = opInfo("26lc");
        enum_lookup_table[instruction_strings[i]] = i;
    }
    operationstyles[HLT] = opInfo("sb");
    operationstyles[DSP] = opInfo("sb");
    operationstyles[DBP] = opInfo("sb");
    operationstyles[CLF] = opInfo("sb");
    operationstyles[RET] = opInfo("sb");
    operationstyles[IRET] = opInfo("sb");
    operationstyles[SWBS] = opInfo("sb");
    operationstyles[LD] = opInfo("ls");
    operationstyles[ST] = opInfo("ls");
    operationstyles[JMP] = opInfo("cfl");
    operationstyles[JZ] = opInfo("cfl");
    operationstyles[JO] = opInfo("cfl");
    operationstyles[CALL] = opInfo("cfl");
    operationstyles[SSP] = opInfo("cfl");
    operationstyles[POP8] = opInfo("2reg");
    std::vector<char> assembly;
    std::vector<std::string> lines;
    std::vector<unsigned char> output;
    if(argc < 1) {
        printf("You need to specify a file.\n");
        exit(-1);
    }
    FILE *asmfile;
    FILE *outfile;
    outfile = fopen("r.out","w");
    asmfile = fopen("r.asm","r");
    if(!asmfile) {
        printf("The file needs to exist.\n");
        exit(-1);
    }
    size_t file_size = 0;
    fseek(asmfile,0L,SEEK_END);
    file_size = ftell(asmfile);
    rewind(asmfile);
    for(size_t i = 0;i < file_size;i++) {
        char *ptr = new char;
        fread(ptr,1,1,asmfile);
        assembly.push_back(*ptr);
        delete ptr;

    }
    std::string line;
    for(char s1 : assembly) {
        if(s1 == '\n') {
            lines.push_back(line);
            line = "";
            continue;
        }
        line += s1;
    }
    size_t i = 0;
    bool label_run = true;
    nextrun:
    for(std::string assembly_line : lines) {
        vector<string> instruction_components;
        split_string(assembly_line," ",instruction_components);
        for(size_t i = 0;i < instruction_components.size();i++) {
            std::transform(instruction_components[i].begin(),instruction_components[i].end(),instruction_components[i].begin(),::toupper);
        }
        for(size_t i = 0;i < instruction_components.size();i++) {
            std::cout << instruction_components[i] << std::endl;
        }
        if(instruction_components[0] == "DATA") {
            if((instruction_components.size() > 2) && label_run) {
                mnemonics.push_back(dataMnemonic(std::strtoul(instruction_components[1].c_str(),0,10),std::strtoul(instruction_components[2].c_str(),0,10)));
            } else if(label_run) {
                cout << "Data mnemonics take two arguments, a location and a value." << endl;
                goto err;
            }
            continue;
        }
        if(instruction_components[0] == "LABEL") {
            if((instruction_components.size() > 1) && label_run) {
                label_lookup_table[instruction_components[1]] = label(output.size());
            } else if(label_run) {
                cout << "A label needs a name." << endl;
                goto err;
            }
            continue;
        }
        output.push_back(enum_lookup_table[instruction_components[0]]);
        if(operationstyles[enum_lookup_table[instruction_components[0]]].opstring == "26lc") {
            if(instruction_components.size() == 2) {
                vector<string> arguments;
                split_string(instruction_components[1],",",arguments);
                if(arguments.size() == 2) {
                    unsigned char byte = 0;
                    for(size_t i = 0; i < arguments.size();i++) {
                        cout << arguments[i] << endl;
                    }
                    if(arguments[0][0] == 'R') {
                        byte |= (arguments[0][1] - '0') << 5;
                    } else {
                        cout << "Any 26lc instruction takes a register as it's first argument." << endl;
                        goto err;
                    }
                    if(arguments[1][0] == 'R') {
                        if(label_run) {
                            output.resize(output.size() + 1);
                            continue;
                        }
                        byte |= (arguments[1][1] - '0') << 2;
                        output.push_back(byte);
                    } else {
                        if(label_run) {
                            output.resize(output.size() + 5);
                            continue;
                        }
                        byte++;
                        output.push_back(byte);
                        unsigned int value = stoul(arguments[1]);
                        cout << value << endl;
                        output.push_back(value);
                        output.push_back(value >> 8);
                        output.push_back(value >> 16);
                        output.push_back(value >> 24);
                    }
                } else {
                    cout << "26lc insturctions do not take more or less than 2 arguments" << endl;
                    goto err;
                }
            }
            else {
                cout << "26lc instructions are, in fact, more than an opcode(suprise!)." << endl;
                goto err;
            }
        } else if(operationstyles[enum_lookup_table[instruction_components[0]]].opstring == "2reg") {
            if(label_run) {
                output.resize(output.size() + 1);
                continue;
            }
            vector<string> arguments;
            split_string(instruction_components[1],",",arguments);
            if(instruction_components.size() != 2) {
                goto err;
            }
            if((arguments[0][0] != 'R')) {
                goto err;
            }
            unsigned char byte;
            byte |= (arguments[0][1] - '0') << 5;
            output.push_back(byte);

        } else if(operationstyles[enum_lookup_table[instruction_components[0]]].opstring == "ls") {
            if(label_run) {
                output.resize(output.size() + 1);
                continue;
            }
            if(instruction_components.size() == 2) {
                vector<string> arguments;
                split_string(instruction_components[1],",",arguments);
                if((arguments[1][0] == 'R') && arguments[2][0] == 'R') {
                    unsigned char addr = arguments[1][1] - '0';
                    unsigned char val = arguments[2][1] - '0';
                    unsigned char byte = 0;
                    byte |= val << 5;
                    byte |= addr << 2;
                    cout << arguments[0] << endl;
                    if(arguments[0] == "WORD") {
                        byte |= 1 << 0;
                    } else if(arguments[0] == "SPECIAL") {
                        byte |= 1 << 1;
                    } else if(arguments[0] == "DWORD") {
                        byte |= 1 << 1;
                        byte |= 1 << 0;
                    }
                    output.push_back(byte);
                }
            }
            else {
                cout << "Three arguments, size, value, and address are all you need" << endl;
                goto err;
            }

        } else if(operationstyles[enum_lookup_table[instruction_components[0]]].opstring == "cfl") {
            if(instruction_components.size() == 2) {
                vector<string> arguments;
                split_string(instruction_components[1],",",arguments);
                if(arguments.size() == 1) {
                    unsigned char byte = 0;
                    if(arguments[0][0] == 'R') {
                        if(label_run) {
                            output.resize(output.size() + 1);
                            continue;
                        }
                        byte |= (arguments[0][1] - '0') << 5;
                        output.push_back(byte);
                    } else if(isdigit(arguments[0][0])) {
                        if(label_run) {
                            output.resize(output.size() + 5);
                            continue;
                        }
                        byte++;
                        output.push_back(byte);
                        unsigned int value = stoul(arguments[0]);
                        cout << value << endl;
                        output.push_back(value);
                        output.push_back(value >> 8);
                        output.push_back(value >> 16);
                        output.push_back(value >> 24);
                    } else if(isalpha(arguments[0][0])) {
                        byte++;
                        output.push_back(byte);
                        cout << label_lookup_table[arguments[0]].jumpto << endl;
                        output.push_back(label_lookup_table[arguments[0]].jumpto);
                        output.push_back(label_lookup_table[arguments[0]].jumpto >> 8);
                        output.push_back(label_lookup_table[arguments[0]].jumpto >> 16);
                        output.push_back(label_lookup_table[arguments[0]].jumpto >> 24);

                    }
                } else {
                    cout << "Control flow(cfl) instructions do not take more than a single argument." << endl;
                }
            }
        }
        i++;

        continue;
        err:
            cout << "Error on line: " << i + 1 << " which contains " << assembly_line << endl;
            return -1;
    }
    if(label_run) {
        label_run = false;
        output.clear();
        goto nextrun;
    }
    std::sort(mnemonics.begin(), mnemonics.end(),[](dataMnemonic a, dataMnemonic b){ return (a.location < b.location);});
    if(mnemonics[0].location < output.size() - 1) { std::cout << "linking error" << std::endl; return -1; }
    for(dataMnemonic mnem : mnemonics) {
        size_t filler = 0;
        filler = mnem.location - output.size() - 1;
        for(size_t i = 0; i < filler; i++) {
            output.push_back(0);
        }
        output.push_back(mnem.value);
    }
    fwrite(&output[0],sizeof(unsigned char),output.size(),outfile);
    fclose(outfile);
    fclose(asmfile);
    return 0;
}
