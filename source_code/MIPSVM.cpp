#include <MIPSVM.h>

#include <memory>
#include <random>
namespace MIPSVM {
    std::vector<int> Ans;
    int inputcount = 0;
    std::unique_ptr<uint32_t[]> Code::Reg::reg;

    //中断
    int Code::Reg::IE = 1; //= 0中断允许，初值不允许中断
    int Code::Reg::EXL, ERL;//任何一个=1，禁止中断

    int Code::Reg::excpHdAddre[MAXN] = {0,32,64,96};//定义中断向量表，并初始化 
    
    int Code::Reg::inta = 0;//中断请求 =1中断
    int Code::Reg::canInt = 1;//单级中断    

    void Code::ResetPC(uint32_t address) {//重置PC
        PC = address;
    }
}  // namespace MIPSVM
