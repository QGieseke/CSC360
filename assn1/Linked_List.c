#include <stdio.h>
#include <string.h>
#include <stdlib.h>

/* Author: Quinn Gieseke (V00884671)
 * CSC 360 Assignment 0 Linked List
 * Yvonne Coady
 * 1-15-19
 * Linked_List.c
 */


struct node {
	struct node *prev;
	struct node *next;
	int data;	
}node;

//function declarations
int add_idx (struct node **head, int element, int index);	//add to given index
int add		(struct node **head, int element);				//add to end of list
int get		(struct node  *head, int index);					//return given index (non-destructive)
int indexOf	(struct node  *head, int element);				//return index of first occurrence of element (non-destructive)
int isEmpty	(struct node  *head);							//1 if empty, 0 if not
int del_idx (struct node **head, int index);				//delete (and free) given index
int del_val	(struct node **head, int element);				//delete (and free) first occurrence of given element
int clear	(struct node **head);							//delete (and free) entire list
int size	(struct node  *head);							//return size of list (O(n)), value not stored
void print 	(struct node  *head);							//give a one line printout of list contents



int main(){

	//jank, need to learn how to wrap in a function (Tripple deep pointers? I dont think that fixes it (bootstrapping))
	struct node *x = NULL;
	struct node **head = &x;
	
	
	printf("init\n");
	print(*head);
	printf("isEmpty? %d\n", isEmpty(*head));
	
	add(head, 5);
	add(head, 4);
	printf("isEmpty? %d\n", isEmpty(*head));
	
	add(head, 3);
	add(head, 2);
	add(head, 1);
	printf("size is %d\n", size(*head));
	
	add_idx(head, 10, 0);
	printf("%d, %d, %d, ...\n", get(*head, 0), get(*head, 1), get(*head, 2));
	
	printf("index of 1, 2, 3: %d, %d, %d\n", indexOf(*head, 1), indexOf(*head, 2), indexOf(*head, 3));
	printf("index of 100: %d\n", indexOf(*head, 100));
	
	print(*head);
	// printf("\nremove 2: %d\n", del_val(head, 2));
	// print(*head);
	// printf("remove 5: %d\n", del_val(head, 5));
	// print(*head);
	// printf("remove 1: %d\n", del_val(head, 1));
	// print(*head);
	// printf("remove 100: %d\n", del_val(head, 100));
	
	printf("removed %d at index 0\n", del_idx(head, 0));
	print(*head);
	printf("removed %d at end\n", del_idx(head, size(*head)));
	print(*head);
	printf("removed %d at index 2\n", del_idx(head, 2))	;
	printf("added\n");
	
	clear(head);
	printf("\nList cleared\n\n");
	
	
	print(*head);
	add(head, 1);
	
	printf("Remove 0 of size 1:%d \n", del_idx(head, 0));
	printf("Clear empty\n");
	
	clear(head);
	print(*head);
	clear(head);
	print(*head);
	add_idx(head, 10, 0);
	print(*head);
	
	add(head, 6);
	add(head, 7);
	print(*head);
	printf("\n\nprocess done\n");
	return (0);
}


int add_idx(struct node **head, int element, int index){
	int Lsize = size(*head);
	if(index > Lsize){
		printf("index (%d) to large for given list (%d)\n", index, Lsize);
		return(NULL);
	} else if (index == Lsize){
		add(head, element);
		return (0);
	} else if (index == 0){
		if (Lsize == 0){
			return add(head, element);
		} else {
			struct node *new = (struct node*) malloc(sizeof(struct node*));
			new->prev = NULL;
			new->next = *head;
			new->data = element;
			(*head)->prev = new;
			(*head) = new;
			// print(*head);
		}
		return (0);
	} else {
		struct node *curr = *head;
		for (int i = index; i > 0;i--, curr = curr->next);
		
		struct node *new = (struct node*)malloc(sizeof(struct node*));
		new->data = element;
		new->prev = curr->prev;
		new->next = curr;
		// print(new);

		curr->prev->next = new;
		curr->prev = new;
		
		return(0);
	}
	return(-1);
}


