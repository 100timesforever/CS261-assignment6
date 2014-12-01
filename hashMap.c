#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <assert.h>
#include "hashMap.h"

struct hashLink {
   KeyType key; /*the key is what you use to look up a hashLink*/
   ValueType value; /*the value stored with the hashLink, a pointer to int in the case of concordance*/
   struct hashLink * next; /*notice how these are like linked list nodes*/
};
typedef struct hashLink hashLink;

struct hashMap {
    hashLink ** table; /*array of pointers to hashLinks*/
    int tableSize; /*number of buckets in the table*/
    int count; /*number of hashLinks in the table*/
};
typedef struct hashMap hashMap;

/*the first hashing function you can use*/
int stringHash1(char * str)
{
	int i;
	int r = 0;
	for (i = 0; str[i] != '\0'; i++)
		r += str[i];
	return r;
}

/*the second hashing function you can use*/
int stringHash2(char * str)
{
	int i;
	int r = 0;
	for (i = 0; str[i] != '\0'; i++)
		r += (i+1) * str[i]; /*the difference between stringHash1 and stringHash2 is on this line*/
	return r;
}

/* initialize the supplied hashMap struct*/
void _initMap (struct hashMap * ht, int tableSize)
{
	int index;
	if(ht == NULL)
		return;
	ht->table = (hashLink**)malloc(sizeof(hashLink*) * tableSize);
	ht->tableSize = tableSize;
	ht->count = 0;
	for(index = 0; index < tableSize; index++)
		ht->table[index] = NULL;
}

/* allocate memory and initialize a hash map*/
struct hashMap *createMap(int tableSize) {
	assert(tableSize > 0);
	hashMap *ht;
	ht = malloc(sizeof(hashMap));
	assert(ht != 0);
	_initMap(ht, tableSize);
	return ht;
}

/*
 Free all memory used by the buckets.
 */
void _freeLinks (struct hashLink **table, int tableSize)
{
	/*write this*/
	//FIXME
	int i;
	struct hashLink *linkIter;
	struct hashLink *tmp;

	for(i = 0; i < tableSize; i++){
		//for each index, grab the link pointed to by the initial bucket
		linkIter = table[i];
		
		//go through each bucket, untill the linked list of collisions is empty,
		//freeing memory along the way
		while(linkIter != NULL){
			tmp = linkIter->next;	//get the next links address
			free(linkIter);			//free the link itself
			linkIter = tmp;			//advance to the next link in the chain of collisions
		}
	}
}

/* Deallocate buckets, table, and hashTable structure itself.*/
void deleteMap(hashMap *ht) {


	assert(ht!= 0);
	/* Free all memory used by the buckets */
	_freeLinks(ht->table, ht->tableSize);
    /* Free the array of buckets */
    free(ht->table);
	/* free the hashMap struct */
	free(ht);
}

/*
Resizes the hash table to be the size newTableSize
*/
void _setTableSize(struct hashMap * ht, int newTableSize)
{
	/*write this*/
	//FIXME
	int i;
	struct hashLink *iterator;

#ifdef DEBUG
	printf("\n!!!RESIZING!!!\n\n");
#endif
	//
	//make a new, empty table, of size newTableSize
	struct hashMap *new = createMap(newTableSize);

	
	//for each elememnt in the old table, copy the value
	//and hash it into the new table
	for(i = 0; i < ht->tableSize; i++){
		if(ht->table[i] != NULL){
			iterator = ht->table[i];
			while(iterator != NULL){
				insertMap(new, iterator->key, iterator->value);
			    iterator = iterator->next;
			}
		}
	}	

	
	//delete the old table		
	//deleteMap(ht);
	_freeLinks(ht->table, ht->tableSize);
	free(ht->table);
	//set the pointer for ht to point to the new table
	ht->tableSize = newTableSize;
	ht->table = new->table; 
#ifdef DEBUG
	printf("\n!!!END RESIZING!!!\n\n");
#endif
}

/*
 insert the following values into a hashLink, you must create this hashLink but
 only after you confirm that this key does not already exist in the table. For example, you
 cannot have two hashLinks for the word "taco".

 if a hashLink already exists in the table for the key provided you should
 replace the value for that key.  As the developer, you DO NOT FREE UP THE MEMORY FOR THE VALUE.
 We have to leave that up to the user of the hashMap to take care of since they may or may not
 have allocated the space with malloc.


 Also, you must monitor the load factor and resize when the load factor is greater than
 or equal LOAD_FACTOR_THRESHOLD (defined in hashMap.h).
 */
