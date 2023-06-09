#include <iostream>
#include <string>
#include <vector>
#include <fstream>
#include <iomanip>
#include <sstream>  
#include <map>
#include <unordered_map>
#include <bitset>
#include <cmath>
#include <algorithm>

using namespace std;
typedef long long ll;

//Global Variables
ifstream file; // for any input file
ofstream outFile; // for program output
ofstream errorFile; // for error messages
map<unsigned int, int> memory; // memory
map<unsigned int, string> addressToInsruction; //maps the address of the instruction to the string representing it. for example 1-> add x1, x2, x3
map<string, unsigned int> labelToAddress; // we assume labels are case sensitive, same conventions used by RARS
int registers[32] = {}; //registers initialized to zero
unsigned int PC = 0; // Max PC is 2^32-1 = 4294967295


// map for conventional register names 
unordered_map<string, int> reg_to_num = {{"zero", 0}, {"ra", 1}, {"sp", 2}, {"gp", 3}, {"tp", 4}, {"t0", 5}, {"t1", 6}, {"t2", 7},
                               {"s0", 8}, {"s1", 9}, {"a0", 10}, {"a1", 11}, {"a2", 12}, {"a3", 13}, {"a4", 14}, {"a5", 15},
                               {"a6", 16}, {"a7", 17}, {"s2", 18}, {"s3", 19}, {"s4", 20}, {"s5", 21}, {"s6", 22}, {"s7", 23},
                               {"s8", 24}, {"s9", 25}, {"s10", 26}, {"s11", 27}, {"t3", 28}, {"t4", 29}, {"t5", 30}, {"t6", 31}};


unordered_map<string, int> regX_to_num = {{"x0", 0}, {"x1", 1}, {"x2", 2}, {"x3", 3}, {"x4", 4}, {"x5", 5}, {"x6", 6}, {"x7", 7},
                               {"x8", 8}, {"x9", 9}, {"x10", 10}, {"x11", 11}, {"x12", 12}, {"x13", 13}, {"x14", 14}, {"x15", 15},
                               {"x16", 16}, {"x17", 17}, {"x18", 18}, {"x19", 19}, {"x20", 20}, {"x21", 21}, {"x22", 22}, {"x23", 23},
                               {"x24", 24}, {"x25", 25}, {"x26", 26}, {"x27", 27}, {"x28", 28}, {"x29", 29}, {"x30", 30}, {"x31", 31}};


bool isFound(vector<string> data, string target) {
    return find(data.begin(), data.end(), target) != data.end();
}

vector<string> r_type = {"add", "sub", "sll", "slt", "sltu", "xor", "srl", "sra", "or", "and"}; // 10
void r_instruction(string inst, int rd, int rs1, int rs2){
    if (rd == 0)
    {
        return;
    }
    if (inst == r_type[0]) { // add
        registers[rd] = registers[rs1] + registers[rs2];
    } else if (inst == r_type[1]) { // sub
        registers[rd] = registers[rs1] - registers[rs2];
    } else if (inst == r_type[2]) { // sll
        registers[rd] = registers[rs1] << registers[rs2];
    } else if (inst == r_type[3]) { // slt
        registers[rd] = (registers[rs1] < registers[rs2]) ? 1 : 0;
    } else if (inst == r_type[4]) { // sltu 
        registers[rd] = ((unsigned int)registers[rs1] < (unsigned int)registers[rs2]) ? 1 : 0;
    } else if (inst == r_type[5]) { // xor
        registers[rd] = registers[rs1] ^ registers[rs2];
    } else if (inst == r_type[6]) { // srl
        if (registers[rs2] > 31 | registers[rs2] < 0)
            {
                errorFile<<"You are trying to shift right by more than 31 bits in the srl instruction\nPlease try again.\n";
                exit(1);
            }
            else 
            {
                registers[rd] = (unsigned int) registers[rs1] >> registers[rs2];
            }
    } else if (inst == r_type[7]) { // sra : used for unsigned division
        registers[rd] = registers[rs1] >> registers[rs2];
    } else if (inst == r_type[8]) { // or
        registers[rd] = registers[rs1] | registers[rs2];
    } else if (inst == r_type[9]) { // and
        registers[rd] = registers[rs1] & registers[rs2];
    }
    PC += 4;
}

// check whether a number is below or equal to n bites (signed)
bool in_range (int num , int bits)
{
    int maxi = (1<<(bits-1))-1; //  Mmax value to store in n bit register
    int mini = -(1<<(bits-1));
    return (!(num > maxi || num <mini));
    
}

