# USE .subdir AND .depends !
# OTHERWISE PLUGINS WILL BUILD IN WRONG ORDER (DIRECTORIES ARE COMPILED IN PARALLEL)

TEMPLATE  = subdirs

SUBDIRS = \
    welcome \
    makefileproject \
    profileproject  \
    liteeditor \
    liteshell \
    golangast \
    litebuild \
    golangfmt \
    golangcode \
    filebrowser
