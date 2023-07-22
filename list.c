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

	List_Node* head_p;

	List_Cmp_Fnc cmp;
	List_Free_Fnc free;

	pthread_mutex_t lock; // mutex for list access
}
List_t;


#define LIST_ITER_FLAG_REVERSE 0x01
#define LIST_ITER_FLAG_FINISHED 0x02
typedef struct List_Iterator_t
{
	List_p list_p;
	List_Node* curr_p;
	uint8_t flags;
}
List_Iterator_t;

/*
 *  @brief Create a list node structure.
 *  @param void* The data to hold within the node.
 *  @return List_Node* A pointer to the allocated node or NULL on error.
 */
static List_Node* List_Node_Create(void* data_p) //N/A
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
static List_Node* List_Node_At(size_t at, List_t* list_p) //N/A
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
static List_Error_t List_Node_Insert(List_Node* node_p, size_t at, List_t* list_p) //N/A
{
	//param check
	if (NULL == node_p || NULL == list_p)
	{
		return LIST_ERROR_INVALID_PARAM;
	}

	List_Error_t ret_val = LIST_ERROR_SUCCESS;

	if (at > list_p->length)
	{
		ret_val = LIST_ERROR_INVALID_PARAM;
		goto exit;
	}

	//make sure this wouldnt exceed length limits
	if (list_p->max_length && list_p->length >= list_p->max_length)
	{
		ret_val = LIST_ERROR_EXCEED_LIMIT;
		goto exit;
	}
	if (0 != list_p->length) //no need to update any other nodes
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

exit:
	return ret_val;
}

/*
 *  @brief Destroy a given node. Note that this will only destroy the node and not the data within it
 *  @param List_Node* A pointer to the node to destroy.
 *  @return Void
 */
static void List_Node_Destroy(List_Node* node) //N/A
{
	//check params
	if (NULL != node)
	{
		free(node); //setting to null here does nothing
	}
}

/*
 *  @brief Check a list for sortedness. 
 *  @param List_t* A pointer to the list to check for sortedness.
			A list is considered sorted when each node's precedence is ordered from high to low
 *  @param List_Cmp_Fnc An optional custom function pointer describing the precedence to use during sorting.
						If NULL is used here, the list's default cmp function will be used
 *  @return bool True if sorted fully, false otherwise
 */
static bool List_Is_Sorted(List_t* list_p, List_Cmp_Fnc cmp_fnc) //N/A
{
	//check params
	if (NULL == list_p)
	{
		return LIST_ERROR_INVALID_PARAM;
	}
	//get the head node, an empty list will give NULL
	List_Node* current_node = list_p->head_p;
	//loop till we are at the second to last
	for (size_t i = 0; i < list_p->length-1 && NULL != current_node; i++)
	{
		//compare current and next node
		int node_cmp = 0;
		if (NULL == cmp_fnc && NULL != list_p->cmp)
		{
			node_cmp = list_p->cmp(
				current_node->data_p,
				current_node->next_p->data_p
			);
		}
		else if (NULL != cmp_fnc)
		{
			node_cmp = cmp_fnc(
				current_node->data_p,
				current_node->next_p->data_p
			);
		}
		if (0 > node_cmp)
		{
			return false;
		}

		current_node = current_node->next_p;
	}
	return true;
}

/*
 *  @brief Swap the data of two given nodes.
 *  @param List_Node* A pointer to the first node to swap.
 *  @param List_Node* A pointer to the second node to swap.
 *  @return List_Error_t LIST_ERROR_SUCCESS on success or any error that may occur.
 */
static List_Error_t List_Node_Swap(List_Node* node_a, List_Node* node_b) //N/A
{
	//check params
	if (NULL == node_a || NULL == node_b)
	{
		return LIST_ERROR_INVALID_PARAM;
	}

	//store tmp vals for swap
	List_Node* tmp_data_p = node_a->data_p;

	//change a's neighbors
	node_a->data_p = node_b->data_p;

	node_b->data_p = tmp_data_p;

	return LIST_ERROR_SUCCESS;
}