int add (struct node **head, int element){
	printf("add %d\n", element);
	// if(head = NULL){
		
	// }
	
	if((*head) == NULL){
		// printf("Null case\n");
		*head = (struct node*)malloc(sizeof (struct node));
		// printf("allocated\n");
		(*head)->data = element;
		(*head)->prev = NULL;
		(*head)->next = NULL;
		// print(*head);
		return(1);
	}
	// printf("Unnull case\n");
	struct node *curr = *head;
	while (curr->next != NULL){
		// printf("%d", curr->data);
		curr = curr->next;
	}
	struct node *new = (struct node*)malloc(sizeof (struct node));
	// printf("\nallocated new\n");
	new->data = element;
	new->prev = curr;
	new->next = NULL;
	curr->next = new;
	return(1);
}

int get (struct node *head, int index){
	if(index > size(head)) return(NULL);
	
	struct node *curr = head;
	for(int i = index; i > 0; i--, curr = curr->next);
	return curr->data;
}


int indexOf(struct node *head, int element){
	int i = 0;
	for(struct node *curr = head;; curr = curr->next, i++){
		if (curr->data == element){
			return (i);
		}
		if (curr->next == NULL){
			return(NULL);
		}
	}
	return (NULL);
}

 
int isEmpty (struct node *head){
	return ((head == NULL));
}


int del_idx(struct node **head, int index){
	int Lsize = size(*head);
	if (index > Lsize){
		printf("given index (%d) is to large for given list size (%d)", index, Lsize);
		return(NULL);
	} else if (index == Lsize){
		struct node *curr = *head;
		for(; curr->next != NULL; curr = curr->next);
		int val = curr->data;
		curr->prev->next = NULL;
		free(curr);
		return(val);
	} else if (index == 0){
		int val = (*head)->data;
		if(Lsize == 1){
			clear(head);
		} else {
			(*head) = (*head)->next;
			free((*head)->prev);
		}
		return (val);
		
	} else {
		struct node *curr = (*head);
		for(int i = index; i > 0; curr = curr->next, i--);
		struct node *temp = curr->prev;
		int val = curr->data;
		curr->prev->next = curr->next;
		curr->next->prev = temp;
		free(curr);
		return (val);
	}
	return(NULL);
}


int del_val(struct node **head, int element){
	//manually check head, if we have to update pointer
	if((*head)->data == element){
		int val = (*head)->data;
		(*head) = (*head)->next;
		free((*head)->prev);
		return (val);
	}
	//check all middle elements
	struct node *curr = (*head);
	for(; curr->next != NULL; curr = curr->next){
		if(curr->data == element){
			struct node *temp = curr->prev;
			int val = curr->data;
			curr->prev->next = curr->next;
			curr->next->prev = temp;
			free(curr);
			return(val);
		}
	}
	//manually handle last element to avoid null pointer errors
	if(curr->data == element){
		int val = curr->data;
		curr->prev->next = NULL;
		free(curr);
		return(val);
	}
	return(NULL);
}


int clear(struct node **head){
	//trivial case
	if((*head) == NULL){
		return (0);
	//size 1 case
	} else if ((*head)->next == NULL){
		free(*head);
		(*head) = NULL;
		return(0);
	}
	//If we know the size is >2, this offset will not fail.
	struct node *curr = (*head)->next;
	for(; curr->next != NULL; curr = curr->next){
		free(curr->prev);
		
	}
	//clean up the last element outside loop;
	free(curr);
	(*head) = NULL;
	return(0);
}


int size(struct node *head){
	int i = 0;
	for(struct node *curr = head; curr != NULL; curr = curr->next, i++);
	return i;
}


void print(struct node *head){
	struct node *curr = head;
	printf("List: ");
	while(curr != NULL){
		printf("%d ", curr->data);
		curr = curr->next;
	}
	printf("\n");
	//printf("data: %d\n", head->data);
	
	return;
}