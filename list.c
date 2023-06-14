/* 	@file list.c
 *  @brief An implementation of a generic doubly linked list.
 */
 
#include "list.h"

/*
 *  @brief The stucture of a node in the list.
 */
typedef struct List_Node List_Node;
struct List_Node
{
	void* data_p; //data pointer held in node
	List_Node* next_p; //next node
	List_Node* previous_p; //previous node
};

/*
 *  @brief The list itself.
 */
typedef struct List_t
{
	size_t length;
	size_t max_length; //max allowed length
	List_Node* head_p; //first node in the list
	List_Cmp_Fnc cmp;
	List_Free_Fnc free;
}
List_t;

/*
 *  @brief Create a list node structure.
 *  @param void* The data to hold within the node.
 *  @return List_Node* A pointer to the allocated node or NULL on error.
 */
static List_Node* List_Node_Create(void* data_p)
{
	if (NULL == data_p)
	{
		return NULL;
	}
	List_Node* new_node_p = malloc(sizeof(List_Node));
	if (NULL != new_node_p)
	{
		new_node_p->data_p = data_p;
		new_node_p->next_p = NULL;
		new_node_p->previous_p = NULL;
	}
	return new_node_p;
}

/*
 *  @brief Get a pointer to the node at a given index in a given list.
 *  @param size_t The index of the node in question.
 *  @param List_t* A pointer to the list that contains the node in question.
 *  @return List_Node* A pointer to the node at the given index or NULL on error.
 */
static List_Node* List_Node_At(size_t at, List_t* list_p)
{
	//check params
	if (NULL == list_p || at >= list_p->length)
	{
		return NULL;
	}
	//get the head node, an empty list will give NULL
	List_Node* current_node = list_p->head_p;
	//loop till we are at the correct index
	for (size_t i = 0; i < at; i++)
	{
		//make sure the next node can be found
		if (NULL != current_node)
		{
			current_node = current_node->next_p;
		}
		else
		{
			//we have hit a dead end
			break;
		}
	}
	//return the node or NULL if an issue arose
	return current_node;
}

/*
 *  @brief Insert a node at a given index in a given list.
 *  @param size_t The index to put the given node at.
 *  @param List_t* A pointer to the list that the node should belong in.
 *  @return List_Error_t LIST_ERROR_SUCCESS on success or any error that may occur.
 */
static List_Error_t List_Node_Insert(List_Node* node_p, size_t at, List_t* list_p)
{
	//param check
	if (NULL == node_p || NULL == list_p || at > list_p->length)
	{
		return LIST_ERROR_INVALID_PARAM;
	}
	//make sure this wouldnt exceed length limits
	if (list_p->length >= list_p->max_length)
	{
		return LIST_ERROR_EXCEED_LIMIT;
	}
	if (0 != list_p->length)
	{
		//find whos at "at" or the end if appending
		if (list_p->length == at)
		{
			List_Node* tail_p = List_Node_At(at-1, list_p);
			//check that tail exists
			if (NULL != tail_p)
			{
				//link new node to its new neighbors
				node_p->previous_p = tail_p;
				//splice connections to new node
				tail_p->next_p = node_p;
			}
		}
		else
		{
			List_Node* replacee_p = List_Node_At(at, list_p);
			//check that replacee exists
			if (NULL != replacee_p)
			{
				//link new node to its new neighbors
				node_p->next_p = replacee_p;
				node_p->previous_p = replacee_p->previous_p;
				//splice connections to new node
				//make sure that there was a node before the replacee
				if (NULL != replacee_p->previous_p)
				{
					replacee_p->previous_p->next_p = node_p;
				}
				replacee_p->previous_p = node_p;
			}
		}
	}
	//update head if needed
	if (0 == at)
	{
		list_p->head_p = node_p;
	}
	//inc len
	list_p->length++;
	return LIST_ERROR_SUCCESS;
}

/*
 *  @brief Destroy a given node. Note that this will only destroy the node and not the data within it
 *  @param List_Node* A pointer to the node to destroy.
 *  @return Void
 */
