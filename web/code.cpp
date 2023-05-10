#include <iostream>
#include <string>
#include <vector>
#include <fstream>
#include <iomanip>
#include <sstream>  
#include <map>
#include <unordered_map>
#include <bitset>

using namespace std;
typedef long long ll;

unordered_map<string, int> reg_to_num = {{"zero", 0}, {"ra", 1}, {"sp", 2}, {"gp", 3}, {"tp", 4}, {"t0", 5}, {"t1", 6}, {"t2", 7},
                               {"s0", 8}, {"s1", 9}, {"a0", 10}, {"a1", 11}, {"a2", 12}, {"a3", 13}, {"a4", 14}, {"a5", 15},
                               {"a6", 16}, {"a7", 17}, {"s2", 18}, {"s3", 19}, {"s4", 20}, {"s5", 21}, {"s6", 22}, {"s7", 23},
                               {"s8", 24}, {"s9", 25}, {"s10", 26}, {"s11", 27}, {"t3", 28}, {"t4", 29}, {"t5", 30}, {"t6", 31}};

unordered_map<string, int> regX_to_num = {{"x0", 0}, {"x1", 1}, {"x2", 2}, {"x3", 3}, {"x4", 4}, {"x5", 5}, {"x6", 6}, {"x7", 7},
                               {"x8", 8}, {"x9", 9}, {"x10", 10}, {"x11", 11}, {"x12", 12}, {"x13", 13}, {"x14", 14}, {"x15", 15},
                               {"x16", 16}, {"x17", 17}, {"x18", 18}, {"x19", 19}, {"x20", 20}, {"x21", 21}, {"x22", 22}, {"x23", 23},
                               {"x24", 24}, {"x25", 25}, {"x26", 26}, {"x27", 27}, {"x28", 28}, {"x29", 29}, {"x30", 30}, {"x31", 31}};

// macros
const int byte_rep = 255;
const int half_rep = 65535;
const unsigned int maxPC = (1ll<<32) - 1;

//Global Variables
map<unsigned int, int> memory;
map<unsigned int, string> addressToInsruction; //maps the address of the instruction to the string representing it. for example 1-> add x1, x2, x3
map<string, unsigned int> labelToAddress; // we assume labels are case sensitive, same conventions used by RARS
int registers[32] = {}; //registers initialized to zero
unsigned int PC = 0; // Max PC is 2^32-1 = 4294967295
ifstream file;
ofstream outFile;
ofstream errorFile;

//Utility Funciton
bool validFirstAddress();
void openFile(string fileName); 
void strip(string &str); //strip the string of outer spaces tabes and new lines
string lower(string str); ///lower the string
vector<string> getCommaSperated(string to_parse);//get comma separted values of string to_parse
bool in_range (int num , int bits); //checks if num can be representes with specifiied number of bits (signed)

// Logic Fucntions
void mapInstructionsAndLabels();//Function to popubalte addressToInsructions and labelToAddress
int reg_to_int(string s); //map the register name to its index in registers
void executeInstruction(string s);
vector<string> parseParenthesis(string instructionInfo); //parses the Load and Store instructions
void output(string outputForm); //prints memory and registers
void checkLabelExists(string label);

//Instructions
// R-type
void add (int rd, int rs1, int rs2);
void sub (int rd, int rs1, int rs2);
void _or (int rd, int rs1, int rs2);
void _and (int rd, int rs1, int rs2);
void _xor (int rd, int rs1, int rs2);
void sll (int rd, int rs1, int rs2);
void srl (int rd, int rs1, int rs2);
void sra (int rd, int rs1, int rs2);
void slt (int rd, int rs1, int rs2);
void sltu (int rd, int rs1, int rs2);

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

