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
TEST(ListCreateTest, InvalidArgs) {
    EXPECT_EQ(List_Create(0, test_cmp_fnc, test_free_fnc), nullptr); //must allow atleast a single node
    EXPECT_EQ(List_Create(10, test_cmp_fnc, NULL), nullptr); //null free not allowed
}
        
//Tests a valid usage
TEST(ListInsertTest, ValidArgs) {
    List_t* test_list = List_Create(10, test_cmp_fnc, test_free_fnc);

    int test_val1 = 255;

    EXPECT_EQ(List_Insert(&test_val1, 0, test_list), LIST_ERROR_SUCCESS); //first node in list
    EXPECT_EQ(List_Insert(&test_val1, 1, test_list), LIST_ERROR_SUCCESS); //back of list
    EXPECT_EQ(List_Insert(&test_val1, 0, test_list), LIST_ERROR_SUCCESS); //front of list
    EXPECT_EQ(List_Insert(&test_val1, 1, test_list), LIST_ERROR_SUCCESS); //mid of list

    //i dont like that this test doesnt accurately test the validity of each entry...
}
//Test List create with improper args
TEST(ListInsertTest, InvalidArgs) {
    List_t* test_list = List_Create(1, test_cmp_fnc, test_free_fnc);

    int test_val1 = 255;

    EXPECT_EQ(List_Insert(&test_val1, 0, NULL), LIST_ERROR_INVALID_PARAM); //bad args
    EXPECT_EQ(List_Insert(NULL, 0, test_list), LIST_ERROR_INVALID_PARAM); //bad args
    EXPECT_EQ(List_Insert(&test_val1, 1, test_list), LIST_ERROR_INVALID_PARAM); //bad args (invalid location)

    EXPECT_EQ(List_Insert(&test_val1, 0, test_list), LIST_ERROR_SUCCESS); //populate for later
}