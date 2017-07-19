rmdir /s /q Bin\Win32\Debug\intermediate
rmdir /s /q Bin\Win32\Release\intermediate
rmdir /s /q Bin\x64\Debug\intermediate
rmdir /s /q Bin\x64\Release\intermediate
cd Bin
for /r /d %%i in (*) do del "%%i\*.ilk"
for /r /d %%i in (*) do del "%%i\*.pdb"
for /r /d %%i in (*) do del "%%i\*.exp"
for /r /d %%i in (*) do del "%%i\*.lib"
cd ../Source
for /r /d %%i in (*) do del "%%i\*.user"
for /r /d %%i in (*) do del "%%i\*.aps"
cd ..
rmdir /s /q Source\ipch
del Source\*.sdf
del /A:h Source\*.suo
pause

