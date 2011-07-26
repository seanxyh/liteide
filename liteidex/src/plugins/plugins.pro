# USE .subdir AND .depends !
# OTHERWISE PLUGINS WILL BUILD IN WRONG ORDER (DIRECTORIES ARE COMPILED IN PARALLEL)

TEMPLATE  = subdirs

SUBDIRS = \
    welcome \
    liteenv \
    litebuild \
    liteeditor \
    makefileproject \
    profileproject  \
    filebrowser \
    codesearch \
    qsqleditor \
    golangast \
    golangfmt \
    golangcode \
    golangdoc \
