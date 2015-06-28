/**
 * @author Tony Thomas <01tonythomas@gmail.com>
 * Program to show attributes of a file
 **/
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>


#define PERMS 0777 /* RW for owner, group, others */
char buffer[2048];

main(int argc, char *argv[] ) {
  int file;
	if (argc != 2 ) {
		printf("need 1 arguments to show attributes \n"); 
		exit(1);
	}
	struct stat sb;
	file = stat( argv[1], &sb );
	printf("File type:                ");

	switch (sb.st_mode & S_IFMT) {
	case S_IFBLK:  printf("block device\n");            break;
	case S_IFCHR:  printf("character device\n");        break;
	case S_IFDIR:  printf("directory\n");               break;
	case S_IFIFO:  printf("FIFO/pipe\n");               break;
	case S_IFLNK:  printf("symlink\n");                 break;
	case S_IFREG:  printf("regular file\n");            break;
	case S_IFSOCK: printf("socket\n");                  break;
	default:       printf("unknown?\n");                break;
	}

	printf("I-node number:  %ld\n", (long) sb.st_ino);
	printf("Mode: %lo (octal)\n", (unsigned long) sb.st_mode);
	printf("Link count: %ld\n", (long) sb.st_nlink);
	printf("Ownership:UID=%ld   GID=%ld\n",(long) sb.st_uid, (long) sb.st_gid);
	printf("Preferred I/O block size: %ld bytes\n",(long) sb.st_blksize);
	printf("File size: %lld bytes\n",(long long) sb.st_size);
	printf("Blocks allocated: %lld\n",(long long) sb.st_blocks);

	
	exit(0);
}