static void List_Node_Destroy(List_Node* node)
{
	//check params
	if (NULL != node)
	{
		free(node);
	}
}

/*
 *  @brief Remove a given node from a given list.
 *     Note that this will free the node structure BUT WILL NOT free the data held within the node.
 *  @param List_Node* A pointer to the node to remove.
 *  @param List_t* A pointer to the list that contains the node in question.
 *  @return List_Error_t LIST_ERROR_SUCCESS on success or any error that may occur.
 */
static List_Error_t List_Node_Remove(List_Node* node, List_t* list_p)
{
	//check params
	if (NULL == list_p || NULL == node)
	{
		return LIST_ERROR_INVALID_PARAM;
	}
	//update neighbors
	List_Node* before_node = node->previous_p;
	List_Node* after_node = node->next_p;
	if (NULL != before_node)
	{
		before_node->next_p = after_node;
	}
	if (NULL != after_node)
	{
		after_node->previous_p = before_node;
	}
	//update list size
	list_p->length--;
	//update head if needed
	if (list_p->head_p == node)
	{
		list_p->head_p = after_node;
	}
	List_Node_Destroy(node);
	return LIST_ERROR_SUCCESS;
}

/*
 *  @brief Create an empty list.
 *  @param size_t The maximum size to allow the list to grow.
 *  @param List_Cmp_Fnc A function used when comparing data within the list for matches or sorting.
 *	   Passing NULL here will cause each member to have the same precedence.
 *  @param List_Free_Fnc A function used when freeing data within the list.
 *  @return List_t* A pointer to the allocated list or NULL on error.
 */
List_t* List_Create(size_t max_length, List_Cmp_Fnc cmp, List_Free_Fnc free)
{
	if (max_length <= 0 || NULL == free)
	{
		return NULL;
	}
	List_t* new_list_p = malloc(sizeof(List_t));
	if (NULL != new_list_p)
	{
		new_list_p->length = 0;
		new_list_p->max_length = max_length;
		new_list_p->head_p = NULL;
		new_list_p->cmp = cmp;
		new_list_p->free = free;
	}
	return new_list_p;
}

/*
 *  @brief Get the data in the node at the given location.
 *  @param size_t The index of the node in question.
 *  @param List_t* The list that contains the node in question.
 *  @return void* The data held within the node at the given location or NULL on error.
 */
void* List_At(size_t at, List_t* list_p)
{
	//check params
	if (NULL == list_p || at >= list_p->length)
	{
		return NULL;
	}
	List_Node* node_at_p = List_Node_At(at, list_p);
	if (NULL == node_at_p)
	{
		return NULL;
	}
	return node_at_p->data_p;
}

/*
 *  @brief Put data into the end of a given list.
 *  @param void* The data to insert to the end of the list.
 *  @param List_t* The list that should contain the given node.
 *  @return List_Error_t LIST_ERROR_SUCCESS on success or any error that may occur.
 */
List_Error_t List_Push(void* data_p, List_t* list_p)
{
	//check param
	if (NULL == list_p || NULL == data_p)
	{
		return LIST_ERROR_INVALID_PARAM;
	}
	//try to allocate the node
	List_Node* new_node_p = List_Node_Create(data_p);
	//make sure it was allocated properly
	if (NULL == new_node_p)
	{
		return LIST_ERROR_INVALID_PARAM;
	}
	return List_Node_Insert(new_node_p, list_p->length, list_p);
}

/*
 *  @brief Put data into the beginning of a given list.
 *  @param void* The data to insert to the end of the list.
 *  @param List_t* The list that should contain the given node.
 *  @return List_Error_t LIST_ERROR_SUCCESS on success or any error that may occur.
 */
List_Error_t List_Unshift(void* data_p, List_t* list_p)
{
	//check param
	if (NULL == list_p || NULL == data_p)
	{
		return LIST_ERROR_INVALID_PARAM;
	}
	//try to allocate the node
	List_Node* new_node_p = List_Node_Create(data_p);
	//make sure it was allocated properly
	if (NULL == new_node_p)
	{
		return LIST_ERROR_INVALID_PARAM;
	}
	return List_Node_Insert(new_node_p, 0, list_p);
}