// B-type
vector<string> sb_type = {"beq", "bne", "blt", "bge", "bltu", "bgeu"}; // 6
void sb_instruction_offset(string inst,int rs1, int rs2, int offset){
    bool isError = false;
    if (inst == sb_type[0]) { // beq
        if (registers[rs1] == registers[rs2])
        {
            if (in_range(offset, 12))
            {
                PC = PC + 2*offset;
            }
            else
                isError = true;
        }
        else
        {
            PC += 4;
        }
    }
    else if (inst == sb_type[1]) { // bne
        if (registers[rs1] != registers[rs2])
        {
            if (in_range(offset, 12))
            {
                PC = PC + 2*offset;
            }
            else
                isError = true;
        }
        else
        {
            PC += 4;
        }
    }
    else if (inst == sb_type[2]) { // blt
        if (registers[rs1] < registers[rs2])
        {
            if (in_range(offset, 12))
            {
                PC = PC + 2*offset;
            }
            else
                isError = true;
        }
        else
        {
            PC += 4;
        }
    }
    else if (inst == sb_type[3]) { // bge
        if (registers[rs1] >= registers[rs2])
        {
            if (in_range(offset, 12))
                PC = PC + 2*offset;
            else
                isError = true;
        }
        else
        {
            PC += 4;
        }
    }
    else if (inst == sb_type[4]) { // bltu
        if ((unsigned int)registers[rs1] < (unsigned int)registers[rs2])
        {
            if (in_range(offset, 12))
            {
                PC = PC + 2*offset;
            }
            else
                isError = true;
        }
        else
        {
            PC += 4;
        }
    }
    else if (inst == sb_type[5]) { // bgeu
        if ((unsigned int)registers[rs1] >= (unsigned int)registers[rs2])
        {
            if (in_range(offset, 12))
            {
                PC = PC + 2*offset;
            }
            else
                isError = true;
        }
        else
        {
            PC += 4;
        }
    if(isError){
        errorFile<<"You are trying to input more than 12 bits for offset in the " << inst <<" instruction\nPlease try again.\n"; 
        exit(1);
    }
}
}

void strip(string &str){
    string space = " \n\r\t\v\f";
    str.erase(str.find_last_not_of(space)+1); //erasing suffix space
    str.erase(0, str.find_first_not_of(space)); //erasing prefix space
}
string lower(string str){
    string ans="";
    for (char c:str){
        ans.push_back(tolower(c));
    }
    return ans;
}

//Example Input: "t0, t1, t2" => [t0, t1, t2]
vector<string> getCommaSeparated(string to_parse){
    vector<string> parsedInfo;
    stringstream ss(to_parse);
    string cur;
    while(getline(ss, cur, ',')){
        strip(cur);
        if(!cur.empty()){
            parsedInfo.push_back(cur);
        }
    }
    return parsedInfo;
}




void printInFormat(string regFormat ){
        if(regFormat == "b"){
            if(!memory.empty()){
                for(auto it: memory){
                    outFile<<it.first<<" : "<< bitset<32>(it.second)<<"\n";
                }
            }
            else{
                outFile<<"Memory : free\n";
        }
        for(int i=0; i<32;i++){
            outFile<<"Register x"<< i <<" : "<<bitset<32>(registers[i]) <<"\n";
        }

    }
    else if(regFormat == "d"){
        if(!memory.empty()){
            for(auto it: memory)
                outFile<< dec <<it.first<<" : "<< dec <<it.second<<"\n";
        }
        else{
            outFile<<"Memory : free\n";
        }
        for(int i=0; i < 32;i++){
            outFile<<"Register x"<< dec<< i<<" : "<< dec<<registers[i]<<"\n";
        }
    }
    else if(regFormat =="h"){
        if(!memory.empty()){
            for(auto it: memory)
                outFile<<uppercase<<hex<<"0x" << setfill('0')<< setw(8)<<it.first<<" : "
                <<"0x" << setfill('0')<< setw(8)<< it.second<<"\n";
        }
        else{
            outFile<<"Memory : free\n";
        }
        for(int i=0; i<32;i++){
            outFile<<"Register x"<< dec <<i<<" : "<< uppercase << hex << "0x" <<setfill('0')<<setw(8)<< registers[i]<<"\n";
        }
    }
}

// macros 
//del?
const int byte_rep = 255;
const int half_rep = 65535;
const unsigned int maxPC = (1ll<<32) - 1;



//Utility Funciton
bool validFirstAddress();
bool in_range (int num , int bits); //checks if num can be representes with specifiied number of bits (signed)

// Logic Fucntions
void mapInstructionsAndLabels();//Function to popubalte addressToInsructions and labelToAddress
int reg_to_int(string s); //map the register name to its index in registers
void executeInstruction(string s);
vector<string> parseParenthesis(string instructionInfo); //parses the Load and Store instructions
void checkLabelExists(string label);

