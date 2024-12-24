#include "os_filesystem.h"
#include <stdio.h>

#ifdef OS_LINUX
#define __USE_XOPEN2K8 1
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <errno.h>
#include <dirent.h>
#include <fcntl.h>
#include "os_time.h"
#include "../log/log.h"

internal String Fs_PathJoin(Allocator allocator, String path, String segment)
{
  return StrJoin(allocator, path, StrLit("/"), segment);
}

internal FsErrors Fs_CreateDir(Allocator allocator, String directory_path)
{
  char *path = CstrFromStr(allocator, directory_path);
  return Fs_CreateDirCstr(path);
}

internal FsErrors Fs_RemoveDir(Allocator allocator, String directory_path)
{
  char *path = CstrFromStr(allocator, directory_path);
  return Fs_RemoveDirCstr(path);
}

internal bool Fs_DirExists(Allocator allocator, String directory_path)
{
  char *path = CstrFromStr(allocator, directory_path);
  return Fs_DirExistsCstr(path);
}

internal bool Fs_PathExists(Allocator allocator, String path)
{
  char *p = CstrFromStr(allocator, path);
  return Fs_PathExistsCstr(p);
}

internal bool Fs_IsDir(Allocator allocator, String path)
{
  char *p = CstrFromStr(allocator, path);
  return Fs_IsDirCstr(p);
}

internal FsErrors Fs_ReadDir(Allocator allocator, String directory_path, ArrayString *array_str)
{
  char *p = CstrFromStr(allocator, directory_path);
  return Fs_ReadDirCstr(allocator, p, array_str);
}

internal bool Fs_IsFile(Allocator allocator, String path)
{
  char *p = CstrFromStr(allocator, path);
  return Fs_IsFileCstr(p);
}

internal FsErrors Fs_RemoveFile(Allocator allocator, String path)
{
  char *p = CstrFromStr(allocator, path);
  return Fs_RemoveFileCstr(p);
}

internal String Fs_ReadFileFull(Allocator allocator, String path)
{
  char *p = CstrFromStr(allocator, path);
  return Fs_ReadFileFullCstr(allocator, p);
}

internal FsErrors Fs_WriteToFile(Allocator allocator, String path, String content)
{
  char *p = CstrFromStr(allocator, path);
  char *c = CstrFromStr(allocator, content);
  return Fs_WriteToFileCstr(p, c, content.size);
}

internal u64 Fs_LastModifiedTime(Allocator allocator, String path)
{
  char *p = CstrFromStr(allocator, path);
  return Fs_LastModifiedTimeCstr(p);
}

internal FsErrors Fs_CreateDirCstr(char *folder_name)
{
  FsErrors err = FsErrors_None;
  if (mkdir(folder_name, 0755) != 0)
  {
    switch (errno)
    {
    case EACCES:
      err = FsErrors_NoCreatePermission;
      break;
    case EEXIST:
      err = FsErrors_FolderAlreadyExists;
      break;
    case ENAMETOOLONG:
      err = FsErrors_NameIsTooLong;
      break;
    case ENOENT:
      err = FsErrors_ComponentOfPathDoesntExist;
      break;
    case ENOSPC:
      err = FsErrors_NoSpaceLeft;
      break;
    case ENOTDIR:
      err = FsErrors_NotADirectory;
      break;
    // case EROFS:
    //   printf("Error: The file system is read-only for '%s'.\n", folder_name);
    //   break;
    // case ENOTDIR:
    //   printf("Error: A component of the path '%s' is not a directory.\n", folder_name);
    //   break;
    // case ELOOP:
    //   printf("Error: Too many symbolic links encountered in '%s'.\n", folder_name);
    //   break;
    // case EIO:
    //   printf("Error: I/O error occurred while creating '%s'.\n", folder_name);
    //   break;
    // case EFAULT:
    //   printf("Error: Bad address for '%s'.\n", folder_name);
    //   break;
    // case EMLINK:
    //   printf("Error: Too many links in parent directory of '%s'.\n", folder_name);
    //   break;
    default:
      err = FsErrors_GeneralFolderCreationError;
      Errorf("Unknown error while creating '%s' (errno: %d).", folder_name, errno);
      break;
    }
  }
  return err;
}