// B-type
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
    file.open("data.txt");
    outFile.open("output.txt");
    errorFile.open("errorMessages.txt");

    // initialize the first address
    int faddr;
    file >> faddr;
    PC=faddr;

    //dbg
    // cout << PC;
    //Initializing registers to 0
    for(int i=0;i<32;i++){
        registers[i]=0;
    }

    string c;
    // cout<<"Enter Y if you want to initilize the memory with file. Otherwise, Enter N\n";
    file >> c;


    // TODO: This should be added in the webPage
    if (c=="yes"){
        // cout<<"The format of memory file should be on form 'address value' without quotations\n";
        // cout<<"Enter the memory file name with the extension. The file should be in the current direcory.\n";
        // openFile("Memory.txt");
        file.open("Memory.txt");
        unsigned int address;
        int val;
        // We assume the memory file format is on form
        // Address1 val1
        // Address2 val2 ...
        while(file>>address){
            file >> val;
            memory[address] = val;
        }
        // for (auto it: memory){
        //     cout<<it.first<<" "<<it.second<<"\n";
        // }
        
    }
    string outputForm; // format
    file >>outputForm;
    file.close();

    // cout << "Enter the assembly code file name with the extension. The file should be in the current direcory.\n";
    openFile("assemblyCode.txt");
    mapInstructionsAndLabels();

    //instruction word itslef is case insensitive
    while (lower(addressToInsruction[PC].substr(0, 5)) != "fence" && lower(addressToInsruction[PC]).substr(0,5) != "ecall"
     && lower(addressToInsruction[PC]).substr(0,6) != "ebreak") {
        cout<<"PC is "<< dec<<PC<<"\n";
        cout<<"Executing Instruction "<< addressToInsruction[PC]<<"\n";
        executeInstruction(addressToInsruction[PC]); //changes values of registers and memory as required
        output(outputForm);
    }

    cout<<"Yaaaaay. Your program has finished executing.\n";
    system("pause");
    return 0;
}




