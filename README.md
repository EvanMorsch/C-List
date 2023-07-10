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
## Creating build files with CMake
- Start by making and changing to a ```build``` directory.  
This will keep all of your build artifacts in one place.
```bash
mkdir build
cd build
```
- Build makefile with cmake.
```bash
cmake ..
```
- By default, cmake builds libraries statically, to build it as a shared library, add the following argument:
```bash
cmake -DBUILD_SHARED_LIBS=ON ..
```
<br/>
<br/>

## Building the library
- While in the same build directory, just run make using the makefile previously created by cmake.
```bash
make
```
- The library is created as ```liblist.a```. the extension will depend on whatever system you're on and whether you made it statically or not.  
(I'm using a mac so I get either a ```".a"``` or ```".dylib"```)
<br/>
<br/>

## Building with tests
- Add the following arguemnt to the cmake build file creation command:
```bash
cmake -DTEST=ON ..
```
- And make as normal:
```bash
make
```
- The program ```test``` is created, run it to test all modules.
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
<br/>
<br/>

## Structures
Both structures are not exposed to the user through the header as they are both meant to be seen but not understood. Their use (To any meaningful extent) should require the use of the library and its functions.

There are two structures used in this list implementation. One is used to help organize data and metadata within nodes, and the other is to help organize list data and metadata.
<br/>

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
These functions are used throughout the library as a way for the library Top know how to handle the data you tell it to work with.
For example, when comparing two ints, subtracting them together can provide an integer representing the difference between them. How would the library know how to compare structures, floats, or files.  
All of these can be compared (and thus handled by this library) using a specially made comparison function.  
These functions also include ways to handle freeing, finding, reducing, and more with the data held in a list.
<br/>

### List_Cmp_Fnc
```C
/*
 *  @brief 			A function used to compare two data entries within a list.
 *  @param void* 	The first data entry to compare.
 *  @param void* 	The second data entry to compare.
 *  @return int  	The result of a comparison. 
 * 					Positive numbers indicate precedence to the first member,
 *	   					Negative will indicate precedence to the second, 
 *						0 will indicate no precedence difference.
 */
typedef int (*List_Cmp_Fnc) (const void*, const void*);
```
#### Notes
This function is used pretty extensively throughout the library.  
This is the main way that the library can tell how two entries of data relate.  
In terms of how this function is used to sort entries, this should give a numerical difference in data representing the proper order of each, or 0 if they are the same or order does not matter.  
When this function is used to find or match with another, this function should preoperly detect ANY difference in the data, and return 0 if they are truly the same.  
There are lots of tricks that can provide rich functionality using this function and a good understanding and ewxperimentation can help reveal some of these advanced usages.
<br/>
<br/>

### List_Free_Fnc
```C
/*
 *  @brief 			A function used to free data within a list.
 *  @param void* 	The data to free.
 *  @return void.
 */
typedef void (*List_Free_Fnc) (void*);
```
#### Notes
This function is used wherever a destruction or deletion and thus a free happens on data in a list node.  
An attempt to make this convenient to override where possible will be made and the user can decide to have this function do nothing if they wsh to manually track and free data.  
In a normal implementation, this will simply pass the data to the c standard `free()` function.
<br/>
<br/>

### List_Find_Fnc
```C
/*
 *  @brief 			Used to perform a boolean test using data within a list.
 *  @param void* 	The data to perform the test on.
 *  @return bool 	True should indicate a passed test and False should indicate a failed test.
 */
typedef bool (*List_Find_Fnc) (const void*);
```
#### Notes
This function is used wherever a value needs to be 'found'. A return from this function should clearly indicate whether an item passed in is the item in fits a criteria. Not to be confused with the CMP function, this function is not entirely meant to match data together. 
In a normal implementation, True means that the data passed in fits the criteria while False indicates a failure.
<br/>
<br/>

### List_Do_Fnc
```C
/*
 *  @brief 			Used to perform an action using data within a list.
 *  @param void* 	The data to perform a task on.
 *  @return void.
 */
typedef void (*List_Do_Fnc) (void*);
```
#### Notes
This function is used to perform (hopefully simple) tasks on data entries. Sort of a free-form function to do whatever you like with the data in an entry.
<br/>
<br/>

### List_Reduce_Fnc
```C
/*
 *  @brief 			A function used to perform reduction on a list.
 *  @param void* 	The data to perform a single reduction.
 *  @param int 		The value of the accumulator at it current state
 *  @return void 	The value of the accumulator after reduction.
 */
typedef void* (*List_Reduce_Fnc) (const void*, void*);
```
#### Notes
Used during reduce pretty exclusively to take data, add it to the accumulator, and return the sum (or whatver transformation you wish to occur to the accumulator.).
<br/>
<br/>

### List_Copy_Fnc
```C
/*
 *  @brief 			A function used to perform a copy of a value and return a pointer to the newly created data.
 *  @param void* 	The data to perform a copy on. This should not be altered.
 *  @return void* 	A pointer to the newly copied data.
 */
typedef void* (*List_Copy_Fnc) (const void*);
```
#### Notes
This function is meant to be used to allocate a copy of the given data.
<br/>
<br/>


<br/>
<br/>
<br/>

## Functions
The following functions are the basic functions available during normal usage of the library.  
<br/>

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
Users really shouldnt ever need to touch these but, theyre there to be used i guess so i'll go over them.  
<br/>

### List_Node_Create
```C
/*
 *  @brief 				Alloocate and instantiate a list node.
 *  @param void* 		The data to hold within the node.
 *  @return List_Node* 	A pointer to the allocated node or NULL on error.
 */
static List_Node* List_Node_Create(void* data_p);
```
<br/>
<br/>

### List_Node_At
```C
/*
 *  @brief 				Get a pointer to the node at a given index in a given list.
 *  @param size_t 		The index of the node in question.
 *  @param List_t* 		A pointer to the list that contains the node in question.
 *  @return List_Node* 	A pointer to the node at the given index or NULL on error.
 */
static List_Node* List_Node_At(size_t at, List_t* list_p);
```
<br/>
<br/>

### List_Node_Insert
```C
/*
 *  @brief 					Insert a node at a given index in a given list.
 *  @param size_t 			The index to insert the given node at.
 *  @param List_t* 			A pointer to the list that the node should belong in.
 *  @return List_Error_t 	LIST_ERROR_SUCCESS on success or any error that may occur.
 */
static List_Error_t List_Node_Insert(List_Node* node_p, size_t at, List_t* list_p);
```
<br/>
<br/>

### List_Node_Destroy
```C
/*
 *  @brief 				Destroy a given node. 
 *						Note that this will only destroy the node and not the data within it.
 *  @param List_Node* 	A pointer to the node to destroy.
 *  @return void
 */
static void List_Node_Destroy(List_Node* node);
```
<br/>
<br/>

### List_Is_Sorted
```C
/*
 *  @brief 					Check a list for sortedness.
 *							A list is considered sorted when each node's precedence is ordered 
 *								from highest to lowest.
 *  @param List_t* 			A pointer to the list to check for sortedness.
 *  @param List_Cmp_Fnc 	An optional custom function pointer describing the precedence to use during sorting.
 *							If NULL is used here, the list's default cmp_fnc will be used.
 *  @return bool 			True if sorted, false if not sorted or an error occurs.
 */
static bool List_Is_Sorted(List_t* list_p, List_Cmp_Fnc cmp_fnc);
```
#### Notes
Again, not having a descriptive failure case annoys me, but its not worth sacrificing readability to pass in more parameters or something
<br/>
<br/>

### List_Node_Swap
```C
/*
 *  @brief 					Swap the data of two given nodes.
 *  @param List_Node* 		A pointer to the first node to swap.
 *  @param List_Node* 		A pointer to the second node to swap.
 *  @return List_Error_t 	LIST_ERROR_SUCCESS on success or any error that may occur.
 */
static List_Error_t List_Node_Swap(List_Node* node_a, List_Node* node_b);
```
<br/>
<br/>

### List_Node_Remove
```C
/*
 *  @brief 					Remove a given node from a given list.
 *     						Note that this will free the node structure but will NOT free 
 * 								the data held within the node.
 *  @param List_Node* 		A pointer to the node to remove.
 *  @param List_t* 			A pointer to the list that should be removed from.
 *  @return List_Error_t 	LIST_ERROR_SUCCESS on success or any error that may occur.
 */
static List_Error_t List_Node_Remove(List_Node* node, List_t* list_p);
```
<br/>
<br/>

<br/>
<br/>
<br/>

## Notes
<br/>

- Removal will NOT free data (As far as the user is concerned), Deletion will
- NULL data is not allowed because it messes up error checking, you can, however, use a pointer to NULL data