internal FsErrors Fs_RemoveDirCstr(char *folder_name)
{
  FsErrors err = FsErrors_None;
  if (rmdir(folder_name) != 0)
  {
    switch (errno)
    {
    case EACCES:
      err = FsErrors_NoRemovePermission;
      break;
    case EBUSY:
      err = FsErrors_FolderIsInUse;
      break;
    case ENOTEMPTY:
    case EEXIST: // Older systems may use EEXIST instead of ENOTEMPTY
      err = FsErrors_FolderIsNotEmpty;
      break;
    case ENOENT:
      err = FsErrors_FolderDoesntExist;
      break;
    // case EROFS:
    //   printf("Error: Folder '%s' is on a read-only file system.\n", folder_name);
    //   break;
    case ENOTDIR:
      err = FsErrors_NotADirectory;
      break;
    // case ELOOP:
    //   printf("Error: Too many symbolic links in '%s'.\n", folder_name);
    //   break;
    case ENAMETOOLONG:
      err = FsErrors_NameIsTooLong;
      break;
    case EPERM:
      err = FsErrors_NoRemovePermission;
      break;
    // case EIO:
    //   printf("Error: I/O error occurred while removing '%s'.\n", folder_name);
    //   break;
    default:
      err = FsErrors_GeneralFolderRemovalError;
      Errorf("Unknown error while removing '%s' (errno: %d).\n", folder_name, errno);
      break;
    }
  }
  return err;
}

internal bool Fs_DirExistsCstr(char *folder_name)
{
  bool        exists = false;
  struct stat st;
  if (stat(folder_name, &st) == 0 && S_ISDIR(st.st_mode))
  {
    exists = true;
  }
  return exists;
}

internal bool Fs_PathExistsCstr(char *path)
{
  bool        exists = false;
  struct stat st;
  if (stat(path, &st) == 0)
  {
    exists = true;
  }
  return exists;
}

internal bool Fs_IsDirCstr(char *path)
{
  return Fs_DirExistsCstr(path);
}

internal FsErrors Fs_ReadDirCstr(Allocator allocator, char *directory_path, ArrayString *array_str)
{
  FsErrors err = FsErrors_None;
  DIR     *dir = opendir(directory_path);
  if (dir == NULL)
  {
    err = FsErrors_GeneralFolderOpenError;
  }
  else
  {
    struct dirent *entry;
    char           path[1024];
    while ((entry = readdir(dir)) != NULL)
    {
      if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0)
      {
        continue;
      }
      snprintf(path, sizeof path, "%s/%s", directory_path, entry->d_name);

      String s = StrFromCstrClone(allocator, path, strlen(path));
      ArrayString_Push(allocator, array_str, s);
    }
    closedir(dir);
  }
  return err;
}

internal bool Fs_IsFileCstr(char *path)
{
  bool        res = false;
  struct stat statbuf;
  if (stat(path, &statbuf) == 0)
  {
    if (S_ISREG(statbuf.st_mode))
    {
      res = true;
    }
  }
  return res;
}

internal FsErrors Fs_RemoveFileCstr(char *path)
{
  FsErrors err = FsErrors_None;
  if (unlink(path) != 0)
  {
    switch (errno)
    {
    case EACCES:
      err = FsErrors_NoRemovePermission;
      break;
    case EBUSY:
      err = FsErrors_FileIsInUse;
      break;
    case ENOENT:
      err = FsErrors_FileDoesntExist;
      break;
    default:
      err = FsErrors_GeneralFileRemovalError;
      break;
    }
  }
  return err;
}

internal String Fs_ReadFileFullCstr(Allocator allocator, char *path)
{
  String res  = {0};
  FILE  *file = fopen(path, "rb");
  if (file != NULL)
  {
    if (fseek(file, 0, SEEK_END) != 0)
    {
      fclose(file);
    }
    else
    {
      i64 file_size = ftell(file);
      if (file_size == -1)
      {
        fclose(file);
      }
      else
      {
        rewind(file);
        res.data      = Alloc(u8, (u64)file_size);
        u64 read_size = fread(res.data, 1, (u64)file_size, file);
        if (read_size != (u64)file_size)
        {
          Free(res.data, (u64)file_size);
          fclose(file);
        }
        else
        {
          res.size = (u64)file_size;
        }
      }
    }
    fclose(file);
  }
  return res;
}

internal FsErrors Fs_WriteToFileCstr(char *path, char *buffer, u64 buffer_size)
{
  FsErrors err = FsErrors_None;
  int      fd  = open(path, O_WRONLY | O_CREAT | O_TRUNC, 0644);
  if (fd == -1)
  {
    err = FsErrors_GeneralFileOpenError;
  }
  else
  {
    ssize_t bytes_written = write(fd, buffer, buffer_size);
    if (bytes_written == -1)
    {
      err = FsErrors_GeneralFileWriteError;
    }
    close(fd);
  }
  // FILE    *file = fopen(path, "w");
  // if (file == NULL)
  // {
  //   err = FsErrors_GeneralFileOpenError;
  // }
  // else
  // {
  //   if (fputs(content, file) == EOF)
  //   {
  //     err = FsErrors_GeneralFileWriteError;
  //   }
  //   fclose(file);
  // }
  return err;
}

internal u64 Fs_LastModifiedTimeCstr(char *path)
{
  u64         res = 0;
  struct stat file_stat;
  if (stat(path, &file_stat) == 0)
  {
    res = file_stat.st_mtim.tv_nsec + Seconds(file_stat.st_mtim.tv_sec);
  }
  return res;
}

#else
#error Filesystem interface is not implemented on this OS
#endif