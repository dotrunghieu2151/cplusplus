#include <array>
#include <cstddef>
#include <gtest/gtest.h>
#include <helpers.hpp>
#include <static-circular-buffer.hpp>

using TestObj = helpers::Test;
class ContainerTest : public ::testing::Test {
public:
  StaticCircularBuffer<TestObj, 5> buffer1;
  ContainerTest() : buffer1{} {}

  void testBufferEq(const StaticCircularBuffer<TestObj, 5>& buff1,
                    const StaticCircularBuffer<TestObj, 5>& buff2) {
    EXPECT_EQ(buff1.size(), buff2.size());
    std::size_t index{0};
    for (const TestObj& i : buff2) {
      EXPECT_EQ(i.num(), buff1[index].num());
      ++index;
    }
  }

protected:
  void SetUp() override{};
};

TEST_F(ContainerTest, CtorDefaultValue) {
  StaticCircularBuffer<TestObj, 5> buff{TestObj{0}};
  EXPECT_EQ(buff.size(), 5);
  for (const TestObj& i : buff) {
    EXPECT_EQ(i.num(), 0);
  }
}

TEST_F(ContainerTest, copyCtor) {
  StaticCircularBuffer<TestObj, 5> buff{};
  buff.push_back(TestObj{1});
  buff.pop_front();
  buff.push_back(TestObj{1});
  buff.pop_front();
  buff.push_back(TestObj{1});
  buffer1 = buff;
  EXPECT_EQ(buffer1.size(), 1);
  buffer1.push_back(TestObj{2});
  EXPECT_EQ(buffer1.size(), 2);
}

TEST_F(ContainerTest, moveCtor) {
  StaticCircularBuffer<TestObj, 5> buff{};
  buff.push_back(TestObj{1});
  buff.pop_front();
  buff.push_back(TestObj{1});
  buff.pop_front();
  buff.push_back(TestObj{1});
  buffer1 = std::move(buff);
  EXPECT_EQ(buffer1.size(), 1);
  buffer1.push_back(TestObj{2});
  EXPECT_EQ(buffer1.size(), 2);
}

TEST_F(ContainerTest, Iterator) {
  StaticCircularBuffer<TestObj, 5> buff{};
  buff.push_back(TestObj{1});
  buff.pop_front();
  buff.push_back(TestObj{1});
  buff.pop_front();
  buff.push_back(TestObj{1});
  buff.pop_front();
  buff.push_back(TestObj{1});
  buff.pop_front();
  buff.push_back(TestObj{0});
  buff.push_back(TestObj{1});
  buff.push_back(TestObj{2});
  std::size_t index{0};
  for (const TestObj& i : buff) {
    EXPECT_EQ(i.num(), index);
    ++index;
  }
  EXPECT_EQ(buff.size(), 3);
  EXPECT_EQ(index, 3);

  EXPECT_EQ(buff[0].num(), 0);
  EXPECT_EQ(buff[1].num(), 1);
  EXPECT_EQ(buff[2].num(), 2);

  StaticCircularBuffer<TestObj, 5> buff2{TestObj{0}};
  index = 0;
  for (const TestObj& i : buff2) {
    EXPECT_EQ(i.num(), 0);
    ++index;
  }
  EXPECT_EQ(buff2.size(), index);

  StaticCircularBuffer<TestObj, 5> buff3{TestObj{0}};
  buff3.pop_front();
  buff3.pop_front();
  buff3.push_back(TestObj{0});
  index = 0;
  for (std::size_t i{}; i < 4; ++i) {
    buff3[i] = TestObj{static_cast<int>(i)};
  }
  EXPECT_EQ(buff3.size(), 4);

  EXPECT_EQ(buff3.begin() + buff3.size() == buff3.end(), true);
  EXPECT_EQ(buff3.end() - buff3.size() == buff3.begin(), true);
  EXPECT_EQ(buff3.begin() + 1 == buff3.end() - (buff3.size() - 1), true);
  buff3.pop_front();
  EXPECT_EQ(buff3.begin() + buff3.size() == buff3.end(), true);
  EXPECT_EQ(buff3.end() - buff3.size() == buff3.begin(), true);
  EXPECT_EQ(buff3.begin() + 1 == buff3.end() - (buff3.size() - 1), true);
}

