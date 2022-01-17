#include "windows/pdb.h"
#include "YACIL.h"
#include <stdio.h>

int main() {
    struct YACILModule* sizes = load_from_debug("Sizes");
    printf("count: %d\n", sizes->count);
    struct YACILEnum* shapeVariants = find_enum_by_name(sizes, "ShapeVariant");
    if (shapeVariants == NULL) printf("No ENUM shape variant present in the compilation unit");
    struct YACILEnumMember** members = find_enum_members(shapeVariants);

    printf("found enum: %s\n", shapeVariants->name);
    for (size_t i = 0; i < shapeVariants->numEnumerators; i++) {
        printf("found member with name %s and value %x\n", members[i]->name, members[i]->value.data);
    }
    return 0;
}