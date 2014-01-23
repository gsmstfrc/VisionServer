//
//  Reference.h
//  mythreal
//
//  Created by Ian Ewell on 10/2/12.
//
//

#ifndef mythreal_Reference_h
#define mythreal_Reference_h

#include "Types.h"

// All objects that need strong reference counted memory management should inherit from this class
class CCReferenceBase
{
public:
    CCReferenceBase() { mReferenceCount = 0; }
    virtual ~CCReferenceBase() {};
    
    S32 getReferenceCount() const { return mReferenceCount; }
    
    virtual void destroy() { delete this; }
    
    void retain()
    {
        mReferenceCount++;
    }
    
    void release()
    {
        if (--mReferenceCount <= 0)
            destroy();
    }
    
    
protected:
    S32 mReferenceCount;
};

class CCReferenceStrong
{
public:
    CCReferenceStrong() { mObject = NULL; }
    CCReferenceStrong(CCReferenceBase *obj) { mObject = obj; retain(); }
    virtual ~CCReferenceStrong() { release(); }
    
    void set(CCReferenceBase *obj)
    {
        if (mObject != obj)
        {
            release();
            mObject = obj;
            retain();
        }
    }
    
protected:
    CCReferenceBase *mObject;
    
    void retain()
    {
        if (mObject)
            mObject->retain();
    }
    
    void release()
    {
        if (mObject)
            mObject->release();
    }

};

template <class T>
class CCPointerStrong : public CCReferenceStrong
{
public:
    CCPointerStrong() : CCReferenceStrong() {};
    CCPointerStrong(const T *ptr) : CCReferenceStrong(ptr) {};
    CCPointerStrong(const CCPointerStrong<T>&ref) : CCReferenceStrong(ref.mObject) {};
    ~CCPointerStrong() {};
    
    CCPointerStrong<T>& operator=(const CCPointerStrong<T>& ref)
    {
        set(ref.mObject);
        return *this;
    }
    
    CCPointerStrong<T>& operator=(T *ptr)
    {
        set(ptr);
        return *this;
    }
    T* operator->() const { return getPointer(); }
    T& operator*() const  { return *getPointer(); }
    operator T*() const { return getPointer(); }
    T* getPointer() const { return const_cast<T*>(static_cast<T* const>(mObject)); }

};

#endif
