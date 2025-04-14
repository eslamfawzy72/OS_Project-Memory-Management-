#include <inc/memlayout.h>
#include <kern/kheap.h>
#include <kern/memory_manager.h>

bool first_time = 1;
// Use a macro for heap size (number of pages)
#define HEAP_PAGES ((KERNEL_HEAP_MAX - KERNEL_HEAP_START) / PAGE_SIZE)
int HeapTrack[HEAP_PAGES] = {0};

void* kmalloc(unsigned int size)
{
    static int call_count = 0;
    call_count++;
   // cprintf("kmalloc called: %d times\n", call_count);
    //cprintf("Requested size: %d\n", size);

    if (first_time) {
        HeapTrack[0] = -1 * HEAP_PAGES;
        HeapTrack[HEAP_PAGES - 1] = -1 * HEAP_PAGES;
        first_time = 0;
    }

    // Round size to nearest multiple of PAGE_SIZE
    int Used_size =ROUNDUP(size,PAGE_SIZE);
  /*  while (Used_size % PAGE_SIZE != 0) {
        Used_size++;
    }
    int num_of_pages = Used_size / PAGE_SIZE;
   */
   // cprintf("Number of pages: %d\n", num_of_pages);

    // Find best-fit free block
    int maxIndex = -1;
    int maxSize = -1;
    int i=0;
    while(i < HEAP_PAGES) {
        if (HeapTrack[i] > 0) {
            i += HeapTrack[i];  // skip allocated block
        } else if (HeapTrack[i] < 0) {
            int currentSize = -1 * HeapTrack[i];
            if (currentSize >= num_of_pages && currentSize > maxSize) {
                maxSize = currentSize;
                maxIndex = i;
            }
            i += currentSize;
        } else {
        	  i += HeapTrack[i-1]-1;
             // prevent infinite loop
        }
    }

    // No sufficient block found
    if (maxSize < num_of_pages || maxIndex == -1) {
        cprintf("HEAP IS FULL or no block fits\n");
        return NULL;
    }

    // Allocate the block
    HeapTrack[maxIndex] = num_of_pages;
    HeapTrack[maxIndex + num_of_pages - 1] = num_of_pages;

    // Split the remaining part if larger
    if (maxSize > num_of_pages) {
        int rem = maxSize - num_of_pages;
        HeapTrack[maxIndex + num_of_pages] = -1 * rem;
        HeapTrack[maxIndex + maxSize - 1] = -1 * rem;
    }

    // Map frames
    for (int i = 0; i < num_of_pages; i++) {
        uint32* ptr_table = NULL;
        uint32 va = ((maxIndex + i) * PAGE_SIZE) + KERNEL_HEAP_START;
        struct Frame_Info* ptr_frame_info = get_frame_info(ptr_page_directory, (void*)va, &ptr_table);

        if (ptr_frame_info != NULL) {
            cprintf("Page already mapped at index %d\n", maxIndex + i);
            return NULL;
        }

        int ret = allocate_frame(&ptr_frame_info);
        if (ret == E_NO_MEM) {
            cprintf("No enough memory for page!\n");
            return NULL;
        }

        ret = map_frame(ptr_page_directory, ptr_frame_info, (void*)va,PERM_PRESENT |PERM_WRITEABLE);
        if (ret == E_NO_MEM) {
            cprintf("No enough memory for page table!\n");
            free_frame(ptr_frame_info);
            return NULL;
        }
    }

    // Return virtual address of allocated block
    return (void*)(KERNEL_HEAP_START + maxIndex * PAGE_SIZE);
}

void kfree(void* virtual_address)
{
    int index = ((uint32)virtual_address - KERNEL_HEAP_START) / PAGE_SIZE;
	int Base_page=HeapTrack[index];
	if(Base_page<0){
		cprintf("Can not free this page,already free page\n");
		return;
	}
	int total=Base_page;
	int first=index;
	int last=index+Base_page-1;
	int before=-1;
	int after=-1;
	if(Base_page>0){
	before=index-1;
	after=index+Base_page;
		}

	if(before >= 0 && HeapTrack[before] < 0){
	total+=(-HeapTrack[before]);
	first = before - (-HeapTrack[before]) + 1;
	}
	if(after < HEAP_PAGES && HeapTrack[after] < 0)
	{
		total+=(-HeapTrack[after]);
		last = after + (-HeapTrack[after]) - 1;
	}
	HeapTrack[first]=-total;
	HeapTrack[last]=-total;
	 for (int i = 0; i < Base_page; i++) {
	        unsigned char *va = (unsigned char *)(KERNEL_HEAP_START + (index + i) * PAGE_SIZE);
	        unmap_frame(ptr_page_directory, va);
	    }

	 cprintf("Freed block from %p to %p, total size: %d pages\n", virtual_address, (void *)(KERNEL_HEAP_START + last * PAGE_SIZE), total);

	//TODO: [PROJECT 2025 - MS1 - [1] Kernel Heap] kfree()
	// Write your code here, remove the panic and write your code
	//panic("kfree() is not implemented yet...!!");

	//you need to get the size of the given allocation using its address
	//refer to the project presentation and documentation for details

}

unsigned int kheap_virtual_address(unsigned int physical_address)
{
	//TODO: [PROJECT 2025 - MS1 - [1] Kernel Heap] kheap_virtual_address()
	// Write your code here, remove the panic and write your code
	panic("kheap_virtual_address() is not implemented yet...!!");

	//return the virtual address corresponding to given physical_address
	//refer to the project presentation and documentation for details

	//change this "return" according to your answer

	return 0;
}

unsigned int kheap_physical_address(unsigned int virtual_address)
{
	//TODO: [PROJECT 2025 - MS1 - [1] Kernel Heap] kheap_physical_address()
	// Write your code here, remove the panic and write your code
	panic("kheap_physical_address() is not implemented yet...!!");

	//return the physical address corresponding to given virtual_address
	//refer to the project presentation and documentation for details

	//change this "return" according to your answer

	return 0;
}

void *krealloc(void *virtual_address, uint32 new_size)
{
	panic("krealloc() is not required...!!");
	return NULL;

}
