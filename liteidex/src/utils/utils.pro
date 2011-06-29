# USE .subdir AND .depends !
# OTHERWISE PLUGINS WILL BUILD IN WRONG ORDER (DIRECTORIES ARE COMPILED IN PARALLEL)

TEMPLATE  = subdirs
CONFIG   += ordered

SUBDIRS = \
    fileutil \
    mimetype \
    browsereditor \
    extension \
    textoutput \
    symboltreeview \
    modelproject \
    wordapi \
    processex \
    colorstyle \


