#include "functions.h"



////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////	
void print_folder_sub(char* folder,char* dirA)
{
	
	struct stat mybuf ;
	char* added_str=NULL;
	char* super_added=NULL;
	char* fixed=NULL;
	DIR *dp;
	struct dirent * dir;
	fixed=add_dir(folder,dirA); 
	dp= opendir ( fixed);
	if ( dp== NULL ) {perror(" opendir "); return;}
	free(fixed);
	while (( dir = readdir (dp)) != NULL ) {/*Prints every child of directory, if child a directory, call recursevly until no folders remaining*/
		if (dir -> d_ino == 0 ) continue ;
		if( strcmp(dir->d_name, ".")!=0 && strcmp(dir->d_name, "..")!=0) {
			added_str=add_dir(dir->d_name,folder);
			super_added=add_dir(added_str,dirA);
			printf("	%s\n",added_str); 
			if (lstat ( super_added, & mybuf ) < 0) {perror (super_added); exit(0) ;}
			free(super_added);
			if (S_ISDIR(mybuf.st_mode) ){ 
				print_folder_sub(added_str,dirA);/*Recursion*/
			}
			free(added_str);
		}
	}
	closedir(dp);
	
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////	
int Exist(char* path){ /*1 if it doesnt exist, 0 if it does*/
    	if (open(path, O_RDONLY) != -1)
            return 1;
    	else{   
            return 0;
	}
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////	
char* add_dir(char* path,char* dir) 
{
	/*Takes path, addes dir before it*/	
	char* temp=( char *) malloc ( (strlen(path)+strlen(dir)+3)*sizeof(char)) ;
	char temppath[strlen(path)+2];
	temppath[0]='/';
	temppath[1]='\0';
	strcat(temppath,path); /*My paths don't have / in front, this addes it, making everything work*/
	strcpy(temp,dir);
	strcat( temp, temppath);	
	return temp;
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////
char* add_path(char* path_no_link,char* link_valA)
{
	/*Returns a path(for a link) combining path+ link_val*/	
	char* temp=( char *) malloc ( (strlen(path_no_link)+strlen(link_valA))*sizeof(char)) ;
	strcpy(temp,path_no_link);
	strcat(temp,link_valA);
		
	return temp;
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////
char* eat_link(char* path)
{
	/*Function that cuts the symlink from the path(in order to call: add_path afterwards)*/	
	int i;
	char* temp=( char *) malloc ( strlen(path)*sizeof(char)) ;
	strcpy(temp,path);
	for(i=strlen(path);i>=0;i--){
		if( temp[i]=='/' ){
			temp[i+1]='\0';
			break;
		}	
	}
	if( i== -1){
		
		temp[0]='\0';
		return temp;
	}
		
	return temp;	
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////
void insert_link(char* dirA, char* dirC, char* path)
{
	/*Insert link into dirC if it doesn't exist*/
	char* added_str=add_dir(path,dirC);
	char* fixed=add_dir(path,dirA);
	int readlinksize;
	char link_valA [ BUFFERSIZE ];
	if( Exist(added_str)==0 ){	
	/*If link doesn't already exist, because of the swap of arguments of Differences*/	
		readlinksize=readlink ( fixed , link_valA , BUFFERSIZE );
		if (  readlinksize==-1){perror ( " Failed to read value of symbolic link  " ) ;}
		else{ 
			link_valA[readlinksize]='\0';
			if ( symlink(link_valA, added_str)== -1 ) {perror(" Symlink"); return;} 
		}	
		
	}
	free(added_str);
	free(fixed);
	return;
		

}
////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////
void choose_newest_link(char* dirA, char* dirB,char* dirC, char* path)
{
	/*Compare two links, whichever is the newest is inserted into dirC*/
	char* strA=add_dir(path,dirA);
	char* strB=add_dir(path,dirB);
	struct stat mybufA;
	struct stat mybufB;
	if (lstat ( strA, & mybufA ) < 0) {perror (strA); exit(0) ;}
	if (lstat ( strB, & mybufB ) < 0) {perror (strB); exit(0) ;}
	
	if( mybufA.st_mtime >= mybufB.st_mtime){
		
		insert_link( dirA, dirC, path);
	}
	else{ 
		insert_link(dirB, dirC, path);
	}
	free(strA);
	free(strB);
	return;
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////
int copyfile(char* source, char* destination)
{
	/*Copies file from source to destination*/
	/*Special thanks to code from IOMaterial.pdf*/
	int infile , outfile ;
	ssize_t nread ;
	char buffer[ BUFFERSIZE ];
	if ( (infile =open (source , O_RDONLY )) == -1 )
		return (-1);
	if ( (outfile =open (destination , O_WRONLY |O_CREAT |O_TRUNC , 0644 )) == -1){
		close (infile );
		return (-2);
	}
	while ( (nread =read (infile , buffer , BUFFERSIZE ) ) > 0 ){
		if ( write (outfile ,buffer ,nread ) < nread ){
			close (infile ); close ( outfile ); return (-3);
		}
	}
	close (infile ); close ( outfile );
	
	if ( nread == -1 ) return (-4);
		else return (0) ;


}
////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////
void choose_newest_file(char* dirA, char* dirB,char* dirC, char* path,struct node** inode_list)
{ 
	/*Compare two files, whichever is the newest is inserted into dirC*/
	char* strA=add_dir(path,dirA);
	char* strB=add_dir(path,dirB);
	struct stat mybufA;
	struct stat mybufB;
	if (lstat ( strA, & mybufA ) < 0) {perror (strA); exit(0) ;}
	if (lstat ( strB, & mybufB ) < 0) {perror (strB); exit(0) ;}
	
	if( mybufA.st_mtime >= mybufB.st_mtime){
		add_file(dirA,  dirC, path, inode_list );
	}
	else{ 
		add_file(dirB,  dirC, path, inode_list );
	}
	free(strA);
	free(strB);
	return;
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////
void add_file(char* dir, char* dirC, char* path, struct node** inode_list )
{
	/*Attemps to add file to dirC*/
	char* path_search=NULL;
	int result;
	struct stat mybuf;
	char* str=add_dir(path,dir);
	if (lstat ( str, & mybuf ) < 0) {perror (str); exit(0) ;}
	path_search=search_inode(*inode_list, mybuf.st_ino,&result); /*Searchs inode_list for any nodes with current inode number*/
	/*This is done for hardlink management, if an same inode exists, link to that inode and do not create new file*/
	char* str_C=add_dir(path_search,dirC);
	char* str_C_orig=add_dir(path,dirC);
	
	if( Exist(str_C_orig) ==0) /*If file doesn't already exist*/
	{
		if( result==1){ /*Result is set from search_inode, and is 1 if inode already exists, 0 otherwise*/
			
			if( link(str_C,str_C_orig )==-1){ printf("strC: %s and strCorg: %s\n",str_C,str_C_orig);
				perror("link problem:"); /*Creates link to file that exists(Anywhere in the dirC)*/ 
			}
		}
		else{
			/*No inode exists in list, create the file in dirC and add its inode to the list*/
			*inode_list = insert( *inode_list,path,mybuf.st_ino );
			copyfile(str,str_C_orig);		
		}
	}
	
	free(str);
	free(str_C);
	free(str_C_orig);
	return;						
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////
char* real_cut(char* real,char* real_dir)
{
	/*Takes two strings, returns the portion of real that doesn't contain real_dir */
	int sub= strlen(real)-strlen(real_dir);
	char* temp=malloc(sizeof(char)*(sub+1));
	strcpy ( temp, &real[strlen(real_dir)] );
	
	return temp;
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////
void insert_simple_folder(char* dirC,char* path)
{
	/*Adds a folder to dirC if it doesn't already exist*/
	char* added_str=add_dir(path,dirC);
	if( Exist(added_str)==0 ){ /*If folder doesn't already exist*/
		if ( mkdir(added_str, 00777)== -1 ) {perror(" mkdir"); return;}
	}
	free(added_str);
	return;


}
////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////
void insert_folder_entire(char* dirA, char* dirB, char* dirC, char* path,struct node** inode_list) 
{ 
	/*This is the case that an entire folder must be added to dirC(because it belongs in 1 of the 2 sources only)*/
	char* added_str=NULL,*super_added=NULL,* C_added_str=NULL,*C_added=NULL; 
	DIR *dp;
	struct dirent * dir;
	struct stat mybuf ;
	char* fixed=add_dir(path,dirA);
	insert_simple_folder(dirC, path); /*Add the specific folder mentioned only*/
	dp= opendir ( fixed);
	if ( dp== NULL ) {perror(" opendir "); return;}
	free(fixed);
	while (( dir = readdir (dp)) != NULL ) { /*Search through the specific folder's children*/
		if (dir -> d_ino == 0 ) continue ;
		if( strcmp(dir->d_name, ".")!=0 && strcmp(dir->d_name, "..")!=0) {
			added_str=add_dir(dir->d_name,path);
			super_added=add_dir(added_str,dirA); /*Path with dirA*/
			C_added_str=add_dir(added_str,dirC); /*Path with dirC*/
			if (lstat ( super_added, & mybuf ) < 0) {perror (super_added); exit(0) ;}
			
			if (S_ISDIR(mybuf.st_mode) ){ /*If child is folder, call same function again, until no new directories exist*/
				insert_folder_entire(dirA, dirB, dirC, added_str,inode_list);
			}
			else if (S_ISLNK(mybuf.st_mode) ){ /*If child is link, add it to dirC*/
				insert_link(dirA, dirC, added_str);
			}
			else if (S_ISREG(mybuf.st_mode) ){ /*If child is file, add it to dirC*/
				 add_file(dirA, dirC,  added_str, inode_list );
			}
			free(added_str);
			free(super_added);
			free(C_added_str);
		}
	}

	closedir(dp);

}
////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////
void choose_newest_general(char* dirA,char* dirB,char* dirC,char* path,struct node** inode_list,int option)
{
	/*Function that takes a path(that exists in both dir.) and decides which is newer*/
	char* a_str=NULL,*b_str=NULL,*c_str=NULL;
	a_str=add_dir(path,dirA);
	b_str=add_dir(path,dirB);
	struct stat mybufA;
	struct stat mybufB;
	if (lstat ( a_str, & mybufA ) < 0) {perror (a_str); exit(0) ;}
	if (lstat ( b_str, & mybufB ) < 0) {perror (b_str); exit(0) ;}
	if( S_ISDIR(mybufA.st_mode ) ) /*If path is an folder, print it out(for Differences)*/
		print_folder_sub(path,dirA);
	
	int result=largest(mybufA.st_mtime,mybufB.st_mtime); /*returns largest*/
	if( result==1){ /*Only do things if current is larger*/
			/*Note: because I call differences 2 times, with reversed arguments, we need only the winner to add its dir/files... */
		if( S_ISDIR(mybufA.st_mode ) ){ /*If path is a folder, add everything inside*/
			if(option==1){ insert_folder_entire(dirA,dirB,dirC, path,inode_list); } 
		}
		else if( S_ISLNK(mybufA.st_mode )){ /*If link, add it to dirC*/	
			if(option==1){insert_link(dirA, dirC,path);} 
		}
		else if( S_ISREG(mybufA.st_mode ) ){ /*Likewise, add to dirC*/
			if(option==1){	add_file(dirA, dirC, path, inode_list );}
		}
	}
	free(a_str);
	free(b_str);
	
	return;
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////
int largest(int a,int b)
{ 
	if( a>=b )
		return 1;
	else
		return 2;
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////


