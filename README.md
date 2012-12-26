VRender
=======

VRender - это программа для визуализации скалярных полей, построенных при помощи задаваемой функции

Компиляция
----------

### Ubuntu

1. Устанавливаем Qt4 и QtOpenGL: `sudo apt-get install qt4-dev-tools libqt4-opengl-dev`
2. Устанавливаем CMake: `sudo apt-get install cmake`
3. Кладём исходник в любую папку, например, в `~/vrender` и переходим в неё: `cd ~/vrender/`
4. Создаём папку, куда скомпилится libvrender: `mkdir libvrender-build` (лучше использовать имя `libvrender-build`, иначе нужно править `vrender-gui/vrender-gui.pro`) и переходим в неё: `cd libvrender-build`
5. Создаём Makefile с помощью CMake: `cmake ../libvrender`
6. Компилируем libvrender: `make`
7. Переходим обратно в vrender: `cd ../`
8. Теперь создаём папку для vrender-gui: `mkdir vrender-gui-build` и переходим в неё: `cd vrender-gui-build`
9. Создаём Makefile уже с помощью qmake: `qmake ../vrender-gui`
10. Компилируем vrender-gui: `make`
11. Готово! Теперь в папке `vrender-gui-build` лежит программа `vrender-gui`. Приятного пользования!

### Windows
Work In Progress
