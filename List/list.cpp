/***************************************************************************************
Header file : PostingList.h
Description : Implementation of methods of classes BucketNode and ResultsList,
              which are used to store and retrieve the results of a join query.
****************************************************************************************/

using namespace std;

#include "ResultsList.hpp"



#define CHECK_OR_EXIT(value)     \
{                           \
	if (value == NULL) {      \
		cout << "Error at memory allocation! New() returned NULL! Program must exit!!" << endl;\
		exit(EXIT_FAILURE);           \
	}                         \
}

BucketNode::BucketNode()
:next(NULL),offset(0)
{
    my_results = new int32_t[BUCKET_SIZE];
    CHECK_OR_EXIT(my_results);
}

BucketNode::~BucketNode(){
    delete[] my_results;
}

BucketNode* BucketNode::GetNext(){
    return next;
}

void BucketNode::SetNext(BucketNode* my_next){
    next = my_next;
}

int BucketNode::GetOffset(){
    return offset;
}

void BucketNode::SetOffset( int new_offset ){
    offset = new_offset;
}

ResultsList::ResultsList()
:first(NULL),last(NULL),num_of_buckets(0),num_of_results(0)
{

}

ResultsList::~ResultsList(){
    BucketNode* current;

    while( first != NULL ){
        current = first;
        first = current->GetNext();
        delete current;
    }

}

void ResultsList::Insert( Result& result_to_insert ){

    if( IsEmpty() ){
        /* Make the first bucket */
        first = new BucketNode();
        CHECK_OR_EXIT(first);
        last = first;
        num_of_buckets ++;
    }
    /* Calculate total size of result_to_insert */
    int total_size = RESULT_SIZE_VAL1 + RESULT_SIZE_VAL2;

    /* Go to last BucketNode */
    BucketNode* current_node = last;

    int offset = last->GetOffset();

    /* free number of bytes we can use to store */
    int free_bytes = BUCKET_SIZE - offset;

    if( free_bytes >= total_size ){ /* We can store the whole entry here */

        /* Store first field of the entry */
        memcpy(current_node->my_results+offset,&(result_to_insert.value1),RESULT_SIZE_VAL1);
        offset += RESULT_SIZE_VAL1;

        /* Store second field of the entry */
        memcpy(current_node->my_results+offset,&(result_to_insert.value2),RESULT_SIZE_VAL2);
        offset += RESULT_SIZE_VAL1;

        current_node->SetOffset(offset);

    }
    else{

        /* Option1 , the con is that we use more memory */
        /* We first convert struct into a flat entry */
        int32_t* flat_entry = new int32_t[ RESULT_SIZE_VAL1 + RESULT_SIZE_VAL2];
        CHECK_OR_EXIT(flat_entry);
        int temp_offset = 0;

        /* flat first field */
        memcpy(flat_entry+offset,&(result_to_insert.value1),RESULT_SIZE_VAL1);
        temp_offset += RESULT_SIZE_VAL1;

        /* flat second field */
        memcpy(flat_entry+offset,&(result_to_insert.value2),RESULT_SIZE_VAL2);
        temp_offset += RESULT_SIZE_VAL2;

        /* Now we store the whole entry in as many BucketNode as we may need */
        while( free_bytes <= total_size ){

            /* Store in current_bucket as many bytes as we can */
            memcpy(current_node->my_results+offset,&flat_entry,free_bytes);

            offset += free_bytes;
            current_node->SetOffset(offset);

            total_size -= free_bytes;

            /* Make new BucketNode */
            last = new BucketNode();
            CHECK_OR_EXIT(last);
            current_node->SetNext(last);
            num_of_buckets ++;

            current_node = last;
            offset = current_node->GetOffset(); /* offset should be 0 in this case */

            /* Prepare for next loop */
            free_bytes = BUCKET_SIZE - offset;

        }

        if( total_size > 0 ){ /* Store remaining bytes */
            /* Here we have free_bytes > total_size */
            memcpy(current_node->my_results+offset,&flat_entry,total_size);

            offset += total_size;
            current_node->SetOffset(offset);
        }

        delete[] flat_entry;

        /* End of Option1 */

        /* Option2 */
        /* End of Option2 */

    }

    num_of_results ++;

}

int ResultsList::GetBucketsNum(){
    return num_of_buckets;
}

int ResultsList::GetResultsNum(){
    return num_of_results;
}

bool ResultsList::IsEmpty(){
    return ( first == NULL );
}

void ResultsList::PrintResults(){

    /* Option1 , the con is that we use more memory */
    /* We will use a flat_entry to retrieve each result */

    /* Calculate total size of each result entry */
    int total_size = RESULT_SIZE_VAL1 + RESULT_SIZE_VAL2;

    int32_t* flat_entry = new int32_t[ RESULT_SIZE_VAL1 + RESULT_SIZE_VAL2];
    CHECK_OR_EXIT(flat_entry);

    BucketNode* current_node = first;

    /* for each result entry */
    for( int i = 0; i < num_of_results; i++ ){
        cout << "Result " + i << " is: ";

        while( total_size > 0 ){

        }

    }


    delete[] flat_entry;
    /* End of Option1 */


}

BucketNode* ResultsList::GetFirst(){
    return first;
}
