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
//test List_Find_Fnc
bool is_not_255(const void* a)
{
    if (*(int*)a == 255) return false;
    return true;
}
//test List_Find_Fnc
void* double_int(const void* a)
{
    void* ret_int = malloc(sizeof(int));
    *(int*)ret_int = (*(int*)a) * 2;
    return ret_int;
}
//test List_do_fnc
void half_int(void* a)
{
    *(int*)a /= 2;
}
//test List_Reduce_Fnc
void* avg_reducer(const void* a, void* acc)
{
    *(int*)acc += *(int*)a;
    return acc;
}

//test values for list population
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
        List_t* test_list3 = List_Create(0, test_cmp_fnc, test_free_fnc);
        EXPECT_NE(test_list3, nullptr); //null cmp is allowed

        List_Destroy(test_list1);
        List_Destroy(test_list2);
        List_Destroy(test_list3);
    }
    //Test List create with improper args
    TEST(ListCreateTest, InvalidArgs) {
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
        EXPECT_EQ(List_Length(NULL), 0);  
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

        EXPECT_EQ(List_Insert(&test_val1, 0, NULL), LIST_ERROR_INVALID_PARAM);  
        EXPECT_EQ(List_Insert(NULL, 0, test_list), LIST_ERROR_INVALID_PARAM);  
        EXPECT_EQ(List_Insert(&test_val1, 1, test_list), LIST_ERROR_INVALID_PARAM);

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

        EXPECT_EQ(List_Unshift(&test_val1, NULL), LIST_ERROR_INVALID_PARAM);  
        EXPECT_EQ(List_Unshift(NULL, test_list), LIST_ERROR_INVALID_PARAM);  

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

        EXPECT_EQ(List_Push(&test_val1, NULL), LIST_ERROR_INVALID_PARAM);  
        EXPECT_EQ(List_Push(NULL, test_list), LIST_ERROR_INVALID_PARAM);  

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
        EXPECT_EQ(List_Remove_At(0, NULL), nullptr); 

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
        List_Delete_At(0, NULL); 

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
        EXPECT_EQ(List_Pop(NULL), nullptr);  
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
        EXPECT_EQ(List_Shift(NULL), nullptr);  
    }
//}
//List_Verify
//{
    //Tests a valid usage
    TEST(ListVerifyTest, ValidArgs) {
        List_t* test_list = List_Create(10, test_cmp_fnc, test_free_fnc);

        EXPECT_EQ(List_Push(&test_val2, test_list), LIST_ERROR_SUCCESS);//add entry
        EXPECT_EQ(List_Push(&test_val3, test_list), LIST_ERROR_SUCCESS);//add entry
        EXPECT_EQ(List_Length(test_list), 2);

        EXPECT_EQ(List_Verify(test_list, NULL), LIST_ERROR_SUCCESS);
        EXPECT_EQ(List_Verify(test_list, is_not_255), LIST_ERROR_SUCCESS);

        List_Destroy(test_list);
    }
    //Test List verify with improper args
    TEST(ListVerifyTest, InvalidArgs) {
        List_t* test_list = List_Create(10, test_cmp_fnc, test_free_fnc);

        EXPECT_EQ(List_Push(&test_val1, test_list), LIST_ERROR_SUCCESS);//add entry
        EXPECT_EQ(List_Push(&test_val2, test_list), LIST_ERROR_SUCCESS);//add entry
        EXPECT_EQ(List_Length(test_list), 2);

        EXPECT_EQ(List_Verify(test_list, NULL), LIST_ERROR_SUCCESS);
        EXPECT_EQ(List_Verify(test_list, is_not_255), LIST_ERROR_BAD_ENTRY);

        List_Destroy(test_list);
    }
