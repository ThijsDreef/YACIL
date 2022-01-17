#pragma once
#include "YACRLTypes.h"

/**
 * Load a YACRLModule from DLL/SO info, module name without debug information can be passed
 * Tries to load a debug assocaited file before stored information.
 * Priority is: .pdb/.dSYM/.DWARF -> .BIN
*/
struct YACRLModule* load_from_debug(char* name);

/**
 * Loads a YACRL module from an exported file
*/
struct YACRLModule* load_from_export(char* name);

/**
 * Dumps a module to a file on disk to load later on so no debug information has to be shipped.
*/
void save_module(struct YACRLModule module, const char* name);

struct YACRLEnum* find_enum_by_name(struct YACRLModlue* module, const char* name);
struct YACRLEnumMember** find_enum_members(struct YACRLEnum* value);
