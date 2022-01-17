add_library(Reflect SHARED 
    source/windows/msf.c
    source/windows/pdb.c
    source/YACIL.c
    source/GrowAbleStream.c)
target_include_directories(Reflect PUBLIC include/)