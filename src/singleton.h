/*************************************************************************
 * Copyright (c) 2015, LiChuang. All rights reserved.
 * Author: lichuang(whlichuang@126.com)
 * Created Time: Wed 03 Jun 2015 02:53:51 PM CST
 * Description: 
 ************************************************************************/

#ifndef __SINGLETON_H__
#define __SINGLETON_H__

#define DECLARE_SINGLETON(classname)  \
	public: \
		static classname * instance(){ \
			if (NULL == _instance) \
				_instance = new classname(); \
			return _instance; \
		} \
		static bool isCreated(){ \
			return NULL != _instance; \
		} \
	protected: \
		classname(); \
		static classname * _instance; \
	private:

#define IMPLEMENT_SINGLETON(classname) \
	classname* classname::_instance = NULL; \
	classname::classname(){}

#define IMPLEMENT_SINGLETON_NO_CONSTRACTOR(classname) \
	classname * classname::_instance = NULL;

#endif //__SINGLETON_H__
