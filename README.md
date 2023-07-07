# C-List
List library for C aimed at wide, general, thread-safe usage with enough functionality to make list usage super convenient!
<br/>
<br/>

# A little about the design
Everytime I start a project, I find myself re-coding some pretty basic stuff like this because nothing fits my needs how I'd like. Well, I've had enough!

I loosely took inspiration from the Javascript Array API because I love the method selection and have never been left wanting whenever I use it. Whenever I come across a task that would benefit from an addition, I wouldn't mind adding it.

I tried to keep everything small, readable, and efficient enough not to embarrass myself but I'm sure theres room for improvement. Once I take note of them, I'll be quick to update this repo.

I've made a (lazy) attempt at ensuring that this library is thread safe and doesnt leak memory but extensive testing is yet to be done. My development machine as of now is a running MACOS so valgrind is a bit of a difficulty to get running. After testing is done, more of an attempt at ensuring thread efficiency and stability will be made
<br/>
<br/>

# Building
## Building with CMake
## Building manually
<br/>
<br/>

# Including
## Including shared library with CMake
## Including manually
<br/>
<br/>

# Usage
For those interested (And so I remember...), I'll go fairly in-depth for portions of the following sections of the documentation. I feel like it helps to understand how best to use things when you understand them more than really required.

That being said, This project is *really* meant to be my attempt at never needing to worry about a list ever again when I develop in C going forward so don't let the over-explanations bore you if you don't care.
## Structures
Both structures are not exposed to the user through the header as they are both meant to be seen but not understood. Their use (To any meaningful extent) should require the use of the library and its functions.

There are two structures used in this list implementation. One is used to help organize data and metadata within nodes, and the other is to help organize list data and metadata.

The List_t structure defines list metadata and holds references to the nodes themselves. The structure is defined as follows:
```C
typedef struct List_t
{
	size_t length; //current length
	size_t max_length; //max allowed length
	List_Node* head_p; //first node in the list
	List_Cmp_Fnc cmp; //function used to compare nodes
	List_Free_Fnc free; //used on destruction of a node or its data
}
List_t;
```

Each node's data and metadata (namely it's neighbors) are held in the second structure, being defined as follows:
```C
typedef struct List_Node List_Node;
struct List_Node
{
	void* data_p; //data pointer held in node
	List_Node* next_p; //next node
	List_Node* previous_p; //previous node
};
```

<br/>
<br/>

## User-defined functions

<br/>
<br/>

## Functions
### List_Create
```C
/*
 *  @brief                  - Create an empty list.
 *  @param size_t           - The maximum size to allow the list to grow.
				            If 0 is passed as this value, no maximum will be enforced (Not recommended!)
 *  @param List_Cmp_Fnc     - A function pointer used when comparing data within the list for matches or sorting.
 *	                        Passing NULL here will cause each member to have the same precedence during comparisons.
 *  @param List_Free_Fnc    - A function pointer used when freeing data within the list.
 *  @return List_t*         - A pointer to the allocated list or NULL on error.
 */
List_t* List_Create(size_t max_length, List_Cmp_Fnc cmp, List_Free_Fnc free);
```
<br/>
<br/>

### List_Copy
```C
/*
 *  @brief                  - Create a new list, and copy the given list's content to it.
 *  @param List_t*          - A pointer to the list to copy.
 *  @param List_Copy_Fnc    - A user provided function pointer that is used to copy node values.
			                If Null is passed, a shallow copy will be formed where:
				                * Each node value is the same pointer
				                * The returned list itself will still be a newly-allocated list
 *  @return List_t*         - A pointer to the allocated list or NULL on error.
 */
List_t* List_Copy(List_t* list_p, List_Copy_Fnc copy_node_fnc);
```
<br/>
<br/>

### List_Verify
```C
/*
 *  @brief                  - Verify that the given list is valid to a reasonable extent.
 *  @param List_t*          - A pointer to the list to verify.
 *  @param List_Find_Fnc    - A user provided function pointer to check the validity of individual node values.
			                If Null is passed, this will not be used and only metadata will be validated.
 *  @return List_Error_t    - LIST_ERROR_SUCCESS if the list is valid or an error attempting to describe the failure.
 */
List_Error_t List_Verify(List_t* list_p, List_Find_Fnc valid_check);
```
#### Notes
This function really lost its meaning over time, it was supposed to help validate things for tests but I dont like trusting code designed to validate itself
I've been thinking about deprecating this function
<br/>
<br/>

### List_At
```C
/*
 *  @brief          - Get a pointer to the data in the node at the given location without 
                        affecting the node itself
 *  @param size_t   - The index of the node in question.
 *  @param List_t*  - A pointer to the list that contains the node in question.
 *  @return void*   - A pointer to the data held within the node at the given location or a NULL on error.
 */
void* List_At(size_t at, List_t* list_p);
```
<br/>
<br/>

