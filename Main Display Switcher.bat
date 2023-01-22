@echo off
SETLOCAL EnableDelayedExpansion
IF EXIST active.txt (
    set /p display=< active.txt
    del active.txt
    IF "!display!" == "1" (
        nircmd.exe setprimarydisplay 2
        echo | set /P ="2"> "active.txt" 
    ) ELSE (
        nircmd.exe setprimarydisplay 1
        echo | set /P ="1"> "active.txt" 
    )
) ELSE (
    nircmd.exe setprimarydisplay 1
    echo | set /P ="1"> "active.txt" 
)