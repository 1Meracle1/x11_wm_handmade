#ifndef OS_FILESYSTEM
#define OS_FILESYSTEM

#include "os_defines.h"
#include "../containers/string.h"

typedef enum
{
  FsErrors_None                       = 0,
  FsErrors_NotADirectory              = 1,
  FsErrors_NoCreatePermission         = 2,
  FsErrors_NoRemovePermission         = 3,
  FsErrors_FolderAlreadyExists        = 4,
  FsErrors_FolderDoesntExist          = 5,
  FsErrors_FolderIsInUse              = 6,
  FsErrors_FolderIsNotEmpty           = 7,
  FsErrors_FileIsInUse                = 8,
  FsErrors_FileDoesntExist            = 9,
  FsErrors_NameIsTooLong              = 10,
  FsErrors_ComponentOfPathDoesntExist = 11,
  FsErrors_NoSpaceLeft                = 12,
  FsErrors_GeneralFolderCreationError = 13,
  FsErrors_GeneralFolderRemovalError  = 14,
  FsErrors_GeneralFolderOpenError     = 15,
  FsErrors_GeneralFileRemovalError    = 16,
  FsErrors_GeneralFileOpenError       = 17,
  FsErrors_GeneralFileWriteError      = 18,
} FsErrors;

internal String Fs_PathJoin(Allocator allocator, String path, String segment);

internal FsErrors Fs_CreateDir(Allocator allocator, String directory_path);
internal FsErrors Fs_RemoveDir(Allocator allocator, String directory_path);
internal bool     Fs_DirExists(Allocator allocator, String directory_path);
internal bool     Fs_PathExists(Allocator allocator, String path);
internal bool     Fs_IsDir(Allocator allocator, String path);

/*
Example:
  String root = StrLit(PROJECT_DIR);
  String       core_dir = Fs_PathJoin(allocator, root, StrLit("core"));
  FsErrors     err      = Fs_ReadDir(allocator, core_dir, &elements);
  if (err != FsErrors_None)
  {
    Errorf("Failed to read dir elements, err: %d, path: %s", err, core_dir);
  }
  Debugf("Retrieved %zu items from folder %s", elements.size, core_dir);
  for (u64 i = 0; i < elements.size; i += 1)
  {
    String elem_path = elements.data[i];
    bool   is_dir    = Fs_IsDir(allocator, elem_path);
    Debugf("%zuth element, is_dir: %d, path: %s", i, is_dir, elem_path);
  }
*/
internal FsErrors Fs_ReadDir(Allocator allocator, String directory_path, Array_String *array_str);

internal bool     Fs_IsFile(Allocator allocator, String path);
internal FsErrors Fs_RemoveFile(Allocator allocator, String path);

/*
Example:
  String wm_main_file = Fs_PathJoin(allocator, root, StrLit("wm/main.c"));
  String wm_main_cont = Fs_ReadFileFull(allocator, wm_main_file);
  Debugf("wm's main.c file:\n%s", wm_main_cont);
  Array_String lines = StrSplit(allocator, wm_main_cont, StrLit("\n"));
  Debugf("lines: %zu", lines.size);
  for (u64 i = 0; i < lines.size; i += 1)
  {
    Debugf("line: %zu, str: %.*s", i, StrFmtVal(lines.data[i]));
  }
*/
internal String Fs_ReadFileFull(Allocator allocator, String path);

internal FsErrors Fs_WriteToFile(Allocator allocator, String path, String content);
internal u64      Fs_LastModifiedTime(Allocator allocator, String path);

internal FsErrors Fs_CreateDirCstr(char *directory_path);
internal FsErrors Fs_RemoveDirCstr(char *directory_path);
internal bool     Fs_DirExistsCstr(char *directory_path);
internal bool     Fs_PathExistsCstr(char *path);
internal bool     Fs_IsDirCstr(char *path);
internal FsErrors Fs_ReadDirCstr(Allocator allocator, char *directory_path,
                                 Array_String *array_str);
internal bool     Fs_IsFileCstr(char *path);
internal FsErrors Fs_RemoveFileCstr(char *path);
internal String   Fs_ReadFileFullCstr(Allocator allocator, char *path);
internal FsErrors Fs_WriteToFileCstr(char *path, char *buffer, u64 buffer_size);
internal u64      Fs_LastModifiedTimeCstr(char *path);

#endif