//Instructions
// I-type
void addi (int rd, int rs1, int imm);
void andi (int rd, int rs1, int imm);
void ori (int rd, int rs1, int imm);
void xori (int rd, int rs1, int imm);
void slli (int rd, int rs1, int imm);
void srli (int rd, int rs1, int imm);
void srai (int rd, int rs1, int imm);
void jalr (int rd, int rs1, int imm);
void lw (int rd, int rs1, int imm);
void lh (int rd, int rs1, int imm);
void lb (int rd, int rs1, int imm);
void lhu (int rd, int rs1, int imm);
void lbu (int rd, int rs1, int imm);
void slti (int rd, int rs1, int imm);
void sltiu (int rd, int rs1, int imm);

// S-type
void sw (int rs1, int rs2, int imm);
void sh (int rs1, int rs2, int imm);
void sb (int rs1, int rs2, int imm);

// J-type
void jal (int rd, string label);
void jal (int rd, int offset);

// U-type
void lui (int rd, int imm);
void auipc (int rd, int imm);

// SB-type
void beq (int rs1, int rs2, string label);
void beq (int rs1, int rs2, int offset);
void bne (int rs1, int rs2, string label);
void bne (int rs1, int rs2, int offset);
void blt (int rs1, int rs2, string label);
void blt (int rs1, int rs2, int offset);
void bltu (int rs1, int rs2, string label);
void bltu (int rs1, int rs2, int offset);
void bge (int rs1, int rs2, string label);
void bge (int rs1, int rs2,int offset);
void bgeu (int rs1, int rs2, string label);
void bgeu (int rs1, int rs2,int offset);

int main() {
    // open files
    file.open("data.txt"); // data.txt contains: First address, mem_init flag ,ouput format
    outFile.open("output.txt");
    errorFile.open("errorMessages.txt");

    // initialize the first address
    int faddr;
    file >> faddr;
    PC=faddr;

    //Initializing registers to 0
    for(int i=0;i<32;i++){
        registers[i]=0;
    }

    // flag to initialize the memory
    string c;
    file >> c; 
    if (c=="yes"){
        ifstream file_2;
        file_2.open("Memory.txt");
        unsigned int address;
        int val;
        while(file_2>>address){
            file_2 >> val;
            memory[address] = val;
        }    
    }

    string regFormat; // format
    file >>regFormat;
    file.close();
    file.open("assemblyCode.txt");
    mapInstructionsAndLabels();

    while (lower(addressToInsruction[PC]).substr(0, 5) != "fence" && lower(addressToInsruction[PC]).substr(0,5) != "ecall"
     && lower(addressToInsruction[PC]).substr(0,6) != "ebreak") {
        outFile<< "pc" <<PC<< " : " << addressToInsruction[PC] <<"\n";
        executeInstruction(addressToInsruction[PC]); //changes values of registers and memory as required
        printInFormat(regFormat);

    }
    return 0;
}





// map register given to its index in registers array
// notice registers are case sensitive, so X0 is considered invalid
int reg_to_int(string s) {
    lower(s);
    if(regX_to_num.find(s) != regX_to_num.end())
        return regX_to_num[s];
    else if(reg_to_num.find(s) != reg_to_num.end())
        return reg_to_num[s];
    else{
        errorFile <<" You entered an invalid register (" << s << ") Please Try again.\n";
        exit(1);
    }
}

//TODO: add to output page column for address for each instruction
//TODO: add function to validate instruction.
void mapInstructionsAndLabels(){
    unsigned int tmpPC = PC;
    string Line;
    while(getline(file, Line)){
        if(!Line.empty()){
            strip(Line);
            size_t colonPos = Line.find(":"); // to mark labels
            if (colonPos != string::npos) //if there is a label
            {
                string label = Line.substr(0, colonPos);
                string possibleInst = Line.substr(colonPos+1);
                strip(label); 
                strip(possibleInst);
                // validate that the label does not exist
                if(labelToAddress.find(label) != labelToAddress.end()){ 
                    errorFile << "You cannot define a label twice. Please try again.\n";
                    exit(1);
                }
                if(isdigit(label[0])){
                    errorFile <<"You cannot start a label with number. Please try again.\n";
                    exit(1);
                }
                labelToAddress[label] = tmpPC;
                if(possibleInst.size()>1){
                    addressToInsruction[tmpPC] = possibleInst;
                    if (tmpPC > maxPC-4) {
                        errorFile <<"You entered too many instructions. Please try again.\n";
                        exit(1);
                    }
                    tmpPC+=4;
                }
            }
            // if not label
            else if (Line.size()>1){
                addressToInsruction[tmpPC] = Line;
                if (tmpPC > maxPC-4) {
                    errorFile <<"You entered too many instructions. Please try again later.\n";
                    exit(1);
                }
                tmpPC+=4;
            }
        }
    }
}