void openFile(string fileName){
    file.open(fileName);
    while(!file.is_open()){
        cout<<"The file can not be located.\nPlease enter the name again: ";
        cin>>fileName;
        file.open(fileName);
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
vector<string> getCommaSperated(string to_parse){
    stringstream ss(to_parse);
    string cur;
    vector<string> parsedInfo;
    while(getline(ss, cur, ',')){
        strip(cur);
        if(!cur.empty()){
            parsedInfo.push_back(cur);
        }
    }
    return parsedInfo;
}


// check whether a number is below or equal to n bites (signed)
bool in_range (int num , int bits)
{
    int maxi = (1<<(bits-1))-1;
    int mini = -(1<<(bits-1));
    return (!(num > maxi || num <mini));
    
}

void output(string outputForm ){
    if(outputForm == "d" || outputForm =="D"){
        if(!memory.empty()){
            cout<<"Printing Memory on the form Address : value\n";
            for(auto it: memory)
                cout<<it.first<<" : "<<it.second<<"\n";
        }
        else{
            cout<<"Memory is free\n";
        }

        cout<<"Registers values\n";
        for(int i=0; i<32;i++){
            cout<<"Register x"<<i<<" : "<<registers[i]<<"\n";
        }
    }
    else if(outputForm =="h" || outputForm=="H"){
        if(!memory.empty()){
            cout<<"Printing Memory on the form Address : value\n";
            for(auto it: memory)
                cout<<uppercase<<hex<<"0x" << setfill('0')<< setw(8)<<it.first<<" : "
                <<"0x" << setfill('0')<< setw(8)<< it.second<<"\n";
        }
        else{
            cout<<"Memory is free\n";
        }

        cout<<"Registers values\n";
        for(int i=0; i<32;i++){
            cout<<"Register x"<<dec<<i<<" : "<<uppercase<<hex<< "0x" <<setfill('0')<<setw(8)<< registers[i]<<"\n";
        }
    }
    else{//binary 
        bitset<32> num;
        if(!memory.empty()){
            cout<<"Printing Memory on the form Address : value\n";
            // I will print the address as decimal and value as binary in order not to have 64 bits on same line
            // Also, that makes more sense for formatting as it would be easier to read addresses.
            // We do not believe it is logical to print the addresses in binary
            for(auto it: memory){
                num = it.second;
                cout<<it.first<<" : "<<num<<"\n";
            }
        }
        else{
            cout<<"Memory is free\n";
        }

        cout<<"Registers values\n";
        for(int i=0; i<32;i++){
            num = registers[i];
            cout<<"Register x"<<i<<" : "<<num<<"\n";
        }

    }
}


// map register given to its index in registers array
// notice registers are case sensitive, so X0 is considered invalid
int reg_to_int(string s) {
    if(regX_to_num.find(s) != regX_to_num.end())
        return regX_to_num[s];
    else if(reg_to_num.find(s) != reg_to_num.end())
        return reg_to_num[s];
    else{
        cout<<"There is an invalid register used. The program will be terminated\n";
        system("pause");
        exit(1);
    }
}


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
                if(labelToAddress.find(label) != labelToAddress.end()){//Lables are case snestitive according to RARS
                    errorFile << "Labels must be unique. The program will terminate\n";
                    exit(1);
                }
                if(isdigit(label[0])){ //We assume that labels do not start with integers as in RARS 
                    //That will help us parse J and B type instrucitons, and that is what RARS does
                    errorFile <<"Labels can not start with integers. The program will terminate\n";
                    exit(1);
                }
                labelToAddress[label] = tmpPC;
                if(possibleInst.size()>1){
                    addressToInsruction[tmpPC] = possibleInst;
                    if (tmpPC > maxPC-4) {
                        errorFile <<"You entered too many instructions. The program will terminate\n";
                        exit(1);
                    }
                    tmpPC+=4;
                }
            }
            else if (Line.size()>1){
                addressToInsruction[tmpPC] = Line;
                if (tmpPC > maxPC-4) {
                    errorFile <<"You entered too many instructions. The program will terminate\n";
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
    instructionWord = lower(instructionWord);//instructionWords are case insensitive
    string insructionInfo;
    getline(ss, insructionInfo);
    strip(insructionInfo);
    vector<string> infoParsed;

    infoParsed = getCommaSperated(insructionInfo);

    //parse instructions with paranthesis in them. That only happen in second argemnt
    if (infoParsed[1].find('(') != string::npos){
        infoParsed = parseParenthesis(insructionInfo);
    }
   
    //R-type
    if(instructionWord == "add"){
        add(reg_to_int(infoParsed[0]), reg_to_int(infoParsed[1]), reg_to_int(infoParsed[2]));
    }
    else if (instructionWord == "sub"){
        sub(reg_to_int(infoParsed[0]), reg_to_int(infoParsed[1]), reg_to_int(infoParsed[2]));
    }
    else if (instructionWord == "or"){
        _or(reg_to_int(infoParsed[0]), reg_to_int(infoParsed[1]), reg_to_int(infoParsed[2]));
    }
    else if (instructionWord == "and"){
        _and(reg_to_int(infoParsed[0]), reg_to_int(infoParsed[1]), reg_to_int(infoParsed[2]));
    }
    else if (instructionWord == "xor"){
        _xor(reg_to_int(infoParsed[0]), reg_to_int(infoParsed[1]), reg_to_int(infoParsed[2]));
    }
    else if (instructionWord == "sll"){
        sll(reg_to_int(infoParsed[0]), reg_to_int(infoParsed[1]), reg_to_int(infoParsed[2]));
    }
    else if (instructionWord == "srl"){
        infoParsed = getCommaSperated(insructionInfo);
        srl(reg_to_int(infoParsed[0]), reg_to_int(infoParsed[1]), reg_to_int(infoParsed[2]));
    }
    else if (instructionWord == "sra"){
        sra(reg_to_int(infoParsed[0]), reg_to_int(infoParsed[1]), reg_to_int(infoParsed[2]));
    }
    else if (instructionWord == "slt"){
        slt(reg_to_int(infoParsed[0]), reg_to_int(infoParsed[1]), reg_to_int(infoParsed[2]));
    }
    else if (instructionWord == "sltu"){
        sltu(reg_to_int(infoParsed[0]), reg_to_int(infoParsed[1]), reg_to_int(infoParsed[2]));
    }
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
        cout<<"Instructin "<<instructionWord<<" is not supported. The program will terminate\n";
        system("pause");
        exit(1);
    }

}


//parses the Load and Store instructions
vector<string> parseParenthesis(string instructionInfo){
    vector<string> InfoParsed = getCommaSperated(instructionInfo);
    string imm = InfoParsed[1].substr(0, InfoParsed[1].find('('));
    strip(imm);
    if(imm.empty())
        imm="0";
    string secondReg = InfoParsed[1].substr(InfoParsed[1].find('(')+1);
    secondReg.pop_back();//poping the closing paranthesis
    strip(secondReg);

    InfoParsed.pop_back();
    InfoParsed.push_back(secondReg);
    InfoParsed.push_back(imm);

    return InfoParsed;
}


void checkLabelExists(string label){
    if(labelToAddress.find(label) ==labelToAddress.end()){
        cout<<"You are trying to jump to a label that does not exist. The program will terminate\n";
        system("pause");
        exit(1);
    }
}


/////////////////////////////////////////////Definitions///////////////////////////////////////////////////////////////////

// R-type
void add (int rd, int rs1, int rs2)
{
    if (rd == 0)
    {
        return;
    }
    else
    {
        registers[rd] = registers[rs1] + registers[rs2];
    }

    PC += 4;
}

void sub (int rd, int rs1, int rs2)
{
    if (rd == 0)
    {
        return;
    }
    else
    {
        registers[rd] = registers[rs1] - registers[rs2];
    }

    PC += 4;
}

void _or (int rd, int rs1, int rs2)
{
    if (rd == 0)
    {
        return;
    }
    else
    {
        registers[rd] = registers[rs1] | registers[rs2];
    }

    PC += 4;
}

void _and (int rd, int rs1, int rs2)
{
    if (rd == 0)
    {
        return;
    }
    else
    {
        registers[rd] = registers[rs1] & registers[rs2];
    }

    PC += 4;
}

void _xor (int rd, int rs1, int rs2)
{
    if (rd == 0)
    {
        return;
    }
    else
    {
        registers[rd] = registers[rs1] ^ registers[rs2];
    }

    PC += 4;
}

void sll (int rd, int rs1, int rs2) // unsigned
{
    if (rd == 0)
    {
        return;
    }
    else if (registers[rs2] > 31 | registers[rs2] < 0)
    {
        cout<<"You are trying to shift left by more than 31 bits in the sll instruction\nthe program will terminate\n";
        system("pause");
        exit(1);
    }
    else
    {
        registers[rd] = registers[rs1] << registers[rs2];
    }

    PC += 4;
}

void srl (int rd, int rs1, int rs2) // unsigned
{
    if (rd == 0)
    {
        return;
    }
    else if (registers[rs2] > 31 | registers[rs2] < 0)
    {
        cout<<"You are trying to shift right by more than 31 bits in the srl instruction\nthe program will terminate\n";
         system("pause");
        exit(1);
    }
    else 
    {
        registers[rd] = (unsigned int) registers[rs1] >> registers[rs2];
    }

    PC += 4;
}

void sra (int rd, int rs1, int rs2) // sign extend
{
    if (rd == 0)
    {
        return;
    }
    else if (registers[rs2] > 31 | registers[rs2] < 0)
    {
        cout<<"You are trying to shift right by more than 31 bits in the sra instruction\nthe program will terminate\n";
         system("pause");
        exit(1);
    }
    else
    {
        registers[rd] = registers[rs1] >> registers[rs2];
    }

    PC += 4;
}

void slt (int rd, int rs1, int rs2)
{
    if (rd == 0)
    {
        return;
    }
    if(registers[rs1] < registers[rs2])
    {
        registers[rd] = 1;
    }
    else
    {
        registers[rd] = 0;
    }
    PC += 4;
}

void sltu (int rd, int rs1, int rs2)
{
    if (rd == 0)
    {
        return;
    }
    if((unsigned int)registers[rs1] < (unsigned int)registers[rs2])
    {
        registers[rd] = 1;
    }
    else
    {
        registers[rd] = 0;
    }
    PC += 4;
}


// I-type
void addi (int rd, int rs1, int imm)
{
    if (rd == 0)
    {
        return;
    }
    else if (!in_range(imm, 12))
    {
        cout<<"You are trying to input more than 12 bits for immediate in the addi instruction\nthe program will terminate\n";
         system("pause");
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
        cout<<"You are trying to input more than 12 bits for immediate in the andi instruction\nthe program will terminate\n";
         system("pause");
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
        cout<<"You are trying to input more than 12 bits for immediate in the ori instruction\nthe program will terminate\n";
         system("pause");
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
        cout<<"You are trying to input more than 12 bits for immediate in the xori instruction\nthe program will terminate\n";
         system("pause");
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
        cout<<"You are trying to shift left by more than 31 bits in the slli instruction\nthe program will terminate\n";
         system("pause");
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
        cout<<"You are trying to shift right by more than 31 bits in the srli instruction\nthe program will terminate\n";
         system("pause");
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
        cout<<"You are trying to shift right by more than 31 bits in the srai instruction\nthe program will terminate\n";
         system("pause");
        exit(1);
    }
    else
    {
        registers[rd] = registers[rs1] >> imm;
    }

    PC += 4;
}

void jalr (int rd, int rs1, int imm)
{
    if (rd != 0)
    {
        registers[rd] = PC + 4;
    }
    if (!in_range(imm, 12))
    {
        cout<<"You are trying to input more than 12 bits for immediate in the jalr instruction\nthe program will terminate\n";
         system("pause");
        exit(1);
    }
    else
    {
        PC = registers[rs1] + imm;
    }
}


void lw (int rd, int rs1, int imm)
{
    if (rd == 0)
    {
        return;
    }
    if (!in_range(imm, 12))
    {
        cout<<"You are trying to input more than 12 bits for immediate in the lw instruction\nthe program will terminate\n";
         system("pause");
        exit(1);
    }

    int address = registers[rs1] + imm;
		
    if (memory.find(address) != memory.end())
    {
        registers[rd] = memory[address];
    }
    else
    {
        cout<<"You are trying to load from a non-allocated address in the xori instruction\nthe program will terminate\n";
        system("pause");
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
        int address = registers[rs1] + imm - r;
		
        if (memory.find(address) != memory.end())
        {
            registers[rd] = (memory[address] << (8 * (2 - r))) >> 16;
        }
        else
        {
            cout<<"You are trying to load from a non-allocated address in the lh instruction\nthe program will terminate\n";
             system("pause");
            exit(1);
        }
    }
    else
    {
        cout<<"You are trying to input more than 12 bits for immediate in the lh instruction\nthe program will terminate\n";
         system("pause");
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
            cout<<"You are trying to load from a non-allocated address in the lb instruction\nthe program will terminate\n";
             system("pause");
            exit(1);
        }
    }
    else
    {
        cout<<"You are trying to input more than 12 bits for immediate in the lb instruction\nthe program will terminate\n";
         system("pause");
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
            cout<<"You are trying to load from a non-allocated address in the lhu instruction\nthe program will terminate\n";
             system("pause");
            exit(1);
        }
    }
    else
    {
        cout<<"You are trying to input more than 12 bits for immediate in the lhu instruction\nthe program will terminate\n";
         system("pause");
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
            cout<<"You are trying to load from a non-allocated address in the lbu instruction\nthe program will terminate\n";
             system("pause");
            exit(1);
        }
    }
    else
    {
        cout<<"You are trying to input more than 12 bits for immediate in the lbu instruction\nthe program will terminate\n";
         system("pause");
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
        cout<<"You are trying to input more than 12 bits for immediate in the slti instruction\nthe program will terminate\n";
         system("pause");
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
        cout<<"You are trying to input more than 12 bits for immediate in the sltiu instruction\nthe program will terminate\n";
         system("pause");
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
        cout<<"You are trying to store a word into an address not divisable by 4\nthe program will terminate\n";
         system("pause");
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
        cout<<"You are trying to store a half word into an address not divisable by 2\nthe program will terminate\n";
         system("pause");
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

// B-type
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

void beq (int rs1, int rs2, int offset) // ver2 --> based on offset
{
    if (registers[rs1] == registers[rs2])
    {
        if (in_range(offset, 12))
        {
            PC = PC + 2*offset;
        }
        else
        {
            cout<<"You are trying to input more than 12 bits for offset in the beq ver2 instruction\nthe program will terminate\n";
             system("pause");
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

void bne (int rs1, int rs2, int offset) // ver2 --> based on offset
{
    if (registers[rs1] != registers[rs2])
    {
        if (in_range(offset, 12))
        {
            PC = PC + 2*offset;
        }
        else
        {
            cout<<"You are trying to input more than 12 bits for offset in the bne ver2 instruction\nthe program will terminate\n";
             system("pause");
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

void blt (int rs1, int rs2, int offset) // ver2 --> based on offset
{
    if (registers[rs1] < registers[rs2])
    {
        if (in_range(offset, 12))
        {
            PC = PC + 2*offset;
        }
        else
        {
            cout<<"You are trying to input more than 12 bits for offset in the blt ver2 instruction\nthe program will terminate\n";
             system("pause");
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

void bltu (int rs1, int rs2, int offset) // ver2 --> based on offset
{
    if ((unsigned int)registers[rs1] < (unsigned int)registers[rs2])
    {
        if (in_range(offset, 12))
        {
            PC = PC + 2*offset;
        }
        else
        {
            cout<<"You are trying to input more than 12 bits for offset in the bltu ver2 instruction\nthe program will terminate\n";
             system("pause");
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

void bge (int rs1, int rs2, int offset) // ver2 --> based on offset
{
    if (registers[rs1] >= registers[rs2])
    {
        if (in_range(offset, 12))
        {
            PC = PC + 2*offset;
        }
        else
        {
            cout<<"You are trying to input more than 12 bits for offset in the bge ver2 instruction\nthe program will terminate\n";
             system("pause");
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

void bgeu (int rs1, int rs2, int offset) // ver2 --> based on offset
{
    if ((unsigned int)registers[rs1] >= (unsigned int)registers[rs2])
    {
        if (in_range(offset, 12))
        {
            PC = PC + 2*offset;
        }
        else
        {
            cout<<"You are trying to input more than 12 bits for offset in the begu ver2 instruction\nthe program will terminate\n";
             system("pause");
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
    if (rd != 0)
    {
        registers[rd] = PC + 4;
    }

    PC = labelToAddress[label];

}

void jal (int rd, int offset) // ver2 --> jump based on offset
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
        cout<<"You are trying to input more than 20 bits for offset in the jal ver2 instruction\nthe program will terminate\n";
         system("pause");
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
        cout<<"You are trying to input more than 20 bits from immidiate in the lui instruction\nthe program will terminate\n";
         system("pause");
        exit(1);
    }
    PC += 4;
}

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
        cout<<"You are trying to input more than 20 bits from immidiate in the auip instruction\nthe program will terminate\n";
         system("pause");
        exit(1);
    }
    PC += 4;
}