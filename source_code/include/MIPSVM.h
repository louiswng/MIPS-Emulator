#pragma once
#include <iostream>
#include <memory>
#include <stdexcept>
#include <unordered_map>
#include <vector>
const int MAXN = 10;
namespace MIPSVM {
    extern std::vector<int> Ans;
    extern int inputcount;
    
    class Code {
    public:
        const uint32_t Max_idx = 1 << 28;
        Code() : memory(new uint32_t[Max_idx]{}) {//初始化列表来初始化内存空间
            Reg::ResetReg();
            //ResetPC(0);
        };
        //获取当前PC值
        uint32_t GetPC() { return PC; }
        //获取idx位置的内存值
        uint32_t& mem(uint32_t idx) {
            idx /= 4;
            idx %= Max_idx;
            return memory[idx];
        }
        //重置当前PC值
        void ResetPC(uint32_t);

        struct Reg {
            //中断
            static int IE; //= 0中断允许，初值不允许中断
            static int EXL, ERL;//任何一个=1，禁止中断
            static int excpHdAddre[MAXN];//定义中断向量表首地址
            static int inta;//中断响应
            static int canInt;//单级中断

            uint8_t idx = 0;
            //隐式构造函数,以便传参时直接传入寄存器的序号
            Reg(uint8_t val) : idx(val) {
                if (reg == nullptr) {
                    reg =
                        std::move(std::unique_ptr<uint32_t[]>(new uint32_t[32]{}));
                }
                if (val >= 32) {
                    throw std::invalid_argument("Only have 32 Register !");
                }
            }
            //重载赋值操作符
            Reg& operator=(const Reg& rhs) {
                if (idx) {
                    reg[idx] = reg[rhs.idx];
                }
                else {
                    throw std::runtime_error("Can not set Zero Register!");
                }
                return *this;
            }
            Reg& operator=(uint32_t rhs) {
                if (idx) {
                    reg[idx] = rhs;
                }
                else {
                    throw std::runtime_error("Can not set Zero Register!");
                }
                return *this;
            }
            operator uint32_t() const { return reg[idx]; }
            static void ResetReg() {
                reg = std::move(std::unique_ptr<uint32_t[]>(new uint32_t[32]{}));
            }
        //private:
            static std::unique_ptr<uint32_t[]> reg;
        };
        struct MCode {
            explicit MCode(uint8_t op, Reg rs, Reg rt, Reg rd, uint8_t shamt,
                uint8_t fcn) {//实现 R-Type 指令功能
                R_type.opcode = op;
                R_type.rs = rs.idx;
                R_type.rt = rt.idx;
                R_type.rd = rd.idx;
                R_type.shamt = shamt;
                R_type.funct = fcn;
            }
            explicit MCode(uint8_t op, Reg rt, Reg rs, uint16_t imm) {//实现 I-Type 指令功能
                I_type.opcode = op;
                I_type.rt = rt.idx;
                I_type.rs = rs.idx;
                I_type.imm = imm;
            }
            explicit MCode(uint8_t op, uint32_t addr) {//实现 J-Type 指令功能
                J_type.opcode = op;
                J_type.address = addr;
            }
            explicit MCode(uint32_t Native) { NativeCode = Native; };
            MCode() = default;

            union {//使用 union+struct 建立指令格式
                struct {
                    uint32_t address : 26;
                    uint32_t opcode : 6;
                } J_type;
                struct {
                    uint32_t imm : 16, rt : 5, rs : 5, opcode : 6;
                } I_type;
                struct {
                    uint32_t funct : 6, shamt : 5, rd : 5, rt : 5, rs : 5,
                        opcode : 6;
                } R_type;
                uint32_t NativeCode;
            };
        };

        //中断函数
        void Interrupt() {//跳转
            static_cast<Reg>(ra) = PC + 4;            
            int address = 0;
            PC = ((PC + 4) & 0xf0000000) | ((address & 0x03ffffff) << 2);//跳转
            std::cout << "中断响应结束，进入中断服务" << std::endl;
        }

