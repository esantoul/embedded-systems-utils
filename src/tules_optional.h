#pragma once

/**
 * Optional and Optional Reference data container
 */

#include <new>

namespace tules // Template Utility Library for Embedded Systems
{
  template <typename T>
  class Optional
  {
  private:
    union {
      T mValue;
    };
    bool mLive = false;

  public:
    Optional() {}
    Optional(const T &val) : mValue{val}, mLive(true) {}
    template <typename... Args>
    explicit constexpr Optional(const Args &... args) : mValue{args...}, mLive(true) {}

    Optional &operator=(const Optional &other)
    {
      if (mLive)
        mValue.~T();
      if (other.mLive)
        new (&mValue) T(other.mValue);
      mLive = other.mLive;
      return *this;
    }

    template <typename O>
    Optional &operator=(const O &val)
    {
      if (mLive)
        mValue.~T();
      new (&mValue) T(val);
      mLive = true;
      return *this;
    }

    ~Optional()
    {
      if (mLive)
        mValue.~T();
    }

    bool HasValue() { return mLive; }
    operator bool() { return mLive; }
    T Value() { return mValue; }
    T operator*() { return mValue; }
    T *operator->() { return &mValue; }
    T ValueOr(const T &other) { return mLive ? mValue : other; }
    const T &ValueConstRefOr(const T &other) { return mLive ? mValue : other; }
  };

  template <typename T>
  class OptionalRef // When using this, all the proper checks need to be done by the user
  {
  private:
    T *pValue;
    bool mLive = false;

  public:
    OptionalRef() {}
    OptionalRef(T &val) : pValue{&val}, mLive(true) {}

    bool HasValue() { return mLive; }
    operator bool() { return mLive; }
    T &ValueRef() { return *pValue; }
    T &operator*() { return *pValue; }
    T *operator->() { return pValue; }
  };
} // namespace tules

/*

#include <cstdio>

struct NoDefaultCtor
{
    int i;
    NoDefaultCtor(int ii) : i{ii} {}
    NoDefaultCtor(int ii, int ii2) : i{ii + ii2} {}
    int val() const {return i;}
    ~NoDefaultCtor() {printf("ndct dies %d\n", i);}
};

int main()
{
  int ii = 10;

  tules::Optional<NoDefaultCtor> op{};

  tules::OptionalRef<int> oor{ii};
  if (oor)
    oor.ValueRef() = 4;

  tules::Optional<NoDefaultCtor> oo{6};
  oo = 10;
  if (oo)
    return oo->val() * ii;
  else
    return NoDefaultCtor(4).val() * ii;
}
//*/
