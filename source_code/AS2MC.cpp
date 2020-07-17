#include <AS2MC.h>
#include "MIPSVM.h"
namespace {
    using namespace std;  //将 using语句放置在匿名命名空间内，防止污染全局命名空间
}
std::map<string, int> AS2MC::Reg_num = {
    {"$zero", 0}, {"$at", 1},  {"$v0", 2},  {"$v1", 3},  {"$a0", 4},
    {"$a1", 5},   {"$a2", 6},  {"$a3", 7},  {"$t0", 8},  {"$t1", 9},
    {"$t2", 10},  {"$t3", 11}, {"$t4", 12}, {"$t5", 13}, {"$t6", 14},
    {"$t7", 15},  {"$s0", 16}, {"$s1", 17}, {"$s2", 18}, {"$s3", 19},
    {"$s4", 20},  {"$s5", 21}, {"$s6", 22}, {"$s7", 23}, {"$t8", 24},
    {"$t9", 25},  {"$k0", 26}, {"$k1", 27}, {"$gp", 28}, {"$sp", 29},
    {"$fp", 30},  {"$ra", 31} };

vector<string> AS2MC::split(const string& s, const string& seperator) {//对输入字符串根据操作符进行分割格式化，s 为输入字符串，seperator 为分隔符
    vector<string> result;
    typedef string::size_type string_size;
    string_size i = 0;

    while (i != s.size()) {
        //找到字符串中首个不等于分隔符的字母；
        int flag = 0;
        while (i != s.size() && flag == 0) {
            flag = 1;
            for (string_size x = 0; x < seperator.size(); ++x)
                if (s[i] == seperator[x]) {
                    ++i;
                    flag = 0;
                    break;
                }
        }

        //找到又一个分隔符，将两个分隔符之间的字符串取出；
        flag = 0;
        string_size j = i;
        while (j != s.size() && flag == 0) {
            for (string_size x = 0; x < seperator.size(); ++x)
                if (s[j] == seperator[x]) {
                    flag = 1;
                    break;
                }
            if (flag == 0) ++j;
        }
        if (i != j) {
            result.push_back(s.substr(i, j - i));
            i = j;
        }
    }
    return result;
}

