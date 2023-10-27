/**********************************************
    C++ code for project Bloom filter in 
    CSE 107 in 2022 winter, UC Santa Cruz,
            for Prof. Chen Qian. 
**********************************************/
/* 
    Student name:
    UCSC email: ___@ucsc.edu
*/

#include <iostream>
#include <cstdio>
#include <vector>
#include <fstream>
#include <string>
#include <cassert>

class Bloomfilter{
    
public:
    Bloomfilter(){};
    explicit Bloomfilter(int k_ = 10, uint64_t m_ = 100000): k(k_), m(m_) {
        /**********************
        :param k: The number of hash functions (rows).
        :param m: The number of buckets (cols).
        Initializes the bloom filter to all zeros, as a
        boolean array where True = 1 and False = 0.
        ***********************/
        bitmap.clear();
        std::vector<bool> temp(m, false);
        bitmap.resize(k, temp);
    };

    void add(const void * x);
    bool contains(const void * x);
    uint64_t MurmurHash64A (const void * key, unsigned int seed, int len);
    uint64_t hash(const void * key, unsigned int i);
    
    ~Bloomfilter(){};

public:
    int k;
    uint32_t m;
    std::vector<std::vector<bool>> bitmap;

};


uint64_t Bloomfilter::MurmurHash64A (const void * key, unsigned int seed, int len = 32)
{
	const uint64_t m = 0xc6a4a7935bd1e995;
	const int r = 47;

	uint64_t h = seed ^ (len * m);

	const uint64_t * data = (const uint64_t *)key;
	const uint64_t * end = data + (len/8);

	while(data != end)
	{
		uint64_t k = *data++;

		k *= m; 
		k ^= k >> r; 
		k *= m; 
		
		h ^= k;
		h *= m; 
	}

	const unsigned char * data2 = (const unsigned char*)data;

	switch(len & 7)
	{
	case 7: h ^= uint64_t(data2[6]) << 48;
	case 6: h ^= uint64_t(data2[5]) << 40;
	case 5: h ^= uint64_t(data2[4]) << 32;
	case 4: h ^= uint64_t(data2[3]) << 24;
	case 3: h ^= uint64_t(data2[2]) << 16;
	case 2: h ^= uint64_t(data2[1]) << 8;
	case 1: h ^= uint64_t(data2[0]);
	        h *= m;
	};
 
	h ^= h >> r;
	h *= m;
	h ^= h >> r;

	return h;
} 
uint64_t Bloomfilter::hash(const void * key, unsigned int i){
    return Bloomfilter::MurmurHash64A(key, i) % m;
}

void Bloomfilter::add(const void * x){
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
bool Bloomfilter::contains(const void * x){
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
    
    Bloomfilter bf(10, 100000);
    
    std::cout << ("Adding malicious URLS to Bloom Filter") << std::endl;
    //Create our bloom filter of malicious URLs
    std::fstream f("data/mal_urls.txt");
    std::string mal_url;

    while(getline(f, mal_url)){
        bf.add(mal_url.c_str());
        assert(bf.contains(mal_url.c_str())); //After adding, should definitely be in
    }
    f.close();



    /****************************************
        Check contains on 10000 different URLs to see what percentage
        incorrectly are marked as being contained.
    *****************************************/
    std:: cout << "\n(a) Computing False Positive Rate (FPR) using Unseen test URLs" << std::endl;
    double fpr = 0.0; uint64_t total = 0;
    std::fstream g("data/test_urls.txt");
    std::string test_url;
    while(getline(g, test_url)){
        if(bf.contains(test_url.c_str())){
            fpr += 1;
        }
        total += 1;
    }
    g.close();
    fpr /= (total + 0.0);
    
    std::cout << "False positive rate is " << fpr << std::endl;

    std:: cout << "Please include your Solution for problem (b) in the submitted pdf file." << std::endl;
    return 0;
}

