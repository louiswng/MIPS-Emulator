#pragma once
#include <bitset>
#include <cstdio>
#include <cstring>
#include <iostream>
#include <map>
#include <memory>
#include <string>
#include <vector>
#include <type_traits>

class AS2MC {
    static std::map<std::string, int> Reg_num;//建立寄存器名称与序号的映射
    static std::vector<std::string> split(const std::string& s, 
        const std::string& seperator);//拆分汇编代码
    static bool check_map(const std::string& s) {//检查一个参数是否为有效的寄存器
        return Reg_num.find(s) == Reg_num.cend();
    }
    template<typename...Args, std::enable_if_t<sizeof...(Args) >= 1, int> = 0>
    static bool check_map(const std::string& s, Args&&... args) {
        //检查给定的n个参数是否为有效的寄存器
        return Reg_num.find(s) == Reg_num.cend() || 
            check_map(std::forward<Args>(args)...);
    }

public:
    static uint32_t trans(std::string ins);
    AS2MC() = delete;
    virtual ~AS2MC() = 0; ////纯虚函数，父类中实现无意义
};
