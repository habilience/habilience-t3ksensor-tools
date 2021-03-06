#ifndef DEFINESTRING_H
#define DEFINESTRING_H

const char cstrT3000Help[] = "T3k series command interpreter.\r\n\r\nT3kCmd [/t] [/T] [/c:Command]\r\n\r\n/t\tDisplays the system time of T3k series.\r\n/T\tDisplays the operation time of PC.\r\n/c:Command\tCarries out the command specified by starting.\r\n";
const char cstrTitleOut[] = "T3kCmd [Version %1]\r\nCopyright (c) Habilience. All rights reserved.\r\n\r\n";
#ifdef Q_OS_WIN
const char cstrHelpOut[] = "/?\tProvides Help information for T3k series commands.\r\nlicense\tAbout license.\r\ncls\tClears the screen.\r\nexit\tQuits this program (T3kCmd.exe).\r\nload_nv [/f] [>FILENAME]\r\n\tDisplays the system values.\r\n\t/f\tDisplays the factorial values.\r\n\t>FILENAME\tSaves the values to PC.\r\ninstant_mode=[C][M]\r\n\tSelects contents to displaying.\r\n\tC\tDisplays Commands and Responses.\r\n\tM\tDisplays Messages.\r\nhelp\tProvides Help information for T3k series commands.\r\n\r\n";
#else
const char cstrHelpOut[] = "/?\tProvides Help information for T3k series commands.\r\nlicense\tAbout license.\r\nclear\tClears the screen.\r\nexit\tQuits this program (T3kCmd.exe).\r\nload_nv [/f] [>FILENAME]\r\n\tDisplays the system values.\r\n\t/f\tDisplays the factorial values.\r\n\t>FILENAME\tSaves the values to PC.\r\ninstant_mode=[C][M]\r\n\tSelects contents to displaying.\r\n\tC\tDisplays Commands and Responses.\r\n\tM\tDisplays Messages.\r\nhelp\tProvides Help information for T3k series commands.\r\n\r\n";
#endif
const char cstrExitOut[] = "Good-bye.\r\n\r\n";

const char cstrHelp0[] = "/?";
const char cstrHelp1[] = "help";
const char cstrLicense[] = "license";
#ifdef Q_OS_WIN
const char cstrCls[] = "cls";
#else
const char cstrCls[] = "clear";
#endif
const char cstrExit[] = "exit";
const char cstrGetNv[] = "load_nv";

const char cszInstantMode[] = "instant_mode=";
const char cszIsNotConnected[] = "T3k series is not connected.\r\n";
const char cstrGetNvString[] = "Displays the system values of T3k series.\r\n\r\n";
const char cszGetNv_End[] = "\r\nThe end of a command.\r\n";

#endif // DEFINESTRING_H
