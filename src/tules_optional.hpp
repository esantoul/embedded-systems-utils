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
   * @tparam T the type that is contained
   */
  template <typename T>
  class Optional
  {
  public:
    /**
     * @brief Empty constructor
     */
    Optional() {}

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

    /**
     * @return true if contains a value else false
     */
    bool has_value() { return mLive; }

    /**
     * @return true if contains a value else false
     */
    operator bool() { return mLive; }

    /**
     * @return the stored value. Undefined behaviour if no value is contained
     */
    T value() { return mValue; }

    /**
     * @return a copy of the contained value
     */
    T operator*() { return mValue; }

    /**
     * @brief to access members of the contained value
     */
    T *operator->() { return &mValue; }

    /**
     * @param other a value to return if the Optional does not contain a value
     * @return the value contained by the Optional or other
     */
    T value_or(const T &other) { return mLive ? mValue : other; }

    /**
     * @param other a value to return if the Optional does not contain a value
     * @return a const reference to the value contained by the Optional or other
     */
    const T &value_cref_or(const T &other) { return mLive ? mValue : other; }

  private:
    union {
      T mValue;
    };
    bool mLive = false;
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
  tules::Optional<TObj> oo{6};
  tules::Optional<TObj> oo2{};
  oo2 = tules::Optional<TObj>{5, 2};
  if (oo)
    return oo->val();
  else
    return TObj(4).val();
}
//*/