//takes the whole string of the instruction, i.e. add t0, t1, t2 and redirects it to the suitable function
void executeInstruction(string s){
    stringstream ss(s);
    string instructionWord;
    ss>>instructionWord;
    strip(instructionWord);
    instructionWord = lower(instructionWord);
    string instructionInfo;
    getline(ss, instructionInfo);
    strip(instructionInfo);
    vector<string> infoParsed;

   
    infoParsed = getCommaSeparated(instructionInfo);  // ['t0', 't1', 't2']
    if (infoParsed.size() <= 1){
        errorFile << "You entered invalid instruction, please try again.";
    }

    //parse instructions with parenthesis in them. That only happen in second argument
    if (infoParsed[1].find('(') != string::npos){
        infoParsed = parseParenthesis(instructionInfo);
    }
    //R-type
    if(isFound(r_type,instructionWord))
        r_instruction(instructionWord,reg_to_int(infoParsed[0]),reg_to_int(infoParsed[1]),reg_to_int(infoParsed[2])); 
    //I type
    else if (instructionWord == "addi"){
        addi(reg_to_int(infoParsed[0]), reg_to_int(infoParsed[1]), stoi(infoParsed[2]));
    }
    else if(instructionWord == "andi"){
        andi(reg_to_int(infoParsed[0]), reg_to_int(infoParsed[1]), stoi(infoParsed[2]));
    }
    else if(instructionWord == "ori"){
        ori(reg_to_int(infoParsed[0]), reg_to_int(infoParsed[1]), stoi(infoParsed[2]));
    }
    else if(instructionWord == "xori"){
        xori(reg_to_int(infoParsed[0]), reg_to_int(infoParsed[1]), stoi(infoParsed[2]));
    }
    else if(instructionWord == "slli"){
        slli(reg_to_int(infoParsed[0]), reg_to_int(infoParsed[1]), stoi(infoParsed[2]));
    }
    else if(instructionWord == "srli"){
        srli(reg_to_int(infoParsed[0]), reg_to_int(infoParsed[1]), stoi(infoParsed[2]));
    }
    else if(instructionWord == "srai"){
        srai(reg_to_int(infoParsed[0]), reg_to_int(infoParsed[1]), stoi(infoParsed[2]));
    }
    else if(instructionWord == "slti"){
        slti(reg_to_int(infoParsed[0]), reg_to_int(infoParsed[1]), stoi(infoParsed[2]));
    }
    else if(instructionWord == "sltiu"){
        sltiu(reg_to_int(infoParsed[0]), reg_to_int(infoParsed[1]), stoi(infoParsed[2]));
    }
    else if(instructionWord == "jalr"){
        jalr(reg_to_int(infoParsed[0]), reg_to_int(infoParsed[1]), stoi(infoParsed[2]));
    }
    else if (instructionWord == "lw"){
        lw(reg_to_int(infoParsed[0]), reg_to_int(infoParsed[1]), stoi(infoParsed[2]));
    }
    else if (instructionWord == "lh"){
        lh(reg_to_int(infoParsed[0]), reg_to_int(infoParsed[1]), stoi(infoParsed[2]));
    }
    else if (instructionWord == "lb"){
        lb(reg_to_int(infoParsed[0]), reg_to_int(infoParsed[1]), stoi(infoParsed[2]));
    }
    else if (instructionWord == "lhu"){
        lhu(reg_to_int(infoParsed[0]), reg_to_int(infoParsed[1]), stoi(infoParsed[2]));
    }
    else if (instructionWord == "lbu"){
        lbu(reg_to_int(infoParsed[0]), reg_to_int(infoParsed[1]), stoi(infoParsed[2]));
    }
    //S type
    else if (instructionWord =="sw"){
        sw(reg_to_int(infoParsed[0]), reg_to_int(infoParsed[1]), stoi(infoParsed[2]));
    }
    else if (instructionWord =="sh"){
        sh(reg_to_int(infoParsed[0]), reg_to_int(infoParsed[1]), stoi(infoParsed[2]));
    }
    else if (instructionWord =="sb"){
        sb(reg_to_int(infoParsed[0]), reg_to_int(infoParsed[1]), stoi(infoParsed[2]));
    }
    // J type
    else if (instructionWord == "jal"){
        if(isdigit(infoParsed[1][0])){ //If second argument in jal begins with integer, then it is an offset not a label
            // we checked that every label does not begin with an integer
            jal(reg_to_int(infoParsed[0]), stoi(infoParsed[1]));
        }
        else{//second argument is a label
            jal(reg_to_int(infoParsed[0]), infoParsed[1]);
        }
        
    }
    // U-type
    else if (instructionWord == "lui"){
        lui(reg_to_int(infoParsed[0]), stoi(infoParsed[1]));
    }
    else if (instructionWord == "auipc"){
        auipc(reg_to_int(infoParsed[0]), stoi(infoParsed[1]));
    }
    // B type. All B type are similar to jal but with two registers
    else if(instructionWord =="beq"){
        if(isdigit(infoParsed[2][0])){
            beq(reg_to_int(infoParsed[0]), reg_to_int(infoParsed[1]), stoi(infoParsed[2]));
        }
        else{
            beq(reg_to_int(infoParsed[0]), reg_to_int(infoParsed[1]), infoParsed[2]);
        }
    }
    else if(instructionWord =="bne"){
        if(isdigit(infoParsed[2][0])){
            bne(reg_to_int(infoParsed[0]), reg_to_int(infoParsed[1]), stoi(infoParsed[2]));
        }
        else{
            bne(reg_to_int(infoParsed[0]), reg_to_int(infoParsed[1]), infoParsed[2]);
        }
    }
    else if(instructionWord =="blt"){
        if(isdigit(infoParsed[2][0])){
            blt(reg_to_int(infoParsed[0]), reg_to_int(infoParsed[1]), stoi(infoParsed[2]));
        }
        else{
            blt(reg_to_int(infoParsed[0]), reg_to_int(infoParsed[1]), infoParsed[2]);
        }
    }
    else if(instructionWord =="bltu"){
        if(isdigit(infoParsed[2][0])){
            bltu(reg_to_int(infoParsed[0]), reg_to_int(infoParsed[1]), stoi(infoParsed[2]));
        }
        else{
            bltu(reg_to_int(infoParsed[0]), reg_to_int(infoParsed[1]), infoParsed[2]);
        }
    }
    else if(instructionWord =="bge"){
        if(isdigit(infoParsed[2][0])){
            bge(reg_to_int(infoParsed[0]), reg_to_int(infoParsed[1]), stoi(infoParsed[2]));
        }
        else{
            bge(reg_to_int(infoParsed[0]), reg_to_int(infoParsed[1]), infoParsed[2]);
        }
    }
    else if(instructionWord =="bgeu"){
        if(isdigit(infoParsed[2][0])){
            bgeu(reg_to_int(infoParsed[0]), reg_to_int(infoParsed[1]), stoi(infoParsed[2]));
        }
        else{
            bgeu(reg_to_int(infoParsed[0]), reg_to_int(infoParsed[1]), infoParsed[2]);
        }
    }
    else{
        errorFile <<"Instruction "<<instructionWord<<" is not supported. Please try again.\n";
        exit(1);
    }

}


