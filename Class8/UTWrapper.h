/* UTWrapper.h
*
* Copyright (c) 2016
* Diva Analytics
*/

#ifndef UTWrapper_H
#define UTWrapper_H

// UTWrapper around a naked pointer that always clones contained object when copy constructing or copy assigning (unless contained ptr is NULL )
// It is assumed that clone() in class X will return an object of (dynamic) type X
template< typename T>
class UTWrapper
{
public:

	UTWrapper(T* p = 0) : DataPtr(p) { }

	UTWrapper(const T& inner)
	{
		DataPtr = static_cast<T *> (inner.clone());
	}

	~UTWrapper()
	{
		if (DataPtr != 0)
			delete DataPtr;
	}

	UTWrapper(const UTWrapper<T>& original)
	{
		if (original.DataPtr != 0)
			DataPtr = static_cast<T *>(original.DataPtr->clone());
		else
			DataPtr = 0;
	}

	UTWrapper& operator=(const UTWrapper<T>& original)
	{
		if (this != &original)
		{
			if (DataPtr != 0)
				delete DataPtr;

			DataPtr = (original.DataPtr != 0) ? static_cast<T *>(original.DataPtr->clone()) : 0;
		}

		return *this;
	}

	T& operator*()
	{
		return *DataPtr;
	}

	T& operator*() const
	{
		return *DataPtr;
	}

	T* operator->() 
	{
		return DataPtr;
	}


	T* const operator->() const
	{
		return DataPtr;
	}


private:
	T* DataPtr;


};

#endif //UTWrapper_H