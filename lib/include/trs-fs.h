
#ifndef __TRS_FS_H__
#define __TRS_FS_H__

#include <defs.h>

#define TRS_FS_ID 4

#define FS_COMMAND_OPEN     1
#define FS_COMMAND_WRITE    2
#define FS_COMMAND_READ     3
#define FS_COMMAND_CLOSE    4

#ifndef FRESULT_DEFINED
#define FRESULT_DEFINED

typedef enum {
    FR_OK = 0,				/* (0) Succeeded */
    FR_DISK_ERR,			/* (1) A hard error occurred in the low level disk I/O layer */
    FR_INT_ERR,				/* (2) Assertion failed */
    FR_NOT_READY,			/* (3) The physical drive cannot work */
    FR_NO_FILE,				/* (4) Could not find the file */
    FR_NO_PATH,				/* (5) Could not find the path */
    FR_INVALID_NAME,		/* (6) The path name format is invalid */
    FR_DENIED,				/* (7) Access denied due to prohibited access or directory full */
    FR_EXIST,				/* (8) Access denied due to prohibited access */
    FR_INVALID_OBJECT,		/* (9) The file/directory object is invalid */
    FR_WRITE_PROTECTED,		/* (10) The physical drive is write protected */
    FR_INVALID_DRIVE,		/* (11) The logical drive number is invalid */
    FR_NOT_ENABLED,			/* (12) The volume has no work area */
    FR_NO_FILESYSTEM,		/* (13) There is no valid FAT volume */
    FR_MKFS_ABORTED,		/* (14) The f_mkfs() aborted due to any problem */
    FR_TIMEOUT,				/* (15) Could not get a grant to access the volume within defined period */
    FR_LOCKED,				/* (16) The operation is rejected according to the file sharing policy */
    FR_NOT_ENOUGH_CORE,		/* (17) LFN working buffer could not be allocated */
    FR_TOO_MANY_OPEN_FILES,	/* (18) Number of open files > FF_FS_LOCK */
    FR_INVALID_PARAMETER,     /* (19) Given parameter is invalid */
    FR_DISK_FULL
} FRESULT;

#define	FA_READ				0x01
#define	FA_WRITE			0x02
#define	FA_OPEN_EXISTING	0x00
#define	FA_CREATE_NEW		0x04
#define	FA_CREATE_ALWAYS	0x08
#define	FA_OPEN_ALWAYS		0x10
#define	FA_OPEN_APPEND		0x30

#endif

const char* trs_fs_get_error(int error);
int8_t trs_fs_open(const char* path, uint8_t mode);
int8_t trs_fs_read(uint8_t fd, void* buf, uint16_t bytes_to_read, uint16_t* bytes_read);
int8_t trs_fs_write(uint8_t fd, void* buf, uint16_t bytes_to_write, uint16_t* bytes_written);
int8_t trs_fs_close(uint8_t fd);

#endif