//parses the Load and Store instructions
// jalr x0, 0(x1)
vector<string> parseParenthesis(string instructionInfo){
    vector<string> InfoParsed = getCommaSeparated(instructionInfo);
    string imm = InfoParsed[1].substr(0, InfoParsed[1].find('('));
    strip(imm);
    if(imm.empty())
        imm="0";
    string secondReg = InfoParsed[1].substr(InfoParsed[1].find('(')+1);
    secondReg.pop_back();//poping the closing paranthesis
    strip(secondReg);

    InfoParsed.pop_back(); // remove 0(x1)
    InfoParsed.push_back(secondReg); // add x1
    InfoParsed.push_back(imm); // add 0

    return InfoParsed;
}


void checkLabelExists(string label){
    if(labelToAddress.find(label) ==labelToAddress.end()){
        errorFile<<"You are trying to jump to a label that does not exist. Please try again.\n";
        exit(1);
    }
}


/////////////////////////////////////////////Definitions///////////////////////////////////////////////////////////////////
// I-type
void addi (int rd, int rs1, int imm)
{
    if (rd == 0)
    {
        return;
    }
    else if (!in_range(imm, 12))
    {
        errorFile<<"You are trying to input more than 12 bits for immediate in the addi instruction\nPlease try again.\n";
         
        exit(1);
    }
    else
    {
        registers[rd] = registers[rs1] + imm;
    }

    PC += 4;
}

void andi (int rd, int rs1, int imm)
{
    if (rd == 0)
    {
        return;
    }
    else if (!in_range(imm, 12))
    {
        errorFile<<"You are trying to input more than 12 bits for immediate in the andi instruction\nPlease try again.\n";
         
        exit(1);
    }
    else
    {
        registers[rd] = registers[rs1] & imm;
    }

    PC += 4;
}

