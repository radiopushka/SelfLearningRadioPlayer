#include<stdio.h>
#include<string.h>
#include<stdlib.h>
#include"stack.h"

void add_item_to_stack(struct StackPointer** staddress,void* inpointer){
	struct StackPointer* newpt=malloc(sizeof(struct StackPointer));
	newpt->address=inpointer;
	newpt->next=*staddress;
	*staddress=newpt;
}
void add_item_bottom(struct StackPointer** staddress,void* inpointer){
	struct StackPointer* pt=*staddress;
	struct StackPointer* newpt=malloc(sizeof(struct StackPointer));
	newpt->address=inpointer;
	if(pt==NULL){
		newpt->next=NULL;
		*staddress=newpt;
		return;
	}
	struct StackPointer* tsugi;
	while(pt!=NULL){
		tsugi=pt->next;
		if(tsugi==NULL){
			pt->next=newpt;
			newpt->next=NULL;
			return;
		}
		pt=tsugi;		
	}
}
void* read_stack_bottom(struct StackPointer** stadd){
	struct StackPointer* pt=*stadd;
	if(pt==NULL){
		return NULL;
	}
	while(pt!=NULL){
		if(pt->next==NULL){
			return pt->address;
		}
		pt=pt->next;
	}
	return NULL;
}
void pop_stack_bottom(struct StackPointer** stadd){
	struct StackPointer* pt=*stadd;
	struct StackPointer* tsugi;
	struct StackPointer* prev=NULL;
	while(pt!=NULL){
		tsugi=pt->next;
		if(tsugi==NULL){
			if(prev==NULL){
				*stadd=NULL;
				free(pt);
				return;
			}else{
				prev->next=NULL;
				free(pt);
				return;
			}
		}
		prev=pt;
		pt=pt->next;
	}
}
void* read_stack_top(struct StackPointer** stadd){
	if(*stadd==NULL){
		return NULL;
	}
	return (*stadd)->address;
}
void pop_from_stack(struct StackPointer** stadd){
	if((*stadd)==NULL){
		return;
	}
	struct StackPointer* next=(*stadd)->next;
	free(*stadd);
	*stadd=next;
}
void dump_stack(struct StackPointer** stadd){
	struct StackPointer* bsp=*stadd;
	while(bsp!=NULL){
		printf("%s\n",(char*)(bsp->address));
		bsp=bsp->next;
	}

}
void free_stack(struct StackPointer** stadd){
	while(read_stack_top(stadd)!=NULL){
		pop_from_stack(stadd);
	}

}
char* pointerize(char* input){
	int s=strlen(input);
	char* new=malloc(sizeof(char)*(s+1));
	sprintf(new,"%s",input);
	return new;
}
int count_elements(struct StackPointer** stadd){
	int sum=0;
	struct StackPointer* bsp=*stadd;
	while(bsp!=NULL){
		sum++;
		bsp=bsp->next;
	}
	return sum;
}
void* element_at(struct StackPointer** stadd,int index){
	int sum=0;
	struct StackPointer* bsp=*stadd;
	while(bsp!=NULL){
		if(sum==index){
			return bsp->address;
		}
		sum++;
		bsp=bsp->next;
	}
	return NULL;
}
void reorder_stack(struct StackPointer** stadd){
	struct StackPointer* bsp=NULL;
	struct StackPointer* vict=*stadd;
	struct StackPointer* tmp;
	while(vict!=NULL){
		add_item_to_stack(&bsp,vict->address);
		tmp=vict->next;
		free(vict);
		vict=tmp;
	}
	*stadd=bsp;
}