/*
 *  @brief Remove a given node from a given list.
 *     Note that this will free the node structure BUT WILL NOT free the data held within the node.
 *  @param List_Node* A pointer to the node to remove.
 *  @param List_t* A pointer to the list that contains the node in question.
 *  @return List_Error_t LIST_ERROR_SUCCESS on success or any error that may occur.
 */
static List_Error_t List_Node_Remove(List_Node* node, List_t* list_p) //N/A
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
					If 0 is passed as this value, no maximum will be enforced (Not recommended!)
 *  @param List_Cmp_Fnc A function used when comparing data within the list for matches or sorting.
 *	   Passing NULL here will cause each member to have the same precedence.
 *  @param List_Free_Fnc A function used when freeing data within the list.
 *  @return List_t* A pointer to the allocated list or NULL on error.
 */
List_t* List_Create(size_t max_length, List_Cmp_Fnc cmp, List_Free_Fnc free) //N/A
{
	if (NULL == free)
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

		int lock_created = pthread_mutex_init(&(new_list_p->lock), NULL);
		if (lock_created != 0)
		{
			free(new_list_p);
			new_list_p = NULL;
		}
	}
	return new_list_p;
}

/*
 *  @brief Copy that the given list is valid.
 *  @param List_t* The list to copy.
 *  @param List_Copy_Fnc A user provided function to copy node values.
			If Null is passed, a shallow copy will be formed where:
				Each node value is the same pointer
				The list itself will be in different
 *  @return List_t* A pointer to the allocated list or NULL on error.
 */
List_t* List_Copy(List_t* list_p, List_Copy_Fnc copy_node_fnc) //safe
{
	//check params
	if (NULL == list_p)
	{
		return NULL;
	}

	pthread_mutex_lock(&(list_p->lock));

	List_t* copy_list = List_Create(list_p->max_length, list_p->cmp, list_p->free);
	if (NULL == copy_list)
	{
		goto exit;
	}
	
	//get the head node, an empty list will give NULL
	List_Node* current_node = list_p->head_p;
	//loop till we are at the end
	for (size_t i = 0; i < list_p->length && NULL != current_node; i++)
	{
		List_Error_t could_push;
		if (NULL == copy_node_fnc)
		{
			could_push = List_Push(current_node->data_p, copy_list);
		}
		else
		{
			could_push = List_Push(copy_node_fnc(current_node->data_p), copy_list);
		}

		if (LIST_ERROR_SUCCESS != could_push)
		{
			List_Destroy(copy_list);
			copy_list = NULL;
			goto exit;
		}

		current_node = current_node->next_p;
	}

exit:
	pthread_mutex_unlock(&(list_p->lock));
	return copy_list;
}

/*
 *  @brief Verify that the given list is valid.
 *  @param List_t* The list to verify.
 *  @param List_Find_Fnc A user provided function to cehck validity of list values.
			If Null is passed, this will not be used and only metadata will be validated.
 *  @return List_Error_t LIST_ERROR_SUCCESS if the list is valid or an error attempting to describe the failure.
 */
List_Error_t List_Verify(List_t* list_p, List_Find_Fnc valid_check) //safe
{
	//check params
	if (NULL == list_p)
	{
		return LIST_ERROR_INVALID_PARAM;
	}

	List_Error_t ret_val = LIST_ERROR_SUCCESS;

	pthread_mutex_lock(&(list_p->lock));
	
	//get the head node, an empty list will give NULL
	List_Node* current_node = list_p->head_p;
	//loop till we are at the end
	for (size_t i = 0; i < list_p->length && NULL != current_node; i++)
	{
		//user validity check
		if (NULL != valid_check && !valid_check(current_node->data_p))
		{
			ret_val = LIST_ERROR_BAD_ENTRY;
			goto exit;
		}
		current_node = current_node->next_p;
	}

exit:
	pthread_mutex_unlock(&(list_p->lock));
	return ret_val;
}

