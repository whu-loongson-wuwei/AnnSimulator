#include"common.h"
#include<iostream>
using namespace std;

class Mul:public AtomOperation
{
public:
    Mul():AtomOperation(2,1)
    {
        type=MUL;
    }
    virtual void Process() override
    {
        fan_out[0]=fan_in[0]*fan_in[1];
        func_set_data();
    }
};
class Add:public AtomOperation
{
public:
    Add():AtomOperation(2,1)
    {
        type=ADD;
    }
    virtual void Process() override
    {
        fan_out[0]=fan_in[0]+fan_in[1];
        func_set_data();
    }
};
class Register:public AtomOperation
{
    // fan_in  is value in wire which is connected to this register from others
    // fan_out is the actual value of this  register 
public:

    Register(int a,int b):AtomOperation(a,b)
    {  
        type = REGISTER;
    }
    virtual void Process() override
    {   
        for(int i=0;i<count_fan_out;i++)
        {
            fan_out[i] = fan_in[0];
        }
    }
};
class Bobble:public Register
{
public:
    Bobble():Register(1,1)
    {
        type=BOBBLE;
    }
};



class PE:public AtomOperation
{
private:
     Modules modules;
     Wires wires;
public:
    PE():AtomOperation(2,1)
    { 
        modules.push_back(new Register(1,1));
        modules.push_back(new Register(1,1));
        modules.push_back(new Register(1,1));
        modules.push_back(new Register(1,1));
        modules.push_back(new Register(1,1));
        modules.push_back(new Register(1,2));
       
        Mul* mul=new Mul;
        Add* add=new Add; 

        for(int i=0;i<8;i++)
        {
            wires.push_back(Wire());
        }
        wires[0](modules[0],mul);
        wires[1](modules[1],mul);
        wires[2](mul,modules[2]);
        wires[3](modules[2],modules[3]);
        wires[4](modules[3],add);
        wires[5](modules[4],add);
        wires[6](add,modules[5]);
        wires[7](modules[5],modules[4]);
    }
    virtual void Process()
    {
        modules[0]->Input(0,fan_in[0]);
        modules[1]->Input(0,fan_in[1]);
        EXECUTE;
        fan_out[0]= modules[5]->Output(0);
        for(int i=0;i<5;i++)
            cout<<modules[i]->fan_in[0]<<'-'<<modules[i]->fan_out[0]<<' ';
        cout<<endl;
     }
};

int main()
{  
    PE *pe=new PE;
    for(int i=1;i<15;i++)
    {
        pe->Input(0,i);
        pe->Input(1,i);
        pe->Process();
         cout<<pe->Output(0)<<endl;
    }
     
    return 0;
}