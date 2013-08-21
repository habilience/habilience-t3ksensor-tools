if exist .\debug\T3kCfgFE.rsc del .\debug\T3kCfgFE.rsc
if exist .\release\T3kCfgFE.rsc del .\release\T3kCfgFE.rsc
.\ziputil\7za.exe a -tzip -plangzip T3kCfgFE.rsc ".\Languages\*.*" -mx5
copy /y T3kCfgFE.rsc .\debug\
copy /y T3kCfgFE.rsc .\release\
del T3kCfgFE.rsc