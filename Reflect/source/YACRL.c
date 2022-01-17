#include "YACRL.h"
#ifdef _WIN32 || _WIN64
    #include "windows/pdb.h"
    #define LOAD_YACRL(name) load_yacrl_from_pdb(name);
#endif

struct YACRLModule* load_from_debug(const char* name) {
    return LOAD_YACRL(name);
    #ifndef LOAD_YACRL
        printf("YACRL not implemented for your system");
        return {};
    #endif
}

struct YACRLModule* load_from_export(const char* name) { }
void save_module(struct YACRLModule module, const char* name) { }

struct YACRLEnum* find_enum_by_name(struct YACRLModule* module, const char* name) {
    for (size_t i = 0; i < module->count; i++) {
        struct YACRLHeader* header = module->types[i];
        if (header->kind != ENUM) continue;
        struct YACRLEnum* enumerator = header;
        if (strcmp(enumerator->name, name) == 0) return enumerator;
    }
    return NULL;
}

struct YACRLEnumMember** find_enum_members(struct YACRLEnum* value) {
    struct YACRLEnumMember** values = malloc(sizeof(struct YACRLEnumMember*) * value->numEnumerators);

    size_t offset = 0;
    uint8_t* stream = value;
    offset += sizeof(struct YACRLEnum);
    offset += strlen(stream + offset) + 1;
    for (size_t i = 0; i < value->numEnumerators; i++) {
        values[i] = stream + offset;
        offset += sizeof(struct YACRLEnumMember);
        offset += strlen(values[i]->name) + 1;
    }

    return values;
}