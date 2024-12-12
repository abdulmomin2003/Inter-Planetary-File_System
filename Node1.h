#pragma once
#include "Project.cpp"
class Node1 {
public:
    int id;
    Btree<int, string>* b;
    Node1* next;
    string machinepath;
    RoutingTable* rt;
    int rtsize;
    Node1(int id1 = 0, int rsize = 0) {
        id = id1;
        next = nullptr;
        rtsize = rsize;
        rt = new RoutingTable(rsize);
    }
    int getNumber(int here) {
        int thiss = pow(2, here);
        thiss += id;
        return thiss;
    }
    void print() {
        for (int i = 0; i < rtsize; i++) {
            if (rt->table[i] == nullptr)
                continue;
            cout << rt->table[i]->id << " ";
        }
        cout << endl;
    }
};