//}
//List_At
//{
    //Tests a valid usage
    TEST(ListAtTest, ValidArgs) {
        List_t* test_list = List_Create(10, test_cmp_fnc, test_free_fnc);

        EXPECT_EQ(List_Push(&test_val1, test_list), LIST_ERROR_SUCCESS);//add entry
        EXPECT_EQ(List_Push(&test_val2, test_list), LIST_ERROR_SUCCESS);//add entry
        EXPECT_EQ(List_Push(&test_val3, test_list), LIST_ERROR_SUCCESS);//add entry
        EXPECT_EQ(List_Length(test_list), 3);

        EXPECT_EQ(List_At(0, test_list), &test_val1);
        EXPECT_EQ(List_At(1, test_list), &test_val2);
        EXPECT_EQ(List_At(2, test_list), &test_val3);

        List_Destroy(test_list);
    }
    //Test List at with improper args
    TEST(ListAtTest, InvalidArgs) {
        List_t* test_list = List_Create(10, test_cmp_fnc, test_free_fnc);

        EXPECT_EQ(List_At(0, test_list), nullptr); //oob
        EXPECT_EQ(List_At(1, NULL), nullptr);

        List_Destroy(test_list);
    }
//}
//List_Find
//{
    //Tests a valid usage
    TEST(ListFindTest, ValidArgs) {
        List_t* test_list = List_Create(10, test_cmp_fnc, test_free_fnc);

        EXPECT_EQ(List_Push(&test_val1, test_list), LIST_ERROR_SUCCESS);//add entry
        EXPECT_EQ(List_Push(&test_val2, test_list), LIST_ERROR_SUCCESS);//add entry
        EXPECT_EQ(List_Length(test_list), 2);

        size_t response_index = 255; //will be overwreitten
        EXPECT_EQ(List_Find(&test_val2, test_list, &response_index), LIST_ERROR_SUCCESS);
        EXPECT_EQ(response_index, 1);
        response_index = 255; //reset for verification
        EXPECT_EQ(List_Find(&test_val3, test_list, &response_index), LIST_ERROR_EXCEED_LIMIT); //not in list
        EXPECT_EQ(response_index, 255);
        EXPECT_EQ(List_Find(&test_val1, test_list, NULL), LIST_ERROR_SUCCESS); //dont use index ret

        List_Destroy(test_list);
    }
    //Test List find with improper args
    TEST(ListFindTest, InvalidArgs) {
        EXPECT_EQ(List_Find(0, NULL, NULL), LIST_ERROR_INVALID_PARAM);  
    }
//}
//List_Reduce
//{
    //Tests a valid usage
    TEST(ListReduceTest, ValidArgs) {
        List_t* test_list = List_Create(10, test_cmp_fnc, test_free_fnc);

        EXPECT_EQ(List_Push(&test_val1, test_list), LIST_ERROR_SUCCESS);//add entry
        EXPECT_EQ(List_Push(&test_val2, test_list), LIST_ERROR_SUCCESS);//add entry
        EXPECT_EQ(List_Length(test_list), 2);

        int accumulator = 0;
        EXPECT_EQ(List_Reduce(test_list, avg_reducer, &accumulator), LIST_ERROR_SUCCESS);
        EXPECT_EQ(accumulator, 255+127);

        List_Destroy(test_list);
    }
    //Test List reduce with improper args
    TEST(ListReduceTest, InvalidArgs) {
        List_t* test_list = List_Create(10, test_cmp_fnc, test_free_fnc);

        int accumulator = 0;
        EXPECT_EQ(List_Reduce(test_list, avg_reducer, NULL), LIST_ERROR_INVALID_PARAM);  
        EXPECT_EQ(List_Reduce(test_list, NULL, &accumulator), LIST_ERROR_INVALID_PARAM);  
        EXPECT_EQ(List_Reduce(NULL, avg_reducer, &accumulator), LIST_ERROR_INVALID_PARAM);  

        List_Destroy(test_list);
    }
//}

