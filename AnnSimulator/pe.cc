#include<iostream>
#include<queue>
#include<algorithm>
#include<vector>
#include<cstdio>
#include<cstdlib>
#include<string.h>
#include "common.h"
using namespace std;
namespace PE {
    #define ASSIGN_OP_1 list[0]
    #define ASSIGN_OP_2 list[1]
    #define ASSIGN_OP_3 list[2]
    #define MUL_OP_1 list[3]
    #define MUL_OP_2 list[4]
    #define MUL2_OP list[5]
    #define ADD_OP_1 list[6]
    #define ADD_OP_2 list[7]

    class Mul : public AtomOperation {
    public:
        Mul() {
            type = MUL;
            MAKE_ROOM(2, 1)
        }

        virtual void Process() override {
            results[0] = ops[0] * ops[1];
            _set_data();
        }
    };

    class Bobble : public AtomOperation {
    public:
        Bobble() {
            type = BOBBLE;
            MAKE_ROOM(1, 1)
        }

        virtual void Process() override {
            results[0] = ops[0];
        }
    };

    class Add : public AtomOperation {
    public:
        Add() {
            type = ADD;
            MAKE_ROOM(2, 1)
        }

        virtual void Process() override {
            results[0] = ops[0] + ops[1];
        }
    };

    class Assign : public AtomOperation {
    public:
        Assign() {
            type = ASSIGN;
            MAKE_ROOM(3, 3)
        }

        virtual void Process() override {
            results[0] = ops[0];
            results[1] = ops[1];
            results[2] = ops[2];
        }
    };

    class PE {
    public:
        std::vector<AtomOperation *> modules;
        IN REG op_mul_1;
        IN REG op_mul_2;
        REG op_add;
        REG temporary;

        OUT queue<REG> op_dest;
        static const REG const0 = 0;
        static const REG const1 = 1;
        static const REG const2 = 2;
        static const REG const_1 = -1;
        int list[8];
    public:
        PE() {
            op_mul_1 = 0;
            op_mul_2 = 0;
            op_add = 0;
            memset(list, 0, sizeof(list));
            modules.push_back(new Assign);
            modules.push_back(new Mul);
            modules.push_back(new Bobble);
            modules.push_back(new Add);
            for (int i = 0; i < 2; i++)
                op_dest.push(0);

            Connect<Mul,Bobble> line1;
            line1(modules[0],modules[1]);
        }

        ~PE() {}

        void pipeline(int buffer_a, int buff_b);
    };
    void PE::pipeline(int buffer_a, int buffer_b) {
        ASSIGN_OP_1 = buffer_a;
        ASSIGN_OP_2 = buffer_b;
        ASSIGN_OP_3 = op_dest.front();
        op_dest.pop();

        modules[0]->Input(3, ASSIGN_OP_1, ASSIGN_OP_2, ASSIGN_OP_3);
        modules[1]->Input(2, MUL_OP_1, MUL_OP_2);
        modules[2]->Input(1, MUL2_OP);
        modules[3]->Input(2, ADD_OP_1, ADD_OP_2);

        EXECUTE;

        MUL_OP_1 = modules[0]->Output()[0];
        MUL_OP_2 = modules[0]->Output()[1];
        MUL2_OP = modules[1]->Output()[0];
        ADD_OP_1 = modules[2]->Output()[0];
        ADD_OP_2 = modules[0]->Output()[2];


        op_dest.push(modules[3]->Output()[0]);
        op_mul_1 = MUL_OP_1;//a
        op_mul_2 = MUL_OP_2;//b
        op_add = ADD_OP_2;//c

        for (int i = 2; i < 8; i++)
            cout << list[i] << ' ';
        cout << endl;
    }
}
int main()
{
    PE::PE * pe = new PE::PE;
    int buffer1[15],buffer2[15];
   for(int i=0;i<15;i++)
   {
       buffer1[i]=1;
       buffer2[i]=1;
   }
    for(int i=0;i<15;i++)
        pe->pipeline(buffer1[i],buffer2[i]);
}