TEST_F(ContainerTest, PushPop) {
  StaticCircularBuffer<TestObj, 5> buff{};
  buff.push_back(TestObj{1});
  buff.pop_front();
  buff.push_back(TestObj{1});
  buff.pop_front();
  buff.push_back(TestObj{1});
  buff.pop_front();

  buff.push_front(TestObj{2});
  buff.push_front(TestObj{1});
  buff.push_front(TestObj{0});

  std::size_t index{0};
  for (const TestObj& i : buff) {
    EXPECT_EQ(i.num(), index);
    ++index;
  }

  EXPECT_EQ(buff[2].num(), 2);

  buff.pop_back();
  EXPECT_EQ(buff[buff.size() - 1].num(), 1);

  buff.pop_front();
  EXPECT_EQ(buff[0].num(), 1);
  buff.push_back(TestObj{2});
  buff.push_back(TestObj{3});
  buff.push_back(TestObj{4});
  buff.push_back(TestObj{0});

  for (std::size_t i{1}; i < 4; ++i) {
    EXPECT_EQ(buff[i].num(), i + 1);
  }
  EXPECT_EQ(buff.size(), 5);
  EXPECT_EQ(buff[buff.size() - 1].num(), 0);
  index = 0;
  for (const TestObj& i : buff) {
    EXPECT_EQ(i.num(), buff[index].num());
    ++index;
  }

  buff.pop_front();
  buff.pop_front();
  index = 0;
  for (const TestObj& i : buff) {
    EXPECT_EQ(i.num(), buff[index].num());
    ++index;
  }

  buff.pop_back();
  buff.pop_back();
  EXPECT_EQ(buff[0].num(), 3);

  buff.push_back(TestObj{4});
  buff.push_back(TestObj{0});
  EXPECT_EQ(buff[buff.size() - 1].num(), 0);
  buff.pop_back();
  buff.pop_back();
  buff.pop_back();
  EXPECT_EQ(buff.size(), 0);
  buff.push_front(TestObj{2});
  buff.push_front(TestObj{1});
  buff.push_front(TestObj{0});
  buff.push_front(TestObj{4});
  buff.push_front(TestObj{3});
  EXPECT_EQ(buff.size(), 5);
  index = 0;
  for (const TestObj& i : buff) {
    EXPECT_EQ(i.num(), buff[index].num());
    ++index;
  }
  EXPECT_EQ(buff[3].num(), 1);
}

TEST_F(ContainerTest, PushFrontPop) {
  StaticCircularBuffer<TestObj, 5> buff{};
  buff.push_front(TestObj{4});
  buff.push_front(TestObj{3});
  buff.push_front(TestObj{2});
  buff.push_front(TestObj{1});
  buff.push_front(TestObj{0});
  EXPECT_EQ(buff.size(), 5);
  std::size_t index{0};
  for (const TestObj& i : buff) {
    EXPECT_EQ(i.num(), index);
    ++index;
  }
  EXPECT_EQ(index, 5);

  index = 5;
  while (!buff.empty()) {
    --index;
    TestObj ele{buff.pop_back()};
    EXPECT_EQ(ele.num(), index);
  }
}

TEST_F(ContainerTest, PushBackPop) {
  StaticCircularBuffer<TestObj, 5> buff{};
  buff.push_back(TestObj{0});
  buff.push_back(TestObj{1});
  buff.push_back(TestObj{2});
  buff.push_back(TestObj{3});
  buff.push_back(TestObj{4});
  EXPECT_EQ(buff.size(), 5);
  std::size_t index{0};
  for (const TestObj& i : buff) {
    EXPECT_EQ(i.num(), index);
    ++index;
  }
  EXPECT_EQ(index, 5);

  index = 0;
  while (!buff.empty()) {
    TestObj ele{buff.pop_front()};
    EXPECT_EQ(ele.num(), index);
    ++index;
  }
}

