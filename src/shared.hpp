#ifndef SHARED_PTR
#define SHARED_PTR
#include <cstddef>
#include <functional>
#include <memory>

// template <typename T>
// using SharedPtr = std::shared_ptr<T>;

template <typename T>
class SharedPtr
{
public:
    SharedPtr();
    SharedPtr(T *p);
    ~SharedPtr();
    SharedPtr(const SharedPtr &other);
    SharedPtr &operator=(const SharedPtr &other);
    operator bool() const;
    size_t use_count() const;
    T *get() const;
    T &operator*() const;
    T *operator->() const;
    void reset();
    void reset(T *p);

private:
    // add whatever u want bro
    T *_p;
    size_t *count;
};

template <typename T>
SharedPtr<T>::SharedPtr() : _p(nullptr), count(nullptr){};

template <typename T>
SharedPtr<T>::SharedPtr(T *p) : _p(p), count(p ? new size_t(1) : nullptr){};

template <typename T>
SharedPtr<T>::~SharedPtr()
{
    if (count)
    {
        if ((*count) == 1)
        {
            delete count;
            delete _p;
        }
        else
        {
            (*count)--;
        }
    }
}

template <typename T>
SharedPtr<T>::SharedPtr(const SharedPtr &other)
{
    _p = other._p;
    count = other.count;
    if (_p)
        (*count)++;
}
// const:参数不会被修改

template <typename T>
SharedPtr<T> &SharedPtr<T>::operator=(const SharedPtr &other)
{
    if (this != &other)
    {
        if (count)
        {
            if ((*count) == 1)
            {
                delete _p;
                delete count;
            }
            else
            {
                (*count)--;
            }
        }
        _p = other._p;
        count = other.count;
        if (_p)
            (*count)++;
    }
    return *this;
}

template <typename T>
SharedPtr<T>::operator bool() const
{
    return _p != nullptr;
}

template <typename T>
size_t SharedPtr<T>::use_count() const
{
    if (!count)
        return 0;
    return *count;
}

template <typename T>
T *SharedPtr<T>::get() const
{
    return _p;
}

template <typename T>
T &SharedPtr<T>::operator*() const
{
    return *_p;
}

template <typename T>
T *SharedPtr<T>::operator->() const
{
    return _p;
}

template <typename T>
void SharedPtr<T>::reset()
{
    if (count)
    {
        if ((*count) == 1)
        {
            delete count;
            delete _p;
        }
        else
        {
            (*count)--;
        }
    }
    count = nullptr;
    _p = nullptr;
}

template <typename T>
void SharedPtr<T>::reset(T *p)
{
    if (count)
    {
        if ((*count) == 1)
        {
            delete count;
            delete _p;
        }
        else
        {
            (*count)--;
        }
    }
    _p = p;
    count = p ? new size_t(1) : nullptr;
}

template <typename T, typename... Args>
SharedPtr<T> make_shared(Args &&...args)
{
    return SharedPtr<T>(new T(std::forward<Args>(args)...));
}
// Then implement the make_shared function
// I think I'd better leave you to design the prototype :)

// template <?>
//? make_shared(?);*/

#endif // SHARED_PTR