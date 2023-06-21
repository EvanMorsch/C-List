extern "C"
{
        #include "list.h"
}
#include "gtest/gtest.h"

//setup stuff
int test_cmp_fnc(const void* a, const void* b)
{
        if (NULL == a || NULL == b) return 0;
        return (*(int*)a)-(*(int*)b);
}
void test_free_fnc(void* a)
{
    (void)a;
    return;
}

int test_val1 = 255;
int test_val2 = 127;
int test_val3 = 63;

//List_Create
//{
    //Tests a valid usage
    TEST(ListCreateTest, ValidArgs) {
        List_t* test_list1 = List_Create(10, test_cmp_fnc, test_free_fnc);
        EXPECT_NE(test_list1, nullptr); //all valid args
        List_t* test_list2 = List_Create(10, NULL, test_free_fnc);
        EXPECT_NE(test_list2, nullptr); //null cmp is allowed

        List_Destroy(test_list1);
        List_Destroy(test_list2);
    }
    //Test List create with improper args
    TEST(ListCreateTest, InvalidArgs) {
        EXPECT_EQ(List_Create(0, test_cmp_fnc, test_free_fnc), nullptr); //must allow atleast a single node
        EXPECT_EQ(List_Create(10, test_cmp_fnc, NULL), nullptr); //null free not allowed
    }
//}
//List_Purge
//{
    //Tests a valid usage
    TEST(ListPurgeTest, ValidArgs) {
        List_t* test_list = List_Create(10, test_cmp_fnc, test_free_fnc);

        EXPECT_EQ(List_Push(&test_val1, test_list), LIST_ERROR_SUCCESS);

        List_Purge(test_list); //just make sure nothign crashes

        List_Destroy(test_list);
    }
    //Test List purge with improper args
    TEST(ListPurgeTest, InvalidArgs) {
        List_Purge(NULL); //just make sure nothign crashes
    }
//}
//List_Destroy
//{
    //Tests a valid usage
    TEST(ListDestroyTest, ValidArgs) {
        List_t* test_list = List_Create(10, test_cmp_fnc, test_free_fnc);

        EXPECT_EQ(List_Push(&test_val1, test_list), LIST_ERROR_SUCCESS);

        List_Destroy(test_list); //just make sure nothign crashes
    }
    //Test List destroy with improper args
    TEST(ListDestroyTest, InvalidArgs) {
        List_Destroy(NULL); //just make sure nothign crashes
    }
//}
//List_Insert  
//{
    //Tests a valid usage
    TEST(ListInsertTest, ValidArgs) {
        List_t* test_list = List_Create(10, test_cmp_fnc, test_free_fnc);

        EXPECT_EQ(List_Insert(&test_val1, 0, test_list), LIST_ERROR_SUCCESS); //first node in list
        EXPECT_EQ(List_Insert(&test_val1, 1, test_list), LIST_ERROR_SUCCESS); //back of list
        EXPECT_EQ(List_Insert(&test_val1, 0, test_list), LIST_ERROR_SUCCESS); //front of list
        EXPECT_EQ(List_Insert(&test_val1, 1, test_list), LIST_ERROR_SUCCESS); //mid of list

        List_Destroy(test_list);

        //i dont like that this test doesnt accurately test the validity of each entry...
    }
    //Test List insert with improper args
    TEST(ListInsertTest, InvalidArgs) {
        List_t* test_list = List_Create(1, test_cmp_fnc, test_free_fnc);

        EXPECT_EQ(List_Insert(&test_val1, 0, NULL), LIST_ERROR_INVALID_PARAM); //bad args
        EXPECT_EQ(List_Insert(NULL, 0, test_list), LIST_ERROR_INVALID_PARAM); //bad args
        EXPECT_EQ(List_Insert(&test_val1, 1, test_list), LIST_ERROR_INVALID_PARAM); //bad args (invalid location)

        EXPECT_EQ(List_Insert(&test_val1, 0, test_list), LIST_ERROR_SUCCESS); //populate for later

        EXPECT_EQ(List_Insert(&test_val1, 0, test_list), LIST_ERROR_EXCEED_LIMIT); //LIST_ERROR_EXCEED_LIMIT

        List_Destroy(test_list);
    }
