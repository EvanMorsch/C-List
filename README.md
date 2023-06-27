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
 *  @brief                  - Place data into the last index of a given list, making it the last item in the list.
 *  @param void*            - The data to insert into the list.
 *  @param List_t*          - The list being inserted into.
 *  @return List_Error_t    - LIST_ERROR_SUCCESS on success or any error that may occur.
 */
List_Error_t List_Push(void* data_p, List_t* list_p);
```
<br/>
<br/>



<br/>
<br/>

## Static functions
You really shouldnt ever need to touch these but, theyre there to be used i guess so i'll go over them
