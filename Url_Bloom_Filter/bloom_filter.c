/**********************************************
    C code for project Bloom filter in 
    CSE 107 in 2022 winter, UC Santa Cruz,
            for Prof. Chen Qian. 
**********************************************/
/* 
    Student name:
    UCSC email: ___@ucsc.edu
*/

#include <stdio.h>
#include <stdbool.h>
#include <assert.h>

unsigned int MurmurHash2 ( const void * key, unsigned int seed )
{
	// 'm' and 'r' are mixing constants generated offline.
	// They're not really 'magic', they just happen to work well.

    int len = 32;
	const unsigned int m = 0x5bd1e995;
	const int r = 24;

	// Initialize the hash to a 'random' value

	unsigned int h = seed ^ len;

	// Mix 4 bytes at a time into the hash

	const unsigned char * data = (const unsigned char *)key;

	while(len >= 4)
	{
		unsigned int k = *(unsigned int *)data;

		k *= m; 
		k ^= k >> r; 
		k *= m; 
		
		h *= m; 
		h ^= k;

		data += 4;
		len -= 4;
	}
	
	// Handle the last few bytes of the input array

	switch(len)
	{
	case 3: h ^= data[2] << 16;
	case 2: h ^= data[1] << 8;
	case 1: h ^= data[0];
	        h *= m;
	};

	// Do a few final mixes of the hash to ensure the last few
	// bytes are well-incorporated.

	h ^= h >> 13;
	h *= m;
	h ^= h >> 15;

	return h;
}

unsigned int hash(char *str, unsigned int i){
    return MurmurHash2((void *) str, i);
}

void add(bool bf, char *x){
    /**********************
        param x: The element to add to the bloom filter.
        In this function, we add x to our bloom filter.
        Hint(s):
        1. Read the pseudocode provided!
        2. You will want to use self.hash(...).
        3. Remember we initialized our bit array to be of type
        boolean, so 1 should be represented as True, and 0 as 
        False.
    ************************/

    /* TODO: Your code here */
}

bool contains(bool bf, char *x){
    /**************************
    :param x: The element to check whether or not it belongs
    to the bloom filter.
    :return: True or False; whether or not it is in the bloom filter.
    As described in the notes, this is not always accurate and may give
    false positives sometimes. That is, if this function returns False, 
    the element is definitely not in our structure, but if this function 
    returns True, the element may or may not be in our structure.
    Hint(s):
    1. Read the pseudocode provided!
    2. Again, remember we initialized our bit array to be of type
    boolean, so 1 should be represented as True, and 0 as 
    False.
    ******************************/

    /* TODO: your code here */
    return true;
}

int main(){

    bool bf[10][100000];

    printf("Adding malicious URLS to Bloom Filter\n");
    FILE *fp;
    char str[36];
    if ( (fp = fopen("data/mal_urls.txt", "rt")) == NULL ) {
        puts("Fail to open file!");
        return(0);
    }
    while( fgets(str, 36, fp) != NULL ) {
        add(&bf, str);
        //hash(str,1);
        assert(contains(&bf, str));
    }
    fclose(fp);


    printf("(a) Computing False Positive Rate (FPR) using Unseen test URLs\n");
    double fpr = 0.0; int total = 0;
    if ( (fp = fopen("data/test_urls.txt", "rt")) == NULL ) {
        puts("Fail to open file!");
        return(0);
    }
    while( fgets(str, 36, fp) != NULL ) {
        if(contains(bf, str)){
            fpr += 1;
        }
        total += 1;
    }
    fclose(fp);
    fpr /= (total + 0.0);

    printf("The False positive rate is %f\n", fpr);

    printf("p.s. Please include your Solution for problem (b) in the submitted pdf file. \n");

    return 0;
}
