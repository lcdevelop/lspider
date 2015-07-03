/*************************************************************************
 * Copyright (c) 2015, LiChuang. All rights reserved.
 * Author: lichuang(whlichuang@126.com)
 * Created Time: Mon 29 Jun 2015 11:29:20 AM CST
 * Description: 
 ************************************************************************/

#ifndef __CONTROLLABLE_H__
#define __CONTROLLABLE_H__

#include <string>

using std::string;

class Controllable
{
public:
    virtual ~Controllable();
    virtual void control(const string& cmd) = 0;
};

#endif //__CONTROLLABLE_H__
