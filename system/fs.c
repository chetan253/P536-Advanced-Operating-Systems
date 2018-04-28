#include <xinu.h>
#include <kernel.h>
#include <stddef.h>
#include <stdio.h>
#include <string.h>
#include <stdbool.h>

#ifdef FS
#include <fs.h>

static struct fsystem fsd;
int dev0_numblocks;
int dev0_blocksize;
char *dev0_blocks;

extern int dev0;

char block_cache[512];

#define SB_BLK 0
#define BM_BLK 1
#define RT_BLK 2

#define NUM_FD 16
struct filetable oft[NUM_FD];
int next_open_fd = 0;


#define INODES_PER_BLOCK (fsd.blocksz / sizeof(struct inode))
#define NUM_INODE_BLOCKS (( (fsd.ninodes % INODES_PER_BLOCK) == 0) ? fsd.ninodes / INODES_PER_BLOCK : (fsd.ninodes / INODES_PER_BLOCK) + 1)
#define FIRST_INODE_BLOCK 2

int fs_fileblock_to_diskblock(int dev, int fd, int fileblock);
/* YOUR CODE GOES HERE */

int fs_fileblock_to_diskblock(int dev, int fd, int fileblock) {
  int diskblock;

  if (fileblock >= INODEBLOCKS - 2) {
    printf("No indirect block support\n");
    return SYSERR;
  }

  diskblock = oft[fd].in.blocks[fileblock]; //get the logical block address

  return diskblock;
}

/* read in an inode and fill in the pointer */
int fs_get_inode_by_num(int dev, int inode_number, struct inode *in) {
  int bl, inn;
  int inode_off;

  if (dev != 0) {
    printf("Unsupported device\n");
    return SYSERR;
  }
  if (inode_number > fsd.ninodes) {
    printf("fs_get_inode_by_num: inode %d out of range\n", inode_number);
    return SYSERR;
  }

  bl = inode_number / INODES_PER_BLOCK;
  inn = inode_number % INODES_PER_BLOCK;
  bl += FIRST_INODE_BLOCK;

  inode_off = inn * sizeof(struct inode);

  /*
  printf("in_no: %d = %d/%d\n", inode_number, bl, inn);
  printf("inn*sizeof(struct inode): %d\n", inode_off);
  */

  bs_bread(dev0, bl, 0, &block_cache[0], fsd.blocksz);
  memcpy(in, &block_cache[inode_off], sizeof(struct inode));

  return OK;

}

int
fs_put_inode_by_num(int dev, int inode_number, struct inode *in) {
  int bl, inn;

  if (dev != 0) {
    printf("Unsupported device\n");
    return SYSERR;
  }
  if (inode_number > fsd.ninodes) {
    printf("fs_put_inode_by_num: inode %d out of range\n", inode_number);
    return SYSERR;
  }

  bl = inode_number / INODES_PER_BLOCK;
  inn = inode_number % INODES_PER_BLOCK;
  bl += FIRST_INODE_BLOCK;

  /*
  printf("in_no: %d = %d/%d\n", inode_number, bl, inn);
  */

  bs_bread(dev0, bl, 0, block_cache, fsd.blocksz);
  memcpy(&block_cache[(inn*sizeof(struct inode))], in, sizeof(struct inode));
  bs_bwrite(dev0, bl, 0, block_cache, fsd.blocksz);

  return OK;
}
     
int fs_mkfs(int dev, int num_inodes) {
  int i;
  
  if (dev == 0) {
    fsd.nblocks = dev0_numblocks;
    fsd.blocksz = dev0_blocksize;
  }
  else {
    printf("Unsupported device\n");
    return SYSERR;
  }

  if (num_inodes < 1) {
    fsd.ninodes = DEFAULT_NUM_INODES;
  }
  else {
    fsd.ninodes = num_inodes;
  }

  i = fsd.nblocks;
  while ( (i % 8) != 0) {i++;}
  fsd.freemaskbytes = i / 8; 
  
  if ((fsd.freemask = getmem(fsd.freemaskbytes)) == (void *)SYSERR) {
    printf("fs_mkfs memget failed.\n");
    return SYSERR;
  }
  
  /* zero the free mask */
  for(i=0;i<fsd.freemaskbytes;i++) {
    fsd.freemask[i] = '\0';
  }
  
  fsd.inodes_used = 0;
  
  /* write the fsystem block to SB_BLK, mark block used */
  fs_setmaskbit(SB_BLK);
  bs_bwrite(dev0, SB_BLK, 0, &fsd, sizeof(struct fsystem));
  
  /* write the free block bitmask in BM_BLK, mark block used */
  fs_setmaskbit(BM_BLK);
  bs_bwrite(dev0, BM_BLK, 0, fsd.freemask, fsd.freemaskbytes);

  return 1;
}

