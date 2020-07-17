#include "AS2MC.h"
#include "MIPSVM.h"

#include <bitset>
#include <fstream>
#include <iostream>
#include <string>
#include <vector>
#include <windows.h>
#include <conio.h>

#define KEY_DOWN(VK_NONAME) ((GetAsyncKeyState(VK_NONAME) & 0x8000) ? 1:0)

// 可以先Write机器码到内存中用Run执行
// 也可直接在operator()函数内顺序执行,默认不会将指令写入，也不会改变PC值
// TODO：测试指令正确性 实现汇编语言读入
class m_Code : public MIPSVM::Code {
public:
    m_Code() = default;
    void Run() {
        while (mem(PC)) {
            MCode mc(mem(PC));

            //取指
            std::cout << "PC=" << PC << std::endl;
            std::cout << "PC->AR AR=" << PC << std::endl;
            std::cout << "DR=mem(" << PC << ")" << std::endl;
            std::cout << "DR->IR" << std::endl;
            std::cout << "Command:" << std::bitset<32>(mem(PC)) << std::endl;

            //执行
            if (mc.R_type.opcode == OP_CODE_R_TYPE) {
                auto f = funct_to_R_type_fcn[mc.R_type.funct];
                if (f == nullptr) {
                    throw std::runtime_error("Unsupport Instruction!");
                }
                (this->*f)(mc.R_type.rs, mc.R_type.rt, mc.R_type.rd,
                    mc.R_type.shamt);
                switch (mc.R_type.funct) {
                case FUNC_SLL:
                    std::cout << "Command:sll Reg" << mc.R_type.rd
                        << ", Reg" << mc.R_type.rt << ", "
                        << mc.R_type.shamt << std::endl;
                    std::cout << "Reg" << mc.R_type.rt << "="
                        << Reg(mc.R_type.rt) << std::endl;
                    std::cout << "Reg" << mc.R_type.rt << "<<"
                        << mc.R_type.shamt << "=" << Reg(mc.R_type.rd)
                        << std::endl;
                    std::cout << "Reg" << mc.R_type.rd << "="
                        << Reg(mc.R_type.rd) << std::endl;
                    break;
                case FUNC_SRL:
                case FUNC_SRA:
                    std::cout << "Command:srl Reg" << mc.R_type.rd
                        << ", Reg" << mc.R_type.rt << ", "
                        << mc.R_type.shamt << std::endl;
                    std::cout << "Reg" << mc.R_type.rt << "="
                        << Reg(mc.R_type.rt) << std::endl;
                    std::cout << "Reg" << mc.R_type.rt << ">>"
                        << mc.R_type.shamt << "=" << Reg(mc.R_type.rd)
                        << std::endl;
                    std::cout << "Reg" << mc.R_type.rd << "="
                        << Reg(mc.R_type.rd) << std::endl;
                    break;
                case FUNC_SLLV:
                    std::cout << "Command:sllv Reg" << mc.R_type.rd
                        << ", Reg" << mc.R_type.rt << ", Reg"
                        << mc.R_type.rs << std::endl;
                    std::cout << "Reg" << mc.R_type.rt << "="
                        << Reg(mc.R_type.rt) << ",Reg" << mc.R_type.rs
                        << "=" << Reg(mc.R_type.rs) << std::endl;
                    std::cout << "Reg" << mc.R_type.rt << "<<"
                        << "Reg" << mc.R_type.rs << "="
                        << Reg(mc.R_type.rd) << std::endl;
                    std::cout << "Reg" << mc.R_type.rd << "="
                        << Reg(mc.R_type.rd) << std::endl;
                    break;
                case FUNC_SRLV:
                case FUNC_SRAV:
                    std::cout << "Command:srlv Reg" << mc.R_type.rd
                        << ", Reg" << mc.R_type.rt << ", Reg"
                        << mc.R_type.rs << std::endl;
                    std::cout << "Reg" << mc.R_type.rt << "="
                        << Reg(mc.R_type.rt) << ",Reg" << mc.R_type.rs
                        << "=" << Reg(mc.R_type.rs) << std::endl;
                    std::cout << "Reg" << mc.R_type.rt << ">>"
                        << "Reg" << mc.R_type.rs << "="
                        << Reg(mc.R_type.rd) << std::endl;
                    std::cout << "Reg" << mc.R_type.rd << "="
                        << Reg(mc.R_type.rd) << std::endl;
                    break;
                case FUNC_JR:
                    std::cout << "Command:jr Reg" << mc.R_type.rs
                        << std::endl;
                    std::cout << "Reg" << mc.R_type.rs << "="
                        << Reg(mc.R_type.rs) << std::endl;
                    PC = Reg(mc.R_type.rs);
                    std::cout << "PC=" << PC << std::endl;
                    PC -= 4;
                    break;
                case FUNC_ADD:
                case FUNC_ADDU:
                    std::cout << "Command:add Reg" << mc.R_type.rd
                        << ", Reg" << mc.R_type.rs << ", Reg"
                        << mc.R_type.rt << std::endl;
                    if (mc.R_type.rd != mc.R_type.rs &&
                        mc.R_type.rd != mc.R_type.rt)
                        std::cout << "Reg" << mc.R_type.rs << "="
                        << Reg(mc.R_type.rs) << ",Reg"
                        << mc.R_type.rt << "="
                        << Reg(mc.R_type.rt) << std::endl;
                    std::cout << "Reg" << mc.R_type.rs << "+"
                        << "Reg" << mc.R_type.rt << "="
                        << Reg(mc.R_type.rd)
                        << " overflow=0,Regwrite=1" << std::endl;
                    std::cout << "Reg" << mc.R_type.rd << "="
                        << Reg(mc.R_type.rd) << std::endl;
                    break;
                case FUNC_SUB:
                case FUNC_SUBU:
                    std::cout << "Command:sub Reg" << mc.R_type.rd
                        << ", Reg" << mc.R_type.rs << ", Reg"
                        << mc.R_type.rt << std::endl;
                    if (mc.R_type.rd != mc.R_type.rs &&
                        mc.R_type.rd != mc.R_type.rt)
                        std::cout << "add Reg" << mc.R_type.rd << ", Reg"
                        << mc.R_type.rs << ", Reg" << mc.R_type.rt
                        << std::endl;
                    std::cout << "Reg" << mc.R_type.rs << "="
                        << Reg(mc.R_type.rs) << ",Reg" << mc.R_type.rt
                        << "=" << Reg(mc.R_type.rt) << std::endl;
                    std::cout << "Reg" << mc.R_type.rs << "-"
                        << "Reg" << mc.R_type.rt << "="
                        << Reg(mc.R_type.rd)
                        << " overflow=0,Regwrite=1" << std::endl;
                    std::cout << "Reg" << mc.R_type.rd << "="
                        << Reg(mc.R_type.rd) << std::endl;
                    break;
                case FUNC_AND:
                    std::cout << "Command:and Reg" << mc.R_type.rd
                        << ", Reg" << mc.R_type.rs << ", Reg"
                        << mc.R_type.rt << std::endl;
                    if (mc.R_type.rd != mc.R_type.rs &&
                        mc.R_type.rd != mc.R_type.rt)
                        std::cout << "Reg" << mc.R_type.rs << "="
                        << Reg(mc.R_type.rs) << ",Reg"
                        << mc.R_type.rt << "="
                        << Reg(mc.R_type.rt) << std::endl;
                    std::cout << "Reg" << mc.R_type.rs << "&"
                        << "Reg" << mc.R_type.rt << "="
                        << Reg(mc.R_type.rd) << std::endl;
                    std::cout << "Reg" << mc.R_type.rd << "="
                        << Reg(mc.R_type.rd) << std::endl;
                    break;
                case FUNC_OR:
                    std::cout << "Command:or Reg" << mc.R_type.rd << ", Reg"
                        << mc.R_type.rs << ", Reg" << mc.R_type.rt
                        << std::endl;
                    if (mc.R_type.rd != mc.R_type.rs &&
                        mc.R_type.rd != mc.R_type.rt)
                        std::cout << "Reg" << mc.R_type.rs << "="
                        << Reg(mc.R_type.rs) << ",Reg"
                        << mc.R_type.rt << "="
                        << Reg(mc.R_type.rt) << std::endl;
                    std::cout << "Reg" << mc.R_type.rs << "|"
                        << "Reg" << mc.R_type.rt << "="
                        << Reg(mc.R_type.rd) << std::endl;
                    std::cout << "Reg" << mc.R_type.rd << "="
                        << Reg(mc.R_type.rd) << std::endl;
                    break;
                case FUNC_XOR:
                    std::cout << "Command:xor Reg" << mc.R_type.rd
                        << ", Reg" << mc.R_type.rs << ", Reg"
                        << mc.R_type.rt << std::endl;
                    if (mc.R_type.rd != mc.R_type.rs &&
                        mc.R_type.rd != mc.R_type.rt)
                        std::cout << "Reg" << mc.R_type.rs << "="
                        << Reg(mc.R_type.rs) << ",Reg"
                        << mc.R_type.rt << "="
                        << Reg(mc.R_type.rt) << std::endl;
                    std::cout << "Reg" << mc.R_type.rs << "^"
                        << "Reg" << mc.R_type.rt << "="
                        << Reg(mc.R_type.rd) << std::endl;
                    std::cout << "Reg" << mc.R_type.rd << "="
                        << Reg(mc.R_type.rd) << std::endl;
                    break;
                case FUNC_NOR:
                    std::cout << "Command:nor Reg" << mc.R_type.rd
                        << ", Reg" << mc.R_type.rs << ", Reg"
                        << mc.R_type.rt << std::endl;
                    if (mc.R_type.rd != mc.R_type.rs &&
                        mc.R_type.rd != mc.R_type.rt)
                        std::cout << "Reg" << mc.R_type.rs << "="
                        << Reg(mc.R_type.rs) << ",Reg"
                        << mc.R_type.rt << "="
                        << Reg(mc.R_type.rt) << std::endl;
                    std::cout << "Reg" << mc.R_type.rs << "nor"
                        << "Reg" << mc.R_type.rt << "="
                        << Reg(mc.R_type.rd) << std::endl;
                    std::cout << "Reg" << mc.R_type.rd << "="
                        << Reg(mc.R_type.rd) << std::endl;
                    break;
                case FUNC_SLT:
                case FUNC_SLTU:
                    std::cout << "Command:slt Reg" << mc.R_type.rd
                        << ", Reg" << mc.R_type.rs << ", Reg"
                        << mc.R_type.rt << std::endl;
                    std::cout << "Reg" << mc.R_type.rs << "="
                        << Reg(mc.R_type.rs) << ",Reg" << mc.R_type.rt
                        << "=" << Reg(mc.R_type.rt) << std::endl;
                    std::cout << "Reg" << mc.R_type.rs << "<"
                        << "Reg" << mc.R_type.rt << "?" << std::endl;
                    if (Reg(mc.R_type.rd))
                        std::cout << "True,Reg" << mc.R_type.rd << "=1"
                        << std::endl;
                    else
                        std::cout << "True,Reg" << mc.R_type.rd << "=0"
                        << std::endl;
                    break;
                default:
                    break;
                }
            }
            else if (mc.J_type.opcode == OP_CODE_J ||
                mc.J_type.opcode == OP_CODE_JAL|| mc.J_type.opcode == OP_CODE_INTI) {
                auto f = opcode_to_J_type_fcn[mc.J_type.opcode];
                if (mc.J_type.opcode == OP_CODE_JAL)
                    std::cout << "Command:jal " << mc.J_type.address
                    << std::endl;
                else if(mc.J_type.opcode == OP_CODE_J)
                    std::cout << "Command:j " << mc.J_type.address << std::endl;
                else
                    std::cout << "Command:inti " << mc.J_type.address << std::endl;

                std::cout << "Reg(ra)=" << Reg(ra) << std::endl;

                (this->*f)(mc.J_type.address);//实现 ra = PC + 4

                PC = mc.J_type.address - 4;
                std::cout << "PC = ((PC + 4) & 0xf0000000) | (("
                    << mc.J_type.address
                    << " & 0x03ffffff) << 2)=" << PC + 4 << std::endl;

                if (mc.J_type.opcode == OP_CODE_JAL|| mc.J_type.opcode == OP_CODE_INTI)
                    std::cout << "Reg(ra)=" << Reg(ra) << std::endl;
                if (mc.J_type.opcode == OP_CODE_INTI)
                    std::cout << std::endl << "中断响应结束，进入中断服务" << std::endl;
            }
            else {//I-type
                auto f = opcode_to_I_type_fcn[mc.I_type.opcode];

                if (f == nullptr) {
                    throw std::runtime_error("Unsupport Instruction!");
                }
                (this->*f)(mc.I_type.rt, mc.I_type.rs, mc.I_type.imm);
                switch (mc.I_type.opcode) {
                case OP_CODE_BEQ:
                    std::cout << "Command:beq Reg" << mc.I_type.rs
                        << ", Reg" << mc.I_type.rt << ", "
                        << mc.I_type.imm << std::endl;
                    std::cout << "Reg" << mc.I_type.rs << "="
                        << Reg(mc.I_type.rs) << ",Reg" << mc.I_type.rt
                        << "=" << Reg(mc.I_type.rt) << std::endl;
                    std::cout << "Reg" << mc.I_type.rt << "==Reg"
                        << mc.I_type.rs << "?" << std::endl;
                    if (Reg(mc.I_type.rs) == Reg(mc.I_type.rt))
                        std::cout << "True,PC+4+expand(imm)=" << PC + 4
                        << std::endl;
                    else
                        std::cout << "False" << std::endl;
                    break;
                case OP_CODE_BNE:
                    std::cout << "Command:bne Reg" << mc.I_type.rs
                        << ", Reg" << mc.I_type.rt << ", "
                        << mc.I_type.imm << std::endl;
                    std::cout << "Reg" << mc.I_type.rs << "="
                        << Reg(mc.I_type.rs) << ",Reg" << mc.I_type.rt
                        << "=" << Reg(mc.I_type.rt) << std::endl;
                    std::cout << "Reg" << mc.I_type.rt << "!=Reg"
                        << mc.I_type.rs << "?" << std::endl;
                    if (Reg(mc.I_type.rs) != Reg(mc.I_type.rt))
                        std::cout << "True,PC+4+expand(imm)=" << PC + 4
                        << std::endl;
                    else
                        std::cout << "False" << std::endl;
                    break;
                case OP_CODE_ADDI:
                case OP_CODE_ADDIU:
                    std::cout << "Command:addi Reg" << mc.I_type.rt
                        << ", Reg" << mc.I_type.rs << ", "
                        << mc.I_type.imm << std::endl;
                    if (mc.I_type.rt != mc.I_type.rs)
                        std::cout << "Reg" << mc.I_type.rs << "="
                        << Reg(mc.I_type.rs) << std::endl;
                    std::cout << "Reg" << mc.I_type.rs << "+"
                        << mc.I_type.imm << "=" << Reg(mc.I_type.rt)
                        << " overflow=0,Regwrite=1" << std::endl;
                    std::cout << "Reg" << mc.I_type.rt << "="
                        << Reg(mc.I_type.rt) << std::endl;
                    break;
                case OP_CODE_SLTI:
                case OP_CODE_SLTIU:
                    std::cout << "Command:slti Reg" << mc.I_type.rs
                        << ", Reg" << mc.I_type.rt << ", "
                        << mc.I_type.imm << std::endl;
                    std::cout << "Reg" << mc.I_type.rs << "="
                        << Reg(mc.I_type.rs) << std::endl;
                    std::cout << "Reg" << mc.I_type.rs << "<"
                        << mc.I_type.imm << "?" << std::endl;
                    if (Reg(mc.I_type.rt))
                        std::cout << "True,Reg" << mc.I_type.rt << "=1"
                        << std::endl;
                    else
                        std::cout << "True,Reg" << mc.I_type.rt << "=0"
                        << std::endl;
                    break;
                case OP_CODE_ANDI:
                    std::cout << "Command:andi Reg" << mc.I_type.rt
                        << ", Reg" << mc.I_type.rs << ", "
                        << mc.I_type.imm << std::endl;
                    if (mc.I_type.rt != mc.I_type.rs)
                        std::cout << "Reg" << mc.I_type.rs << "="
                        << Reg(mc.I_type.rs) << std::endl;
                    std::cout << "Reg" << mc.I_type.rs << "&"
                        << mc.I_type.imm << "=" << Reg(mc.I_type.rt)
                        << std::endl;
                    std::cout << "Reg" << mc.I_type.rt << "="
                        << Reg(mc.I_type.rt) << std::endl;
                    break;
                case OP_CODE_ORI:
                    std::cout << "Command:ori Reg" << mc.I_type.rt
                        << ", Reg" << mc.I_type.rs << ", "
                        << mc.I_type.imm << std::endl;
                    if (mc.I_type.rt != mc.I_type.rs)
                        std::cout << "Reg" << mc.I_type.rs << "="
                        << Reg(mc.I_type.rs) << std::endl;
                    std::cout << "Reg" << mc.I_type.rs << "|"
                        << mc.I_type.imm << "=" << Reg(mc.I_type.rt)
                        << std::endl;
                    std::cout << "Reg" << mc.I_type.rt << "="
                        << Reg(mc.I_type.rt) << std::endl;
                    break;
                case OP_CODE_XORI:
                    std::cout << "Command:xori Reg" << mc.I_type.rt
                        << ", Reg" << mc.I_type.rs << ", "
                        << mc.I_type.imm << std::endl;
                    if (mc.I_type.rt != mc.I_type.rs)
                        std::cout << "Reg" << mc.I_type.rs << "="
                        << Reg(mc.I_type.rs) << std::endl;
                    std::cout << "Reg" << mc.I_type.rs << "^"
                        << mc.I_type.imm << "=" << Reg(mc.I_type.rt)
                        << std::endl;
                    std::cout << "Reg" << mc.I_type.rt << "="
                        << Reg(mc.I_type.rt) << std::endl;
                    break;
                case OP_CODE_LUI:
                    std::cout << "Command:lui Reg" << mc.I_type.rt << ", "
                        << mc.I_type.imm << std::endl;
                    std::cout << "Reg" << mc.I_type.rt << "="
                        << mc.I_type.imm
                        << "<<16=" << Reg(mc.I_type.rt) << std::endl;
                    break;
                case OP_CODE_LW:
                    std::cout << "Command:lw Reg" << mc.I_type.rt << ", "
                        << mc.I_type.imm << "(Reg" << mc.I_type.rs
                        << ")" << std::endl;
                    std::cout << "Reg" << mc.I_type.rs << "="
                        << Reg(mc.I_type.rs) << std::endl;
                    std::cout
                        << "Reg" << mc.I_type.rs << "+expand(imm)="
                        << Reg(mc.I_type.rs) +
                        static_cast<int32_t>(
                            static_cast<int16_t>(mc.I_type.imm))
                        << std::endl;
                    std::cout
                        << "Reg" << mc.I_type.rt
                        << "=mem(rs+expand(imm))=" << Reg(mc.I_type.rt)
                        << std::endl;
                    break;
                case OP_CODE_SW:
                    std::cout << "Command:sw Reg" << mc.I_type.rt << ", "
                        << mc.I_type.imm << "(Reg" << mc.I_type.rs
                        << ")" << std::endl;
                    std::cout << "Reg" << mc.I_type.rs << "="
                        << Reg(mc.I_type.rs) << std::endl;
                    std::cout << "expand(imm)="
                        << static_cast<int32_t>(
                            static_cast<int16_t>(mc.I_type.imm))
                        << std::endl;
                    std::cout
                        << "Reg" << mc.I_type.rs << "+expand(imm)="
                        << Reg(mc.I_type.rs) +
                        int(static_cast<int32_t>(
                            static_cast<int16_t>(mc.I_type.imm)))
                        << std::endl;
                    std::cout
                        << "mem(rs+expand(imm))=rt=" << Reg(mc.I_type.rt)
                        << std::endl;
                    break;
                case OP_CODE_DISP:
                    std::cout << "Command:disp" << std::endl;
                    if (mc.I_type.imm == 1) std::cout << "测试中断" << std::endl;
                    if (mc.I_type.imm == 2) std::cout << "中断服务" << std::endl;
                    if (mc.I_type.imm == 3) std::cout << "中断响应结束，进入中断服务" << std::endl;
                    if (mc.I_type.imm == 4) std::cout << "中断了，成功" << std::endl;
                    if (mc.I_type.imm == 5) std::cout << "测试结束" << std::endl;
                    break;
                case OP_CODE_EI:
                    std::cout << "Command:ei " << std::endl;
                    std::cout << "EI=" << Code::Reg::IE << std::endl;
                    break;

                default:
                    break;
                }
            }
            PC += 4;
            std::cout << std::endl;            

            //公操作            
            if (KEY_DOWN(VK_LBUTTON) || KEY_DOWN(VK_RBUTTON)|| _kbhit()) {
                if (Code::Reg::canInt) {
                    Code::Reg::canInt = 0;
                    Code::Reg::inta = 1;
                    char ch;                    
                    if(KEY_DOWN(VK_LBUTTON) || KEY_DOWN(VK_RBUTTON))
                        std::cout << "鼠标按下，进入中断" << std::endl;
                    else {
                        ch = _getch();
                        std::cout << "键盘按下 "<<ch<<"，进入中断" << std::endl;
                    }                        
                    std::cout << "**************************************" << std::endl;
                    if (Code::Reg::inta == 1 && !Code::Reg::IE)
                        Interrupt();//执行中断
                }                
            }
            Sleep(500);//减慢程序执行速度便于观察
        }
        
        std::cout << "打印存储器中的结果:" << std::endl;
        int l = MIPSVM::Ans.size();
        for (int i = 0; i < l; ++i) {
            std::cout << "Mem[" << 1000 + i * 4 << "]="
                << MIPSVM::Ans[i] << std::endl;
        }
        std::cout << std::endl;
    }
    //将机器码写入当前PC,并将PC加4
    void Store(uint32_t inst) {//中断使用
        uint32_t pos_to_store = PC;
        mem(pos_to_store) = inst;
        PC += 4;        
    }
    void Write(uint32_t inst) {//执行使用
        static uint32_t pos_to_write = PC;
        mem(pos_to_write) = inst;
        pos_to_write += 4;
    }
};

