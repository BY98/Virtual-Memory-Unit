// EECS: bb1990
#include <stdio.h>

#include <fcntl.h>

#include <stdlib.h>

#include <sys/mman.h>

#include <string.h>

#define TBLSIZE 16
# define PAGETBLSIZE 256 // 256 page numbers 
# define FRAMEMAX 127 // frames start at 0 so 0-127 is 128 frames

int PAGE_MASK = 255;
int pg_offset_MASK = 255;
int MEMSIZE = 256 * 256;
int i = 0;
int j = 0;
int PageFault = 0;
int TBLHits = 0;
int Hit = 0;
int logical = 0;
int FrameNumber = 0;
int FrameToBeChanged = 0;
int PhysicalAddress = 0;
int value = 0;
int numaddresses = 0;
int indexes = 0;
int LRU = 0;
int NewFrame = 0;
int FULL = 0;
int top;

char line[256]; // each line of input
int PageTable[256][3];
int TBL[TBLSIZE][2];
char MemoryTable[128][256];

char
const *
const AddressFileName = "addresses.txt";


char
const *
const BackingFileName = "BACKING_STORE.bin";
signed char * backing_store_ptr;

int main() {

  int current = 0;

  FILE * ap = fopen(AddressFileName, "r");
  FILE * fptr;
  fptr = fopen("output2.csv", "w");
  int bd = open(BackingFileName, O_RDONLY);

  for (i = 0; i < PAGETBLSIZE; i++) {

    for (j = 0; j < 3; j++) {

      PageTable[i][j] = -1; // initialize contents of Array as -1 for comparison

    }

  }

  if (ap == NULL) {

    printf("The addresses.txt file doesnt exist");

  }

  backing_store_ptr = mmap(NULL, MEMSIZE, PROT_READ, MAP_SHARED, bd, 0); //map backingstore bytes to ptr bytes with pg_offset of 0      

  while (fgets(line, sizeof(line), ap)) {
    numaddresses++;

    strtok(line, "\n"); //removes new line character at the end of the string
    sscanf(line, "%d", & logical); // puts contents of array into an integer

    int pg_offset = logical & pg_offset_MASK;
    int pg_number = (logical >> 8) & PAGE_MASK;
    //printf("Page Number : %d  Page Offset : %d \n", pg_number, pg_offset);

    Hit = 0; // need to reset Hit

    for (i = 0; i < TBLSIZE; i++) {

      if (TBL[i][0] == pg_number) { // if the TBL has a page number and frame

        Hit = 1;
        TBLHits++;
        
        for (j = 0 ; j < 256 ; j++ ) {
        
        if (PageTable[j][2] > PageTable[pg_number][2]) { // decrement the index of pages higher in the stack
        PageTable[j][2] = PageTable[j][2] - 1;        
        }
        }
        // Update the Page Table with most recently used
        if (FULL ==1) {
        PageTable[pg_number][2] = FRAMEMAX;
        }
        else {
        PageTable[pg_number][2] = indexes-1;    
        }    




        PhysicalAddress = (TBL[i][1] << 8) | pg_offset; // calculate the physical address

        value = * (MemoryTable[TBL[i][1]] + pg_offset); // get signed byte when the page table has the pg_number

      }

    }

    if (Hit == 0) { // if the TBL didnt find the page number and frame

      if (PageTable[pg_number][2] < 0 && PageTable[pg_number][1] <0) { // if pg number doesnt have a frame and index then Page Fault)
        
        
        if (bd < 0) {

          printf("The Backing file doesnt exist");

          break;

        }

        

        PageTable[pg_number][0] = pg_number; // assign page number to Page Table
        TBL[current][0] = pg_number; // FIFO Updating TBL with pg number

        if (FULL == 1) {
          for (j = 0; j < PAGETBLSIZE; j++) {

            if (PageTable[j][2] == 0) { // if the index is 0 (item is at the bottom of the stack)
              NewFrame = PageTable[j][1]; // Frame of LRU will be used for new page
              PageTable[j][1] = -1; // change frame to - 1
              PageTable[j][2] = -1; // change index to -1
            }
          }

          for (j = 0; j < PAGETBLSIZE; j++) {
            PageTable[j][2] = PageTable[j][2] - 1; // reduce index of everything          
          }
          PageTable[pg_number][1] = NewFrame; // set new frame
          PageTable[pg_number][2] = FRAMEMAX; // set index
          TBL[current][1] = NewFrame;
          memcpy(MemoryTable[NewFrame], backing_store_ptr + pg_number * PAGETBLSIZE, PAGETBLSIZE); // copy page from backing store array to Physical MemoryTable
         PhysicalAddress = (NewFrame << 8) | pg_offset; // calculate the physical address
          value = * (MemoryTable[NewFrame] + pg_offset); // signed byte

        } else { // if the 2D array is not full
        
          memcpy(MemoryTable[FrameToBeChanged], backing_store_ptr + pg_number * PAGETBLSIZE, PAGETBLSIZE); // copy page from backing store array to Physical MemoryTable
         PhysicalAddress = (FrameToBeChanged << 8) | pg_offset; // calculate the physical address
          value = * (MemoryTable[FrameToBeChanged] + pg_offset); // signed byte
          if (FrameToBeChanged < FRAMEMAX) {
            PageTable[pg_number][2] = indexes; // assign index to page
            PageTable[pg_number][1] = FrameToBeChanged; // assign frame to page
            TBL[current][1] = FrameToBeChanged; // FIFO Updating TBL with frame Number  
          }

          if ((FrameToBeChanged == FRAMEMAX)) { // Check if frame number is highest frame number 
            FULL = 1;
            PageTable[pg_number][2] = FRAMEMAX; // assign index to pg_number
            PageTable[pg_number][1] = FrameToBeChanged; // assign frame number to page in second column
            TBL[current][1] = FrameToBeChanged;
          }
        
        
 

          if (FrameToBeChanged < FRAMEMAX) {
            FrameToBeChanged++;
            indexes++;

          }
          
 
        }


        PageFault++; // Page Fault occurs
      } // ending of Page Fault if statement
      else { // PAGE TABLE HIT

        if (FULL == 1) { // if the stack is full
          top = FRAMEMAX; // index corresponding to the last frame
        } else { // if the stack has space
          top = indexes-1;  // index corresponding to the
        }

        if ((PageTable[pg_number][2] != top)) { // if the pg is not at the top of the stack

          for (i = 0; i < PAGETBLSIZE; i++) {

            if (PageTable[i][2] > PageTable[pg_number][2]) { // reduce the index of the pages which come after this page

              PageTable[i][2] = PageTable[i][2] - 1; // reduce the index of everything by 1     

            }
          }

        }

        PageTable[pg_number][2] = top; // move page to top of the stack        

        //update TBL
        TBL[current][0] = pg_number;
        TBL[current][1] = PageTable[pg_number][1]; // update TBL with frame number
        
        
        
        PhysicalAddress = (PageTable[pg_number][1] << 8) | pg_offset; // calculate the physical address

        value =  *(MemoryTable[PageTable[pg_number][1]] + pg_offset); // get signed byte when the page table has the pg_number

      }


      // TBL counter
        if (current < (TBLSIZE - 1)) {

        current++;

      } else {
        current = 0; // set current back to very first row of TBL   
      }
      
    } // ending of TBL MISS if statement

  fprintf(fptr, "%d,", logical);
  fprintf(fptr, "%d,", PhysicalAddress);
  fprintf(fptr, "%d\n", value);
  } // ending of while loop
 

  printf("Page Faults Rate = %0.3f  \n", (float) PageFault / numaddresses);
  printf("TLB Hits Rate = %0.3f  \n", (float) TBLHits / numaddresses);

  fclose(fptr);
  fclose(ap);

}
