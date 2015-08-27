#include <stdio.h>  
#include <stdlib.h>  
#include <time.h>  
#include <unistd.h>  
#include <fcntl.h>  
#include <unistd.h>
#include <errno.h>
#include <sys/mman.h>

#define MAP_SIZE        0x1000

#define READ	0
#define WRITE	1

int main(int argc, char **argv)  
{
	int dev_fd;  
	unsigned char *map_base = NULL;
	unsigned int base = 0;
	unsigned int offs = 0;
	unsigned int val = 0;
	int rw = READ;
	int i;

	printf("--<argc=%d\n", argc);
	for (i=0; i<argc; i++) {
		printf("--<argv[%d]=%s\n", i, argv[i]);
	}

	if (argc < 3) {
		printf("Usage: regrw <r/w> <base> <offset> [val]\n");
		return 0;
	}

	base = strtoul(argv[2], NULL, 16);	// reg base address
	offs = strtoul(argv[3], NULL, 16);	// reg offset

	if (strcmp("r", argv[1]) == 0) {
		rw = READ;
	} else {
		rw = WRITE;

		if (argc < 5) {
			printf("Usage: regrw <r/w> <base> <offset> [val]\n");
			return 0;
		}

		val = strtoul(argv[4], NULL, 16);
	}

	printf("%s: base=0x%08x, offs=0x%08x, val=0x%08x\n", rw == READ ? "Read" : "Write", base, offs, val);

	dev_fd = open("/dev/mem", O_RDWR | O_NDELAY);        
	if (dev_fd < 0) {
        	printf("open(/dev/mem) failed.");      
        	return -1;
	}

	map_base = (unsigned char * )mmap(NULL, MAP_SIZE, PROT_READ | PROT_WRITE, MAP_SHARED, dev_fd, base);
	if (MAP_FAILED == map_base || NULL == map_base) {
		printf("map failed! errno=%d\n", errno);
		close(dev_fd);
		return -1;
	}

	switch (rw) {
	case READ:
		val = *(volatile unsigned int *)(map_base+offs);
		printf("Read Register 0x%08x: 0x%08x\n", base+offs, val);
		break;
	case WRITE:
		*(volatile unsigned int *)(map_base+offs) = val;
		printf("Write Register 0x%08x with 0x%08x\n", base+offs, val);
		break;
	default:
		printf("ERROR: invalid rw %d\n", rw);
	}

	if(dev_fd)  
		close(dev_fd);  
  
	munmap(map_base,MAP_SIZE);//解除映射关系  
  
	return 0;  
}
