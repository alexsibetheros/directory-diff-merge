#include <stdio.h>
#include <stdlib.h>
#include "list.h"   
#include <string.h>
////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////
struct node *insert(struct node *p, char* path,int inode){
      	
      	struct node *temp;
      	if(p==NULL){
          	p=(struct node *)malloc(sizeof(struct node));
        	if(p==NULL) {printf("Error\n");exit(0);}
        p->path=malloc(strlen(path)*sizeof(char));
        strcpy( p-> path , path);
        p->inode=inode;
        p-> link = NULL;
      	} else {
         	temp = p; 
         	while (temp-> link != NULL)
            	{ temp = temp-> link;}
            	temp-> link = (struct node *)malloc(sizeof(struct node));
            	temp = temp-> link;
            	temp->path=malloc((strlen(path)+1)*sizeof(char));
        	strcpy( temp-> path , path);
            	temp->inode=inode;
            	temp-> link = NULL;          	
      	}
        return (p);
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////
void printlist ( struct node *p )
{
      	struct node *temp;
      	temp = p;
      	printf("The data values in the list are:{--------------\n");
      	if(p!= NULL)
      	{
        	do
         	{
          	      	printf(" path:{%s} inode:{%d}\n",temp->path,temp->inode);
                	temp=temp->link;
            	} while (temp!= NULL);
            	printf("-----------------}\n");
      	}
      	else
        	printf("The list is empty}\n");
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////
char* pop_list(struct node **p)
{
	char* path;
	struct node *temp;
	temp=*p;
	if((*p)!= NULL)
      	{
        	path=malloc(strlen((*p)->path)*sizeof(char));
        	strcpy(path, (*p)->path);
        	free( (*p)->path);
        	(*p)=(*p)->link;
        	free(temp);
        	return path;   	
      	}
      	else
        {	printf("The list is empty\n");
		return '\0';
	}
} 
////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////
void delete_list(struct node *p)
{
	struct node *temp;
	if((p)!= NULL)
      	{
      		do{    	
      			temp=p;
                	p=(p)->link;
                	free(temp->path);
                	free(temp);
            	} while (p!= NULL);
	} 	
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////
char* search_inode(struct node* p, int inode_num,int* result) /*0 didnt find, 1 found*/
{
	struct node *temp;
      	temp = p;
      	if(p!= NULL){
        	do{
          	      	if( temp->inode==inode_num){	
                		*result=1;
                		return temp->path;
                	}
                	temp=temp->link;
            	} while (temp!= NULL);
      		*result=0;
      		return "";	
      	}
      	else{
        	*result= 0;
        	return "";	
        }	
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////







 
