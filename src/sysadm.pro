TEMPLATE = subdirs
CONFIG += recursive

SUBDIRS+= library binary server

#Make sure to list the library as a requirement for the others (for parallellized builds)
binary.depends = library
server.depends = library

