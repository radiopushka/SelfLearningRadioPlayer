#ifndef __STACK
struct StackPointer{
	struct StackPointer* next;
	void* address;
};
//methods:
void add_item_to_stack(struct StackPointer** staddress,void* inpointer);
void add_item_bottom(struct StackPointer** staddress,void* inpointer);
void* read_stack_top(struct StackPointer** stadd);
void* read_stack_bottom(struct StackPointer** stadd);
void pop_from_stack(struct StackPointer** stadd);
void pop_stack_bottom(struct StackPointer** stadd);
void dump_stack(struct StackPointer** stadd);
void free_stack(struct StackPointer** stadd);
//this does not free the void pointer
char* pointerize(char* input);
int count_elements(struct StackPointer** stadd);
void* element_at(struct StackPointer** stadd,int index);
void reorder_stack(struct StackPointer** stadd);
#endif // !__STACK
