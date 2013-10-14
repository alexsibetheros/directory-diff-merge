#include "functions.h"
#include "core_functions.h"

////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////
void main (int argc , char *argv []) 
{
	int option;
	struct node *inode_list = NULL;
	if( argc <3){
		printf("Not enough arguments. Please give: {executable} {source directory 1} {source directory 2}\n");
		return;
	}
	else if( argc==3){
		option=0; /*0 means differences only*/
		printf("Calculating Differences\n");
		differences(argv[1],argv[2],"",option,&inode_list);
		printf("|||||||||||||||||||||||||||||||||||||||||||||\n");
		differences(argv[2],argv[1],"",option,&inode_list);
		delete_list(inode_list);
	}
	else if( argc==4){
		option=1;/*1 means differences and merging of directories*/
		if ( mkdir(argv[3], 00777)== -1 ) {perror(" Arxh"); return;}
		printf("Merging\n");
		differences(argv[1],argv[2],argv[3],option,&inode_list);
		printf("|||||||||||||||||||||||||||||||||||||||||||||\n");
		differences(argv[2],argv[1],argv[3],option,&inode_list);
		delete_list(inode_list);
	}
	else if( argc >= 4){
		printf("Too many arguments.\nPlease give: {executable} {source directory 1} {source directory 2} \n\t or {executable} {source directory 1} {source directory 2} {destination directory} for merge\n");
		return;
	}
	
	
	return;
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////

