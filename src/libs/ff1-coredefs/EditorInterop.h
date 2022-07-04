#pragma once

#include <stdint.h>

struct EditorInterop;

typedef char* (*EditorAllocatorFunc)(int64_t bytes);

typedef char* (*InvokeFunc)(const EditorInterop * ei);

// Command verbs used for EditorInterop.command
#define INVOKE_INIT "init"
#define INVOKE_GETEDITOR "geteditor"
#define INVOKE_GETALLEDITORS "getalleditors"
#define INVOKE_EDIT "edit"                     // ei.name = editor name
#define INVOKE_RCLICK "rclick"                 // ei.name = editor name, ei.params = hwndowner mousex mousey

struct EditorInterop
{
	EditorAllocatorFunc allocator;
	const char* projectini;
	const char* command;
	const char* name;
	const char* params;
};
