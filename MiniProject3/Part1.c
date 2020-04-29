// EECS: bb1990
#include <stdio.h>

#include <fcntl.h>

#include <stdlib.h>

#include <sys/mman.h>

#include <string.h>

int PAGE_MASK = 255;
int pg_offset_MASK = 255;
int MEMSIZE = 256*256 ;
int i = 0 ;
int j = 0;
int PageFault = 0;
int TBLHits = 0;
int Hit = 0;
int logical = 0;
int FrameToBeChanged = 0;
int PhysicalAddress = 0;
int value = 0;
int numaddresses = 0;


char line[256]; // store line contents in here
int PageTable[256][2];
int TBL[16][2];
char MemoryTable[256][256];


char const * const AddressFileName = "addresses.txt";

char const * const BackingFileName = "BACKING_STORE.bin";
signed char * backing_store_ptr; 


int main() {
	
//current keeps track of TBL row for "First In"
int current = 0;

FILE * ap = fopen(AddressFileName, "r");
FILE * fptr ;
fptr = fopen("output.csv","w");
int bd = open(BackingFileName,O_RDONLY); // file descriptor for BACKING_STORE.bin
 
 for(i=0; i<256; i++) {
     
    for(j=0;j<2;j++) {
        
    PageTable[i][j] = -1; // Set as an integer <0 rather than leaving it as a garbage value and this allows for proper comparison when searching for page numbers or frame numbers
    
    
    }
    
    }
    
 
  if (ap == NULL) {
    
    printf("The address file doesnt exist");
  
  }

  backing_store_ptr = mmap(NULL,MEMSIZE,PROT_READ,MAP_SHARED,bd,0); //map backingstore bytes to ptr bytes with pg_offset of 0      


  while(fgets(line, sizeof(line), ap)) {
  numaddresses++;
 
  strtok(line, "\n");                 //removes new line character at the end of the string
  sscanf(line, "%d", &logical);      // puts contents of array into an integer
 
   
  int pg_offset = logical & pg_offset_MASK;
  int pg_number = (logical >> 8) & PAGE_MASK;
 

 
  Hit = 0; 
 
 // Check for TBL Hit 
  for (i = 0 ; i < 16 ; i ++) {
      
    if(TBL[i][0] == pg_number) { 
    
    Hit = 1;
    TBLHits++;
    
    PhysicalAddress = (TBL[i][1] << 8) | pg_offset;    // calculate the physical address
 
    value = *(MemoryTable[TBL[i][1]] + pg_offset); // get signed byte using TBL frame number

    }  
            
    }
    
 // Check for TBL MISS
    if ( Hit == 0) { 
 
 // Check for Page Fault
        if (PageTable[pg_number][0] == -1) { 
           
        if (bd < 0) {

        printf("The Backing file doesnt exist");

        break;
        
        }
        
    
    memcpy( MemoryTable[FrameToBeChanged] , backing_store_ptr + pg_number * 256 , 256); // copy page bytes from backing store array to Physical MemoryTable
    PhysicalAddress = (FrameToBeChanged << 8) | pg_offset;    // calculate the physical address	
	value = *(MemoryTable[FrameToBeChanged] + pg_offset); // signed byte 
        
    
    PageTable[pg_number][0] = pg_number; // assign frame number to page in second column
    PageTable[pg_number][1] = FrameToBeChanged; // assign frame number to page in second column
    TBL[current][0] = pg_number; //assign pg number to TBL
    TBL[current][1] = FrameToBeChanged;  // assign Frame to TBL
    
    
    if (FrameToBeChanged <255) {
    
       FrameToBeChanged++; // increase frame number
    
    }
        
    PageFault++; // Page Fault occurs
 
        }
        
    else {
        
    // if the Page Table has the pg 
    PhysicalAddress = (PageTable[pg_number][1] << 8) | pg_offset;    // calculate the physical address	
    value = *(MemoryTable[PageTable[pg_number][1]] + pg_offset); // get signed byte when the page table has the pg_number
    
    
    }
        
    
    
    if (current <15) {

        current++; 
    }
    
    else {
    current = 0 ; 
    
    }    

    }
    
    printf("%d\n",value); 
    
	fprintf(fptr,"%d,",logical); 
    fprintf(fptr,"%d,",PhysicalAddress);     
    fprintf(fptr,"%d\n",value);
            
     }     // ending of while loop 
 
 printf("Page Faults = %d\n",PageFault);      
 printf("TLB Hits = %d\n",TBLHits);      
 
 //printf("Page Faults Rate = %0.3f  \n",(float)PageFault/numaddresses);      
 //printf("TLB Hits Rate = %0.3f  \n", (float)TBLHits/numaddresses);      

 fclose(fptr);
 fclose(ap);

}