void ori (int rd, int rs1, int imm)
{
    if (rd == 0)
    {
        return;
    }
    else if (!in_range(imm, 12))
    {
        errorFile<<"You are trying to input more than 12 bits for immediate in the ori instruction\nPlease try again.\n";
         
        exit(1);
    }
    else
    {
        registers[rd] = registers[rs1] | imm;
    }

    PC += 4;
}

void xori (int rd, int rs1, int imm)
{
    if (rd == 0)
    {
        return;
    }
    else if (!in_range(imm, 12))
    {
        errorFile<<"You are trying to input more than 12 bits for immediate in the xori instruction\nPlease try again.\n";
         
        exit(1);
    }
    else
    {
        registers[rd] = registers[rs1] ^ imm;
    }

    PC += 4;
}

void slli (int rd, int rs1, int imm) // unsigned
{
    if (rd == 0)
    {
        return;
    }
    else if (imm > 31 || imm < 0)
    {
        errorFile<<"You are trying to shift left by more than 31 bits in the slli instruction\nPlease try again.\n";
         
        exit(1);
    }
    else
    {
        registers[rd] = registers[rs1] << imm;
    }

    PC += 4;
}

void srli (int rd, int rs1, int imm) // unsigned
{
    if (rd == 0)
    {
        return;
    }
    else if (imm > 31 || imm < 0)
    {
        errorFile<<"You are trying to shift right by more than 31 bits in the srli instruction\nPlease try again.\n";
         
        exit(1);
    }
    else
    {
        registers[rd] = (unsigned int) registers[rs1] >> imm;
    }

    PC += 4;
}

void srai (int rd, int rs1, int imm) // sign extended
{
    if (rd == 0)
    {
        return;
    }
    else if (imm > 31 | imm < 0)
    {
        errorFile<<"You are trying to shift right by more than 31 bits in the srai instruction\nPlease try again.\n";
         
        exit(1);
    }
    else
    {
        registers[rd] = registers[rs1] >> imm;
    }

    PC += 4;
}

// jalr x0, 0(x2)
void jalr (int rd, int rs1, int imm)
{
    if (rd != 0)
    {
        registers[rd] = PC + 4;
    }
    if (!in_range(imm, 12))
    {
        errorFile<<"You are trying to input more than 12 bits for immediate in the jalr instruction\nPlease try again.\n";
         
        exit(1);
    }
    else
    {
        PC = registers[rs1] + imm;
    }
}

// lw x2, 4(x6)
void lw (int rd, int rs1, int imm)
{
    if (rd == 0)
    {
        return;
    }
    if (!in_range(imm, 12))
    {
        errorFile<<"You are trying to input more than 12 bits for immediate in the (lw) instruction\nPlease try again.\n";
         
        exit(1);
    }

    int address = registers[rs1] + imm; 
		
    if (memory.find(address) != memory.end())
    {
        registers[rd] = memory[address];
    }
    else
    {
        errorFile<<"You are trying to load from a non-allocated address in the (lw) instruction\nPlease try again.\n";
        
        exit(1);
    }

    PC += 4;
}

void lh (int rd, int rs1, int imm)
{
    if (rd == 0)
    {
        return;
    }
    if (in_range(imm, 12))
    {
        int r = (registers[rs1] + imm) % 4;
        int address = registers[rs1] + imm - r; // rounding down to the nearst multiple of 4
		
        if (memory.find(address) != memory.end())
        {
            registers[rd] = (memory[address] << (8 * (2 - r))) >> 16;
        }
        else
        {
            errorFile<<"You are trying to load from a non-allocated address in the lh instruction\nPlease try again.\n";
             
            exit(1);
        }
    }
    else
    {
        errorFile<<"You are trying to input more than 12 bits for immediate in the lh instruction\nPlease try again.\n";
         
        exit(1);
    }

    PC += 4;
}

void lb (int rd, int rs1, int imm)
{
    if (rd == 0)
    {
        return;
    }
    if (in_range(imm, 12))
    {
        int r = (registers[rs1]+imm)%4;
        int address = registers[rs1] + imm - r;
			
        if (memory.find(address) != memory.end())
        {
            registers[rd] = (memory[address] << (8 * (3 - r))) >> 24;
        }
        else
        {
            errorFile<<"You are trying to load from a non-allocated address in the lb instruction\nPlease try again.\n";
             
            exit(1);
        }
    }
    else
    {
        errorFile<<"You are trying to input more than 12 bits for immediate in the lb instruction\nPlease try again.\n";
         
        exit(1);
    }

    PC += 4;
}

