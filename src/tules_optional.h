#pragma once

/**
 * Optional and Optional Reference data container
 */

namespace tules
{
  template <typename T>
  class Optional
  {
  private:
    T mValue;
    bool mLive;

  public:
    Optional() : mValue{}, mLive(false) {}
    Optional(const T &val) : mValue{val}, mLive(true) {}

    bool HasValue() { return mLive; }
    operator bool() { return mLive; }
    T Value() { return mValue; }
    T operator*() { return mValue; }
    T *operator->() { return &mValue; }
    T ValueOr(const T &other) { return mLive ? Value() : other; }
  };

  template <typename T>
  class OptionalRef // When using this, all the proper checks need to be done by the user
  {
  private:
    T *pValue;
    bool mLive;

  public:
    OptionalRef() : pValue{nullptr}, mLive(false) {}
    OptionalRef(T &val) : pValue{&val}, mLive(true) {}

    operator bool() { return mLive; }
    T &ValueRef() { return *pValue; }
    T &operator*() { return *pValue; }
    T *operator->() { return pValue; }
  };
} // namespace tules

/*
#include <string>
int main()
{
  int ii = 10;
  tules::OptionalRef<int> oor{ii};
  if (oor)
    oor.ValueRef() = 4;

  std::string cc = "coucou";
  tules::Optional<std::string> oo{cc};
  return oo.ValueOr("oups").size() * ii;
}
//*/
