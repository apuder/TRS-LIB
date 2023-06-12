
set(src lib/font.c lib/form.c lib/header.c lib/key.c lib/list.c lib/menu.c
        lib/screen.c lib/window.c lib/screen.c)

idf_component_register(SRCS ${src}
                       INCLUDE_DIRS "lib/include")