void
fs_print_fsd(void) {

  printf("fsd.ninodes: %d\n", fsd.ninodes);
  printf("sizeof(struct inode): %d\n", sizeof(struct inode));
  printf("INODES_PER_BLOCK: %d\n", INODES_PER_BLOCK);
  printf("NUM_INODE_BLOCKS: %d\n", NUM_INODE_BLOCKS);
}

/* specify the block number to be set in the mask */
int fs_setmaskbit(int b) {
  int mbyte, mbit;
  mbyte = b / 8;
  mbit = b % 8;

  fsd.freemask[mbyte] |= (0x80 >> mbit);
  return OK;
}

/* specify the block number to be read in the mask */
int fs_getmaskbit(int b) {
  int mbyte, mbit;
  mbyte = b / 8;
  mbit = b % 8;

  return( ( (fsd.freemask[mbyte] << mbit) & 0x80 ) >> 7);
  return OK;

}

/* specify the block number to be unset in the mask */
int fs_clearmaskbit(int b) {
  int mbyte, mbit, invb;
  mbyte = b / 8;
  mbit = b % 8;

  invb = ~(0x80 >> mbit);
  invb &= 0xFF;

  fsd.freemask[mbyte] &= invb;
  return OK;
}

/* This is maybe a little overcomplicated since the lowest-numbered
   block is indicated in the high-order bit.  Shift the byte by j
   positions to make the match in bit7 (the 8th bit) and then shift
   that value 7 times to the low-order bit to print.  Yes, it could be
   the other way...  */
void fs_printfreemask(void) {
  int i,j;

  for (i=0; i < fsd.freemaskbytes; i++) {
    for (j=0; j < 8; j++) {
      printf("%d", ((fsd.freemask[i] << j) & 0x80) >> 7);
    }
    if ( (i % 8) == 7) {
      printf("\n");
    }
  }
  printf("\n");
}

int fs_open(char *filename, int flags) {
	//printf("In fs_open\n");
	struct inode f_inode;
	int i = 0;
	//printf("%d\n",fsd.root_dir.numentries);
	if(fsd.root_dir.numentries > 0){
		for(i = 0; i < fsd.root_dir.numentries; i++){
			if(strcmp(filename, fsd.root_dir.entry[i].name) == 0){
				fs_get_inode_by_num(dev0, i, &(f_inode));
				oft[i].state = FSTATE_OPEN;
				oft[i].fileptr = 0;
				oft[i].in = f_inode;
				oft[i].de = &fsd.root_dir.entry[i];
				oft[i].flags = flags;
				return i;
			}
		}
	}
	else if(fsd.root_dir.numentries == 0){
		printf("No files in fsd.root_dir dir\n");
		return SYSERR;
	}
	printf("File not found \n");
	return SYSERR;
}

int isValid(int fd){
        return (fd < 0 || fd > NUM_FD) ? 0 : 1;
}

int fs_close(int fd) {
	if(!isValid(fd)){
		printf("Not a valid file\n");
                return SYSERR;
	}
	if(oft[fd].state == FSTATE_OPEN){
		oft[fd].state = FSTATE_CLOSED;
		oft[fd].fileptr = 0;
		//printf("File closed\n");
		return OK;
	}
	return OK;
}

int fs_create(char *filename, int mode) {
	//printf("create prev num entries %d\n",fsd.root_dir.numentries);
	if(mode == O_CREAT){
		int i = 0;
		int new_inode_num = 0;
		for(i = 0; i < fsd.root_dir.numentries; i++){
			if(strcmp(filename, fsd.root_dir.entry[i].name) == 0){
				printf("File already exists!!");
				return SYSERR;
			}
		}
		struct inode new_inode;
		//new_inode_num = fsd.inodes_used;
		fsd.inodes_used++;	
		fs_get_inode_by_num(dev0, fsd.inodes_used, &(new_inode));
		new_inode.id = fsd.inodes_used;
		new_inode.device = dev0;
		new_inode.type = INODE_TYPE_FILE;
		new_inode.size = 0;
		fs_put_inode_by_num(dev0, fsd.inodes_used, &(new_inode));

		//Update fsd.root_dir directory
		fsd.root_dir.entry[fsd.root_dir.numentries].inode_num = fsd.inodes_used;
		strcpy(fsd.root_dir.entry[fsd.root_dir.numentries].name, filename);
		fsd.root_dir.numentries++;
		//printf("File created with name : %s\n",fsd.root_dir.entry[fsd.root_dir.numentries].name);
		//printf("create after numentries %d\n",fsd.root_dir.numentries);
		return fs_open(filename, O_RDWR);
	}
	
	printf("Wrong mode passed");
	return SYSERR;
}

