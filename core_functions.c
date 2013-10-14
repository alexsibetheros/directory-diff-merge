#include "core_functions.h"




void differences(char* dirA, char* dirB,char* dirC, int option, struct node** inode_list)
{
	/*The core function of the program, prints differences and computes merge*/
	/*IMPORTANT!: the second time differences is run, dirA,dirB are reversed, this allow the program to print differcens of B to A and also to merge all of dirB files into dirC*/
	/*The program may get a bit confusing when running the second time, since dirA is in essence (source2) and dirB is (source1)*/
	printf("*-----------------In %s :-------------*\n",dirA);	
	struct node *start = NULL ;	
	char* added_str=NULL;
	char* added_str_reverse=NULL;
	struct stat mybufA ;
	struct stat mybufB ;
	int result;
	DIR *dp;
	struct dirent * dir;
	dp= opendir ( dirA ); 		if ( dp== NULL ) {perror(" opendir "); return;}
	
	while (( dir = readdir (dp)) != NULL ) { /*Open directory and work with children in layer 1 only*/
		if (dir -> d_ino == 0 ) continue ;
		
		if( strcmp(dir->d_name, ".")!=0 && strcmp(dir->d_name, "..")!=0) { /*Keep only the import children*/
			added_str=add_dir(dir->d_name,dirB);
			if( Exist(added_str)==1 ){ /*If path exists in dirB aswell*/				
				if (lstat( added_str, &mybufB)< 0){perror(added_str); exit(0);}
				if( compare(dirA,dirB,dir->d_name) ==0){ /*Returns 0 if 100% same, 1 if differend*/	
					if (S_ISDIR(mybufB.st_mode) ){ /*If the two directories(which are 100% identical), then add to fringe to check later*/
						start = insert( start,dir->d_name,0 );
						if(option==1) /*Because folder are the same, add to dirC*/
							insert_simple_folder(dirC, dir->d_name); 	
					}
					if(option==1){
						if (S_ISLNK(mybufB.st_mode) ) /*If links are exactly the same, add to dirC*/
							insert_link(dirA, dirC, dir->d_name);
						if (S_ISREG(mybufB.st_mode) )/*If files are the same(path/name and size) then add the newest*/
							choose_newest_file(dirA,dirB,dirC, dir->d_name,inode_list);										
					}
				}
				else{ 	/* Directories with same path/name are always the same, here we have links and files*/
						printf("	%s\n",dir->d_name); 					
						added_str_reverse=add_dir(dir->d_name,dirA); 
						if (lstat( added_str_reverse, &mybufA)< 0){perror(added_str_reverse); exit(0);}
						free(added_str_reverse);
						if (S_ISLNK(mybufB.st_mode) && S_ISLNK(mybufA.st_mode) ){ /*If both links(same path/name) but have diff. content, add newest*/
							if(option==1){	choose_newest_link(dirA, dirB,dirC,  dir->d_name);}
						}
						else if (S_ISREG(mybufB.st_mode) && S_ISREG(mybufA.st_mode) ){ /*Files with same path/name, different size. Add newest*/
							if(option==1){choose_newest_file(dirA,dirB,dirC, dir->d_name,inode_list);}
						}
						else{  /*Any other combination(file-folder,file-link,folder-link...) choose which is newer and add*/
							choose_newest_general(dirA,dirB,dirC,dir->d_name,inode_list,option);							
						}	
						
				} 
			}
			else{ /*If child doesn't exist in B*/				
				added_str_reverse=add_dir(dir->d_name,dirA); 
				if (lstat( added_str_reverse, &mybufA)< 0){perror(added_str_reverse); exit(0);}
				free(added_str_reverse);
				if (S_ISDIR(mybufA.st_mode) ){ /*If dir then all its children(all theirs respectively) are unique*/ 
					printf("	%s\n",dir->d_name);	 
					print_folder_sub(dir->d_name,dirA); /*Print everything under this folder*/
					if(option==1) /*Insert everything inside to dirC*/
							insert_folder_entire(dirA, dirB, dirC, dir->d_name,inode_list);
				}
				else{ /*Otherwise either a link or a file. Add if possible*/
					if(option==1){
						if (S_ISLNK(mybufA.st_mode) ){
							insert_link(dirA, dirC, dir->d_name);
						}
						if (S_ISREG(mybufA.st_mode) ){						
							add_file(dirA,  dirC, dir->d_name,inode_list );
						}
					}
					printf("	%s\n",dir->d_name);		
				}										
				
			}
			free(added_str);				
		}
	
	}
	
	closedir(dp);
	char* popped=NULL;
	char* tempstr=NULL;
	char* extra_str=NULL;
	/*Loop that works with all the directories above that were common between the two sources*/
	while(start!=NULL){	
		popped=pop_list(&start); /*Retrieve a dir from the list*/
		added_str=add_dir(popped,dirA);
		dp= opendir ( added_str );
		if ( dp== NULL ) {perror(" opendir "); return;}	
		
		while (( dir = readdir (dp)) != NULL ) { /*Loop for children of current dir*/
			if (dir -> d_ino == 0 ) continue ;
			if( strcmp(dir->d_name, ".")!=0 && strcmp(dir->d_name, "..")!=0) {
				free(added_str);
				added_str=add_dir(dir->d_name,popped);
				extra_str=add_dir(added_str,dirB);
				if( Exist(extra_str)==1 ){ /*Check to see if current child of dir(which dir is in dirA) exists in dirB*/
					
					if (lstat( extra_str, &mybufB)< 0){perror(extra_str); exit(0);}
					free(extra_str);
					if( compare(dirA,dirB,added_str) ==0){ /*Compare the two, which have the same exact path/name*/
						if (S_ISDIR(mybufB.st_mode) ){ /*If the same and one is folder(The other is automatically from the above)*/ 
							start = insert( start,added_str,0 ); /*Add to fringe to check later*/
							if(option==1) /*If option enabled, add folder to dirC*/
								insert_simple_folder(dirC, added_str); 	
						}
						if(option==1){
							if (S_ISLNK(mybufB.st_mode) ) /*If links the same add*/
								insert_link(dirA, dirC, added_str);
							if (S_ISREG(mybufB.st_mode) ) /*If files the same, add the newest*/
								choose_newest_file(dirA,dirB,dirC, added_str,inode_list);										
						}
					}
					else{ /*Exists in dirB but not the same*/
						printf("	%s\n",added_str);					
						if(option==1){ 
							added_str_reverse=add_dir(added_str,dirA); 
							if (lstat( added_str_reverse, &mybufA)< 0){perror(added_str_reverse); exit(0);}
							free(added_str_reverse);
							if (S_ISLNK(mybufB.st_mode) && S_ISLNK(mybufA.st_mode) ) /*If both links, add the newest*/
								choose_newest_link(dirA, dirB,dirC,  added_str);
							else if (S_ISREG(mybufB.st_mode) && S_ISREG(mybufA.st_mode) ) /*If both files, add the newest*/
								choose_newest_file(dirA,dirB,dirC, added_str,inode_list);
							else /*All other cased, find the newest and add*/
								choose_newest_general(dirA,dirB,dirC,dir->d_name,inode_list,option);
						}	
					}
				}
				else{ /*File does not exist in dirB*/;
					tempstr=add_dir(dir->d_name,popped);		
					free(extra_str);
					extra_str=add_dir(tempstr,dirA);		
					if (lstat( extra_str, &mybufA)< 0){perror(extra_str); exit(0);}
					free(extra_str);
					if (S_ISDIR(mybufA.st_mode) ){ 
						printf("	%s\n",tempstr);	 
						print_folder_sub(tempstr,dirA);/*Print folder and everything underneath*/
						if(option==1) /*Add folder(and everything inside) to dirC*/
							insert_folder_entire(dirA, dirB,dirC, tempstr,inode_list); 
					}
					else{ /*If not folder, print and add to dirC if enabled*/
						printf("	%s\n",tempstr);
						if(option==1){
							if (S_ISLNK(mybufA.st_mode) ){
								insert_link(dirA, dirC, tempstr);
							}
							if (S_ISREG(mybufA.st_mode) ){						
								add_file(dirA,  dirC, tempstr,inode_list );
							}
						}					
					}										
				free(tempstr);
				}	
			}
		}
		closedir(dp);
		free(popped);
		free(added_str);	
	}		
	printf("*-----------------Out %s :------------*\n",dirA);
}
	