/*
 *  @brief Get the data in the node at the given location.
 *  @param size_t The index of the node in question.
 *  @param List_t* The list that contains the node in question.
 *  @return void* The data held within the node at the given location or NULL on error.
 */
void* List_At(size_t at, List_t* list_p) //safe
{
	//check params
	if (NULL == list_p)
	{
		return NULL;
	}

	pthread_mutex_lock(&(list_p->lock));
	List_Node* node_at_p = List_Node_At(at, list_p);
	pthread_mutex_unlock(&(list_p->lock));

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
List_Error_t List_Push(void* data_p, List_t* list_p) //safe
{
	//check param
	if (NULL == list_p || NULL == data_p)
	{
		return LIST_ERROR_INVALID_PARAM;
	}
	return List_Insert(data_p, list_p->length, list_p); //safe call
}

/*
 *  @brief Put data into the beginning of a given list.
 *  @param void* The data to insert to the end of the list.
 *  @param List_t* The list that should contain the given node.
 *  @return List_Error_t LIST_ERROR_SUCCESS on success or any error that may occur.
 */
List_Error_t List_Unshift(void* data_p, List_t* list_p) //safe
{
	//check param
	if (NULL == list_p || NULL == data_p)
	{
		return LIST_ERROR_INVALID_PARAM;
	}
	return List_Insert(data_p, 0, list_p); //safe call
}

/*
 *  @brief Put data into the given index of a given list.
 *  @param void* The data to insert to the list.
 *  @param size_t The index in which to insert the given data to.
 *  @param List_t* The list that should contain the given node.
 *  @return List_Error_t LIST_ERROR_SUCCESS on success or any error that may occur.
 */
List_Error_t List_Insert(void* data_p, size_t at, List_t* list_p) //safe
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
		return LIST_ERROR_BAD_ENTRY;
	}
	//at is checked in insert node

	pthread_mutex_lock(&(list_p->lock));
	List_Error_t ret_val = List_Node_Insert(new_node_p, at, list_p);
	pthread_mutex_unlock(&(list_p->lock));

	return ret_val;
}

/*
 *  @brief Find the first instance of a given search data using the defined compare function.
 *  @param void* The data to use as a 'search term'.
 *  @param List_t* The list that should contain the given search term.
 		The function uses the list's cmp function to decide if a node is "equal" to the search term.
		 If the cmp function is not defined, the function will assume that all nodes are the same,
		 	therefore returning the first node it 'sees'.
 *  @param size_t* A pointer in which to put the index if any should be found.
	   Note that this will not be changed if no match is found.
 *  @return List_Error_t LIST_ERROR_SUCCESS on successful find, LIST_ERROR_EXCEED_LIMIT if not found in list,
 						or any error that may occur.
 */
List_Error_t List_Find(void* search_data_p, List_t* list_p, size_t* response) //safe
{
	//check params
	if (NULL == list_p || NULL == search_data_p)
	{
		return LIST_ERROR_INVALID_PARAM;
	}
	List_Error_t ret_val = LIST_ERROR_SUCCESS;

	pthread_mutex_lock(&(list_p->lock));

	//get the head node, an empty list will give NULL
	List_Node* current_node = list_p->head_p;
	//loop till we are at the end
	for (size_t i = 0; i < list_p->length && NULL != current_node; i++)
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
				//return success code
				goto exit;
			}
			//otherwise, just try the next node
			current_node = current_node->next_p;
		}
		else
		{
			//we have hit a dead end
			ret_val = LIST_ERROR_BAD_ENTRY;
			goto exit;
		}
	}

	//return success code
	ret_val = LIST_ERROR_EXCEED_LIMIT;
exit:
	pthread_mutex_unlock(&(list_p->lock));
	return ret_val;
}

/*
 *  @brief Test each node in a list using a given find function until an entry passes or no entries are left.
 *  @param List_t* The list to test entries within.
 *  @param List_Find_Fnc The function to test each entry using.
	   A test is passed when this function returns true after being given data.
 *  @return bool True if an entry is found that passes the given test or false if no nodes pass or an error occurs.
 */
