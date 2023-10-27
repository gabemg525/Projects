
/*********************************************
python code for project naive bayes classifier
in CSE 107 in 2022 winter, UC Santa Cruz
Student Name: ____
UW Email    : ____@ucsc.edu
=============================================================
You may define helper functions, but DO NOT MODIFY
the parameters or names of the provided functions.
=============================================================
*********************************************/

// If you are not familiar with C, please try C++ and python version
// of this project. Python and C++ version have more hints and skeleton codes
// You are more encouraged to finish this project in Python or C++
// You MAY and SHOULD change the functions and their parameters if you 
// plan to use C for this project. You may also include your favourite library
// for this project but provide instrutctions on how to compiling it and 
// the source/library of your dependency

#include<stdio.h>
#include<string.h>

#define HAM 0
#define SPAM 1

struct Map {
    char* key;
    long value;
}

void load_data() {
    // train_hams train_spams test_hams test_spams
    // are arrays of strings that stores
    // the data set. Each element in the vector is a 
    // filename. The file should contains an email
    // that is either a spam or a ham

}

void word_set() {
    //This function reads in a file and returns a set of all 
    //the words. It ignores the subject line.

    //For example, if the email had the following content:
    //Subject: Get rid of your student loans
    //Hi there,
    //If you work for us, we will give you money
    //to repay your student loans. You will be
    //debt free!
    //FakePerson_22393

    //This function would return to you the set:
    //{'', 'work', 'give', 'money', 'rid', 'your', 'there,',
    //     'for', 'Get', 'to', 'Hi', 'you', 'be', 'we', 'student',
    //     'debt', 'loans', 'loans.', 'of', 'us,', 'will', 'repay',
    //     'FakePerson_22393', 'free!', 'You', 'If'}
    // You could use your favourite way to implement a set of strings in C
}


void fit() {
    // At the end of this function, the following should be true:
    // 1. num_train_hams is set to the number of ham emails given.
    // 2. num_train_spams is set to the number of spam emails given.
    // 3. word_counts_spam is a DICTIONARY where word_counts_spam[word]
    // is the number of spam emails which contained this word. 
    // 4. word_counts_ham is a DICTIONARY where word_counts_ham[word]
    // is the number of ham emails which contained this word. 

    // Hint(s):
    // 1. You may want to use the word_set function provided.
    // 2. You should not worry about Laplace smoothing or anything here:
    // simply make sure your counts are correct from the data you're given.

}

int predit() {
    // :param filename: The filename of an email to classify.
    // :return: The prediction of our Naive Bayes classifier. This
    // should either return HAM_LABEL or SPAM_LABEL.

    // Guidelines:
    // 1. Make sure to use the log-trick to avoid underflow.
    // 2. Make sure for Laplace smoothing you use +1 in numerator 
    // and +2 in denominator. This is even for words you haven't seen before
    // in the training data, which would otherwise have 0 probability.
    // 3. Remember, the goal is not 100% accuracy - a correct implementation will get
    // exactly a certain accuracy.
    // 4. In case of a tie, predict 'self.HAM_LABEL'.
    // 5. Do NOT apply Laplace smoothing to P(ham) and P(spam).

    // Hint(s):
    // 1. You may want to use the word_set function provided.
    // 2. For a dictionary d, d[key] returns the value if the key exists.
    // But if it doesn't, you get an error.
    // 3. You'll want to use the values you set during the fit function.
    // Access those variables with a 'self' prefix, like num_train_hams.
    return HAM;
}

double accuracy(char *hams[], char *spams[], long num_train_hams, long num_train_spams, Map word_counts_spam[], Map word_counts_ham[]) {
    // using the predict function above to predict whether the file is a HAM or a SPAM and 
    // caculate the accuracy rate of the naive bayes classifier
    // see more in the c++ code.
}

int main() {

    const char *train_hams[3400];
    const char *train_spams[1500];
    const char *test_hams[1000];
    const char *test_spams[1000];

    long num_train_hams;
    long num_train_spams;

    Map word_counts_spam[30000];
    Map word_counts_ham[30000];

    load_data(train_hams, train_spams, test_hams, test_spams);
    fit(train_hams, train_spams, num_train_hams, num_train_spams, word_counts_ham, word_counts_spam);

    printf("Train Accuracy: %f\n", accuracy(train_hams, train_spams, num_train_hams, num_train_spams, word_counts_ham, word_counts_spam));
    printf("Test Accuracy: %f\n", accuracy(test_hams, test_spams, num_train_hams, num_train_spams, word_counts_ham, word_counts_spam));

}