TEST_F(ContainerTest, InsertIndex) {
  StaticCircularBuffer<TestObj, 5> buff{};
  buff.push_back(TestObj{0});
  buff.push_back(TestObj{1});
  buff.push_back(TestObj{3});
  buff.push_back(TestObj{4});
  EXPECT_EQ(buff.size(), 4);

  StaticCircularBuffer<TestObj, 5>::iterator iter{buff.insert(2, TestObj{2})};
  std::size_t index{0};
  for (const TestObj& i : buff) {
    EXPECT_EQ(i.num(), index);
    ++index;
  }
  index = 0;
  for (const TestObj& i : buff) {
    EXPECT_EQ(i.num(), buff[index]);
    ++index;
  }
  EXPECT_EQ(index, 5);
  EXPECT_EQ(iter->num(), 2);
}

TEST_F(ContainerTest, InsertIter) {
  StaticCircularBuffer<TestObj, 5> buff{};
  buff.push_back(TestObj{0});
  buff.push_back(TestObj{1});
  buff.push_back(TestObj{3});
  buff.push_back(TestObj{4});
  EXPECT_EQ(buff.size(), 4);

  StaticCircularBuffer<TestObj, 5>::iterator iter{
      buff.insert(buff.begin() + 2, TestObj{2})};
  std::size_t index{0};
  for (const TestObj& i : buff) {
    EXPECT_EQ(i.num(), index);
    ++index;
  }
  index = 0;
  for (const TestObj& i : buff) {
    EXPECT_EQ(i.num(), buff[index]);
    ++index;
  }
  EXPECT_EQ(index, 5);
  EXPECT_EQ(iter->num(), 2);
}

TEST_F(ContainerTest, EraseIndex) {
  StaticCircularBuffer<TestObj, 5> buff{};
  buff.push_back(TestObj{0});
  buff.push_back(TestObj{1});
  buff.push_back(TestObj{2});
  buff.push_back(TestObj{3});
  buff.push_back(TestObj{4});
  EXPECT_EQ(buff.size(), 5);

  StaticCircularBuffer<TestObj, 5>::iterator iter{buff.erase(2)};
  std::size_t index{0};
  for (const TestObj& i : buff) {
    EXPECT_EQ(i.num(), buff[index]);
    ++index;
  }
  EXPECT_EQ(index, 4);
  EXPECT_EQ(iter->num(), 3);
}

TEST_F(ContainerTest, EraseIter) {
  StaticCircularBuffer<TestObj, 5> buff{};
  buff.push_back(TestObj{0});
  buff.push_back(TestObj{1});
  buff.push_back(TestObj{2});
  buff.push_back(TestObj{3});
  buff.push_back(TestObj{4});
  EXPECT_EQ(buff.size(), 5);

  StaticCircularBuffer<TestObj, 5>::iterator iter{buff.erase(buff.begin() + 2)};
  std::size_t index{0};
  for (const TestObj& i : buff) {
    EXPECT_EQ(i.num(), buff[index]);
    ++index;
  }
  EXPECT_EQ(index, 4);
  EXPECT_EQ(iter->num(), 3);
}

TEST_F(ContainerTest, InsertErase) {
  StaticCircularBuffer<TestObj, 5> buff{};
  buff.push_back(TestObj{0});
  buff.push_back(TestObj{1});
  buff.push_back(TestObj{3});
  buff.push_back(TestObj{4});
  EXPECT_EQ(buff.size(), 4);

  StaticCircularBuffer<TestObj, 5>::iterator iter{buff.insert(2, TestObj{2})};
  std::size_t index{0};
  for (const TestObj& i : buff) {
    EXPECT_EQ(i.num(), index);
    ++index;
  }
  index = 0;
  for (const TestObj& i : buff) {
    EXPECT_EQ(i.num(), buff[index]);
    ++index;
  }
  EXPECT_EQ(index, 5);
  EXPECT_EQ(iter->num(), 2);

  // erase

  iter = buff.erase(iter);
  EXPECT_EQ(buff.size(), 4);
  EXPECT_EQ(iter->num(), 3);
}