### List_Push
```C
/*
 *  @brief                  - Place data at the end of a given list.
 *  @param void*            - The data to insert into the list.
 *  @param List_t*          - The list being inserted into.
 *  @return List_Error_t    - LIST_ERROR_SUCCESS on success or any error that may occur.
 */
List_Error_t List_Push(void* data_p, List_t* list_p);
```
<br/>
<br/>

### List_Unshift
```C
/*
 *  @brief                  - Place data at the first index of a given list.
 *  @param void*            - The data to insert into the list.
 *  @param List_t*          - The list being inserted into.
 *  @return List_Error_t    - LIST_ERROR_SUCCESS on success or any error that may occur.
 */
List_Error_t List_Unshift(void* data_p, List_t* list_p);
```
<br/>
<br/>

### List_Insert
```C
/*
 *  @brief                  - Put data into the given index of a given list.
 *  @param void*            - The data to insert to the list.
 *  @param size_t           - The index to insert the given data into.
 *  @param List_t*          - The list being inserted into.
 *  @return List_Error_t    - LIST_ERROR_SUCCESS on success or any error that may occur.
 */
List_Error_t List_Insert(void* data_p, size_t at, List_t* list_p);
```
<br/>
<br/>

### List_Find
```C
/*
 *  @brief                  - Find the first instance of a given search data.
 *  @param void*            - The data to use as a 'search term', usually representing a copy of the data to find.
                            The list's cmp function is used to compare the search term and each list entry.
		                    A matching precedence is seen as a 'match' in data
 *  @param List_t*          - The list that is being searched.
 *  @param size_t*          - A pointer in which to put the index if any should be found.
	                        This value will not be changed unless a match is found.
 *  @return List_Error_t    - LIST_ERROR_SUCCESS on successful find, LIST_ERROR_EXCEED_LIMIT if no match is found,
 						        or any error that may occur.
 */
List_Error_t List_Find(void* search_data_p, List_t* list_p, size_t* response);
```
#### Notes
So I really wasn't sure how I wanted to do this function and was hoping it would just hit me at some point...
The problem i had was that, on failure to find an item, i am unsure what return value I would expect to see.
I would probably expect either a NULL, undefined, or -1. Since, what i want is a size_t representing the index, none
of these are really a good option. My second idea, and what i went with for now, was to 'return' by assigning a parameter's value. This ends up feeling unnatural to type and is kinda annoying, but ill keep it for now and see how it feels over time.

The second issue, and one that I already have planned out, is that, I'd like to include the ability to pass a cmp function as a parameter for this and some other functions that use the cmp function (like sort). I see some overlap in forcing its use for more than one thing and may get annoying to update just for one function at a time.
<br/>
<br/>

### List_Some
```C
/*
 *  @brief 					- Test each node in a list using a given find function until an entry passes or all entries fail.
 *  @param List_t* 			- The list to test entries within.
 *  @param List_Find_Fnc	- The function to test each entry using.
	   						A test is passed when this function returns true after being given data.
 *  @return bool 			- True if an entry is found that passes the given test, false if no nodes pass or an error occurs.
 */
bool List_Some(List_t* list_p, List_Find_Fnc do_fnc);
```
#### Notes
I tend to go overboard with error checking but i would ideally like this (and similar) function to return an error code so there is no ambiguity as to the response's meaning... False on an error isnt entirely clear to me
<br/>
<br/>

### List_Every
```C
/*
 *  @brief 					- Test each node in a list using a given find function until an entry fails or all entries pass.
 *  @param List_t* 			- The list to test entries within.
 *  @param List_Find_Fnc 	- The function to test each entry using.
	   						A test is passed when this function returns true after being given data and failed if false is returned.
 *  @return bool 			- True if all entries pass the given test, false when one fails or an error occurs.
 */
bool List_Every(List_t* list_p, List_Find_Fnc do_fnc);
```
<br/>
<br/>

### List_For_Each
```C
/*
 *  @brief 					Perform a given function passing all data entries within a list,
 								one at a time, to the function.
 *  @param List_t* 			The list to test entries within.
 *  @param List_Do_Fnc 		The function to perform using each entry as a parameter, one at a time.
 *  @return List_Error_t 	LIST_ERROR_SUCCESS on success or any error that may occur.
 */
List_Error_t List_For_Each(List_t* list_p, List_Do_Fnc do_fnc);
```
<br/>
<br/>

### List_Remove_At
```C
/*
 *  @brief 			Remove a given index from the list.
					Note that this WILL NOT free the data.
 *  @param size_t 	The index of the data to remove from the list.
 *  @param List_t* 	The list to remove the given index from.
 *  @return void* 	The data held within the removed node or NULL on error.
 */
void* List_Remove_At(size_t at, List_t* list_p);
```
<br/>
<br/>

### List_Delete_At
```C
/*
 *  @brief 			Delete a given index from the list.
					Note that this will free the data being removed using the free function assigned to the list upon creation
 *  @param size_t 	The index to delete from the list.
 *  @param List_t* 	The list to delete the given index from.
 *  @return void.
 */
void List_Delete_At(size_t at, List_t* list_p);
```
#### Notes
I tend to make functions that are made to free memory return nothing. That being said, I can see use in detecting failures here. Say for example, an index is to be deleted that is out of bounds or otherwise fails to be deleted... A lack of return value can lead to ambiguity as to the list's current size after supposed deletion.
I may change this.
<br/>
<br/>