int fs_seek(int fd, int offset){ 
	if(!isValid(fd)){
		printf("Not a valid file\n");
		return SYSERR;
	}
	oft[fd].fileptr += offset;
	return OK;
}

int fs_read(int fd, void *buf, int nbytes) {
	int i = 0;
	if(!isValid(fd)){
		printf("Not a valid file\n");
		return SYSERR;
	}
	if(oft[fd].state !=  FSTATE_OPEN){
		printf("Open the file first\n");
		
		return SYSERR;
	}
	if(oft[fd].flags == O_RDWR || oft[fd].flags == O_RDONLY){
		int blk_limit = oft[fd].in.size;
		int offset = 0;
		int blk_size = 0;
		nbytes += oft[fd].fileptr;
		int block = oft[fd].fileptr / MDEV_BLOCK_SIZE;
		int bytes = 0;
		for(i = block ; i < blk_limit; i++,bytes = strlen(buf)){
			if(offset && i == 0)
				blk_size = oft[fd].fileptr % MDEV_BLOCK_SIZE;
			else
				blk_size = MDEV_BLOCK_SIZE;
			char temp_buf[MDEV_BLOCK_SIZE];
			if(bs_bread(dev0, oft[fd].in.blocks[i], 0, temp_buf, blk_size - (oft[fd].fileptr % MDEV_BLOCK_SIZE)) == OK){
				strcpy(buf + bytes, temp_buf);
				//printf("Read active\n");
				continue;
			}
			printf("Reading failed\n");
			return SYSERR;
		}
		return strlen(buf);
	}
	printf("File is not in read mode\n");
	return SYSERR;
}

int fs_write(int fd, void *buf, int nbytes) {
	//printf("bytes to write total %d\n",nbytes);
	//printf("Buffer len %d\n", strlen(buf));
	if(!isValid(fd)){
		printf("Not a valid file\n");
		return SYSERR;
	}
	if(oft[fd].state != FSTATE_OPEN){
		printf("Open the file first\n");
		return SYSERR;
	}
	if(oft[fd].flags == O_RDWR || oft[fd].flags == O_WRONLY){
		int i = FIRST_INODE_BLOCK + NUM_INODE_BLOCKS;
		int blk_limit = 0;
		int offset = 0;
		if(nbytes % MDEV_BLOCK_SIZE != 0){
			offset = 1;
			blk_limit = nbytes / MDEV_BLOCK_SIZE + 1;
		}
		else
			blk_limit = nbytes / MDEV_BLOCK_SIZE;
		
		int data_length = nbytes;
		int blk_size = 0, counter = 0, inode_count = 0;
		while(i < MDEV_BLOCK_SIZE && inode_count < blk_limit){
			if(fs_getmaskbit(i) != 0){
				i++;
				continue;
			}
			if(offset && data_length  < MDEV_BLOCK_SIZE)
                                blk_size = data_length;
                        else
                                blk_size = MDEV_BLOCK_SIZE;
			char temp_buf[MDEV_BLOCK_SIZE];	
			memset(temp_buf, NULL, MDEV_BLOCK_SIZE);
			memcpy(temp_buf, buf, blk_size);
			if(bs_bwrite(dev0, i, 0, temp_buf, MDEV_BLOCK_SIZE) == OK){
				
				oft[fd].in.blocks[inode_count] = i;
				buf = (char *)buf + blk_size;
				data_length -= blk_size;
				//printf("%d bytes written so far\n",data_length );
				fs_setmaskbit(i);
				i++;
				//printf("Block written");
			}
			else
				printf("Writing failed\n");
			inode_count++;
		}
		oft[fd].in.size = blk_limit;
		oft[fd].fileptr = nbytes;
		return nbytes;
	}
	printf("File is not in write mode\n");
	return SYSERR;
}

#endif /* FS */
