#include <stdio.h> 
#include <stdlib.h>

#include "simulator.h"

void pageit(Pentry q[MAXPROCESSES]) { 
    
    /* This file contains the stub for a predictive pager */
    /* You may need to add/remove/modify any part of this file */

    /* Static vars */
    static int initialized = 0;
    static int tick = 1; // artificial time
    static int timestamps[MAXPROCESSES][MAXPROCPAGES];
    static int previous[MAXPROCESSES];
    static int probIncrement[MAXPROCPAGES][MAXPROCPAGES];

    /* Local vars */
    int pc;
    int proc;
    int page;
    int lrt;
    

    /* initialize static vars on first run */
    if(!initialized){
        for(int i = 0; i < MAXPROCPAGES; i++) {
            previous[i] = -1;

            for(int j = 0; j < MAXPROCPAGES; j++) {
                timestamps[i][j] = 0;
                probIncrement[i][j] = 0;
            }   
        }
        initialized = 1;
    }
    
    // Implement LRU Paging
    for (int proc = 0; proc < MAXPROCESSES; proc++) {
	    if (q[proc].active == 1){
            int pc = q[proc].pc; 		            	// program counter for process
            int page = pc/PAGESIZE; 		        	// current page the PC is on
            timestamps[proc][page] = tick;              //We need to keep track of the tick so we can choose the smallest time stamp
            
            if(previous[proc] != -1 && previous[proc] != page) {
                probIncrement[previous[proc]][page] += 1;
            }

            previous[proc] = page;

            if (q[proc].pages[page] != 1){       // if the page is already in memory, then we're done
               if (pagein(proc,page)==0){     	    // if not in memory, then page it in cause page fault
                //Continue
                    int lrt;
                    //Now we have to find the least recently used page by the ticks
                    lrt = __INT_MAX__; //Set to max int so we can swap if we find smaller values
                    int pageNum = page;

                    for (int vic = 0; vic < MAXPROCPAGES; vic++) {  // if the pagein fails, find pages to pageout
                        if (timestamps[proc][vic] < lrt){ // only interested in pages that are in memory
                            if(q[proc].pages[vic] == 1){
                                lrt = timestamps[proc][vic];
                                pageNum = vic;
                            }
                        }
                    }
                    //Now that we have LRU page we can page out
                    pageout(proc,pageNum);
               }
            }
            
        }  

        
        
    }





    for(proc = 0; proc < MAXPROCESSES; proc++) {
        // Check if the process is active
        if(q[proc].active) {
            if(!q[proc].pages[page]) {
                // Try to swap in
                if(!pagein(proc, page)) {
                    // If swapping fails, need to find another page to swap out
                    int min = tick;
                    int choice;
                    // Find least recently used page for the current process which is currently paged in
                    for(int i = 0; i < q[proc].npages; i++) {
                        if(timestamps[proc][i] < min && q[proc].pages[i]) {
                            choice = i;
                            min = timestamps[proc][i];
                        }
                    }
                    // Pageout least recently used page
                    pageout(proc, choice);
                }
            }
        }
        // Advance time for next pageit iteration
        tick++;
    } //Do printing of transition matrix if finished
    if(tick > 3000000 && tick < 3000020) {
        int rowsums[MAXPROCPAGES] = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
        for(int i = 0; i < MAXPROCPAGES; i++) {
            for(int j = 0; j < MAXPROCPAGES; j++) {
                rowsums[i] += probIncrement[i][j];
            }
        }
        for(int i = 0; i < MAXPROCPAGES; i++) {
            for(int j = 0; j < MAXPROCPAGES; j++) {
                if(rowsums[i] > 0) {
                    printf("%.2f\t", (double)probIncrement[i][j]/rowsums[i]);
                } else {
                    printf("%d\t", 0);
                }
            }
            printf("\n");
        }
    }
}