void insertMap (struct hashMap * ht, KeyType k, ValueType v)
{
	/*write this*/
	//FIXME
	int hashIndex;
	//Monitor the load factor here

	//get the index, based on the different hash functions.
	if(HASHING_FUNCTION == 1){
		hashIndex = stringHash1(k) % ht->tableSize;
	}
	else if(HASHING_FUNCTION == 2){
		hashIndex = stringHash2(k) % ht->tableSize;
	}

	//starting off with the case were the bucket is not yet empty
	if(ht->table[hashIndex] != NULL) {
		struct hashLink * itr = ht->table[hashIndex];
		while(strcmp(itr->key, k) != 0 && itr->next != NULL) {
			itr = itr->next;
		}
		if(itr->next == NULL && strcmp(itr->key, k) != 0) {
			struct hashLink * new = malloc(sizeof(struct hashLink));
			new->next = NULL;
			new->key = k;
			new->value = (int *)v;
			itr->next = new;
			ht->count++;
		}
		else {
			itr->value = v;
		}
	}
	//The else handles the case where the bucket is empty
	else {
		struct hashLink * new = malloc(sizeof(struct hashLink));
		new->next = NULL;
		new->value = (int *)v;
		new->key = k;
#ifdef DEBUG
		printf("___INSIDE insertMap___\n");
		printf("V, which was passed in: %d\n", v);
		printf("V deref: %d\n", *(int *)v);
		printf("deref new->value: %d\n", *(int *)new->value);
		printf("new->key: %s\n", new->key);
#endif
		ht->table[hashIndex] = new;
		ht->count++;
	}
#ifdef DEBUG
	printf("LOAD FACTOR: %f\n", tableLoad(ht) );
#endif
	if(tableLoad(ht) >= LOAD_FACTOR_THRESHOLD) {
		_setTableSize(ht, (ht->tableSize)*2);
	}
}

/*
 this returns the value (which is void*) stored in a hashLink specified by the key k.

 if the user supplies the key "taco" you should find taco in the hashTable, then
 return the value member of the hashLink that represents taco.

 if the supplied key is not in the hashtable return NULL.
 */
ValueType atMap (struct hashMap * ht, KeyType k)
{
	/*write this*/
	//FIXME
	
	//take in the key, and generate the hashed index from it
	int hashIndex;
	if(HASHING_FUNCTION == 1){
		hashIndex = stringHash1(k) % ht->tableSize;
	}
	else if(HASHING_FUNCTION == 2){
		hashIndex = stringHash2(k) % ht->tableSize;
	}
#ifdef DEBUG
	printf("==> Hash index for key '%s': %d\n", k, hashIndex);
#endif 
	

	//access the map at that index, and return the value
	if(ht->table[hashIndex] != NULL) {
		struct hashLink * itr = ht->table[hashIndex];
		while(1) {
			//if we hit the end of the bucket list
			if (itr->next == NULL) {
				break;
			}
			// if we find a match for that key, return the value
			else if (strcmp(itr->key, k) == 0 ){
				return itr->value;
			}
			//else, keep iterating
			else{
				itr = itr->next;
			}
		}
//		if(itr->next == NULL && strcmp(itr->key, k) != 0) {
#ifdef DEBUG
//			printf("At map returns NULL case 2\n");
#endif
//			return NULL;
//		}

		return itr->value;
	}
#ifdef DEBUG
	printf("At map returns NULL case 1\n");
#endif
	return NULL;
}

/*
 a simple yes/no if the key is in the hashtable.
 0 is no, all other values are yes.
 */
