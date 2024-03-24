#include "pch.h"
#include "list.h"

const List EmptyList;
const Car_t CarNull{EmptyList};

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
  cons_{new Cons_{car, list.cons_, 1}}
{
  if (cons_->cdr_)
    ++cons_->cdr_->numWatchers_;
}

List::List(const List& list):
  List{list.cons_}
{
}
List& List::operator=(List list)
{
  std::swap(cons_, list.cons_);
  return *this;
}
const Car_t& List::Car() const
{
  return cons_ ? cons_->car_ : CarNull;
}
List List::Cdr() const
{
  return cons_ ? List{cons_->cdr_} : EmptyList;
}

List MakeList()
{
  return List{};
}
List MakeList(const Car_t& car)
{
  return Cons(car, EmptyList);
}

bool List::IsEmpty() const
{
  return !cons_;
}

List Cons(const Car_t& car, const List& cdr)
{
  return List{cdr, car};
}
const Car_t& Car(const Car_t& car)
{
  const auto list = std::get_if<List>(&car);
  return list ? list->Car() : car;
}
List Cdr(const Car_t& car)
{
  const auto list = std::get_if<List>(&car);
  return list ? list->Cdr() : List{};
}
bool IsEmptyList(const Car_t& car)
{
  const auto list = std::get_if<List>(&car);
  return !list || list->IsEmpty();
}
bool IsAtom(const Car_t& car)
{
  const auto list = std::get_if<List>(&car);
  return !list;
}
bool IsEqual(const Car_t& lhs, const Car_t& rhs)
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
        return IsEqual(Car(lhsList),Car(*rhsList))
            && IsEqual(Cdr(lhsList),Cdr(*rhsList));
      break;
    }
  }
  return false;
}
bool operator==(const List& lhs, const List& rhs)
{
  return IsEqual(lhs, rhs);
}

std::string Print(const List& list)
{
  return IsEmptyList(list) ? "" : to_string(Car(list)) + Print(Cdr(list));
}
std::string to_string(const Car_t& car)
{
  if (const auto text = std::get_if<std::string_view>(&car))
    return std::string{*text} + " ";

  if (const auto list = std::get_if<List>(&car))
    return "( " + Print(*list) + ") ";

  return "Unknown type?";
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
      return IsEqual(Car(list), key);
    });
}
List FindKeyValue(const List& list, const Car_t& key)
{
  return Find(list, [&key](const List& list)
    {
      return IsEqual(Car(list), key);
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
List Eval(const List& list, const List& context)
{
  if (IsEmptyList(list))
    return list;

  const auto& car = Car(list);

  const auto& contextEntry = FindKeyValue(context, car);
  (void*)(&contextEntry);
  return list;
}
