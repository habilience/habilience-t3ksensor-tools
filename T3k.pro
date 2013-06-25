TEMPLATE = subdirs
SUBDIRS = \
    external/quazip \
    T3kCfg \
    T3kSoftlogic \
    T3kCfgFE \
    T3kCmd \
    T3kUpgrade
CONFIG += ordered

T3kCfg.depends = external/quazip