//}
//List_Push/List_Unshift 
//{
    //Tests a valid usage
    TEST(ListUnshiftTest, ValidArgs) {
        List_t* test_list = List_Create(10, test_cmp_fnc, test_free_fnc);

        EXPECT_EQ(List_Unshift(&test_val1, test_list), LIST_ERROR_SUCCESS); //first node in list
        EXPECT_EQ(List_Unshift(&test_val1, test_list), LIST_ERROR_SUCCESS); //not first

        List_Destroy(test_list);
    }
    //Test List unshift with improper args
    TEST(ListUnshiftTest, InvalidArgs) {
        List_t* test_list = List_Create(1, test_cmp_fnc, test_free_fnc);

        EXPECT_EQ(List_Unshift(&test_val1, NULL), LIST_ERROR_INVALID_PARAM); //bad args
        EXPECT_EQ(List_Unshift(NULL, test_list), LIST_ERROR_INVALID_PARAM); //bad args

        EXPECT_EQ(List_Unshift(&test_val1, test_list), LIST_ERROR_SUCCESS); //populate for case

        EXPECT_EQ(List_Unshift(&test_val1, test_list), LIST_ERROR_EXCEED_LIMIT); //LIST_ERROR_EXCEED_LIMIT

        List_Destroy(test_list);
    }
    //Tests a valid usage
    TEST(ListPushTest, ValidArgs) {
        List_t* test_list = List_Create(10, test_cmp_fnc, test_free_fnc);

        EXPECT_EQ(List_Push(&test_val1, test_list), LIST_ERROR_SUCCESS); //first node in list
        EXPECT_EQ(List_Push(&test_val1, test_list), LIST_ERROR_SUCCESS); //not first

        List_Destroy(test_list);
    }
    //Test List push with improper args
    TEST(ListPushTest, InvalidArgs) {
        List_t* test_list = List_Create(1, test_cmp_fnc, test_free_fnc);

        EXPECT_EQ(List_Push(&test_val1, NULL), LIST_ERROR_INVALID_PARAM); //bad args
        EXPECT_EQ(List_Push(NULL, test_list), LIST_ERROR_INVALID_PARAM); //bad args

        EXPECT_EQ(List_Push(&test_val1, test_list), LIST_ERROR_SUCCESS); //populate for next case

        EXPECT_EQ(List_Push(&test_val1, test_list), LIST_ERROR_EXCEED_LIMIT); //LIST_ERROR_EXCEED_LIMIT

        List_Destroy(test_list);
    }
//}
//List_Remove_At
//{
    //Tests a valid usage
    TEST(ListRemoveAtTest, ValidArgs) {
        List_t* test_list = List_Create(10, test_cmp_fnc, test_free_fnc);

        EXPECT_EQ(List_Push(&test_val1, test_list), LIST_ERROR_SUCCESS);//add entry
        EXPECT_EQ(List_Push(&test_val2, test_list), LIST_ERROR_SUCCESS);//add entry
        EXPECT_EQ(List_Push(&test_val3, test_list), LIST_ERROR_SUCCESS);//add entry
        EXPECT_EQ(List_Length(test_list), 3);

        EXPECT_EQ(List_Remove_At(1, test_list), &test_val2);//mid entry
        EXPECT_EQ(List_Length(test_list), 2);
        EXPECT_EQ(List_Remove_At(1, test_list), &test_val3);//last entry
        EXPECT_EQ(List_Length(test_list), 1);
        EXPECT_EQ(List_Remove_At(0, test_list), &test_val1);//first/only entry
        EXPECT_EQ(List_Length(test_list), 0);

        List_Destroy(test_list);
    }
    //Test List remove at with improper args
    TEST(ListRemoveAtTest, InvalidArgs) {
        List_t* test_list = List_Create(10, test_cmp_fnc, test_free_fnc);

        EXPECT_EQ(List_Push(&test_val1, test_list), LIST_ERROR_SUCCESS);//add entry
        EXPECT_EQ(List_Length(test_list), 1);

        EXPECT_EQ(List_Remove_At(1, test_list), nullptr);//oob entry
        EXPECT_EQ(List_Length(test_list), 1);
        EXPECT_EQ(List_Remove_At(0, NULL), nullptr);//bad args

        List_Destroy(test_list);
    }
