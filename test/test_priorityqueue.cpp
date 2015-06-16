/*************************************************************************
 * Copyright (c) 2015, LiChuang. All rights reserved.
 * Author: lichuang(whlichuang@126.com)
 * Created Time: Wed 03 Jun 2015 11:20:51 AM CST
 * Description: 
 ************************************************************************/

#include <stdio.h>
#include <queue>
#include <vector>

using namespace std;

struct node {
    int priority;
    float value;
};

struct comp
{
    bool operator() (node *& n1, node *& n2)
    {
        return n1->priority < n2->priority;
    }
};


int main(int argc, char *argv[])
{
    //priority_queue<node, vector<node>, mygreater<node> > q;
    priority_queue<node*, vector<node*>, comp> q;
    node n1 = {3, 13.0};
    node n2 = {2, 2.0};
    node n3 = {5, 35.0};
    node n4 = {9, 19.0};
    node n5 = {2, 992.0};
    q.push(&n1);
    q.push(&n2);
    q.push(&n3);
    q.push(&n4);
    q.push(&n5);
    while (!q.empty())
    {
        node *t = q.top();
        printf("%d %f\n", t->priority, t->value);
        q.pop();
    }
    return 0;
}