### List_Pop
```C
/*
 *  @brief 			Remove the last node from the list.
					Not that this will not free the data upon removal.
 *  @param List_t* 	The list to remove the last node from.
 *  @return void* 	The data held within the removed node or NULL on error.
 */
void* List_Pop(List_t* list_p);
```
<br/>
<br/>

### List_Shift
```C
/*
 *  @brief 			Remove the first node from the list.
 *  @param List_t* 	The list to remove the first node from.
 *  @return void* 	The data held within the removed node or NULL on error.
 */
void* List_Shift(List_t* list_p);
```
<br/>
<br/>

### List_Reduce
```C
/*
 *  @brief 					Remove the first node from the list.
 *  @param List_t* 			The list to reduce.
 *  @param list_reduce_fnc 	The function used to reduce each node.
 * 							This function will be called on each data entry in the list, and assign 
 * 								the accumulator to the return value after each call.
 * 							See list_reduce_fnc for more detail
 *  @param void* 			The value to begin reduction with.
 * 							Thsi value will be overwritten with the accumulation.
 *  @return List_Error_t 	LIST_ERROR_SUCCESS on success or any error that may occur.
 */
List_Error_t List_Reduce(List_t* list_p, List_Reduce_Fnc reducer, void* accumulator);
```
<br/>
<br/>

### List_Filter
```C
/*
 *  @brief 					Test each node in a list using a given find function, removing 
 *								each that fail the given find function.
 *	   						Note that filtered entries will be deleted and thus, freed using the 
 * 								assigned free_fnc.
 *  @param List_t* 			The list to filter entries from.
 *  @param List_Find_Fnc 	The function to test each entry using.
 *	   							A test is passed and the data, allowed to stay when this function returns true.
 *	   							A test is failed and deleted if false is returned.
 *  @return List_Error_t 	LIST_ERROR_SUCCESS on success or any error that may occur.
 */
List_Error_t List_Filter(List_t* list_p, List_Find_Fnc do_fnc);
```
#### Notes
The decision to delete was because it ususally becomes difficult to keep a handle on data after mass removal.
You COULD get around it by reassigning the free_fnc to do nothing, then copy the list before filtering or something, then reassigning the free_fnc to normal afterward.
<br/>
<br/>

### List_Purge
```C
/*
 *  @brief 			Delete all data in the given list.
 *  @param List_t* 	The list to destroy all data within.
 *  @return void.
 */
void List_Purge(List_t* list_p);
```
<br/>
<br/>

### List_Destroy
```C
/*
 *  @brief 			Purge and destroy the given list.
 *  @param List_t* 	The list to purge and destroy.
 *  @return void.
 */
void List_Destroy(List_t* list_p);
```
<br/>
<br/>

### List_Length
```C
/*
 *  @brief 			Get the current length of a given list.
 *  @param List_t* 	The list to get the length of.
 *  @return size_t 	The length of the list.
 * 	   				Note that 0 will be returned if the list does not exist.
 */
size_t List_Length(List_t* list_p);
```
#### Notes
I dont like ssize_t, it clips the top end of size_t just for what is usually one negative number.
Instead, i just made 0 the "error" code. Seeing as a list that does not exist, has no size, i think 0 is reasonable. 
Not to mention, i made plenty of ways to check validity instead, just use them.
<br/>
<br/>

### List_Reverse
```C
/*
 *  @brief 					Reverse a given list in comparison to its current order.
 *  @param List_t* 			The list to reverse.
 *  @return List_Error_t 	LIST_ERROR_SUCCESS on success or any error that may occur.
 */
List_Error_t List_Reverse(List_t* list_p);
```
<br/>
<br/>


### List_Sort
```C
/*
 *  @brief 					Sort a given list using the assigned comparison function or a function passed in
 *								as a parameter.
 *	   						Note that a list is considered sorted when all data's precedence is in order from 
 * 								highest to lowest.
 *  @param List_t* 			The list to sort.
 *  @param List_Cmp_Fnc 	An optional custom function pointer describing the cmp_fnc to use during sorting.
 *							If NULL is used here, the list's default cmp_fnc will be used.
 *  @return List_Error_t 	LIST_ERROR_SUCCESS on success or any error that may occur.
 */
List_Error_t List_Sort(List_t* list_p, List_Cmp_Fnc cmp_fnc);
```
#### Notes
Okay the algorithm i used was really lazy and inefficient, i have a merge-sort implementation planned for later.
Its better because its stable and much more efficient.
<br/>
<br/>


<br/>
<br/>
<br/>

## Static functions
You really shouldnt ever need to touch these but, theyre there to be used i guess so i'll go over them

## Notes
- Removal will NOT free data (As far as the user is concerned), Deletion will
- NULL data is not allowed because it messes up error checking, you can, however, use a pointer to NULL data

