#include "pch.h"
#include "list.h"

const List EmptyList;

List::~List()
{
  auto cons = cons_;
  while (cons)
  {
    --cons->numWatchers_;
    if (cons->numWatchers_ != 0)
      break;

    const auto cdr = cons->cdr_;
    delete cons;
    cons = cdr;
  }
}

List::List() = default;

List::List(Cons_ const * cons):
  cons_{cons}
{
  if (cons_)
    ++cons_->numWatchers_;
}

List::List(const List& list, const Car_t& car):
  cons_{new Cons_{std::make_unique<Car_t const>(car), list.cons_, 1}}
{
  if (cons_->cdr_)
    ++cons_->cdr_->numWatchers_;
}

List::List(const List& list):
  List(list.cons_)
{
}

using ConstCharPtr = char const *;
std::string_view ReadString(char const* const begin, char const* const end, ConstCharPtr& c)
{
  c = std::find(begin, end, '"');
  return std::string_view{begin, static_cast<size_t>(std::distance(begin,c++))};
}
std::string_view ReadSymbol(char const* const begin, char const* const end, ConstCharPtr& c)
{
  c = std::find_if(begin, end, [](const auto c)
      {
        return c == ')' || isspace(c);
      });
  return std::string_view{begin, static_cast<size_t>(std::distance(begin,c))};
}
Cons_ const * List::ReadList_(char const* const begin, char const* const end, ConstCharPtr& c)
{
  Cons_ const * list = nullptr;
  auto * next = &list;

  c = begin;
  while (c != end)
  {
    if (c == nullptr)
    {
      c = end;
      break;
    }
    if (*c == ')')
    {
      ++c;
      break;
    }
    if (*c == '"')
    {
      auto str = ReadString(c+1, end, c);
      *next = new Cons_{std::make_unique<Car_t const>(str), nullptr, 1};
      next = const_cast<Cons_ const **>(&(**next).cdr_);
    }
    else if (*c == '(')
    {
      auto cons = ReadList_(c+1, end, c);
      *next = new Cons_{std::make_unique<Car_t const>(List(cons)), nullptr, 1};
      next = const_cast<Cons_ const **>(&(**next).cdr_);
    }
    else if (!isspace(*c))
    {
      auto str = ReadSymbol(c, end, c);
      *next = new Cons_{std::make_unique<Car_t const>(str), nullptr, 1};
      next = const_cast<Cons_ const **>(&(**next).cdr_);
    }
    else //if (isspace(*c))
    {
      ++c;
    }
  }

  return list;
}
char const * cDummy = nullptr;
List::List(const std::string_view& s):
  List(ReadList_(std::find(s.data(), s.data()+s.size(), '(')+1, s.data() + s.size(), cDummy))
{
}
List& List::operator=(List list)
{
  std::swap(cons_, list.cons_);
  return *this;
}

List::List(const std::initializer_list<Car_t>& contents)
{
  for (auto car = rbegin(contents); car != rend(contents); ++car)
  {
    cons_ = new Cons_{std::make_unique<Car_t const>(*car), cons_, 1};
  }
}

Car_t const * List::Car() const
{
  return cons_ ? cons_->car_.get() : nullptr;
}
List List::Cdr() const
{
  return cons_ ? List(cons_->cdr_) : EmptyList;
}

List Cons(const Car_t& car, const List& cdr)
{
  return List(cdr, car);
}
Car_t const * Car(const Car_t& car)
{
  const auto list = std::get_if<List>(&car);
  return list ? list->Car() : nullptr;
}
List Cdr(const Car_t& car)
{
  const auto list = std::get_if<List>(&car);
  return list ? list->Cdr() : List();
}
bool IsEmptyList(const Car_t& car)
{
  const auto list = std::get_if<List>(&car);
  return !list || !list->Car();
}
bool IsAtom(const Car_t& car)
{
  const auto list = std::get_if<List>(&car);
  return !list;
}
bool operator==(const List& lhs, const List& rhs)
{
  const auto carLhs = lhs.Car();
  const auto carRhs = rhs.Car();

  if (!carLhs != !carRhs)
    return false;
  if (!carLhs)
    return true;

  return *carLhs == *carRhs
      && Cdr(lhs) == Cdr(rhs);
}  
bool operator==(const Car_t& lhs, const Car_t& rhs)
{
  if (lhs.index() != rhs.index())
    return false;

  switch (lhs.index())
  {
    case Car_::text:
    {
      const auto& lhsString = std::get<std::string_view>(lhs);
      if (const auto rhsString = std::get_if<std::string_view>(&rhs))
        return lhsString == *rhsString;
      break;
    }
    case Car_::list:
    {
      const auto& lhsList = std::get<List>(lhs);
      if (const auto rhsList = std::get_if<List>(&rhs))
        return lhsList == *rhsList;
      break;
    }
  }
  return false;
}

std::string PrintContents(const List& list)
{
  const auto& car = Car(list);
  const auto& cdr = Cdr(list);
  std::string s;
  if (car)
  {
    s = to_string(*car);
    if (Car(cdr))
      s += " " + PrintContents(cdr);
  }

  return s;
}
std::string to_string(const List& list)
{
  return "(" + PrintContents(list) + ")";
}
std::string to_string(const Car_t& car)
{
  if (const auto text = std::get_if<std::string_view>(&car))
    return std::string{*text};

  if (const auto list = std::get_if<List>(&car))
    return to_string(*list);

  return "Unknown type?";
}
std::ostream& operator<<(std::ostream& os, const Car_t& car)
{
  return os << to_string(car);
}
std::ostream& operator<<(std::ostream& os, const List& list)
{
  return os << to_string(list);
}

List CreateInitialContext_()
{
  //A list of named entities, with the last added given precedence
  //Each entry is a pair of string name and value
  const auto Entry = [](std::string_view name, const Car_t& value)
    {
      List list;
      list = Cons(value, list);
      list = Cons(name, list);
      return list;
    };

  List list;
  list = Cons(Entry("a", "A"), list);

  return list;
}

const List& InitialContext()
{
  static auto list = CreateInitialContext_();
  return list;
}

List Find(const List& list, const std::function<bool(const List&)>& pred)
{
  if (pred(list))
    return list;

  if (IsEmptyList(list))
    return EmptyList;

  return Find(Cdr(list), pred);
}
List FindKey(const List& list, const Car_t& key)
{
  return Find(list, [&key](const List& list)
    {
      const auto car = Car(list);
      return car && (*car == key);
    });
}

template<typename TChar>
List ReadList(TChar /*begin*/, TChar /*end*/)
{
  return {};
}

List Read(const std::string_view text)
{
  return ReadList(text.cbegin(), text.cend());
}
List Eval(const List& list, const List& /*context*/)
{
  if (IsEmptyList(list))
    return list;

  return list;
}