//List_Copy
//{
    //Tests a valid usage
    TEST(ListCopyTest, DefaultArgs) {
        List_t* test_list = List_Create(10, test_cmp_fnc, test_free_fnc);

        EXPECT_EQ(List_Push(&test_val1, test_list), LIST_ERROR_SUCCESS);
        EXPECT_EQ(List_Push(&test_val2, test_list), LIST_ERROR_SUCCESS);
        EXPECT_EQ(List_Push(&test_val3, test_list), LIST_ERROR_SUCCESS);
        EXPECT_EQ(List_Length(test_list), 3);

        List_t* test_list2 = List_Copy(test_list, NULL);

        EXPECT_EQ(List_Pop(test_list2), &test_val3);
        EXPECT_EQ(List_Length(test_list2), 2);
        EXPECT_EQ(List_Pop(test_list2), &test_val2);
        EXPECT_EQ(List_Length(test_list2), 1);
        EXPECT_EQ(List_Pop(test_list2), &test_val1);
        EXPECT_EQ(List_Length(test_list2), 0);

        EXPECT_EQ(List_Length(test_list), 3); //make sure original is untouched
        EXPECT_EQ(List_Pop(test_list), &test_val3);

        List_Destroy(test_list);
        List_Destroy(test_list2);
    }
    //Tests a valid usage
    TEST(ListCopyTest, CustomArgs) {
        List_t* test_list = List_Create(10, test_cmp_fnc, test_free_fnc);

        EXPECT_EQ(List_Push(&test_val1, test_list), LIST_ERROR_SUCCESS);
        EXPECT_EQ(List_Push(&test_val2, test_list), LIST_ERROR_SUCCESS);
        EXPECT_EQ(List_Push(&test_val3, test_list), LIST_ERROR_SUCCESS);
        EXPECT_EQ(List_Length(test_list), 3);

        List_t* test_list2 = List_Copy(test_list, double_int);

        EXPECT_EQ(List_Length(test_list2), 3);
        void* pop1 = List_Pop(test_list2);
        EXPECT_EQ(List_Length(test_list2), 2);
        void* pop2 = List_Pop(test_list2);
        EXPECT_EQ(List_Length(test_list2), 1);
        void* pop3 = List_Pop(test_list2);
        EXPECT_EQ(List_Length(test_list2), 0);

        EXPECT_EQ(*(int*)pop1, *(int*)double_int(&test_val3));
        EXPECT_EQ(*(int*)pop2, *(int*)double_int(&test_val2));
        EXPECT_EQ(*(int*)pop3, *(int*)double_int(&test_val1));
        free(pop1);
        free(pop2);
        free(pop3);

        EXPECT_EQ(List_Length(test_list), 3); //make sure original is untouched
        EXPECT_EQ(List_Pop(test_list), &test_val3);

        List_Destroy(test_list);
        List_Destroy(test_list2);
    }
    //Test List copy with improper args
    TEST(ListCopyTest, InvalidArgs) {
        EXPECT_EQ(List_Copy(NULL, NULL), nullptr);  
    }
//}
//List_Some
//{
    //Tests a valid usage
    TEST(ListSomeTest, ValidPassCase) {
        List_t* test_list = List_Create(10, test_cmp_fnc, test_free_fnc);

        EXPECT_EQ(List_Push(&test_val1, test_list), LIST_ERROR_SUCCESS);
        EXPECT_EQ(List_Push(&test_val2, test_list), LIST_ERROR_SUCCESS);
        EXPECT_EQ(List_Push(&test_val3, test_list), LIST_ERROR_SUCCESS);
        EXPECT_EQ(List_Length(test_list), 3);

        EXPECT_EQ(List_Some(test_list, is_not_255), true);

        List_Destroy(test_list);
    }
    //Tests a valid usage
    TEST(ListSomeTest, ValidFailCase) {
        List_t* test_list = List_Create(10, test_cmp_fnc, test_free_fnc);

        EXPECT_EQ(List_Push(&test_val1, test_list), LIST_ERROR_SUCCESS);
        EXPECT_EQ(List_Push(&test_val1, test_list), LIST_ERROR_SUCCESS);
        EXPECT_EQ(List_Push(&test_val1, test_list), LIST_ERROR_SUCCESS);
        EXPECT_EQ(List_Length(test_list), 3);

        EXPECT_EQ(List_Some(test_list, is_not_255), false);

        List_Destroy(test_list);
    }
    //Test List some with improper args
    TEST(ListSomeTest, InvalidArgs) {
        List_t* test_list = List_Create(10, test_cmp_fnc, test_free_fnc);

        EXPECT_EQ(List_Some(NULL, NULL), false);  
        EXPECT_EQ(List_Some(NULL, is_not_255), false);  
        EXPECT_EQ(List_Some(test_list, NULL), false);  

        List_Destroy(test_list);
    }