/*
 *  @brief Put data into the given index of a given list.
 *  @param void* The data to insert to the end of the list.
 *  @param size_t The index in which to insert the given data to.
 *  @param List_t* The list that should contain the given node.
 *  @return List_Error_t LIST_ERROR_SUCCESS on success or any error that may occur.
 */
List_Error_t List_Insert(void* data_p, size_t at, List_t* list_p)
{
	//check param
	if (NULL == list_p || NULL == data_p)
	{
		return LIST_ERROR_INVALID_PARAM;
	}
	//try to allocate the node
	List_Node* new_node_p = List_Node_Create(data_p);
	//make sure it was allocated properly
	if (NULL == new_node_p)
	{
		return LIST_ERROR_INVALID_PARAM;
	}
	//at is checked in insert node
	return List_Node_Insert(new_node_p, at, list_p);
}

/*
 *  @brief Find the first instance of a given search data using the defined compare function.
 *  @param void* The data to use as a 'search term'.
 *  @param List_t* The list that should contain the given search term.
 *  @param size_t* A pointer in which to put the index if any should be found.
	   Note that this will not be changed if no match is found.
 *  @return List_Error_t LIST_ERROR_SUCCESS on success or any error that may occur.
 */
List_Error_t List_Find(void* search_data_p, List_t* list_p, size_t* response)
{
	//check params
	if (NULL == list_p || NULL == search_data_p)
	{
		return LIST_ERROR_INVALID_PARAM;
	}
	//get the head node, an empty list will give NULL
	List_Node* current_node = list_p->head_p;
	//loop till the end
	for (size_t i = 0; i < list_p->length; i++)
	{
		//make sure the node is valid
		if (NULL != current_node)
		{
			//check if this node is the right one
			int nodes_match = 0;
			//null cmp function should result in 0
			if (NULL != list_p->cmp)
			{
				nodes_match = list_p->cmp(search_data_p, current_node->data_p);
			}
			if (0 == nodes_match)
			{
				if (NULL != response)
				{
					*response = i;
				}
				//stop search loop
				break;
			}
			//otherwise, just try the next node
			current_node = current_node->next_p;
		}
		else
		{
			//we have hit a dead end
			return LIST_ERROR_BAD_ENTRY;
		}
	}
	//return the node or NULL if an issue arose
	return LIST_ERROR_SUCCESS;
}

/*
 *  @brief Test each node in a list using a given find function until an entry passes or no entries are left.
 *  @param List_t* The list to test entries within.
 *  @param List_Find_Fnc The function to test each entry using.
	   A test is passed when this function returns true after being given data.
 *  @return bool True if an entry is found that passes the given test or false if no nodes pass or an error occurs.
 */
bool List_Some(List_t* list_p, List_Find_Fnc do_fnc)
{
	//check params
	if (NULL == list_p || NULL == do_fnc)
	{
		return false;
	}
	//get the head node, an empty list will give NULL
	List_Node* current_node = list_p->head_p;
	//loop till the end
	for (size_t i = 0; i < list_p->length; i++)
	{
		//make sure the node is valid
		if (NULL != current_node)
		{
			//check if this node is the right one
			bool node_found = do_fnc(current_node->data_p);
			if (node_found)
			{
				return true;
			}
			//otherwise, just try the next node
			current_node = current_node->next_p;
		}
		else
		{
			//we have hit a dead end
			return false;
		}
	}
	//we didnt find a match
	return false;
}

/*
 *  @brief Test each node in a list using a given find function until an entry fails or no entries are left.
 *  @param List_t* The list to test entries within.
 *  @param List_Find_Fnc The function to test each entry using.
	   A test is passed when this function returns true after being given data and failed if false is returned.
 *  @return bool True if all entries pass th egiven test or false when one fails or an error occurs.
 */