TEST_F(ContainerTest, EraseInsert) {
  StaticCircularBuffer<TestObj, 5> buff{};
  buff.push_back(TestObj{0});
  buff.push_back(TestObj{1});
  buff.push_back(TestObj{2});
  buff.push_back(TestObj{3});
  buff.push_back(TestObj{4});
  EXPECT_EQ(buff.size(), 5);

  StaticCircularBuffer<TestObj, 5>::iterator iter{buff.erase(2)};
  std::size_t index{0};
  for (const TestObj& i : buff) {
    EXPECT_EQ(i.num(), buff[index]);
    ++index;
  }
  EXPECT_EQ(index, 4);
  EXPECT_EQ(iter->num(), 3);

  // insert
  iter = buff.insert(iter, TestObj{2});
  EXPECT_EQ(buff.size(), 5);
  EXPECT_EQ(iter->num(), 2);
}

TEST_F(ContainerTest, InsertManyCopyIter) {
  StaticCircularBuffer<TestObj, 7> buff{};
  buff.push_back(TestObj{0});
  buff.push_back(TestObj{1});
  buff.push_back(TestObj{6});
  EXPECT_EQ(buff.size(), 3);
  std::array insertArr{TestObj{2}, TestObj{3}, TestObj{4}, TestObj{5}};

  StaticCircularBuffer<TestObj, 7>::iterator iter{
      buff.insert(2, insertArr.begin(), insertArr.end())};
  std::size_t index{0};
  for (const TestObj& i : buff) {
    EXPECT_EQ(i.num(), index);
    ++index;
  }
  index = 0;
  for (const TestObj& i : buff) {
    EXPECT_EQ(i.num(), buff[index]);
    ++index;
  }
  EXPECT_EQ(index, 7);
  EXPECT_EQ(iter->num(), 2);
}

TEST_F(ContainerTest, InsertManyMoveIter) {
  StaticCircularBuffer<TestObj, 7> buff{};
  buff.push_back(TestObj{0});
  buff.push_back(TestObj{1});
  buff.push_back(TestObj{6});
  EXPECT_EQ(buff.size(), 3);
  std::array insertArr{TestObj{2}, TestObj{3}, TestObj{4}, TestObj{5}};

  StaticCircularBuffer<TestObj, 7>::iterator iter{
      buff.insert(2, std::make_move_iterator(insertArr.begin()),
                  std::make_move_iterator(insertArr.end()))};
  std::size_t index{0};
  for (const TestObj& i : buff) {
    EXPECT_EQ(i.num(), index);
    ++index;
  }
  index = 0;
  for (const TestObj& i : buff) {
    EXPECT_EQ(i.num(), buff[index]);
    ++index;
  }
  EXPECT_EQ(index, 7);
  EXPECT_EQ(iter->num(), 2);
}

TEST_F(ContainerTest, EraseMany) {
  StaticCircularBuffer<TestObj, 7> buff{};
  buff.push_back(TestObj{0});
  buff.push_back(TestObj{1});
  buff.push_back(TestObj{6});
  EXPECT_EQ(buff.size(), 3);
  std::array insertArr{TestObj{2}, TestObj{3}, TestObj{4}, TestObj{5}};

  StaticCircularBuffer<TestObj, 7>::iterator iter{
      buff.insert(2, insertArr.begin(), insertArr.end())};
  std::size_t index{0};
  for (const TestObj& i : buff) {
    EXPECT_EQ(i.num(), index);
    ++index;
  }
  index = 0;
  for (const TestObj& i : buff) {
    EXPECT_EQ(i.num(), buff[index]);
    ++index;
  }
  EXPECT_EQ(index, 7);
  EXPECT_EQ(iter->num(), 2);

  iter = buff.erase(buff.begin() + 3, buff.end() - 1);
  index = 0;
  for (const TestObj& i : buff) {
    EXPECT_EQ(i.num(), buff[index]);
    ++index;
  }
  EXPECT_EQ(index, 4);
  EXPECT_EQ(iter->num(), 6);

  iter = buff.erase(buff.begin() + 2, buff.end());
  index = 0;
  for (const TestObj& i : buff) {
    EXPECT_EQ(i.num(), buff[index]);
    ++index;
  }
  EXPECT_EQ(index, 2);
  EXPECT_EQ(iter, buff.end());
}