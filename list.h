/* 	@file list.h
 *  @brief Function declarations for use in an implementation of a generic linked list.
 */
 
#ifndef list_h
#define list_h

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

/*
 *  @brief Errors that may occur in some List_t functions.
 */
typedef enum List_Error_t {
	LIST_ERROR_SUCCESS,
	LIST_ERROR_FAILURE,
	LIST_ERROR_INVALID_PARAM,
	LIST_ERROR_EXCEED_LIMIT,
	LIST_ERROR_BAD_ENTRY
}
List_Error_t;

/*
 *  @brief A function used to compare data within a list.
 *  @param void* The first data to compare.
 *  @param void* The second data to compare.
 *  @return int  The result of a comparison. Positive numbers indicate precedence to the first member,
 *	   negative will indicate precedence to the second, 0 will indicate no precedence difference.
 */
typedef int (*List_Cmp_Fnc) (const void*, const void*);
/*
 *  @brief A function used to free data within a list.
 *  @param void* The data to free.
 *  @return void.
 */
typedef void (*List_Free_Fnc) (void*);
/*
 *  @brief A function used to perform a boolean test using data within a list.
 *  @param void* The data to perform the test on.
 *  @return bool True should indicate a passed test and false should indicate a failed test.
 */
typedef bool (*List_Find_Fnc) (const void*);
/*
 *  @brief A function used to perform an action using data within a list.
 *  @param void* The data to perform a task on.
 *  @return void.
 */
typedef void (*List_Do_Fnc) (void*);
/*
 *  @brief A function used to perform reduction on a list.
 *  @param void* The data to perform a single reduction.
 *  @param int The value of the accumulator at it current state
 *  @return void The value of the accumulator after reduction.
 */
typedef int (*List_Reduce_Fnc) (const void*, int);
/*
 *  @brief A function used to perform a copy of a value and return a pointer to the newly created data.
 *  @param void* The data to perform a copy on. This should not be altered
 *  @return void* A pointer to the newly copied data.
 */
typedef void* (*List_Copy_Fnc) (const void*);

/*
 *  @brief The list itself.
 */
typedef struct List_t List_t;

/*
 *  @brief Create an empty list.
 *  @param size_t The maximum size to allow the list to grow.
 *  @param List_Cmp_Fnc A function used when comparing data within the list for matches or sorting.
 *	   Passing NULL here will cause each member to have the same precedence.
 *  @param List_Free_Fnc A function used when freeing data within the list.
 *  @return List_t* A pointer to the allocated list or NULL on error.
 */
List_t* List_Create(size_t, List_Cmp_Fnc, List_Free_Fnc);

/*
 *  @brief Copy that the given list is valid.
 *  @param List_t* The list to copy.
 *  @param List_Copy_Fnc A user provided function to copy node values.
			If Null is passed, a shallow copy will be formed where:
				Each node value is the same pointer
				The list itself will be in different
 *  @return List_t* A pointer to the allocated list or NULL on error.
 */
List_t* List_Copy(List_t* list_p, List_Copy_Fnc copy_node);

/*
 *  @brief Verify that the given list is valid.
 *  @param List_t* The list to verify.
 *  @param List_Find_Fnc A user provided function to cehck validity of list values.
			If Null is passed, this will not be used and only metadata will be validated.
 *  @return List_Error_t LIST_ERROR_SUCCESS if the list is valid or an error attempting to describe the failure.
 */
List_Error_t List_Verify(List_t* list_p, List_Find_Fnc valid_check);

/*
 *  @brief Remove and destroy every node in the given list.
 *  @param List_t* The list to destroy nodes within.
 *  @return void.
 */
void List_Purge(List_t*);
/*
 *  @brief Destroy the given list and every node within it.
 *  @param List_t* The list to destroy.
 *  @return void.
 */
void List_Destroy(List_t*);

/*
 *  @brief Get the data in the node at the given location.
 *  @param size_t The index of the node in question.
 *  @param List_t* The list that contains the node in question.
 *  @return void* The data held within the node at the given location or NULL on error.
 */
void* List_At(size_t at, List_t* list_p);
/*
 *  @brief Get the current length of a given list.
 *  @param List_t* The list in question.
 *  @return size_t the length of the list
 * 	   Not ethat 0 will be returned if the list does not exist.
 */
size_t List_Length(List_t*);

/*
 *  @brief Put data into the given index of a given list.
 *  @param void* The data to insert to the end of the list.
 *  @param size_t The index in which to insert the given data to.
 *  @param List_t* The list that should contain the given node.
 *  @return List_Error_t LIST_ERROR_SUCCESS on success or any error that may occur.
 */
