#include "YACIL.h"
#ifdef _WIN32 || _WIN64
    #include "windows/pdb.h"
    #define LOAD_YACIL(name) load_YACIL_from_pdb(name);
#endif

struct YACILModule* load_from_debug(const char* name) {
    return LOAD_YACIL(name);
    #ifndef LOAD_YACIL
        printf("YACIL not implemented for your system");
        return {};
    #endif
}

struct YACILModule* load_from_export(const char* name) { }
void save_module(struct YACILModule module, const char* name) { }

struct YACILEnum* find_enum_by_name(struct YACILModule* module, const char* name) {
    for (size_t i = 0; i < module->count; i++) {
        struct YACILHeader* header = module->types[i];
        if (header->kind != ENUM) continue;
        struct YACILEnum* enumerator = header;
        if (strcmp(enumerator->name, name) == 0) return enumerator;
    }
    return NULL;
}

struct YACILEnumMember** find_enum_members(struct YACILEnum* value) {
    struct YACILEnumMember** values = malloc(sizeof(struct YACILEnumMember*) * value->numEnumerators);

    size_t offset = 0;
    uint8_t* stream = value;
    offset += sizeof(struct YACILEnum);
    offset += strlen(stream + offset) + 1;
    for (size_t i = 0; i < value->numEnumerators; i++) {
        values[i] = stream + offset;
        offset += sizeof(struct YACILEnumMember);
        offset += strlen(values[i]->name) + 1;
    }

    return values;
}