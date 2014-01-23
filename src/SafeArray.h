//
//  SafeArray.h
//  mythreal
//
//  Created by Ian Ewell on 3/23/13.
//
//

#ifndef __mythreal__SafeArray__
#define __mythreal__SafeArray__

#include <iostream>
#include "Reference.h"
#include "Logger.h"
#include "Types.h"

// May be disabled in release builds for performance
#define DEFAULT_CHECK_BOUNDS

template <class T>
class CCSafeArray;

template <class T>
class CCSafeArrayBase : public CCReferenceBase
{
protected:
    U32 mArraySize;
    T* mBuffer;
    
    // In some cases bounds checking may need to be disabled for perfomance
    bool mCheckBounds;
    
    // Should never be called directly
    T* getBuffer() { return mBuffer; }
    
    friend class CCSafeArray<T>;
    
public:
    CCSafeArrayBase(U32 size)
    {
        mArraySize = size;
        mBuffer = new T[mArraySize];
        
#ifdef DEFAULT_CHECK_BOUNDS
        mCheckBounds = true;
#else
        mCheckBounds = false;
#endif
    }
    
    ~CCSafeArrayBase()
    {
        delete [] mBuffer;
    }
    
    U32 getSize() { return mArraySize; }
    bool setCheckBounds(bool check) { mCheckBounds = check; }
};

template <class T>
class CCSafeArray : public CCPointerStrong<CCSafeArrayBase<T> >
{
    
protected:
    friend class CCSafeArrayBase<T>;
    
public:
    CCSafeArray() {};
    CCSafeArray(U32 size)
    {
        this->set(new CCSafeArrayBase<T>(size));
    }
    
    void create(U32 size)
    {
        this->set(new CCSafeArrayBase<T>(size));
    }
    
    T& getElement(U32 idx) const
    {
        if (!this->mObject)
        {
            CCLogger::LogError("CCSafeArray::getElement", "Array not initialized");
            return this->getPointer()->mBuffer[0];
        }
        
        if (idx >= this->getPointer()->getSize())
        {
            CCLogger::LogError("CCSafeArray::getElement", "Index out of bounds");
            return this->getPointer()->mBuffer[0];
        }
        return this->getPointer()->mBuffer[idx];
    }
    
    void setElement(U32 idx, T val)
    {
        if (!this->mObject)
        {
            CCLogger::LogError("CCSafeArray::setElement", "Array not initialized");
            return;
        }
        
        if (idx >= this->getPointer()->getSize())
        {
            CCLogger::LogError("CCSafeArray::setElement", "Index out of bounds");
            return;
        }
        this->getPointer()->mBuffer[idx] = val;
    }
    
    U32 getSize() const
    {
        return this->getPointer()->getSize();
    }
    
    void fillBuffer(U32 start, U32 end, T val)
    {
        if (!this->mObject)
        {
            CCLogger::LogError("CCSafeArray::fillBuffer", "Array not initialized");
            return;
        }
        
        if (start >= this->getPointer()->getSize() || end >= this->getPointer()->getSize())
        {
            CCLogger::LogError("CCSafeArray::fillBuffer", "Start or end index out of bounds");
            return;
        }
        T* buffer = this->getPointer()->mBuffer;
        for (U32 i = start; i <= end; i++)
            buffer[i] = val;
    }
    
    void fillBuffer(T val)
    {
        fillBuffer(0, this->getPointer()->getSize(), val);
    }
    
    void copyFromBuffer(T* buffer, U32 size, U32 offset=0)
    {
        if (!this->mObject)
        {
            CCLogger::LogError("CCSafeArray::copyFromBuffer()", "Array not initialized");
            return;
        }
        
        if (size > this->getPointer()->getSize() || offset+size > this->getPointer()->getSize())
        {
            CCLogger::LogError("CCSafeArray::copyFromBuffer()", "size or offset out of bounds");
            return;
        }
        
        memcpy(this->getPointer()->mBuffer+offset*sizeof(T), buffer, size*sizeof(T));
    }
    
    T& operator[](U32 index)
    {
        return getElement(index);
    }
    
    const T& operator[](U32 index) const
    {
        return getElement(index);
    }
    
    T& operator[](S32 index)
    {
        return getElement(index);
    }
    
    const T& operator[](S32 index) const
    {
        return getElement(index);
    }
    
    CCSafeArray<T>& operator=(CCSafeArrayBase<T> *ptr)
    {
        CCReferenceStrong::set(ptr);
        return *this;
    }
};

#endif /* defined(__mythreal__SafeArray__) */
