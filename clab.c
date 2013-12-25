/* lab2 Starter
   Functionality: 
     declares a char **array (a dynamically allocated array of pointers)
     mallocs the char **array in a function (passing it in via ***)
     passes the char **array to function for load and print
*/

/*
assume input file:

horse
cat
rat
oryx
bat
alligator
crocodile
pig
monkey
aardvark
*/

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#define  WORD_LENGTH  40
#define  INITIAL_ARRAY_MAX 50

int loadArray(char *inFileName, char ***array, int *count, int *capacity);
int insertWord(char **dictionary, int *wordCount, char word[]);
void printArray(char **array, int count);
void menu(int *count, char** outputFile, char*** array);
void wordsCount(int *count, char** outputFile, char*** array);
int quit(int* count, char** outputFile, char*** array);
void saveDictionary(int* count, char** outputFile, char*** array);
int searchWord(int* count, char** outputFile, char*** array, char* word);
void removeWord(int * count, char** outputFile, char*** array, char* word);
void doubleArray(char*** array, int* count, int* capacity);

int main(int argc, char* argv[])
{
    char **wordArray;  /* declare the dynamic array of strings */
                       /* the array is NOT allocated yet */
    int capacity = INITIAL_ARRAY_MAX;
    int wordCount = 0;

    if (argc != 3)
    {
        fprintf(stderr,"Usage: %s inputfile outputfile\n", argv[0]);
        return 1;
    }

    if (loadArray(argv[1], &wordArray, &wordCount, &capacity) != 0)
    {
        fprintf(stderr,"    loadArray failed! Program terminating...\n");
        return 1;  /* don't need to call exit - we're in main! */
    }

    printf("\n  Finished loading %d words.\n", wordCount);

/* HERE IS WHAT OUR ARRAY LOOKS LIKE NOW
    wordArray is pointing to a 50-element block of memory

    wordArray[0] is a char * -> "horse\0"
             [1] is a char * -> "cat\0"
             [2] is a char * -> "rat\0"
             [3] is a char * -> "oryx\0"
             ...      char * -> "bat\0"
             ...      char * -> "alligator\0"
             ...      char * -> "crocodile\0"
             ...      char * -> "pig\0"
             ...      char * -> "monkey\0"
             [9] is a char * -> "aardvark\0"
            [10] is a char * -> ????
             ...
             ...
             ...
            [49] is a char * -> ????

   REMEMBER the 50 char * pointers are stored consecutively in 1 block
   of memory (which is why wordArray[5], for example, is well-defined,
   but the strings are distributed "randomly" throughout the heap.
*/

    //printArray(wordArray, wordCount);/* comment this out if using a big file!*/
    menu(&wordCount,&argv[2],&wordArray);
    int i;
    for(i=0; i < wordCount; i++){
     free(wordArray[i]);
    }
    free(wordArray);   
    return 0;
}


int loadArray(char *inFileName, char ***array, int *count, int *capacity)
{
    FILE *inFile;
    char word[WORD_LENGTH];  /* this is the ONLY auto array we'll need */

    if ((inFile = fopen(inFileName, "r")) == NULL)
    {
	fprintf(stderr,"Error opening input file, %s\n", inFileName);
	return -1;
    }

    *array = (char **)malloc(*capacity * sizeof(char*));
    if (*array == NULL)
    {
	fprintf(stderr, "Malloc of array in loadArray failed!\n");
	return -1;
    }

    printf("Reading file %s (each . is 5000 words read)\n", inFileName);

    *count = 0;
    while (fscanf(inFile, "%s", word) == 1)
    {
	if (*count >= *capacity)
	{
/* call a function that will double the size of the array and copy its contents */
	  doubleArray(array, count, capacity);
	}

	if (insertWord(*array, count, word) != 0)
	{
	    fprintf(stderr,"    Insert returned an error!\n");
	    fclose(inFile);
	    return 1;
	}
	
	if (*count % 5000 == 0)
	{
	    printf(".");
	    fflush(stdout);  /* stdout is buffered, so have to force flush */
	}
    }

    fclose(inFile);

    return 0;
}

void doubleArray(char*** array, int* count, int* capacity){
  char** tempArray = (char**) malloc((*capacity)*(2)* sizeof(char*));
  int i;
  for(i=0; i < *count; i++){
    tempArray[i] = (*array)[i];
  }
  free(*array);
  *array = tempArray;
  *capacity = *capacity * 2;
}

