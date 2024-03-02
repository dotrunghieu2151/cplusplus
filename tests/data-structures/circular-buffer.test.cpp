#include <array>
#include <circular-buffer.hpp>
#include <cstddef>
#include <gtest/gtest.h>
#include <helpers.hpp>

using TestObj = helpers::Test;
class CircularBufferTest : public ::testing::Test {
public:
  CircularBuffer<TestObj, 5> buffer1;
  CircularBufferTest() : buffer1{} {}

  void testBufferEq(const CircularBuffer<TestObj, 5>& buff1,
                    const CircularBuffer<TestObj, 5>& buff2) {
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

TEST_F(CircularBufferTest, CtorDefaultValue) {
  CircularBuffer<TestObj, 5> buff{TestObj{0}};
  EXPECT_EQ(buff.size(), 5);
  for (const TestObj& i : buff) {
    EXPECT_EQ(i.num(), 0);
  }
}

TEST_F(CircularBufferTest, copyCtor) {
  CircularBuffer<TestObj, 5> buff{};
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

TEST_F(CircularBufferTest, moveCtor) {
  CircularBuffer<TestObj, 5> buff{};
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

TEST_F(CircularBufferTest, Iterator) {
  CircularBuffer<TestObj, 5> buff{};
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

  CircularBuffer<TestObj, 5> buff2{TestObj{0}};
  index = 0;
  for (const TestObj& i : buff2) {
    EXPECT_EQ(i.num(), 0);
    ++index;
  }
  EXPECT_EQ(buff2.size(), index);

  CircularBuffer<TestObj, 5> buff3{TestObj{0}};
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

TEST_F(CircularBufferTest, PushPop) {
  CircularBuffer<TestObj, 5> buff{};
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

TEST_F(CircularBufferTest, PushFrontPop) {
  CircularBuffer<TestObj, 5> buff{};
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

TEST_F(CircularBufferTest, PushBackPop) {
  CircularBuffer<TestObj, 5> buff{};
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

TEST_F(CircularBufferTest, InsertIndex) {
  CircularBuffer<TestObj, 5> buff{};
  buff.push_back(TestObj{0});
  buff.push_back(TestObj{1});
  buff.push_back(TestObj{3});
  buff.push_back(TestObj{4});
  EXPECT_EQ(buff.size(), 4);

  CircularBuffer<TestObj, 5>::iterator iter{buff.insert(2, TestObj{2})};
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

TEST_F(CircularBufferTest, InsertIter) {
  CircularBuffer<TestObj, 5> buff{};
  buff.push_back(TestObj{0});
  buff.push_back(TestObj{1});
  buff.push_back(TestObj{3});
  buff.push_back(TestObj{4});
  EXPECT_EQ(buff.size(), 4);

  CircularBuffer<TestObj, 5>::iterator iter{
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

TEST_F(CircularBufferTest, EraseIndex) {
  CircularBuffer<TestObj, 5> buff{};
  buff.push_back(TestObj{0});
  buff.push_back(TestObj{1});
  buff.push_back(TestObj{2});
  buff.push_back(TestObj{3});
  buff.push_back(TestObj{4});
  EXPECT_EQ(buff.size(), 5);

  CircularBuffer<TestObj, 5>::iterator iter{buff.erase(2)};
  std::size_t index{0};
  for (const TestObj& i : buff) {
    EXPECT_EQ(i.num(), buff[index]);
    ++index;
  }
  EXPECT_EQ(index, 4);
  EXPECT_EQ(iter->num(), 3);
}

TEST_F(CircularBufferTest, EraseIter) {
  CircularBuffer<TestObj, 5> buff{};
  buff.push_back(TestObj{0});
  buff.push_back(TestObj{1});
  buff.push_back(TestObj{2});
  buff.push_back(TestObj{3});
  buff.push_back(TestObj{4});
  EXPECT_EQ(buff.size(), 5);

  CircularBuffer<TestObj, 5>::iterator iter{buff.erase(buff.begin() + 2)};
  std::size_t index{0};
  for (const TestObj& i : buff) {
    EXPECT_EQ(i.num(), buff[index]);
    ++index;
  }
  EXPECT_EQ(index, 4);
  EXPECT_EQ(iter->num(), 3);
}

TEST_F(CircularBufferTest, InsertErase) {
  CircularBuffer<TestObj, 5> buff{};
  buff.push_back(TestObj{0});
  buff.push_back(TestObj{1});
  buff.push_back(TestObj{3});
  buff.push_back(TestObj{4});
  EXPECT_EQ(buff.size(), 4);

  CircularBuffer<TestObj, 5>::iterator iter{buff.insert(2, TestObj{2})};
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

TEST_F(CircularBufferTest, EraseInsert) {
  CircularBuffer<TestObj, 5> buff{};
  buff.push_back(TestObj{0});
  buff.push_back(TestObj{1});
  buff.push_back(TestObj{2});
  buff.push_back(TestObj{3});
  buff.push_back(TestObj{4});
  EXPECT_EQ(buff.size(), 5);

  CircularBuffer<TestObj, 5>::iterator iter{buff.erase(2)};
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

TEST_F(CircularBufferTest, InsertManyCopyIter) {
  CircularBuffer<TestObj, 7> buff{};
  buff.push_back(TestObj{0});
  buff.push_back(TestObj{1});
  buff.push_back(TestObj{6});
  EXPECT_EQ(buff.size(), 3);
  std::array insertArr{TestObj{2}, TestObj{3}, TestObj{4}, TestObj{5}};

  CircularBuffer<TestObj, 7>::iterator iter{
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

TEST_F(CircularBufferTest, InsertManyMoveIter) {
  CircularBuffer<TestObj, 7> buff{};
  buff.push_back(TestObj{0});
  buff.push_back(TestObj{1});
  buff.push_back(TestObj{6});
  EXPECT_EQ(buff.size(), 3);
  std::array insertArr{TestObj{2}, TestObj{3}, TestObj{4}, TestObj{5}};

  CircularBuffer<TestObj, 7>::iterator iter{
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

TEST_F(CircularBufferTest, EraseMany) {
  CircularBuffer<TestObj, 7> buff{};
  buff.push_back(TestObj{0});
  buff.push_back(TestObj{1});
  buff.push_back(TestObj{6});
  EXPECT_EQ(buff.size(), 3);
  std::array insertArr{TestObj{2}, TestObj{3}, TestObj{4}, TestObj{5}};

  CircularBuffer<TestObj, 7>::iterator iter{
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