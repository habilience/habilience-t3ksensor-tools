if exist T3kCfgFE.rsc del T3kCfgFE.rsc
.\ziputil\7za.exe a -tzip -plangzip T3kCfgFE.rsc ".\Languages\*.*" -mx5
copy /y T3kCfgFE.rsc .\debug\
copy /y T3kCfgFE.rsc .\release\
del T3kCfgFE.rsc