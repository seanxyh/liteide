# USE .subdir AND .depends !
# OTHERWISE PLUGINS WILL BUILD IN WRONG ORDER (DIRECTORIES ARE COMPILED IN PARALLEL)

TEMPLATE  = subdirs

SUBDIRS = \
    welcome \
    makefileproject \
    profileproject  \
    liteeditor \
    golangast \
    litebuild \
    golangfmt \
#    luabuild \
#    golangbuild \
#    makefilebuild \