void lhu (int rd, int rs1, int imm)
{
    if (rd == 0)
    {
        return;
    }
    if (in_range(imm, 12))
    {
        int r = (registers[rs1]+imm)%4;
        int address = registers[rs1]+imm - r;
		
        if (memory.find(address) != memory.end())
        {
            registers[rd] = (unsigned int)(memory[address] << (8 * (2 - r))) >> 16;
        }
        else
        {
            errorFile<<"You are trying to load from a non-allocated address in the lhu instruction\nPlease try again.\n";
             
            exit(1);
        }
    }
    else
    {
        errorFile<<"You are trying to input more than 12 bits for immediate in the lhu instruction\nPlease try again.\n";
         
        exit(1);
    }

    PC += 4;
}

void lbu (int rd, int rs1, int imm)
{
    if (rd == 0)
    {
        return;
    }
    if (in_range(imm, 12))
    {
        int r = (registers[rs1]+imm)%4;
        int address = registers[rs1]+imm - r;
		
        if (memory.find(address) != memory.end())
        {
            registers[rd] = (unsigned int)(memory[address] << (8 * (3 - r))) >> 24;
        }
        else
        {
            errorFile<<"You are trying to load from a non-allocated address in the lbu instruction\nPlease try again.\n";
             
            exit(1);
        }
    }
    else
    {
        errorFile<<"You are trying to input more than 12 bits for immediate in the lbu instruction\nPlease try again.\n";
         
        exit(1);
    }

    PC += 4;
}

void slti (int rd, int rs1, int imm)
{
    if (rd == 0)
    {
        return;
    }
    if(!in_range(imm, 12))
    {
        errorFile<<"You are trying to input more than 12 bits for immediate in the slti instruction\nPlease try again.\n";
         
        exit(1);
    }
    else
    {
        if(registers[rs1] < imm)
        {
            registers[rd] = 1;
        }
        else
        {
            registers[rd] = 0;
        }
    }
    PC += 4;
}

void sltiu (int rd, int rs1, int imm)
{
    if (rd == 0)
    {
        return;
    }
    if(!in_range(imm, 12))
    {
        errorFile<<"You are trying to input more than 12 bits for immediate in the sltiu instruction\nPlease try again.\n";
         
        exit(1);
    }
    else
    {
        if((unsigned int)registers[rs1] < (unsigned int)imm)
        {
            registers[rd] = 1;
        }
        else
        {
            registers[rd] = 0;
        }
    }
    PC += 4;
}

// S-type
void sw (int rs1, int rs2, int imm)
{
    int final_address = registers[rs2] + imm;
    if (final_address%4)
    {
        errorFile<<"You are trying to store a word into an address not divisable by 4\nPlease try again.\n";
        exit(1);
    }
    else
    {
        memory[final_address] = registers[rs1];
    }

    PC += 4;
}

void sh (int rs1, int rs2, int imm)
{
    int address = registers[rs2] + imm;
    int r = address%4;
    int final_address = address - r;

    if (final_address%2)
    {
        errorFile<<"You are trying to store a half word into an address not divisable by 2\nPlease try again.\n";
        exit(1);
    }
    else
    {
        unsigned int half = registers[rs1] << 16;
        half = half >> 16;
        half = half << 8*r;

        unsigned int mask = half_rep;
        mask = mask << 8*r;
        mask = ~mask;
        int edited_word = memory[final_address];
        edited_word = ((edited_word & mask) | half);

        memory[final_address] = edited_word;
    }
    PC += 4;
}

void sb (int rs1, int rs2, int imm)
{
    int address = registers[rs2] + imm;
    int r = address%4;
    int final_address = address - r;

    unsigned int byte = registers[rs1] << 24;
    byte = byte >> 24;
    byte = byte << 8*r;

    unsigned int mask = byte_rep;
    mask = mask << 8*r;
    mask = ~mask;
    int edited_word = memory[final_address];
    edited_word = ((edited_word & mask) | byte);

    memory[final_address] = edited_word;
    PC += 4;
}


void beq (int rs1, int rs2, string label) // ver1 --> based on label
{
    checkLabelExists(label);
    if (registers[rs1] == registers[rs2])
    {
        PC = labelToAddress[label];
    }
    else
    {
        PC += 4;
    }
}

void beq (int rs1, int rs2, int offset) // --> based on offset
{
    if (registers[rs1] == registers[rs2])
    {
        if (in_range(offset, 12))
        {
            PC = PC + 2*offset;
        }
        else
        {
            errorFile<<"You are trying to input more than 12 bits for offset in the beq instruction\nPlease try again.\n";
             
            exit(1);
        }
    }
    else
    {
        PC += 4;
    }
}

void bne (int rs1, int rs2, string label) // ver1 --> based on label
{
    checkLabelExists(label);
    if (registers[rs1] != registers[rs2])
    {
        PC = labelToAddress[label];
    }
    else
    {
        PC += 4;
    }
}