bool List_Some(List_t* list_p, List_Find_Fnc do_fnc) //safe
{
	//check params
	if (NULL == list_p || NULL == do_fnc)
	{
		return false;
	}
	bool ret_val = false;

	pthread_mutex_lock(&(list_p->lock));

	//get the head node, an empty list will give NULL
	List_Node* current_node = list_p->head_p;
	//loop till we are at the end
	for (size_t i = 0; i < list_p->length && NULL != current_node; i++)
	{
		//make sure the node is valid
		if (NULL != current_node)
		{
			//check if this node is the right one
			bool node_found = do_fnc(current_node->data_p);
			if (node_found)
			{
				ret_val = true;
				goto exit;
			}
		}
		else
		{
			//we have hit a dead end
			goto exit;
		}
		current_node = current_node->next_p;
	}

	//we didnt find a match
exit:
	pthread_mutex_unlock(&(list_p->lock));
	return ret_val;
}

/*
 *  @brief Test each node in a list using a given find function until an entry fails or no entries are left.
 *  @param List_t* The list to test entries within.
 *  @param List_Find_Fnc The function to test each entry using.
	   A test is passed when this function returns true after being given data and failed if false is returned.
 *  @return bool True if all entries pass th egiven test or false when one fails or an error occurs.
 */
bool List_Every(List_t* list_p, List_Find_Fnc do_fnc) //safe
{
	//check params
	if (NULL == list_p || NULL == do_fnc)
	{
		return false;
	}
	bool ret_val = true;

	pthread_mutex_lock(&(list_p->lock));

	//get the head node, an empty list will give NULL
	List_Node* current_node = list_p->head_p;
	//loop till we are at the end
	for (size_t i = 0; i < list_p->length && NULL != current_node; i++)
	{
		//make sure the node is valid
		//im not including contingency "else" on purpose
		if (NULL != current_node)
		{
			//check if this node fails
			bool node_failed = do_fnc(current_node->data_p);
			if (!node_failed)
			{
				ret_val = false;
				goto exit;
			}
		}
		current_node = current_node->next_p;
	}

	//we didnt find a failure!
exit:
	pthread_mutex_unlock(&(list_p->lock));
	return ret_val;
}

/*
 *  @brief Run a given function against all data entries within a list.
 *  @param List_t* The list to test entries within.
 *  @param List_Do_Fnc The function to run with each entry.
 *  @return List_Error_t LIST_ERROR_SUCCESS on success or any error that may occur.
 */
List_Error_t List_For_Each(List_t* list_p, List_Do_Fnc do_fnc) //safe
{
	//check params
	if (NULL == list_p || NULL == do_fnc)
	{
		return LIST_ERROR_INVALID_PARAM;
	}
	List_Error_t ret_val = LIST_ERROR_SUCCESS;

	pthread_mutex_lock(&(list_p->lock));

	//get the head node, an empty list will give NULL
	List_Node* current_node = list_p->head_p;
	//loop till we are at the end
	for (size_t i = 0; i < list_p->length && NULL != current_node; i++)
	{
		//make sure the node is valid
		if (NULL != current_node)
		{
			do_fnc(current_node->data_p);
		}
		else
		{
			ret_val = LIST_ERROR_BAD_ENTRY;
			goto exit;
		}
		current_node = current_node->next_p;
	}

exit:
	pthread_mutex_unlock(&(list_p->lock));
	return ret_val;
}

/*
 *  @brief Remove a given index from the list.
 *  @param size_t The index to remove from the list.
 *  @param List_t* The list to remove the given index from.
 *  @return void* The data held within the removed node or NULL on error.
 */