//}
//List_Every
//{
    //Tests a valid usage
    TEST(ListEveryTest, ValidFailCase) {
        List_t* test_list = List_Create(10, test_cmp_fnc, test_free_fnc);

        EXPECT_EQ(List_Push(&test_val2, test_list), LIST_ERROR_SUCCESS);
        EXPECT_EQ(List_Push(&test_val2, test_list), LIST_ERROR_SUCCESS);
        EXPECT_EQ(List_Push(&test_val3, test_list), LIST_ERROR_SUCCESS);
        EXPECT_EQ(List_Length(test_list), 3);

        EXPECT_EQ(List_Every(test_list, is_not_255), true);

        List_Destroy(test_list);
    }
    //Tests a valid usage
    TEST(ListEveryTest, ValidPassCase) {
        List_t* test_list = List_Create(10, test_cmp_fnc, test_free_fnc);

        EXPECT_EQ(List_Push(&test_val1, test_list), LIST_ERROR_SUCCESS);
        EXPECT_EQ(List_Push(&test_val2, test_list), LIST_ERROR_SUCCESS);
        EXPECT_EQ(List_Push(&test_val3, test_list), LIST_ERROR_SUCCESS);
        EXPECT_EQ(List_Length(test_list), 3);

        EXPECT_EQ(List_Every(test_list, is_not_255), false);

        List_Destroy(test_list);
    }
    //Test List every with improper args
    TEST(ListEveryTest, InvalidArgs) {
        List_t* test_list = List_Create(10, test_cmp_fnc, test_free_fnc);

        EXPECT_EQ(List_Every(NULL, NULL), false);  
        EXPECT_EQ(List_Every(NULL, is_not_255), false);  
        EXPECT_EQ(List_Every(test_list, NULL), false);  

        List_Destroy(test_list);
    }
//}
//List_Reverse
//{
    //Tests a valid usage
    TEST(ListReverseTest, ValidEven) {
        List_t* test_list = List_Create(10, test_cmp_fnc, test_free_fnc);

        EXPECT_EQ(List_Push(&test_val1, test_list), LIST_ERROR_SUCCESS);
        EXPECT_EQ(List_Push(&test_val2, test_list), LIST_ERROR_SUCCESS);
        EXPECT_EQ(List_Push(&test_val3, test_list), LIST_ERROR_SUCCESS);
        EXPECT_EQ(List_Push(&test_val3, test_list), LIST_ERROR_SUCCESS);
        EXPECT_EQ(List_Length(test_list), 4);

        EXPECT_EQ(List_Reverse(test_list), LIST_ERROR_SUCCESS);

        EXPECT_EQ(List_Pop(test_list), &test_val1);
        EXPECT_EQ(List_Pop(test_list), &test_val2);
        EXPECT_EQ(List_Pop(test_list), &test_val3);
        EXPECT_EQ(List_Pop(test_list), &test_val3);

        List_Destroy(test_list);
    }
    //Tests a valid usage
    TEST(ListReverseTest, ValidOdd) {
        List_t* test_list = List_Create(10, test_cmp_fnc, test_free_fnc);

        EXPECT_EQ(List_Push(&test_val1, test_list), LIST_ERROR_SUCCESS);
        EXPECT_EQ(List_Push(&test_val2, test_list), LIST_ERROR_SUCCESS);
        EXPECT_EQ(List_Push(&test_val3, test_list), LIST_ERROR_SUCCESS);
        EXPECT_EQ(List_Length(test_list), 3);

        EXPECT_EQ(List_Reverse(test_list), LIST_ERROR_SUCCESS);

        EXPECT_EQ(List_Pop(test_list), &test_val1);
        EXPECT_EQ(List_Pop(test_list), &test_val2);
        EXPECT_EQ(List_Pop(test_list), &test_val3);

        List_Destroy(test_list);
    }
    //Tests a valid usage
    TEST(ListReverseTest, ValidEmpty) {
        List_t* test_list = List_Create(10, test_cmp_fnc, test_free_fnc);

        EXPECT_EQ(List_Reverse(test_list), LIST_ERROR_SUCCESS);

        List_Destroy(test_list);
    }
    //Test List reverse with improper args
    TEST(ListReverseTest, InvalidArgs) {
        EXPECT_EQ(List_Reverse(NULL), LIST_ERROR_INVALID_PARAM);  
    }
