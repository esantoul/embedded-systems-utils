#pragma once

/**
 * @file Optional and Optional Reference data container header file
 * @author Etienne Santoul
 */

#include <new>
#include <utility>

namespace tules // Template Utility Library for Embedded Systems
{
  /**
   * @brief An optional container where the contained object is constructed only if explicitely needed
   */
  template <typename T>
  class Optional
  {
  private:
    union {
      T mValue;
    };
    bool mLive = false;

  public:
    /**
     * @brief Empty constructor
     */
    Optional()
    {
    }

    /**
     * @brief Copy constructor
     * @param other an Optional<T> lvalue
     */
    Optional(const Optional &other)
    {
      if (other.mLive)
        new (&mValue) T(other.mValue);
      mLive = other.mLive;
    }

    /**
     * @brief Move constructor
     * @param other an Optional<T> rvalue
     */
    Optional(Optional &&other)
    {
      if (other.mLive)
        new (&mValue) T(std::move(other.mValue));
      mLive = other.mLive;
    }

    /**
     * @brief Direct initializing constructor
     * @param args a list of arguments to construct type T
     */
    template <typename... Args>
    Optional(const Args &... args) : mValue{args...}, mLive(true)
    {
    }

    /**
     * @brief Move assignment
     * @param other an Optional<T> rvalue
     * @return a lvalue reference to *this
     */
    Optional &operator=(Optional &&other)
    {
      if (mLive)
        mValue.~T();
      if (other.mLive)
        new (&mValue) T(std::move(other.mValue));
      mLive = other.mLive;
      return *this;
    }

    /**
     * @brief Copy assignment
     * @param other an Optional<T> lvalue
     * @return a lvalue reference to *this
     */
    Optional &operator=(const Optional &other)
    {
      if (mLive)
        mValue.~T();
      if (other.mLive)
        new (&mValue) T(other.mValue);
      mLive = other.mLive;
      return *this;
    }

    /**
     * @brief Direct initializing assignment
     * @param val a value to construct type T
     * @return a lvalue reference to *this
     */
    template <typename O>
    Optional &operator=(const O &val)
    {
      if (mLive)
        mValue.~T();
      new (&mValue) T(val);
      mLive = true;
      return *this;
    }

    /**
     * @brief Destructor
     */
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

struct TObj
{
  int i;
  TObj(int ii) : i{ii} { printf("tobj 1 i ctor %d\n", i); }
  TObj(int ii, int ii2) : i{ii + ii2} { printf("tobj 2 i ctor %d\n", i); }
  TObj(const TObj &o) : i{o.i} { printf("tobj cpy %d\n", i); }
  TObj(TObj &&o) : i{o.i} { printf("tobj move %d\n", i); }
  int val() const { return i; }
  ~TObj() { printf("tobj dies %d\n", i); }
};

int main()
{
  int ii = 10;

  tules::OptionalRef<int> oor{ii};
  if (oor)
    oor.ValueRef() = 4;

  tules::Optional<TObj> oo{6};
  tules::Optional<TObj> oo2{};
  oo2 = tules::Optional<TObj>{5, 2};
  if (oo)
    return oo->val() * ii;
  else
    return TObj(4).val() * ii;
}
//*/