void* List_Remove_At(size_t at, List_t* list_p) //safe
{
	//check params
	if (NULL == list_p || at > list_p->length)
	{
		return NULL;
	}
	void* ret_val = NULL;

	pthread_mutex_lock(&(list_p->lock));

	//find the node
	List_Node* node = List_Node_At(at, list_p);
	if (NULL == node)
	{
		ret_val = NULL;
		goto exit;
	}
	//save the nodes data
	void* node_data = node->data_p;
	//remove the node
	List_Error_t removed_node = List_Node_Remove(node, list_p);
	if (LIST_ERROR_SUCCESS != removed_node)
	{
		ret_val = NULL;
		goto exit;
	}
	ret_val = node_data;

exit:
	pthread_mutex_unlock(&(list_p->lock));
	//return data
	return ret_val;
}

/*
 *  @brief Remove a given index from the list and destroy the data held within it.
 *  @param size_t The index to delete from the list.
 *  @param List_t* The list to delete the given index from.
 *  @return void.
 */
void List_Delete_At(size_t at, List_t* list_p) //safe
{
	//check params
	if (NULL == list_p || at > list_p->length)
	{
		return;
	}

	//find the node and delete it
	void* removing_node_data = List_Remove_At(at, list_p); //safe call
	//i dont see how anyone else could have a handle on removing_node_data by this point
	if (NULL != removing_node_data)
	{
		//we should be on the correct node data, free it
		if (NULL != list_p->free)
		{
			list_p->free(removing_node_data); //setting to null wont do anything
		}
	}
}

/*
 *  @brief Remove the last node from the list.
 *  @param List_t* The list to remove the last node from.
 *  @return void* The data held within the removed node or NULL on error.
 */
void* List_Pop(List_t* list_p) //safe
{
	//check params
	if (NULL == list_p)
	{
		return NULL;
	}

	//get the head node, an empty list will give NULL
	void* removing_node_data = List_Remove_At(list_p->length-1, list_p); //safe call

	//we should be on the correct node data, return it
	return removing_node_data;
}

/*
 *  @brief Remove the first node from the list.
 *  @param List_t* The list to remove the first node from.
 *  @return void* The data held within the removed node or NULL on error.
 */
void* List_Shift(List_t* list_p) //safe
{
	//check params
	if (NULL == list_p)
	{
		return NULL;
	}

	//get the head node, an empty list will give NULL
	void* removing_node_data = List_Remove_At(0, list_p); //safe call

	//we should be on the correct node data, return it
	return removing_node_data;
}

List_Iterator_p List_Iterator_Create(List_p list_p)
{
	List_Iterator_p iter_p = NULL;
	if (NULL != list_p)
	{
		iter_p = calloc(1, sizeof(List_Iterator_t));
		if (NULL != iter_p)
		{
			iter_p->list_p = list_p;
			iter_p->curr_p = NULL;
			iter_p->flags = 0;
		}
	}
	return iter_p;
}

List_Iterator_p List_Iterator_Create_Reverse(List_p list_p)
{
	List_Iterator_p iter_p = NULL;
	if (NULL != list_p)
	{
		iter_p = calloc(1, sizeof(List_Iterator_t));
		if (NULL != iter_p)
		{
			iter_p->list_p = list_p;
			iter_p->curr_p = NULL;
			iter_p->flags = LIST_ITER_FLAG_REVERSE;
		}
	}
	return iter_p;
}

void* List_Iterator_Next(List_Iterator_p iter_p)
{
	void* ret_data = NULL;
	if (NULL != iter_p)
	{
		if (NULL != iter_p->curr_p)
		{
			iter_p->curr_p = (iter_p->flags & LIST_ITER_FLAG_REVERSE) ? iter_p->curr_p->previous_p : iter_p->curr_p->next_p;
		}
		//if this is the first call to next
		else if (!(iter_p->flags & LIST_ITER_FLAG_FINISHED))
		{
			iter_p->curr_p = (iter_p->flags & LIST_ITER_FLAG_REVERSE) ? List_Node_At(List_Length(iter_p->list_p) - 1, iter_p->list_p) : List_Node_At(0, iter_p->list_p);
		}

		//now that we have updated curr_p, set the return value if possible
		if (NULL != iter_p->curr_p)
		{
			ret_data = iter_p->curr_p->data_p;
		}

		//if we are returning NULL this must be the end of the list
		if (NULL == ret_data)
		{
			iter_p->flags ^= LIST_ITER_FLAG_FINISHED;
		}
	}
	return ret_data;
}

