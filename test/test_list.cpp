extern "C"
{
    #include "list.h"
}
#include "gtest/gtest.h"

int test_cmp_fnc(const void* a, const void* b)
{
    if (NULL == a || NULL == b) return 0;
    return (*(int*)a)-(*(int*)b);
}
void test_free_fnc(void* a)
{
    free(a);
}

//Tests a valid usage
TEST(ListCreateTest, ValidArgs) {
  EXPECT_NE(List_Create(10, test_cmp_fnc, test_free_fnc), nullptr); //all valid args
  EXPECT_NE(List_Create(10, NULL, test_free_fnc), nullptr); //null cmp is allowed
}
//Test List create with improper args
TEST(ListCreateTest, ImproperArgs) {
  EXPECT_EQ(List_Create(0, test_cmp_fnc, test_free_fnc), nullptr); //must allow atleast a single node
  EXPECT_EQ(List_Create(10, test_cmp_fnc, NULL), nullptr); //null free not allowed
}