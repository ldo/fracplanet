TARGET = fracplanet
TEMPLATE = app

CONFIG+= qt stl exceptions release  # debug/release
QT += opengl

HEADERS += $$files(*.h)
SOURCES += $$files(*.cpp)
LIBS += -lboost_program_options -lGLU

DEFINES += QT_DLL

#######################################
# Version numbering.  VERSION_NUMBER=x.x.x should be set on qmake arguments.

QMAKE_CXXFLAGS_RELEASE += '-DFRACPLANET_VERSION="$$VERSION_NUMBER"'
QMAKE_CXXFLAGS_DEBUG   += '-DFRACPLANET_VERSION="$$VERSION_NUMBER"'
QMAKE_CXXFLAGS_RELEASE += '-DFRACPLANET_BUILD="$$VERSION_NUMBER (release build)"'
QMAKE_CXXFLAGS_DEBUG   += '-DFRACPLANET_BUILD="$$VERSION_NUMBER (debug build)"'

# qmake's library code can use this too (but only for shared libraries which we don't use)
VERSION=$$VERSION_NUMBER

#######################################
# Disable assertions in release version

QMAKE_CXXFLAGS_RELEASE += -DNDEBUG
QMAKE_CFLAGS_RELEASE += -DNDEBUG

######################################
# Other stuff:
# Disable implicit cast from QString to char*

QMAKE_CXXFLAGS_RELEASE += -DQT_NO_ASCII_CAST
QMAKE_CXXFLAGS_DEBUG += -DQT_NO_ASCII_CAST

######################################
# Pick up any dpkg-buildflags flags via environment, release only
# Not sure what happens if there are repeated/conflicting args
QMAKE_CXXFLAGS_RELEASE += $$(CPPFLAGS) $$(CXXFLAGS)
QMAKE_CFLAGS_RELEASE += $$(CPPFLAGS) $$(CFLAGS)
QMAKE_LFLAGS += $$(LDFLAGS)

######################################
# Hide those crufty moc_ files away

MOC_DIR = moc
OBJECTS_DIR = obj