void printArray(char **array, int count)
{
   int i=0;

   printf("\n");
   do  {
       printf("array[%d]: %s\n", i, array[i]);
   } while (++i<count);
}

int insertWord(char **array, int *count, char word[])
{
    char *wordPtr;

    wordPtr = (char *)malloc((strlen(word) + 1) * sizeof(char));
    if (wordPtr == NULL)
    {
	fprintf(stderr,"    Malloc of array[%d] failed!\n", *count);
	return -1;
    }
    /* Memory for this word has been allocated, so copy characters
       and insert into array */
    strcpy(wordPtr, word);
    array[*count] = wordPtr;
    char* temp = array[*count];
    int tempPos = *count;
    while(tempPos > 0 && strcasecmp(temp,array[tempPos-1]) < 0){
      char* temp2 = array[tempPos-1];
      array[tempPos-1] = array[tempPos];
      array[tempPos] = temp2;
      tempPos--;
      }
    (*count)++;

    return 0;
}

void removeWord(int * count, char** outputFile, char*** array, char* word){
  int index = searchWord(count, outputFile, array, word);
  if(index == -1){
    printf("\n%s NOT REMOVED (Not in array)\n",word);
  }
  else{
    free((*array)[index]);
    int i;
    for(i = index; i < *count - 1; i++){
      (*array)[i] = (*array)[i+1];
    }
    (*count)--;
    printf("%s REMOVED", word);
  }
  menu(count, outputFile, array);
}

void wordsCount(int *count, char** outputFile, char*** array){
  if(count != NULL){
    printf("%d", *count);
  }
  menu(count, outputFile, array);
}

void saveDictionary(int* count, char** outputFile, char*** array){
  FILE* opf;
  opf = fopen(*outputFile, "w");
  if(opf == NULL){
    printf("Failed to open output file %s\n",*outputFile);
  }
  else{
    int i;
    for(i = 0; i < *count; i++){
      fprintf(opf,"%s\n",(*array)[i]);
    }
  }
  fclose(opf);
}

int quit(int* count, char** outputFile, char*** array){
  printf("Program is terminating...\nDictionary will be sent to %s\n",*outputFile);
  saveDictionary(count, outputFile, array);
  return 0;
}

int searchWord(int* count, char** outputFile, char*** array, char* word){
  int found = -1;
  int first = 0;
  int last = *count - 1;
  int middle = last/2;
  while(first <= last){
    int i;
    if(strcasecmp(word,(*array)[middle]) < 0){
      for(i = first; i < middle; i++){
	if(strcasecmp(word,(*array)[i]) == 0){
	  found = i;
	  return found;
	}
      }
      return found;
    }
    else if(strcasecmp(word,(*array)[middle]) > 0){
      first = middle + 1;
      middle = (first + last)/2;
    }
    else{
      found = middle;
      return found;
    }
  }
  return found;
}

void menu(int* count, char** outputFile, char*** array){
  char input;
  char word[100];
  printf("\nChoose: 'S'earch, 'I'nsert, 'R'emove, 'C'ount, 'Q'uit: ");
  scanf(" %c", &input); 
  switch(input){
  case 'Q':
  case 'q':
    quit(count, outputFile, array);
    break;
  case 'C':
  case 'c':
    wordsCount(count, outputFile, array);
    break;
  case 'S':
  case 's':
    printf("\nSearch for: ");
    scanf(" %s", word);
    if(searchWord(count, outputFile, array, word) >= 0){
      printf("\n%s FOUND\n", word);
    }
    else{
      printf("\n%s NOT FOUND\n", word);
    }
    menu(count, outputFile, array);
    break;
  case 'I':
  case 'i':
    printf("\nEnter a word to insert: ");
    scanf(" %s", word);
    if(searchWord(count,outputFile,array,word) >= 0){
      printf("\n%s IGNORED (Duplicate)\n", word);
    }
    else{
      insertWord(*array, count, word);
      printf("\n%s INSERTED\n", word);
    }
    menu(count, outputFile, array);
    break;
  case 'R':
  case 'r':
    printf("\nEnter word to remove: ");
    scanf(" %s", word);
    removeWord(count, outputFile, array, word);
    break;
  default:
    printf("Invalid Input\n");
    menu(count, outputFile, array);
  }
}


