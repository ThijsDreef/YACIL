#pragma once
#include "YACILTypes.h"

/**
 * Load a YACILModule from DLL/SO info, module name without debug information can be passed
 * Tries to load a debug assocaited file before stored information.
 * Priority is: .pdb/.dSYM/.DWARF -> .BIN
*/
struct YACILModule* load_from_debug(char* name);

/**
 * Loads a YACIL module from an exported file
*/
struct YACILModule* load_from_export(char* name);

/**
 * Dumps a module to a file on disk to load later on so no debug information has to be shipped.
*/
void save_module(struct YACILModule module, const char* name);

struct YACILEnum* find_enum_by_name(struct YACILModlue* module, const char* name);
struct YACILEnumMember** find_enum_members(struct YACILEnum* value);