int containsKey (struct hashMap * ht, KeyType k)
{
	/*write this*/
	//FIXME
	int hashIndex;
	struct hashLink * itr;
	//take in the key, and generate the hashed index from it
	if(HASHING_FUNCTION == 1){
		hashIndex = stringHash1(k) % ht->tableSize;
	}
	else if(HASHING_FUNCTION == 2){
		hashIndex = stringHash2(k) % ht->tableSize;
	}
	if(ht->table[hashIndex] == NULL) { return 0;}
	else{
		itr = ht->table[hashIndex];
		while(itr != NULL) {
#ifdef DEBUG
			printf("CONTAINS KEY\n");
			printf("itr: %p\n", itr);
			printf("itr->key: %s\n", itr->key);
#endif
			if (strcmp(itr->key, k) == 0 ) {
				return 1;
			}
			itr = itr->next;
		}
	return 0;
	}
}

/*
 find the hashlink for the supplied key and remove it, also freeing the memory
 for that hashlink. it is not an error to be unable to find the hashlink, if it
 cannot be found do nothing (or print a message) but do not use an assert which
 will end your program.
 */
void removeKey (struct hashMap * ht, KeyType k)
{
	/*write this*/
	//FIXME
	//[ , , , , , , , * , * , * ]
	//                |
	//                L
	//                |
	//                L
	//                |
	//                N
	
	int hashIndex;
	struct hashLink *itr;
	//take in the key, and generate the hashed index from it
	if(HASHING_FUNCTION == 1){
		hashIndex = stringHash1(k) % ht->tableSize;
	}
	else if(HASHING_FUNCTION == 2){
		hashIndex = stringHash2(k) % ht->tableSize;
	}

	if(ht->table[hashIndex] != NULL) {
		itr = ht->table[hashIndex];
		//seperate case for if first value is a match
		if(strcmp(itr->key, k) == 0){
			struct hashLink * del = itr;
			ht->table[hashIndex] = del->next;
			free(del);
			ht->count--;
			return;
		}
		//average case where first value is not a match
		//allows for keeping track of parent
		while(1){
			//we create seperate checks, not simultaneous checks in the while loop
			//because if the first one is true, then the second one will segfault
			if (itr->next == NULL){
				break;
			}
			else if (strcmp(itr->next->key, k) == 0) {
				break;
			}
			else{
				itr = itr->next;
			}
		}
		if(itr->next == NULL) { return;}
		else{
			struct hashLink * del = itr->next;
			itr->next = del->next;
			free(del);
			ht->count--;
		}
	}
}

/*
 returns the number of hashLinks in the table
 */
int size (struct hashMap *ht)
{
	/*write this*/
	//FIXME
	return(ht->count);

}

/*
 returns the number of buckets in the table
 */
int capacity(struct hashMap *ht)
{
	/*write this*/
	//FIXME
	return(ht->tableSize);
}

/*
 returns the number of empty buckets in the table, these are buckets which have
 no hashlinks hanging off of them.
 */
int emptyBuckets(struct hashMap *ht)
{
	/*write this*/
	//FIXME
	int empty = 0;
	int i;
	for(i=0; i<ht->tableSize; i++) {
		if(ht->table[i] == NULL)
			empty++;
	}
	return empty;
}

/*
 returns the ratio of: (number of hashlinks) / (number of buckets)

 this value can range anywhere from zero (an empty table) to more then 1, which
 would mean that there are more hashlinks then buckets (but remember hashlinks
 are like linked list nodes so they can hang from each other)
 */
float tableLoad(struct hashMap *ht)
{
	/*write this*/
	//FIXME
	return((float)ht->count/ht->tableSize);
}

/* print the hashMap */
 void printMap (struct hashMap * ht, keyPrinter kp, valPrinter vp)
{
	int i;
	struct hashLink *temp;
	for(i = 0;i < capacity(ht); i++){
		temp = ht->table[i];
		if(temp != 0) {
			printf("\nBucket Index %d -> ", i);
		}
		while(temp != 0){
                        printf("Key:");
                        (*kp)(temp->key);
                        printf("| Value: ");
                        (*vp)(temp->value);
			printf(" -> ");
			temp=temp->next;
		}
	}
}

/* print the keys/values ..in linear form, no buckets */
 void printKeyValues (struct hashMap * ht, keyPrinter kp, valPrinter vp)
{
	int i;
	struct hashLink *temp;
	for(i = 0;i < capacity(ht); i++){
		temp = ht->table[i];
		while(temp != 0){
                        (*kp)(temp->key);
			printf(":");
                        (*vp)(temp->value);
			printf("\n");
			temp=temp->next;
		}
	}
}
