#include "pch.h"
#include "list.h"

TEST(TestList, TestCreateList)
{
  List list0;
  EXPECT_TRUE(IsEmptyList(list0));

  const auto list1 = Cons("test", list0);
  EXPECT_TRUE(!IsEmptyList(list1));

  EXPECT_EQ("(test)", to_string(list1));

  const auto list2 = Cons("test", list0);
  EXPECT_EQ(list1, list2);

  EXPECT_NE(list0, list1);
}

TEST(TestList, TestFind)
{
  const auto list1 = Cons("a", EmptyList);
  const auto list2 = Cons("b", list1);
  const auto list3 = Cons("c", list2);

  struct
  {
    long line;
    std::string msg;
    List expression;
    List expected;
  } tests[]
  {
    {__LINE__, "find c", FindKey(list3, "c"), list3},
    {__LINE__, "find a", FindKey(list3, "a"), list1},
    {__LINE__, "find z", FindKey(list3, "z"), EmptyList},
  };

  for (const auto& test: tests)
    EXPECT_EQ(test.expected, test.expression) << test.line << " " + test.msg;
}

TEST(TestList, TestListInitializerList)
{
  EXPECT_EQ("()", to_string(List({})));
  EXPECT_EQ("(a)", to_string(List({"a"})));
  EXPECT_EQ("(a b)", to_string(List({"a", "b"})));
  EXPECT_EQ("(a ())", to_string(List({"a", List()})));
  EXPECT_EQ("(a (b c) d)", to_string(List({"a", List({"b", "c"}), "d"})));
}

TEST(TestList, TestStringToString)
{
  struct
  {
    size_t line;
    std::string msg;
    std::string str;
  } tests[] =
  {
    {__LINE__,"","()"},
    {__LINE__,"","(a)"},
    {__LINE__,"","(a b)"},
    {__LINE__,"","(a (b))"},
    {__LINE__,"","((a))"},
    {__LINE__,"","((a) b)"},
    {__LINE__,"","(a (b (b2)) c (() d d2))"},
  };

  for (const auto& test : tests)
  {
    EXPECT_EQ(test.str, to_string(List(test.str))) << "(" << test.line << ") " << test.msg;
  }
}