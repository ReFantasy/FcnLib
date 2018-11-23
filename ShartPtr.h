/************************************************************************************
 *                                                                                  *
 *    File Name: SharePtr.h                                                         *
 *                                                                                  *
 *    Data:      2018-11-23                                                         *
 *                                                                                  *
 *    Author：   ReFantasy                                                          *
 *                                                                                  *
 *    Describe:  the header file define a class which is aimed at realizing         *
 *               a kind if simple smart pointer.You can use "new" operator          *
 *               to initialize it.Once initialize a smart pointer,you should        *
 *               not to release the resourse by delete operator to delete the       *
 *               raw pointer.                                                       *
 *                                                                                  *
 *                                                                                  *
 ************************************************************************************
 */


#ifndef __SHAREPTR_H__
#define __SHAREPTR_H__

template<typename T>
class SharePtr
{
	using CountType = unsigned int;
public:
	using value_type = T;
	// 默认构造函数
	SharePtr()
	{
		_pResourece = nullptr;
		_pCount = new CountType(0);
	}
	SharePtr(T* pResource)
	{
		_pResourece = pResource;
		_pCount = new CountType(1);
	}

	// 拷贝构造
	SharePtr(const SharePtr &rhs)
	{
		_pResourece = rhs._pResourece;
		_pCount = rhs._pCount;
		(*_pCount)++;
	}

	// 拷贝赋值
	SharePtr& operator=(const SharePtr &rhs)
	{
		if (this == &rhs)
			return *this;

		// 先处理自身指向的资源
		(*_pCount)--;
		if ((*_pCount) == 0)
		{
			if(_pResourece)
				delete _pResourece;
			delete _pCount;
		}
		
		// 赋值
		_pResourece = rhs._pResourece;
		_pCount = rhs._pCount;
		(*_pCount)++;

	}

	// 移动构造
	SharePtr(SharePtr &&rhs)
	{
		SharePtr(rhs);
	}

	// 移动赋值
	SharePtr& operator=(SharePtr &&rhs)
	{
		operator=(rhs);
	}

	
	// 析构
	~SharePtr()
	{
		(*_pCount)--;
		if ((*_pCount) == 0)
		{
			if(_pResourece)
				delete _pResourece;
			delete _pCount;
		}
	}

	T operator*()
	{
		return *_pResourece;
	}
private:
	T *_pResourece;  // 指向资源
	CountType *_pCount; // 引用计数
};

#endif // __SHAREPTR_H__头文件