uint32_t AS2MC::trans(std::string ins) {//将汇编语言转换为机器语言便于处理
    uint32_t result = 0;
    std::vector<string> v = split(ins, " ,()");
    //std::cout << "v[0] = " << v[0] << std::endl;
    // R-Type
    if (v[0] == "add") {
        if (v.size() != 4) throw std::invalid_argument("Wrong amount inputs !");
        if (check_map(v[1], v[2], v[3]))
            throw std::invalid_argument("Wrong register name !");
        result = (0 << 26) + (Reg_num[v[2]] << 21) + (Reg_num[v[3]] << 16) +
            (Reg_num[v[1]] << 11) + 0x20;
        return result;
    }
    if (v[0] == "addu") {
        if (v.size() != 4) throw std::invalid_argument("Wrong amount inputs !");
        if (check_map(v[1], v[2], v[3]))
            throw std::invalid_argument("Wrong register name !");
        result = (0 << 26) + (Reg_num[v[2]] << 21) + (Reg_num[v[3]] << 16) +
            (Reg_num[v[1]] << 11) + 0x21;
        return result;
    }
    if (v[0] == "sub") {
        if (v.size() != 4) throw std::invalid_argument("Wrong amount inputs !");
        if (check_map(v[1], v[2], v[3]))
            throw std::invalid_argument("Wrong register name !");
        result = (0 << 26) + (Reg_num[v[2]] << 21) + (Reg_num[v[3]] << 16) +
            (Reg_num[v[1]] << 11) + 0x22;
        return result;
    }
    if (v[0] == "subu") {
        if (v.size() != 4) throw std::invalid_argument("Wrong amount inputs !");
        if (check_map(v[1], v[2], v[3]))
            throw std::invalid_argument("Wrong register name !");
        result = (0 << 26) + (Reg_num[v[2]] << 21) + (Reg_num[v[3]] << 16) +
            (Reg_num[v[1]] << 11) + 0x23;
        return result;
    }
    if (v[0] == "and") {
        if (v.size() != 4) throw std::invalid_argument("Wrong amount inputs !");
        if (check_map(v[1], v[2], v[3]))
            throw std::invalid_argument("Wrong register name !");
        result = (0 << 26) + (Reg_num[v[2]] << 21) + (Reg_num[v[3]] << 16) +
            (Reg_num[v[1]] << 11) + 0x24;
        return result;
    }
    if (v[0] == "or") {
        if (v.size() != 4) throw std::invalid_argument("Wrong amount inputs !");
        if (check_map(v[1], v[2], v[3]))
            throw std::invalid_argument("Wrong register name !");
        result = (0 << 26) + (Reg_num[v[2]] << 21) + (Reg_num[v[3]] << 16) +
            (Reg_num[v[1]] << 11) + 0x25;
        return result;
    }
    if (v[0] == "xor") {
        if (v.size() != 4) throw std::invalid_argument("Wrong amount inputs !");
        if (check_map(v[1], v[2], v[3]))
            throw std::invalid_argument("Wrong register name !");
        result = (0 << 26) + (Reg_num[v[2]] << 21) + (Reg_num[v[3]] << 16) +
            (Reg_num[v[1]] << 11) + 0x26;
        return result;
    }
    if (v[0] == "nor") {
        if (v.size() != 4) throw std::invalid_argument("Wrong amount inputs !");
        if (check_map(v[1], v[2], v[3]))
            throw std::invalid_argument("Wrong register name !");
        result = (0 << 26) + (Reg_num[v[2]] << 21) + (Reg_num[v[3]] << 16) +
            (Reg_num[v[1]] << 11) + 0x27;
        return result;
    }
    if (v[0] == "slt") {
        if (v.size() != 4) throw std::invalid_argument("Wrong amount inputs !");
        if (check_map(v[1], v[2], v[3]))
            throw std::invalid_argument("Wrong register name !");
        result = (0 << 26) + (Reg_num[v[2]] << 21) + (Reg_num[v[3]] << 16) +
            (Reg_num[v[1]] << 11) + 0x2a;
        return result;
    }
    if (v[0] == "sltu") {
        if (v.size() != 4) throw std::invalid_argument("Wrong amount inputs !");
        if (check_map(v[1], v[2], v[3]))
            throw std::invalid_argument("Wrong register name !");
        result = (0 << 26) + (Reg_num[v[2]] << 21) + (Reg_num[v[3]] << 16) +
            (Reg_num[v[1]] << 11) + 0x2b;
        return result;
    }
    if (v[0] == "sll") {
        if (v.size() != 4) throw std::invalid_argument("Wrong amount inputs !");
        if (check_map(v[1], v[2]))
            throw std::invalid_argument("Wrong register name !");
        result = (0 << 26) + (0 << 21) + (Reg_num[v[2]] << 16) +
            (Reg_num[v[1]] << 11) + ((std::stoi(v[3]) & 0x1f) << 6) + 0x0;
        return result;
    }
    if (v[0] == "srl") {
        if (v.size() != 4) throw std::invalid_argument("Wrong amount inputs !");
        if (check_map(v[1], v[2]))
            throw std::invalid_argument("Wrong register name !");
        result = (0 << 26) + (0 << 21) + (Reg_num[v[2]] << 16) +
            (Reg_num[v[1]] << 11) + ((std::stoi(v[3]) & 0x1f) << 6) + 0x2;
        return result;
    }
    if (v[0] == "sra") {
        if (v.size() != 4) throw std::invalid_argument("Wrong amount inputs !");
        if (check_map(v[1], v[2]))
            throw std::invalid_argument("Wrong register name !");
        result = (0 << 26) + (0 << 21) + (Reg_num[v[2]] << 16) +
            (Reg_num[v[1]] << 11) + ((std::stoi(v[3]) & 0x1f) << 6) + 0x3;
        return result;
    }
    if (v[0] == "sllv") {
        if (v.size() != 4) throw std::invalid_argument("Wrong amount inputs !");
        if (check_map(v[1], v[2], v[3]))
            throw std::invalid_argument("Wrong register name !");
        result = (0 << 26) + (Reg_num[v[3]] << 21) + (Reg_num[v[2]] << 16) +
            (Reg_num[v[1]] << 11) + 0x4;
        return result;
    }
    if (v[0] == "srlv") {
        if (v.size() != 4) throw std::invalid_argument("Wrong amount inputs !");
        if (check_map(v[1], v[2], v[3]))
            throw std::invalid_argument("Wrong register name !");
        result = (0 << 26) + (Reg_num[v[3]] << 21) + (Reg_num[v[2]] << 16) +
            (Reg_num[v[1]] << 11) + 0x6;
        return result;
    }
    if (v[0] == "srav") {
        if (v.size() != 4) throw std::invalid_argument("Wrong amount inputs !");
        if (check_map(v[1], v[2], v[3]))
            throw std::invalid_argument("Wrong register name !");
        result = (0 << 26) + (Reg_num[v[3]] << 21) + (Reg_num[v[2]] << 16) +
            (Reg_num[v[1]] << 11) + 0x7;
        return result;
    }
    if (v[0] == "jr") {
        if (v.size() != 2) throw std::invalid_argument("Wrong amount inputs !");
        if (check_map(v[1]))
            throw std::invalid_argument("Wrong register name !");
        result = (0 << 26) + (Reg_num[v[1]] << 21) + 0x8;
        return result;
    }

    // I-type
    if (v[0] == "addi") {
        if (v.size() != 4) throw std::invalid_argument("Wrong amount inputs !");
        if (check_map(v[1], v[2]))
            throw std::invalid_argument("Wrong register name !");
        result = (8 << 26) + (Reg_num[v[2]] << 21) + (Reg_num[v[1]] << 16) +
            static_cast<uint16_t>(std::stoi(v[3]));
        return result;
    }
    if (v[0] == "addiu") {
        if (v.size() != 4) throw std::invalid_argument("Wrong amount inputs !");
        if (check_map(v[1], v[2]))
            throw std::invalid_argument("Wrong register name !");
        result = (9 << 26) + (Reg_num[v[2]] << 21) + (Reg_num[v[1]] << 16) +
            static_cast<uint16_t>(std::stoi(v[3]));
        return result;
    }
    if (v[0] == "andi") {
        if (v.size() != 4) throw std::invalid_argument("Wrong amount inputs !");
        if (check_map(v[1], v[2]))
            throw std::invalid_argument("Wrong register name !");
        result = (12 << 26) + (Reg_num[v[2]] << 21) + (Reg_num[v[1]] << 16) +
            static_cast<uint16_t>(std::stoi(v[3]));
        return result;
    }
    if (v[0] == "ori") {
        if (v.size() != 4) throw std::invalid_argument("Wrong amount inputs !");
        if (check_map(v[1], v[2]))
            throw std::invalid_argument("Wrong register name !");
        result = (13 << 26) + (Reg_num[v[2]] << 21) + (Reg_num[v[1]] << 16) +
            static_cast<uint16_t>(std::stoi(v[3]));
        return result;
    }
    if (v[0] == "xori") {
        if (v.size() != 4) throw std::invalid_argument("Wrong amount inputs !");
        if (check_map(v[1], v[2]))
            throw std::invalid_argument("Wrong register name !");
        result = (14 << 26) + (Reg_num[v[2]] << 21) + (Reg_num[v[1]] << 16) +
            static_cast<uint16_t>(std::stoi(v[3]));
        return result;
    }
    if (v[0] == "lui") {
        if (v.size() != 3) throw std::invalid_argument("Wrong amount inputs !");
        if (check_map(v[1]))
            throw std::invalid_argument("Wrong register name !");
        result = (15 << 26) + (Reg_num[v[1]] << 16) +
            static_cast<uint16_t>(std::stoi(v[2]));
        return result;
    }
    if (v[0] == "lw") {
        if (v.size() != 4) throw std::invalid_argument("Wrong amount inputs !");
        if (check_map(v[1], v[3]))
            throw std::invalid_argument("Wrong register name !");
        result = (35 << 26) + (Reg_num[v[3]] << 21) + (Reg_num[v[1]] << 16) +
            static_cast<uint16_t>(std::stoi(v[2]));
        return result;
    }
    if (v[0] == "sw") {
        if (v.size() != 4) throw std::invalid_argument("Wrong amount inputs !");
        if (check_map(v[1], v[3]))
            throw std::invalid_argument("Wrong register name !");
        result = (43 << 26) + (Reg_num[v[3]] << 21) + (Reg_num[v[1]] << 16) +
            static_cast<uint16_t>(std::stoi(v[2]));
        return result;
    }
    if (v[0] == "beq") {
        if (v.size() != 4) throw std::invalid_argument("Wrong amount inputs !");
        if (check_map(v[1], v[2]))
            throw std::invalid_argument("Wrong register name !");
        result = (4 << 26) + (Reg_num[v[2]] << 21) + (Reg_num[v[1]] << 16) +
            static_cast<uint16_t>(std::stoi(v[3]));
        return result;
    }
    if (v[0] == "bne") {
        if (v.size() != 4) throw std::invalid_argument("Wrong amount inputs !");
        if (check_map(v[1], v[2]))
            throw std::invalid_argument("Wrong register name !");
        result = (5 << 26) + (Reg_num[v[2]] << 21) + (Reg_num[v[1]] << 16) +
            static_cast<uint16_t>(std::stoi(v[3]));
        return result;
    }
    if (v[0] == "ori") {
        if (v.size() != 4) throw std::invalid_argument("Wrong amount inputs !");
        if (check_map(v[1], v[2]))
            throw std::invalid_argument("Wrong register name !");
        result = (5 << 26) + (Reg_num[v[2]] << 21) + (Reg_num[v[1]] << 16) +
            static_cast<uint16_t>(std::stoi(v[3]));
        return result;
    }
    if (v[0] == "slti") {
        if (v.size() != 4) throw std::invalid_argument("Wrong amount inputs !");
        if (check_map(v[1], v[2]))
            throw std::invalid_argument("Wrong register name !");
        result = (10 << 26) + (Reg_num[v[2]] << 21) + (Reg_num[v[1]] << 16) +
            static_cast<uint16_t>(std::stoi(v[3]));
        return result;
    }
    if (v[0] == "sltiu") {
        if (v.size() != 4) throw std::invalid_argument("Wrong amount inputs !");
        if (check_map(v[1], v[2]))
            throw std::invalid_argument("Wrong register name !");
        result = (11 << 26) + (Reg_num[v[2]] << 21) + (Reg_num[v[1]] << 16) +
            static_cast<uint16_t>(std::stoi(v[3]));
        return result;
    }

    // J-Type
    if (v[0] == "j") {
        if (v.size() != 2) throw std::invalid_argument("Wrong amount inputs !");
        result = (2 << 26) |
            (static_cast<uint32_t>(std::stoi(v[1]) / 4) & 0x3ffffff);
        return result;
    }
    if (v[0] == "jal") {
        if (v.size() != 2) throw std::invalid_argument("Wrong amount inputs !");
        result = (3 << 26) |
            //(static_cast<uint32_t>(std::stoi(v[1]) / 4) & 0x3ffffff);
            (static_cast<uint32_t>(std::stoi(v[1])) & 0x3ffffff);
        return result;
    }
    if (v[0] == "inti") {
        if (v.size() != 2) throw std::invalid_argument("Wrong amount inputs !");
        result = (52 << 26) |
            (static_cast<uint32_t>(std::stoi(v[1])) & 0x3ffffff);
        return result;
    }

    // Others
    if (v[0] == "read") {//read rs
        if (v.size() != 2) throw std::invalid_argument("Wrong amount inputs !");
        if (check_map(v[1]))
            throw std::invalid_argument("Wrong register name !");
        result = (48 << 26) + (Reg_num[v[1]] << 21);
        return result;
    }
    if (v[0] == "write") {//write rs
        if (v.size() != 2) throw std::invalid_argument("Wrong amount inputs !");
        if (check_map(v[1]))
            throw std::invalid_argument("Wrong register name !");
        result = (49 << 26) + (Reg_num[v[1]] << 21);
        return result;
    }
    if (v[0] == "disp") {
        if (v.size() != 2) throw std::invalid_argument("Wrong amount inputs !");
        result = (50 << 26) + static_cast<uint16_t>(std::stoi(v[1]));
        return result;
    }
    if (v[0] == "ei") {
        if (v.size() != 1) throw std::invalid_argument("Wrong amount inputs !");       
        result = (51 << 26);
        return result;
    }
    //cout << ins << endl;
    throw std::invalid_argument("Input is not a valid instrution !");
    return 0;
}
