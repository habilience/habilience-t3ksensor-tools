mkdir release

set USERAPP=release\UserApp

mkdir %USERAPP%
copy T3kCfg\release\T3kCfg.exe %USERAPP% /Y
mkdir %USERAPP%\Languages
copy T3kCfg\languages\*.txt %USERAPP%\Languages /Y
mkdir %USERAPP%\config
copy T3kCfg\config\config.ini %USERAPP%\config /Y
copy T3kCfg\config\gestureprofiles.ini %USERAPP%\config /Y

copy T3kCfgFE\release\T3kCfgFE.exe release /Y
copy T3kCmd\release\T3kCmd.exe release /Y
copy T3kSoftlogic\release\T3kSoftlogic.exe release /Y
copy T3kUpgrade\release\T3kUpgrade.exe release /Y