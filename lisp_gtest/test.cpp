#include "pch.h"
#include "list.h"

TEST(TestList, TestCreateList)
{
  List list0;
  EXPECT_TRUE(IsEmptyList(list0));

  const auto list1 = Cons("test", list0);
  EXPECT_TRUE(!IsEmptyList(list1));

  EXPECT_EQ("( test ) ", to_string(list1));

  const auto list2 = Cons("test", list0);
  EXPECT_EQ(list1, list2);

  EXPECT_EQ(list0, list1);
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
#if 0
std::string testMakeList()
{
  auto list0 = MakeList();
  auto list1 = MakeList("a");
  auto list2 = MakeList("a", "b");
  auto list2_0 = MakeList("a", MakeList());
  auto list3_2 = MakeList("a", MakeList("b"), "c");

  return "";
}
#endif