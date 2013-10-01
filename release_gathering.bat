mkdir release

set USERAPP=release\UserApp

copy T3kCfg\release\T3kCfg.exe %USERAPP% /Y

copy T3kCfgFE\release\T3kCfgFE.exe release /Y
copy T3kCmd\release\T3kCmd.exe release /Y
copy T3kSoftlogic\release\T3kSoftlogic.exe release /Y
copy T3kUpgrade\release\T3kUpgrade.exe release /Y