int main(int argc, char** argv) {
    // usage: program.exe [fileName]
    if (argc >= 2) {//命令行运行文件
        m_Code c;
        std::string line;
        bool isMC;
        if (argc > 2) {
            //中断服务程序
            std::ifstream load(argv[2]);
            if (!load) {
                std::cerr << "Can't open file: " << argv[2] << std::endl;
                return -1;
            }
            std::getline(load, line);
            if (line.length() < 3 || line[0] != '#' ||
                (line.substr(1, 2) != "AS" && line.substr(1, 2) != "MC")) {//文件头部错误
                std::cerr << "File header must be #MC, #AS or #IR"
                    << std::endl;
                return -1;
            }
            isMC = line.substr(1, 2) == "MC";
            while (std::getline(load, line)) {//读文件每一行
                if (isdigit((int)line[0])) {//设置地址
                    uint32_t address = atoi(line.c_str());
                    c.ResetPC(address);
                    continue;
                }
                if (isMC && line.length() != 32) {//机器码位数错误
                    std::cerr << "Please input 32bit machine code per line! "
                        << std::endl;
                    return -1;
                }
                std::bitset<32> bits;
                try {
                    if (isMC) {//提取机器语言
                        bits = std::bitset<32>(line);
                    }
                    else {//汇编语言                    
                        uint32_t nc = AS2MC::trans(line);//转为机器码
                        bits = std::bitset<32>(nc);
                    }
                }

                catch (std::exception & e) {
                    std::cerr << e.what() << std::endl;
                    return -1;
                }
                c.Store(bits.to_ulong());//从PC开始将指令写入mem
                auto pc = c.GetPC();
            }
        }

        //测试文件
        c.ResetPC(200);
        std::ifstream fin(argv[1]);//获取文件名称
        if (!fin) {//文件无法打开
            std::cerr << "Can't open file: " << argv[1] << std::endl;
            return -1;
        }
        std::getline(fin, line);
        if (line.length() < 3 || line[0] != '#' ||
            (line.substr(1, 2) != "AS" && line.substr(1, 2) != "MC")) {//文件头部错误
            std::cerr << "File header must be #MC, #AS or #IR"
                << std::endl;
            return -1;
        }
        isMC = line.substr(1, 2) == "MC";
        while (std::getline(fin, line)) {//读文件每一行
            if (isMC && line.length() != 32) {//机器码位数错误
                std::cerr << "Please input 32bit machine code per line! "
                    << std::endl;
                return -1;
            }
            std::bitset<32> bits;
            try {
                if (isMC) {//提取机器语言
                    bits = std::bitset<32>(line);
                }
                else {//汇编语言
                    if (line.substr(0, 3) == "jal") {
                        std::cout << "please input the result of the interupt vector:" << std::endl;
                        std::string address; std::cin >> address;
                        line += address;
                    }
                    uint32_t nc = AS2MC::trans(line);//转为机器码
                    bits = std::bitset<32>(nc);
                }
            }
            catch (std::exception & e) {
                std::cerr << e.what() << std::endl;
                return -1;
            }
            c.Write(bits.to_ulong());
            auto pc = c.GetPC();
            c.Run();
        }
    }
    return 0;
}
