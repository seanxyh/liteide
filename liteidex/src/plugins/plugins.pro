# USE .subdir AND .depends !
# OTHERWISE PLUGINS WILL BUILD IN WRONG ORDER (DIRECTORIES ARE COMPILED IN PARALLEL)

TEMPLATE  = subdirs

SUBDIRS = \
    welcome \
    makefileproject \
    profileproject  \
    liteeditor \
    litebuild \
    filebrowser \
    codesearch \
    golangast \
    golangdoc \
    golangfmt \
    golangcode \
