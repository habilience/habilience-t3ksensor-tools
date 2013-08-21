TEMPLATE = subdirs
SUBDIRS = \
    external/quazip \
    T3kCfgFE \
    T3kUpgrade \
    T3kCfg \
    T3kSoftlogic \
    T3kCmd \

CONFIG += ordered

T3kCfg.depends = external/quazip
T3kUpgrade.depends = external/quazip
T3kCfgFE.depends = external/quazip
