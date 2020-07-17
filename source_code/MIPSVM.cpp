#include <MIPSVM.h>

#include <memory>
#include <random>
namespace MIPSVM {
    std::vector<int> Ans;
    int inputcount = 0;
    std::unique_ptr<uint32_t[]> Code::Reg::reg;

    //�ж�
    int Code::Reg::IE = 1; //= 0�ж�������ֵ�������ж�
    int Code::Reg::EXL, ERL;//�κ�һ��=1����ֹ�ж�

    int Code::Reg::excpHdAddre[MAXN] = {0,32,64,96};//�����ж�����������ʼ�� 
    
    int Code::Reg::inta = 0;//�ж����� =1�ж�
    int Code::Reg::canInt = 1;//�����ж�    

    void Code::ResetPC(uint32_t address) {//����PC
        PC = address;
    }
}  // namespace MIPSVM