void* List_Iterator_Prev(List_Iterator_p iter_p)
{
	void* ret_data = NULL;
	if (NULL != iter_p) //valid check
	{
		if (NULL != iter_p->curr_p)//normal scenario
		{
			iter_p->curr_p = (iter_p->flags & LIST_ITER_FLAG_REVERSE) ? iter_p->curr_p->next_p : iter_p->curr_p->previous_p;
		}
		//if finished
		else if (iter_p->flags & LIST_ITER_FLAG_FINISHED)
		{
			iter_p->curr_p = (iter_p->flags & LIST_ITER_FLAG_REVERSE) ? List_Node_At(0, iter_p->list_p) : List_Node_At(List_Length(iter_p->list_p) - 1, iter_p->list_p);
			iter_p->flags ^= LIST_ITER_FLAG_FINISHED;
		}

		//now that we have updated curr_p, set the return value if possible
		if (NULL != iter_p->curr_p)
		{
			ret_data = iter_p->curr_p->data_p;
		}
	}
	return ret_data;
}

void* List_Iterator_Curr(List_Iterator_p iter_p)
{
	if (NULL != iter_p && NULL != iter_p->curr_p)
	{
		return iter_p->curr_p->data_p;
	}
	return NULL;
}

/*
 *  @brief Remove the first node from the list.
 *  @param List_t* The list to reduce.
 *  @param list_reduce_fnc The function used to reduce each node.
 *  @param void* The value to begin reduction with.
 *  @return List_Error_t LIST_ERROR_SUCCESS on success or any error that may occur.
 */
List_Error_t List_Reduce(List_t* list_p, List_Reduce_Fnc reducer, void* accumulator) //safe
{
	//check params
	if (NULL == list_p || NULL == reducer || NULL == accumulator)
	{
		return LIST_ERROR_INVALID_PARAM;
	}

	pthread_mutex_lock(&(list_p->lock));
	
	//get the head node, an empty list will give NULL
	List_Node* current_node = list_p->head_p;
	//loop till we are at the end
	for (size_t i = 0; i < list_p->length && NULL != current_node; i++)
	{
		//make sure the node is valid
		//im not including contingency "else" on purpose
		if (NULL != current_node)
		{
			accumulator = reducer(current_node->data_p, accumulator);
		}
		current_node = current_node->next_p;
	}

	pthread_mutex_unlock(&(list_p->lock));

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
List_Error_t List_Filter(List_t* list_p, List_Find_Fnc do_fnc) //safe
{
	//check params
	if (NULL == list_p || NULL == do_fnc)
	{
		return LIST_ERROR_INVALID_PARAM;
	}
	List_Error_t ret_val = LIST_ERROR_SUCCESS;

	pthread_mutex_lock(&(list_p->lock));

	//get the head node, an empty list will give NULL
	List_Node* current_node = list_p->head_p;
	//loop till the end (have to use index here)
	for (size_t i = 0; i < list_p->length && NULL != current_node; i++)
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
				//delete node and free its data
				pthread_mutex_unlock(&(list_p->lock)); //just a cheat, i dont really like this
				List_Delete_At(i, list_p); //safe call
				pthread_mutex_lock(&(list_p->lock));
				i--;
			}
			//use next node
			current_node = next_node;
		}
		else
		{
			//we have hit a dead end
			ret_val = LIST_ERROR_BAD_ENTRY;
			goto exit;
		}
	}
	//we didnt find a match
exit:
	pthread_mutex_unlock(&(list_p->lock));
	return ret_val;
}

/*
 *  @brief Remove and destroy every node in the given list.
 *  @param List_t* The list to destroy nodes within.
 *  @return void.
 */