bool List_Every(List_t* list_p, List_Find_Fnc do_fnc)
{
	//check params
	if (NULL == list_p || NULL == do_fnc)
	{
		return false;
	}
	//get the head node, an empty list will give NULL
	List_Node* current_node = list_p->head_p;
	//loop till the end
	for (size_t i = 0; i < list_p->length; i++)
	{
		//make sure the node is valid
		//im not including contingency "else" on purpose
		if (NULL != current_node)
		{
			//check if this node fails
			bool node_failed = do_fnc(current_node->data_p);
			if (!node_failed)
			{
				return false;
			}
			//otherwise, try the next node
			current_node = current_node->next_p;
		}
	}
	//we didnt find a failure!
	return true;
}

/*
 *  @brief Run a given function against all data entries within a list.
 *  @param List_t* The list to test entries within.
 *  @param List_Do_Fnc The function to run with each entry.
 *  @return List_Error_t LIST_ERROR_SUCCESS on success or any error that may occur.
 */
List_Error_t List_For_Each(List_t* list_p, List_Do_Fnc do_fnc)
{
	//check params
	if (NULL == list_p || NULL == do_fnc)
	{
		return false;
	}
	//get the head node, an empty list will give NULL
	List_Node* current_node = list_p->head_p;
	//loop till the end
	for (size_t i = 0; i < list_p->length; i++)
	{
		//make sure the node is valid
		if (NULL != current_node)
		{
			do_fnc(current_node->data_p);
			current_node = current_node->next_p;
		}
		else
		{
			return LIST_ERROR_BAD_ENTRY;
		}
	}
	return LIST_ERROR_SUCCESS;
}

/*
 *  @brief Remove a given index from the list.
 *  @param size_t The index to remove from the list.
 *  @param List_t* The list to remove the given index from.
 *  @return void* The data held within the removed node or NULL on error.
 */
void* List_Remove_At(size_t at, List_t* list_p)
{
	//check params
	if (NULL == list_p || at > list_p->length)
	{
		return NULL;
	}
	//find the node
	List_Node* node = List_Node_At(at, list_p);
	if (NULL == node)
	{
		return NULL;
	}
	//save the nodes data
	void* node_data = node->data_p;
	//remove the node
	List_Error_t removed_node = List_Node_Remove(node, list_p);
	if (LIST_ERROR_SUCCESS != removed_node)
	{
		return NULL;
	}
	//return data
	return node_data;
}

/*
 *  @brief Remove a given index from the list and destroy the data held within it.
 *  @param size_t The index to delete from the list.
 *  @param List_t* The list to delete the given index from.
 *  @return void.
 */
void List_Delete_At(size_t at, List_t* list_p)
{
	//check params
	if (NULL == list_p || at > list_p->length)
	{
		return;
	}
	//find the node and delete it
	void* removing_node_data = List_Remove_At(at, list_p);
	if (NULL != removing_node_data)
	{
		//we should be on the correct node data, free it
		if (NULL != list_p->free)
		{
			list_p->free(removing_node_data);
		}
	}
}

/*
 *  @brief Remove the last node from the list.
 *  @param List_t* The list to remove the last node from.
 *  @return void* The data held within the removed node or NULL on error.
 */
void* List_Pop(List_t* list_p)
{
	//check params
	if (NULL == list_p)
	{
		return NULL;
	}
	//get the head node, an empty list will give NULL
	void* removing_node_data = List_Remove_At(list_p->length-1, list_p);
	if (NULL == removing_node_data)
	{
		return NULL;
	}
	//we should be on the correct node data, return it
	return removing_node_data;
}

/*
 *  @brief Remove the first node from the list.
 *  @param List_t* The list to remove the first node from.
 *  @return void* The data held within the removed node or NULL on error.
 */
void* List_Shift(List_t* list_p)
{
	//check params
	if (NULL == list_p)
	{
		return NULL;
	}
	//get the head node, an empty list will give NULL
	void* removing_node_data = List_Remove_At(0, list_p);
	if (NULL == removing_node_data)
	{
		return NULL;
	}
	//we should be on the correct node data, return it
	return removing_node_data;
}