    protected:
        //uint32_t PC = 0;
        uint32_t PC;
        std::unique_ptr<uint32_t[]> memory;
        enum : uint8_t {
            zero,
            at,
            v0,
            v1,
            a0,
            a1,
            a2,
            a3,
            t0,
            t1,
            t2,
            t3,
            t4,
            t5,
            t6,
            t7,
            s0,
            s1,
            s2,
            s3,
            s4,
            s5,
            s6,
            s7,
            t8,
            t9,
            k0,
            k1,
            gp,
            sp,
            fp,
            ra
        };
        enum : uint8_t {
            OP_CODE_R_TYPE,
            OP_CODE_J = 0x02,
            OP_CODE_JAL,
            OP_CODE_BEQ,
            OP_CODE_BNE,
            OP_CODE_ADDI = 0x08,
            OP_CODE_ADDIU,
            OP_CODE_SLTI,
            OP_CODE_SLTIU,
            OP_CODE_ANDI = 0x0C,
            OP_CODE_ORI,
            OP_CODE_XORI,
            OP_CODE_LUI,
            OP_CODE_LW = 0x23,
            OP_CODE_SW = 0x2B,
            OP_CODE_READ = 0x30,
            OP_CODE_WRITE,
            OP_CODE_DISP,//0x32
            OP_CODE_EI,
            OP_CODE_INTI
        };
        enum : uint8_t {
            FUNC_SLL,
            FUNC_SRL = 0x02,
            FUNC_SRA,
            FUNC_SLLV,
            FUNC_SRLV = 0x06,
            FUNC_SRAV,
            FUNC_JR,
            FUNC_ADD = 0x20,
            FUNC_ADDU,
            FUNC_SUB,
            FUNC_SUBU,
            FUNC_AND,
            FUNC_OR,
            FUNC_XOR,
            FUNC_NOR,
            FUNC_SLT = 0x2A,
            FUNC_SLTU
        };
        std::unordered_map<uint8_t, uint32_t(Code::*)(Reg rs, Reg rt, Reg rd,
            uint8_t shamt)>
            funct_to_R_type_fcn = {
                {FUNC_ADD, &Code::add},  {FUNC_ADDU, &Code::addu},
                {FUNC_AND, &Code::_and}, {FUNC_JR, &Code::jr},
                {FUNC_NOR, &Code::_nor}, {FUNC_OR, &Code::_or},
                {FUNC_SLL, &Code::sll},  {FUNC_SLLV, &Code::sllv},
                {FUNC_SLT, &Code::slt},  {FUNC_SLTU, &Code::sltu},
                {FUNC_SRA, &Code::sra},  {FUNC_SRAV, &Code::srav},
                {FUNC_SUB, &Code::sub},  {FUNC_SUBU, &Code::subu},
                {FUNC_SRL, &Code::srl},  {FUNC_XOR, &Code::_xor} };
        std::unordered_map<uint8_t,
            uint32_t(Code::*)(Reg rt, Reg rs, uint16_t imm)>
            opcode_to_I_type_fcn = {
                {OP_CODE_ADDIU, &Code::addiu}, {OP_CODE_ANDI, &Code::andi},
                {OP_CODE_LUI, &Code::lui},     {OP_CODE_ORI, &Code::ori},
                {OP_CODE_SLTIU, &Code::sltiu}, {OP_CODE_XORI, &Code::xori},
                {OP_CODE_ADDI, &Code::addi},   {OP_CODE_BEQ, &Code::beq},
                {OP_CODE_BNE, &Code::bne},     {OP_CODE_LW, &Code::lw},
                {OP_CODE_SLTI, &Code::slti},   {OP_CODE_SW, &Code::sw},
                {OP_CODE_READ, &Code::read},   {OP_CODE_WRITE, &Code::write},
                {OP_CODE_DISP, &Code::disp},   {OP_CODE_EI, &Code::ei} 
        };
        std::unordered_map<uint8_t, uint32_t(Code::*)(uint32_t imm)>
            opcode_to_J_type_fcn = { {OP_CODE_J, &Code::j},
                                    {OP_CODE_JAL, &Code::jal},
                                    {OP_CODE_INTI,&Code::inti}
        };
        // R-type
        uint32_t add(Reg rs, Reg rt, Reg rd, uint8_t shamt) {//完成 xxx 指令功能
            rd = static_cast<int32_t>(rs) + static_cast<int32_t>(rt);
            return MCode(OP_CODE_R_TYPE, rs, rt, rd, shamt, FUNC_ADD).NativeCode;
        }
        uint32_t addu(Reg rs, Reg rt, Reg rd, uint8_t shamt) {
            rd = rs + rt;
            return MCode(OP_CODE_R_TYPE, rs, rt, rd, shamt, FUNC_ADDU).NativeCode;
        }
        uint32_t sub(Reg rs, Reg rt, Reg rd, uint8_t shamt) {
            rd = static_cast<int32_t>(rs) - static_cast<int32_t>(rt);
            return MCode(OP_CODE_R_TYPE, rs, rt, rd, shamt, FUNC_SUB).NativeCode;
        }
        uint32_t subu(Reg rs, Reg rt, Reg rd, uint8_t shamt) {
            rd = rs - rt;
            return MCode(OP_CODE_R_TYPE, rs, rt, rd, shamt, FUNC_SUBU).NativeCode;
        }
        uint32_t _and(Reg rs, Reg rt, Reg rd, uint8_t shamt) {
            rd = rs & rt;
            return MCode(OP_CODE_R_TYPE, rs, rt, rd, shamt, FUNC_AND).NativeCode;
        }
        uint32_t _or(Reg rs, Reg rt, Reg rd, uint8_t shamt) {
            rd = rs | rt;
            return MCode(OP_CODE_R_TYPE, rs, rt, rd, shamt, FUNC_OR).NativeCode;
        }
        uint32_t _xor(Reg rs, Reg rt, Reg rd, uint8_t shamt) {
            rd = rs ^ rt;
            return MCode(OP_CODE_R_TYPE, rs, rt, rd, shamt, FUNC_XOR).NativeCode;
        }
        uint32_t _nor(Reg rs, Reg rt, Reg rd, uint8_t shamt) {
            rd = ~(rs | rt);
            return MCode(OP_CODE_R_TYPE, rs, rt, rd, shamt, FUNC_NOR).NativeCode;
        }
        uint32_t slt(Reg rs, Reg rt, Reg rd, uint8_t shamt) {
            rd = static_cast<int32_t>(rs) < static_cast<int32_t>(rt);
            return MCode(OP_CODE_R_TYPE, rs, rt, rd, shamt, FUNC_SLT).NativeCode;
        }
        uint32_t sltu(Reg rs, Reg rt, Reg rd, uint8_t shamt) {
            rd = rs < rt;
            return MCode(OP_CODE_R_TYPE, rs, rt, rd, shamt, FUNC_SLTU).NativeCode;
        }
        uint32_t sll(Reg rs, Reg rt, Reg rd, uint8_t shamt) {
            rd = rt << shamt;
            return MCode(OP_CODE_R_TYPE, rs, rt, rd, shamt, FUNC_SLL).NativeCode;
        }
        uint32_t srl(Reg rs, Reg rt, Reg rd, uint8_t shamt) {
            rd = rt >> shamt;
            return MCode(OP_CODE_R_TYPE, rs, rt, rd, shamt, FUNC_SRL).NativeCode;
        }
        uint32_t sra(Reg rs, Reg rt, Reg rd, uint8_t shamt) {
            rd = static_cast<int32_t>(rt) >> shamt;
            return MCode(OP_CODE_R_TYPE, rs, rt, rd, shamt, FUNC_SRA).NativeCode;
        }
        uint32_t sllv(Reg rs, Reg rt, Reg rd, uint8_t shamt) {
            rd = rt << rs;
            return MCode(OP_CODE_R_TYPE, rs, rt, rd, shamt, FUNC_SLLV).NativeCode;
        }
        uint32_t srlv(Reg rs, Reg rt, Reg rd, uint8_t shamt) {
            rd = rt >> rs;
            return MCode(OP_CODE_R_TYPE, rs, rt, rd, shamt, FUNC_SRLV).NativeCode;
        }
        uint32_t srav(Reg rs, Reg rt, Reg rd, uint8_t shamt) {
            rd = static_cast<int32_t>(rt) >> rs;
            return MCode(OP_CODE_R_TYPE, rs, rt, rd, shamt, FUNC_SRAV).NativeCode;
        }
        uint32_t jr(Reg rs, Reg rt, Reg rd, uint8_t shamt) {
            PC = rs;
            return MCode(OP_CODE_R_TYPE, rs, rt, rd, shamt, FUNC_JR).NativeCode;
        }
        // I-type
        uint32_t addi(Reg rt, Reg rs, uint16_t imm) {
            rt = rs + static_cast<int32_t>(static_cast<int16_t>(imm));
            return MCode(OP_CODE_ADDI, rt, rs, imm).NativeCode;
        }
        uint32_t addiu(Reg rt, Reg rs, uint16_t imm) {
            rt = rs + static_cast<uint32_t>(imm);
            return MCode(OP_CODE_ADDIU, rt, rs, imm).NativeCode;
        }
        uint32_t andi(Reg rt, Reg rs, uint16_t imm) {
            rt = rs & static_cast<uint32_t>(imm);
            return MCode(OP_CODE_ANDI, rt, rs, imm).NativeCode;
        }
        uint32_t ori(Reg rt, Reg rs, uint16_t imm) {
            rt = rs | static_cast<uint32_t>(imm);
            return MCode(OP_CODE_ORI, rt, rs, imm).NativeCode;
        }
        uint32_t xori(Reg rt, Reg rs, uint16_t imm) {
            rt = rs ^ static_cast<uint32_t>(imm);
            return MCode(OP_CODE_XORI, rt, rs, imm).NativeCode;
        }
        uint32_t lui(Reg rt, Reg rs, uint16_t imm) {
            rt = imm << 16;
            return MCode(OP_CODE_LUI, rt, rs, imm).NativeCode;
        }
        uint32_t lw(Reg rt, Reg rs, uint16_t imm) {
            rt = mem(static_cast<uint32_t>(
                rs + static_cast<int32_t>(static_cast<int16_t>(imm))));
            return MCode(OP_CODE_LW, rt, rs, imm).NativeCode;
        }
        uint32_t sw(Reg rt, Reg rs, uint16_t imm) {
            mem(static_cast<uint32_t>(
                rs + static_cast<int32_t>(static_cast<int16_t>(imm)))) = rt;
            return MCode(OP_CODE_SW, rt, rs, imm).NativeCode;
        }
        uint32_t beq(Reg rt, Reg rs, uint16_t imm) {
            if (rs == rt)
                PC = PC + 4 + static_cast<int32_t>(static_cast<int16_t>(imm));
            return MCode(OP_CODE_BEQ, rt, rs, imm).NativeCode;
        }
        uint32_t bne(Reg rt, Reg rs, uint16_t imm) {
            if (rs != rt)
                PC = PC + 4 + static_cast<int32_t>(static_cast<int16_t>(imm));
            return MCode(OP_CODE_BNE, rt, rs, imm).NativeCode;
        }
        uint32_t slti(Reg rt, Reg rs, uint16_t imm) {
            rt = static_cast<int32_t>(rs) <
                static_cast<int32_t>(static_cast<int16_t>(imm));
            return MCode(OP_CODE_SLTI, rt, rs, imm).NativeCode;
        }
        uint32_t sltiu(Reg rt, Reg rs, uint16_t imm) {
            rt = static_cast<uint32_t>(rs) < static_cast<uint32_t>(imm);
            return MCode(OP_CODE_SLTIU, rt, rs, imm).NativeCode;
        }
        uint32_t read(Reg rt, Reg rs, uint16_t imm) {//read rs
            uint32_t tmp;
            if (inputcount == 0)
                std::cout << "请输入数列长度或中断服务程序号n:" << std::endl;
            else {
                std::cout << "请输入第" << inputcount << "个数字" << std::endl;
            }
            inputcount++;
            std::cin >> tmp;
            rs = tmp;
            return MCode(OP_CODE_READ, rt, rs, imm).NativeCode;
        }
        uint32_t write(Reg rt, Reg rs, uint16_t imm) {//write rs
            uint32_t tmp;
            tmp = rs;
            std::cout << "rs = " << rs << std::endl;
            std::cout << "tmp = " << tmp << std::endl;
            Ans.push_back(tmp);
            return MCode(OP_CODE_WRITE, rt, rs, imm).NativeCode;
        }
        uint32_t disp(Reg rt, Reg rs, uint16_t imm) {//打印指令 disp imm

            return MCode(OP_CODE_DISP, rt, rs, imm).NativeCode;
        }
        uint32_t ei(Reg rt, Reg rs, uint16_t imm) {//ei指令功能 ei
            ++Code::Reg::IE;//IE取反
            Code::Reg::IE %= 2;
            return MCode(OP_CODE_EI, rt, rs, imm).NativeCode;
        }

        // J-type
        uint32_t j(uint32_t address) {
            PC = ((PC + 4) & 0xf0000000) | ((address & 0x03ffffff) << 2);
            return MCode(OP_CODE_J, address).NativeCode;
        }
        uint32_t jal(uint32_t address) {
            static_cast<Reg>(ra) = PC + 4;
            PC = ((PC + 4) & 0xf0000000) | ((address & 0x03ffffff) << 2);//跳转
            return MCode(OP_CODE_JAL, address).NativeCode;
        }
        uint32_t inti(uint32_t address) {
            static_cast<Reg>(ra) = PC + 4;            
            PC = ((PC + 4) & 0xf0000000) | 
                ((Reg::excpHdAddre[address] & 0x03ffffff) << 2);//跳转            
            return MCode(OP_CODE_INTI, address).NativeCode;
        }

    };
}  // namespace MIPSVM