void List_Purge(List_t* list_p) //safe
{
	//check params
	if (NULL == list_p)
	{
		return;
	}
	//delete all list members until length is 0
	while (list_p->length)
	{
		List_Delete_At(0, list_p); //safe call
	}
	return;
}

/*
 *  @brief Destroy the given list and every node within it.
 *  @param List_t* The list to destroy.
 *  @return void.
 */
void List_Destroy(List_t* list_p) //safe
{
	//check params
	if (NULL == list_p)
	{
		return;
	}
	List_Purge(list_p); //safe call
	//all has been freed
	pthread_mutex_destroy(&(list_p->lock));
	free(list_p);//setting to null here does nothing
	return;
}

/*
 *  @brief Get the current length of a given list.
 *  @param List_t* The list in question.
 *  @return size_t the length of the list
 * 	   Not ethat 0 will be returned if the list does not exist.
 */
size_t List_Length(List_t* list_p) //safe
{
	if (NULL != list_p)
	{
		return list_p->length;
	}
	return 0;
}

/*
 *  @brief Reverse a given list from its current order.
 *  @param List_t* The list to reverse.
 *  @return List_Error_t LIST_ERROR_SUCCESS on success or any error that may occur.
 */
List_Error_t List_Reverse(List_t* list_p)
{
	if (NULL == list_p)
	{
		return LIST_ERROR_INVALID_PARAM;
	}
	List_Error_t ret_val = LIST_ERROR_SUCCESS;

	pthread_mutex_lock(&(list_p->lock));

	size_t midpoint = list_p->length / 2;
	List_Node* front_node = list_p->head_p;
	List_Node* rear_node = List_Node_At(list_p->length-1, list_p);
	for(size_t i = 0; i < midpoint; i++)
	{
		List_Error_t could_swap = List_Node_Swap(front_node, rear_node);
		if (LIST_ERROR_SUCCESS != could_swap)
		{
			ret_val = could_swap;
			goto exit;
		}

		front_node = front_node->next_p;
		rear_node = rear_node->previous_p;
	}

exit:
	pthread_mutex_unlock(&(list_p->lock));
	return ret_val;
}

/*
 *  @brief Sort a given list using the set comparison function.
 *	   A list is considered sorted when precedence is in oredr from high to low
 *  @param List_t* The list to sort.
 *  @param List_Cmp_Fnc An optional custom function pointer describing the precedence to use during sorting.
						If NULL is used here, the list's default cmp function will be used
 *  @return List_Error_t LIST_ERROR_SUCCESS on success or any error that may occur.
 */
List_Error_t List_Sort(List_t* list_p, List_Cmp_Fnc cmp_fnc)
{
	if (NULL == list_p)
	{
		return LIST_ERROR_INVALID_PARAM;
	}
	List_Error_t ret_val = LIST_ERROR_SUCCESS;

	pthread_mutex_lock(&(list_p->lock));
	while(!List_Is_Sorted(list_p, cmp_fnc))
	{
		//loop through every node until the second to last one
		for(size_t i = 0; i < list_p->length-1; i++)
		{
			//compare current and next node
			List_Node* current_node = List_Node_At(i, list_p);
			if (NULL == current_node)
			{
				ret_val = LIST_ERROR_BAD_ENTRY;
				goto exit;
			}
			int node_cmp = 0;
			if (NULL == cmp_fnc && NULL != list_p->cmp)
			{
				node_cmp = list_p->cmp(
					current_node->data_p,
					current_node->next_p->data_p
				);
			}
			else if (NULL != cmp_fnc)
			{
				node_cmp = cmp_fnc(
					current_node->data_p,
					current_node->next_p->data_p
				);
			}
			if (0 > node_cmp)
			{
				List_Error_t could_swap = List_Node_Swap(
					List_Node_At(i, list_p),
					List_Node_At(i+1, list_p)
				);
				if (LIST_ERROR_SUCCESS != could_swap)
				{
					ret_val = could_swap;
					goto exit;
				}
			}
		}
	}

exit:
	pthread_mutex_unlock(&(list_p->lock));
	return ret_val;
}
