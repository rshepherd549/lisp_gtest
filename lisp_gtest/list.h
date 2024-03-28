#pragma once
#include <functional>
#include <memory>
#include <string>
#include <iostream>
#include <variant>

template <typename TVariant, typename TType, size_t idx = 0>
constexpr size_t variant_index()
{
  if constexpr (idx == std::variant_size_v<TVariant>) //NOLINT(readability-braces-around-statements)
  {
    //Return size if idx reaches size without a match
    return idx;
  }
  else if constexpr (std::is_same_v<std::variant_alternative_t<idx, TVariant>, TType>) //NOLINT(readability-braces-around-statements)
  {
    //Return idx if the type matches
    return idx;
  }
  else //NOTE: if constexpr needs the elses
  {
    //Recurse to the next value of idx
    return variant_index<TVariant, TType, idx + 1>();
  }
}

class List;

extern const List EmptyList;

using Car_t = std::variant<std::string_view, List>;
enum Car_
{
  text = variant_index<Car_t, std::string_view>(),
  list = variant_index<Car_t, List>(),
};

struct Cons_
{
  std::unique_ptr<Car_t const> const car_;
  Cons_ const * cdr_{};
  mutable size_t numWatchers_{};
};

class List
{
  Cons_ const * cons_{};
  List(Cons_ const * cons);

  using ConstCharPtr = char const *;
  static Cons_ const * ReadList_(char const* const begin, char const* const end, ConstCharPtr& c);

public:

  ~List();
  List();
  List(const List& list);
  List(const List& list, const Car_t& value);
  List(const std::string_view& s);

  List& operator=(List list);

  List(const std::initializer_list<Car_t>& contents);

  Car_t const * Car() const;
  List Cdr() const;
};

List Cons(const Car_t& car, const List& cdr);
Car_t const * Car(const Car_t& car);
List Cdr(const Car_t& car);

bool IsEmptyList(const Car_t& car);
bool IsAtom(const Car_t& car);
bool operator==(const List& lhs, const List& rhs);
bool operator==(const Car_t& lhs, const Car_t& rhs);
std::string to_string(const List& list);
std::string to_string(const Car_t& car);
std::ostream& operator<<(std::ostream& os, const Car_t& car);
std::ostream& operator<<(std::ostream& os, const List& list);

const List& InitialContext();

List Find(const List& list, const std::function<bool(const List&)>& pred);
List FindKey(const List& list, const Car_t& key);

List Read(const std::string_view text);
List Eval(const List& list, const List& context);