////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////
int compare(char* dirA,char* dirB, char* path) //0 ean idia, 1 diaforetika
{
	/*Takes a path and the two directories and tests to see if exactly the same*/
	/*The path/name exists in both directories and is exactly the same for both*/
	char *apoluto_pathA=NULL,*apoluto_pathB=NULL;
	apoluto_pathA=add_dir(path, dirA);
	apoluto_pathB=add_dir(path, dirB);
	struct stat mybufA ;
	struct stat mybufB ;
	if (lstat ( apoluto_pathA, & mybufA ) < 0) {perror (apoluto_pathA); exit(0) ;}
	if (lstat ( apoluto_pathB, & mybufB ) < 0) {perror (apoluto_pathB); exit(0) ;}	
	char link_valA [ BUFFERSIZE ];
	char link_valB [ BUFFERSIZE ];	
	int readlinksize;
	char* path_no_link=NULL; 
	char* final_path=NULL;
	char* finished=NULL,* finished2=NULL;
	char* test=NULL,*test2=NULL;
	char real[BUFFERSIZE],real_dir[BUFFERSIZE];
	char real2[BUFFERSIZE],real_dir2[BUFFERSIZE];
	if (S_ISDIR(mybufA.st_mode) && S_ISDIR(mybufB.st_mode))/*If both paths directories, automatically the same*/
	{ 
		free(apoluto_pathA);
		free(apoluto_pathB);
		return 0;}
	else if (S_ISREG(mybufA.st_mode) && S_ISREG(mybufB.st_mode)){  /*If both paths/names are files, compare sizes*/
		free(apoluto_pathA);
		free(apoluto_pathB);
		if( (mybufA).st_size!=(mybufB).st_size ){
			return 1;
		}
		return 0;
	}
	else if (S_ISLNK(mybufA.st_mode) && S_ISLNK(mybufB.st_mode)) /*If both symlinks(with same path/name)*/
	{
		/*Read content from both*/
	 	readlinksize=readlink ( apoluto_pathA , link_valA , BUFFERSIZE );
		if (  readlinksize==-1){perror ( " Failed to read value of symbolic link  " ) ;}
		else{ 
			link_valA[readlinksize]='\0';
			//printf ( "Value of symbolic link: CompareA: : %s \n" , link_valA ) ;
		}		
		readlinksize=readlink ( apoluto_pathB , link_valB , BUFFERSIZE );
	 	if (  readlinksize==-1){perror ( " Failed to read value of symbolic link  " ) ;}
		else{
			link_valB[readlinksize]='\0';
			///printf ( "Value of symbolic link: CompareB: : %s \n " , link_valB ) ;
		}
		
		free(apoluto_pathA);
		free(apoluto_pathB);
		/*Now we sanitize the content*/
		test=add_dir(link_valA, dirA); /*Add directory to content*/
		realpath(test, real); /*Get realpath of the above string*/
		realpath(dirA, real_dir); /*Get realpath of the directory*/
		finished=real_cut(real,real_dir);
		/*Remove from the large string(with path inside) the portion that contains the real path of the directory*/
		/*In this manner, the final link is sanitized= no ./ ../ or other funny buisness*/
		free(test);
		/*Likewise for other link*/
		test2=add_dir(link_valB, dirB);
		realpath(test2, real2);
		realpath(dirB, real_dir2);
		finished2=real_cut(real2,real_dir2);
		free(test2);		
		
		if( strcmp(finished, finished2)==0){
			/*If both sanitized links are the same*/
			free(finished);
			free(finished2);
			path_no_link=eat_link(path);
			final_path=add_path(path_no_link,link_valA);
			free(path_no_link);
			if( compare(dirA,dirB, final_path)==0){	
/*If links have the same content(after sanitizing), then call compare again with the content of links to see if they link to something that is the same*/
				free(final_path);
				return 0;
			}
			else{	
				free(final_path);
				return 1;
			}	
		}
		else{ /*Not the same*/
			return 1;
		}
	}/*The following are the other possible combinations which are automatically different */
	else if( (S_ISDIR(mybufA.st_mode) && !S_ISDIR(mybufB.st_mode)) || (!S_ISDIR(mybufA.st_mode) && S_ISDIR(mybufB.st_mode)) )
	{
		free(apoluto_pathA);
		free(apoluto_pathB);
		return 1;}
	else if( (S_ISREG(mybufA.st_mode) && !S_ISREG(mybufB.st_mode)) || (!S_ISREG(mybufA.st_mode) && S_ISREG(mybufB.st_mode)) )
	{
		free(apoluto_pathA);
		free(apoluto_pathB);
		return 1;}
	else if( (S_ISLNK(mybufA.st_mode) && !S_ISLNK(mybufB.st_mode)) || (!S_ISLNK(mybufA.st_mode) && S_ISLNK(mybufB.st_mode)) )
	{
		free(apoluto_pathA);
		free(apoluto_pathB);
	return 1;}
	
	
	return 1;
	
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////


















