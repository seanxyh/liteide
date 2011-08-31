# USE .subdir AND .depends !
# OTHERWISE PLUGINS WILL BUILD IN WRONG ORDER (DIRECTORIES ARE COMPILED IN PARALLEL)

TEMPLATE  = subdirs

SUBDIRS = \
    welcome \
    liteenv \
    litebuild \
    liteeditor \
    litefind    \
    makefileproject \
    profileproject  \
    filebrowser \
    golangast \
    golangfmt \
    golangcode \
    golangdoc \
#    codesearch \
#    qsqleditor \
