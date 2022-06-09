set version=svn
set name=bobpp
set OBJ=%name%-%version%

rmdir /S /Q %OBJ%
md %OBJ%
md %OBJ%\lib
md %OBJ%\include
md %OBJ%\include\bobpp
md %OBJ%\include\bobpp\thr

copy ..\bobpp\*.h %OBJ%\include\bobpp
copy ..\bobpp\thr\*.h %OBJ%\include\bobpp\thr

copy Output\MingW\%name%.dll %OBJ%\lib
copy Output\MingW\lib%name%.a %OBJ%\lib
copy DevPackage %OBJ%\%OBJ%.DevPackage