//}
//List_Filter
//{
    //Tests a valid usage
    TEST(ListFilterTest, ValidArgs) {
        List_t* test_list = List_Create(10, test_cmp_fnc, test_free_fnc);

        EXPECT_EQ(List_Push(&test_val1, test_list), LIST_ERROR_SUCCESS);
        EXPECT_EQ(List_Push(&test_val2, test_list), LIST_ERROR_SUCCESS);
        EXPECT_EQ(List_Push(&test_val3, test_list), LIST_ERROR_SUCCESS);
        EXPECT_EQ(List_Length(test_list), 3);

        EXPECT_EQ(List_Filter(test_list, is_not_255), LIST_ERROR_SUCCESS);

        EXPECT_EQ(List_Length(test_list), 2);
        EXPECT_EQ(List_Pop(test_list), &test_val3);
        EXPECT_EQ(List_Pop(test_list), &test_val2);

        List_Destroy(test_list);
    }
    //Tests a valid usage
    TEST(ListFilterTest, ValidEmpty) {
        List_t* test_list = List_Create(10, test_cmp_fnc, test_free_fnc);

        EXPECT_EQ(List_Filter(test_list, is_not_255), LIST_ERROR_SUCCESS);

        List_Destroy(test_list);
    }
    //Test List filter with improper args
    TEST(ListFilterTest, InvalidArgs) {
        List_t* test_list = List_Create(10, test_cmp_fnc, test_free_fnc);

        EXPECT_EQ(List_Filter(NULL, NULL), LIST_ERROR_INVALID_PARAM);
        EXPECT_EQ(List_Filter(test_list, NULL), LIST_ERROR_INVALID_PARAM);
        EXPECT_EQ(List_Filter(NULL, is_not_255), LIST_ERROR_INVALID_PARAM);

        List_Destroy(test_list);
    }