/*
 *  @brief Remove the first node from the list.
 *  @param List_t* The list to reduce.
 *  @param list_reduce_fnc The function used to reduce each node.
 *  @param int The value to begin reduction with.
 *  @return List_Error_t LIST_ERROR_SUCCESS on success or any error that may occur.
 */
List_Error_t List_Reduce(List_t* list_p, List_Reduce_Fnc reducer, int* accumulator)
{
	//check params
	if (NULL == list_p || NULL == reducer || NULL == accumulator)
	{
		return LIST_ERROR_INVALID_PARAM;
	}
	
	//get the head node, an empty list will give NULL
	List_Node* current_node = list_p->head_p;
	for (size_t i = 0; i < list_p->length; i++)
	{
		//make sure the node is valid
		//im not including contingency "else" on purpose
		if (NULL != current_node)
		{
			*accumulator = reducer(current_node->data_p, *accumulator);
			//next node
			current_node = current_node->next_p;
		}
	}
	return LIST_ERROR_SUCCESS;
}

/*
 *  @brief Test each node in a list using a given find function, removing each that fail the given find function.
 *	   Note that filtered entries will be destroyed.
 *  @param List_t* The list to filter entries from.
 *  @param List_Find_Fnc The function to test each entry using.
 *	   A test is passed when this function returns true after being given data.
 *	   A test is failed and filtered out if false is returned.
 *  @return List_Error_t LIST_ERROR_SUCCESS on success or any error that may occur.
 */
List_Error_t List_Filter(List_t* list_p, List_Find_Fnc do_fnc)
{
	//check params
	if (NULL == list_p || NULL == do_fnc)
	{
		return LIST_ERROR_INVALID_PARAM;
	}
	//get the head node, an empty list will give NULL
	List_Node* current_node = list_p->head_p;
	//loop till the end
	for (size_t i = 0; i < list_p->length; i++)
	{
		//make sure the node is valid
		if (NULL != current_node)
		{
			//check if this node is the right one
			bool do_result = do_fnc(current_node->data_p);
			//store next node
			List_Node* next_node = current_node->next_p;
			if (!do_result)
			{
				//remove node and free its data
				if (NULL != list_p->free)
				{
					list_p->free(current_node->data_p);
				}
				List_Node_Remove(current_node, list_p);
			}
			//get next node
			current_node = next_node;
		}
		else
		{
			//we have hit a dead end
			return LIST_ERROR_BAD_ENTRY;
		}
	}
	//we didnt find a match
	return LIST_ERROR_SUCCESS;
}

/*
 *  @brief Remove and destroy every node in the given list.
 *  @param List_t* The list to destroy nodes within.
 *  @return void.
 */
void List_Purge(List_t* list_p)
{
	//check params
	if (NULL == list_p)
	{
		return;
	}
	//remove all list members and destroy each
	List_Node* shifted_head = List_Shift(list_p);
	while (NULL != shifted_head)
	{
		List_Node_Destroy(shifted_head);
		//get the next node
		shifted_head = List_Shift(list_p);
	}
	return;
}

/*
 *  @brief Destroy the given list and every node within it.
 *  @param List_t* The list to destroy.
 *  @return void.
 */
void List_Destroy(List_t* list_p)
{
	//check params
	if (NULL == list_p)
	{
		return;
	}
	List_Purge(list_p);
	//all has been freed
	free(list_p);
	return;
}

/*
 *  @brief Get the current length of a given list.
 *  @param List_t* The list in question.
 *  @return size_t the length of the list
 * 	   Not ethat 0 will be returned if the list does not exist.
 */
size_t List_Length(List_t* list_p)
{
	if (NULL != list_p)
	{
		return list_p->length;
	}
	return 0;
}

/*
 *  @brief Sort a given list using the set comparison function.
 *	   A list is considered sorted when precedence is in oredr from high to low
 *  @param List_t* The list to sort.
 *  @return List_Error_t LIST_ERROR_SUCCESS on success or any error that may occur.
 */
List_Error_t List_Sort(List_t* list_p)
{
	(void)list_p;
	return LIST_ERROR_FAILURE;
}