void bne (int rs1, int rs2, int offset) // --> based on offset
{
    if (registers[rs1] != registers[rs2])
    {
        if (in_range(offset, 12))
        {
            PC = PC + 2*offset;
        }
        else
        {
            errorFile<<"You are trying to input more than 12 bits for offset in the bne instruction\nPlease try again.\n";
             
            exit(1);
        }
    }
    else
    {
        PC += 4;
    }
}

void blt (int rs1, int rs2, string label) // ver1 --> based on label
{
    checkLabelExists(label);
    if (registers[rs1] < registers[rs2])
    {
        PC = labelToAddress[label];
    }
    else
    {
        PC += 4;
    }
}

void blt (int rs1, int rs2, int offset) // --> based on offset
{
    if (registers[rs1] < registers[rs2])
    {
        if (in_range(offset, 12))
        {
            PC = PC + 2*offset;
        }
        else
        {
            errorFile<<"You are trying to input more than 12 bits for offset in the blt instruction\nPlease try again.\n";
             
            exit(1);
        }
    }
    else
    {
        PC += 4;
    }
}

void bltu (int rs1, int rs2, string label) // ver1 --> based on label
{
    checkLabelExists(label);
    if ((unsigned int)registers[rs1] < (unsigned int)registers[rs2])
    {
        PC = labelToAddress[label];
    }
    else
    {
        PC += 4;
    }
}

void bltu (int rs1, int rs2, int offset) // --> based on offset
{
    if ((unsigned int)registers[rs1] < (unsigned int)registers[rs2])
    {
        if (in_range(offset, 12))
        {
            PC = PC + 2*offset;
        }
        else
        {
            errorFile<<"You are trying to input more than 12 bits for offset in the bltu instruction\nPlease try again.\n";
             
            exit(1);
        }
    }
    else
    {
        PC += 4;
    }
}

void bge (int rs1, int rs2, string label) // ver1 --> based on label
{
    checkLabelExists(label);
    if (registers[rs1] >= registers[rs2])
    {
        PC = labelToAddress[label];
    }
    else
    {
        PC += 4;
    }
}

void bge (int rs1, int rs2, int offset) // --> based on offset
{
    if (registers[rs1] >= registers[rs2])
    {
        if (in_range(offset, 12))
        {
            PC = PC + 2*offset;
        }
        else
        {
            errorFile<<"You are trying to input more than 12 bits for offset in the bge instruction\nPlease try again.\n";
             
            exit(1);
        }
    }
    else
    {
        PC += 4;
    }
}

void bgeu (int rs1, int rs2, string label) // ver1 --> based on label
{
    checkLabelExists(label);
    if ((unsigned int)registers[rs1] >= (unsigned int)registers[rs2])
    {
        PC = labelToAddress[label];
    }
    else
    {
        PC += 4;
    }
}

void bgeu (int rs1, int rs2, int offset) // --> based on offset
{
    if ((unsigned int)registers[rs1] >= (unsigned int)registers[rs2])
    {
        if (in_range(offset, 12))
        {
            PC = PC + 2*offset;
        }
        else
        {
            errorFile<<"You are trying to input more than 12 bits for offset in the begu instruction\nPlease try again.\n";
             
            exit(1);
        }
    }
    else
    {
        PC += 4;
    }
}


// J-type
void jal (int rd, string label) // ver1 --> jump based on label
{
    checkLabelExists(label);
    if (rd != 0) // because x0 is read-only register
    {
        registers[rd] = PC + 4; // address of next instruction in the destination register
    }

    PC = labelToAddress[label]; // go to the address of the label
}

void jal (int rd, int offset) // --> jump based on offset
{
    if (rd != 0)
    {
        registers[rd] = PC + 4;
    }
    if (in_range(offset, 20))
    {
        PC = PC + 2*offset;
    }
    else
    {
        errorFile<<"You are trying to input more than 20 bits for offset in the jal instruction\nPlease try again.\n";
         
        exit(1);
    }
}


// U-type
void lui (int rd, int imm)
{
    if (rd == 0)
    {
        return;
    }
    if(in_range(imm, 20))
    {
        registers[rd] = (imm << 12);
    }
    else
    {
        errorFile<<"You are trying to input more than 20 bits from immidiate in the lui instruction\nPlease try again.\n";
         
        exit(1);
    }
    PC += 4;
}
// add upper imm to pc
// position-independent code or loading data from memory located at a fixed offset from the current code location.
void auipc(int rd, int imm)
{
    if (rd == 0)
    {
        return;
    }
    if(in_range(imm, 20))
    {
        registers[rd] = PC + (imm << 12);
    }
    else
    {
        errorFile<<"You are trying to input more than 20 bits from immidiate in the auip instruction\nPlease try again.\n";
         
        exit(1);
    }
    PC += 4;
}