//}
//List_Sort
//{
    //Tests a valid usage
    TEST(ListSortTest, ValidArgs) {
        List_t* test_list = List_Create(10, test_cmp_fnc, test_free_fnc);

        EXPECT_EQ(List_Push(&test_val3, test_list), LIST_ERROR_SUCCESS);
        EXPECT_EQ(List_Push(&test_val1, test_list), LIST_ERROR_SUCCESS);
        EXPECT_EQ(List_Push(&test_val2, test_list), LIST_ERROR_SUCCESS);
        EXPECT_EQ(List_Length(test_list), 3);

        EXPECT_EQ(List_Sort(test_list, NULL), LIST_ERROR_SUCCESS);

        EXPECT_EQ(List_Length(test_list), 3);
        EXPECT_EQ(List_Pop(test_list), &test_val3);
        EXPECT_EQ(List_Pop(test_list), &test_val2);
        EXPECT_EQ(List_Pop(test_list), &test_val1);

        List_Destroy(test_list);
    }
    //Tests a valid usage
    TEST(ListSortTest, ValidCustomArgs) {
        List_t* test_list = List_Create(10, NULL, test_free_fnc);

        EXPECT_EQ(List_Push(&test_val3, test_list), LIST_ERROR_SUCCESS);
        EXPECT_EQ(List_Push(&test_val1, test_list), LIST_ERROR_SUCCESS);
        EXPECT_EQ(List_Push(&test_val2, test_list), LIST_ERROR_SUCCESS);
        EXPECT_EQ(List_Length(test_list), 3);

        EXPECT_EQ(List_Sort(test_list, test_cmp_fnc), LIST_ERROR_SUCCESS);

        EXPECT_EQ(List_Length(test_list), 3);
        EXPECT_EQ(List_Pop(test_list), &test_val3);
        EXPECT_EQ(List_Pop(test_list), &test_val2);
        EXPECT_EQ(List_Pop(test_list), &test_val1);

        List_Destroy(test_list);
    }
    //Tests a valid usage
    TEST(ListSortTest, ValidEmpty) {
        List_t* test_list = List_Create(10, test_cmp_fnc, test_free_fnc);

        EXPECT_EQ(List_Sort(test_list, NULL), LIST_ERROR_SUCCESS);

        List_Destroy(test_list);
    }
    //Test List sort with improper args
    TEST(ListSortTest, InvalidArgs) {
        EXPECT_EQ(List_Sort(NULL, NULL), LIST_ERROR_INVALID_PARAM);
    }
//}
//List_For_Each
//{
    //Tests a valid usage
    TEST(ListForEachTest, ValidArgs) {
        List_t* test_list = List_Create(10, test_cmp_fnc, test_free_fnc);

        EXPECT_EQ(List_Push(&test_val1, test_list), LIST_ERROR_SUCCESS);
        EXPECT_EQ(List_Push(&test_val2, test_list), LIST_ERROR_SUCCESS);
        EXPECT_EQ(List_Push(&test_val3, test_list), LIST_ERROR_SUCCESS);
        EXPECT_EQ(List_Length(test_list), 3);

        int old_val1 = test_val1;
        int old_val2 = test_val2;
        int old_val3 = test_val3;

        EXPECT_EQ(List_For_Each(test_list,  half_int), LIST_ERROR_SUCCESS);

        EXPECT_EQ(List_Length(test_list), 3);
        EXPECT_EQ(*(int*)List_Pop(test_list), old_val3 / 2);
        EXPECT_EQ(*(int*)List_Pop(test_list), old_val2 / 2);
        EXPECT_EQ(*(int*)List_Pop(test_list), old_val1 / 2);

        //reset for later just in case more tests need it
        test_val1 = old_val1;
        test_val2 = old_val2;
        test_val2 = old_val3;

        List_Destroy(test_list);
    }
    //Tests a valid usage
    TEST(ListForEachTest, ValidEmpty) {
        List_t* test_list = List_Create(10, test_cmp_fnc, test_free_fnc);

        EXPECT_EQ(List_For_Each(test_list,  half_int), LIST_ERROR_SUCCESS);

        List_Destroy(test_list);
    }
    //Test List for each with improper args
    TEST(ListForEachTest, InvalidArgs) {
        List_t* test_list = List_Create(10, test_cmp_fnc, test_free_fnc);

        EXPECT_EQ(List_For_Each(NULL, NULL), LIST_ERROR_INVALID_PARAM);
        EXPECT_EQ(List_For_Each(test_list, NULL), LIST_ERROR_INVALID_PARAM);
        EXPECT_EQ(List_For_Each(NULL,  half_int), LIST_ERROR_INVALID_PARAM);

        List_Destroy(test_list);
    }
//}