List_Error_t List_Insert(void*, size_t, List_t*);
/*
 *  @brief Put data into the end of a given list.
 *  @param void* The data to insert to the end of the list.
 *  @param List_t* The list that should contain the given node.
 *  @return List_Error_t LIST_ERROR_SUCCESS on success or any error that may occur.
 */
List_Error_t List_Push(void*, List_t*);
/*
 *  @brief Put data into the beginning of a given list.
 *  @param void* The data to insert to the end of the list.
 *  @param List_t* The list that should contain the given node.
 *  @return List_Error_t LIST_ERROR_SUCCESS on success or any error that may occur.
 */
List_Error_t List_Unshift(void*, List_t*);

/*
 *  @brief Test each node in a list using a given find function until an entry passes or no entries are left.
 *  @param List_t* The list to test entries within.
 *  @param List_Find_Fnc The function to test each entry using.
	   A test is passed when this function returns true after being given data.
 *  @return bool True if an entry is found that passes the given test or false if no nodes pass or an error occurs.
 */
bool List_Some(List_t*, List_Find_Fnc);
/*
 *  @brief Test each node in a list using a given find function until an entry fails or no entries are left.
 *  @param List_t* The list to test entries within.
 *  @param List_Find_Fnc The function to test each entry using.
	   A test is passed when this function returns true after being given data and failed if false is returned.
 *  @return bool True if all entries pass th egiven test or false when one fails or an error occurs.
 */
bool List_Every(List_t*, List_Find_Fnc);
/*
 *  @brief Run a given function against all data entries within a list.
 *  @param List_t* The list to test entries within.
 *  @param List_Do_Fnc The function to run with each entry.
 *  @return List_Error_t LIST_ERROR_SUCCESS on success or any error that may occur.
 */
List_Error_t List_For_Each(List_t*, List_Do_Fnc);
/*
 *  @brief Find the first instance of a given search data using the defined compare function.
 *  @param void* The data to use as a 'search term'.
 *  @param List_t* The list that should contain the given search term.
 		The function uses the list's cmp function to decide if a node is "equal" to the search term.
		 If the cmp function is not defined, the function will assume that all nodes are the same,
		 	therefore returning the first node it 'sees'.
 *  @param size_t* A pointer in which to put the index if any should be found.
	   Note that this will not be changed if no match is found.
 *  @return List_Error_t LIST_ERROR_SUCCESS on success or any error that may occur.
 */
List_Error_t List_Find(void*, List_t*, size_t*);
/*
 *  @brief Test each node in a list using a given find function, removing each that fail the given find function.
 *	   Note that filtered entries will be destroyed.
 *  @param List_t* The list to filter entries from.
 *  @param List_Find_Fnc The function to test each entry using.
 *	   A test is passed when this function returns true after being given data.
 *	   A test is failed and filtered out if false is returned.
 *  @return List_Error_t LIST_ERROR_SUCCESS on success or any error that may occur.
 */
List_Error_t List_Filter(List_t*, List_Find_Fnc);

/*
 *  @brief Remove a given index from the list.
 *  @param size_t The index to remove from the list.
 *  @param List_t* The list to remove the given index from.
 *  @return void* The data held within the removed node or NULL on error.
 */
void* List_Remove_At(size_t, List_t*);
/*
 *  @brief Remove the last node from the list.
 *  @param List_t* The list to remove the last node from.
 *  @return void* The data held within the removed node or NULL on error.
 */
void* List_Pop(List_t*);
/*
 *  @brief Remove the first node from the list.
 *  @param List_t* The list to remove the first node from.
 *  @return void* The data held within the removed node or NULL on error.
 */
void* List_Shift(List_t*);

/*
 *  @brief Remove the first node from the list.
 *  @param List_t* The list to reduce.
 *  @param list_reduce_fnc The function used to reduce each node.
 *  @param int The value to begin reduction with.
 *  @return List_Error_t LIST_ERROR_SUCCESS on success or any error that may occur.
 */
List_Error_t List_Reduce(List_t* list_p, List_Reduce_Fnc reducer, int* accumulator);

/*
 *  @brief Remove a given index from the list and destroy the data held within it.
 *  @param size_t The index to delete from the list.
 *  @param List_t* The list to delete the given index from.
 *  @return void.
 */
void List_Delete_At(size_t, List_t*);

/*
 *  @brief Reverse a given list from its current order.
 *  @param List_t* The list to reverse.
 *  @return List_Error_t LIST_ERROR_SUCCESS on success or any error that may occur.
 */
List_Error_t List_Reverse(List_t* list_p);

/*
 *  @brief Sort a given list using the set comparison function.
 *	   A list is considered sorted when precedence is in oredr from high to low
 *  @param List_t* The list to sort.
 *  @return List_Error_t LIST_ERROR_SUCCESS on success or any error that may occur.
 */
List_Error_t List_Sort(List_t* list_p);

#endif