//}
//List_Delete_At
//{
    //Tests a valid usage
    TEST(ListDeleteAtTest, ValidArgs) {
        List_t* test_list = List_Create(10, test_cmp_fnc, test_free_fnc);

        EXPECT_EQ(List_Push(&test_val1, test_list), LIST_ERROR_SUCCESS);//add entry
        EXPECT_EQ(List_Push(&test_val2, test_list), LIST_ERROR_SUCCESS);//add entry
        EXPECT_EQ(List_Length(test_list), 2);

        List_Delete_At(1, test_list);
        EXPECT_EQ(List_Length(test_list), 1);
        List_Delete_At(0, test_list);//last entry
        EXPECT_EQ(List_Length(test_list), 0);

        List_Destroy(test_list);
    }
    //Test List delete at with improper args
    TEST(ListDeleteAtTest, InvalidArgs) {
        List_t* test_list = List_Create(10, test_cmp_fnc, test_free_fnc);

        EXPECT_EQ(List_Push(&test_val1, test_list), LIST_ERROR_SUCCESS);//add entry
        EXPECT_EQ(List_Length(test_list), 1);

        List_Delete_At(1, test_list);//oob entry
        EXPECT_EQ(List_Length(test_list), 1);
        List_Delete_At(0, NULL);//bad args

        List_Destroy(test_list);
    }
//}
//List_Pop/List_Shift
//{
    //Tests a valid usage
    TEST(ListPopTest, ValidArgs) {
        List_t* test_list = List_Create(10, test_cmp_fnc, test_free_fnc);

        EXPECT_EQ(List_Push(&test_val1, test_list), LIST_ERROR_SUCCESS);//add entry
        EXPECT_EQ(List_Push(&test_val2, test_list), LIST_ERROR_SUCCESS);//add entry
        EXPECT_EQ(List_Push(&test_val3, test_list), LIST_ERROR_SUCCESS);//add entry
        EXPECT_EQ(List_Length(test_list), 3);

        EXPECT_EQ(List_Pop(test_list), &test_val3);//remove entry
        EXPECT_EQ(List_Length(test_list), 2);
        EXPECT_EQ(List_Pop(test_list), &test_val2);//remove entry
        EXPECT_EQ(List_Length(test_list), 1);
        EXPECT_EQ(List_Pop(test_list), &test_val1);//remove entry
        EXPECT_EQ(List_Length(test_list), 0);

        List_Destroy(test_list);
    }
    //Test List pop with improper args
    TEST(ListPopTest, InvalidArgs) {
        EXPECT_EQ(List_Pop(NULL), nullptr); //bad args
    }
    //Tests a valid usage
    TEST(ListShiftTest, ValidArgs) {
        List_t* test_list = List_Create(10, test_cmp_fnc, test_free_fnc);

        EXPECT_EQ(List_Push(&test_val1, test_list), LIST_ERROR_SUCCESS);//add entry
        EXPECT_EQ(List_Push(&test_val2, test_list), LIST_ERROR_SUCCESS);//add entry
        EXPECT_EQ(List_Push(&test_val3, test_list), LIST_ERROR_SUCCESS);//add entry
        EXPECT_EQ(List_Length(test_list), 3);

        EXPECT_EQ(List_Shift(test_list), &test_val1);//remove entry
        EXPECT_EQ(List_Length(test_list), 2);
        EXPECT_EQ(List_Shift(test_list), &test_val2);//remove entry
        EXPECT_EQ(List_Length(test_list), 1);
        EXPECT_EQ(List_Shift(test_list), &test_val3);//remove entry
        EXPECT_EQ(List_Length(test_list), 0);

        List_Destroy(test_list);
    }
    //Test List shift with improper args
    TEST(ListShiftTest, InvalidArgs) {
        EXPECT_EQ(List_Shift(NULL), nullptr); //bad args
    }
//}
//List_Length
//{
    //Tests a valid usage
    TEST(ListLengthTest, ValidArgs) {
        List_t* test_list = List_Create(10, test_cmp_fnc, test_free_fnc);

        EXPECT_EQ(List_Length(test_list), 0);
        EXPECT_EQ(List_Push(&test_val1, test_list), LIST_ERROR_SUCCESS);//add entry
        EXPECT_EQ(List_Length(test_list), 1);
        EXPECT_EQ(List_Pop(test_list), &test_val1);//remove entry
        EXPECT_EQ(List_Length(test_list), 0);

        List_Destroy(test_list);
    }
    //Test List length with improper args
    TEST(ListLengthTest, InvalidArgs) {
        EXPECT_EQ(List_Length(NULL), 0); //bad args
    }
//}
