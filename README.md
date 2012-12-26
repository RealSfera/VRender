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

1. Качаем Qt4 SDK c сайта Qt Project: http://qt-project.org/downloads
2. Устанавливаем Qt4 SDK
3. Кладём исходник в любую папку, например, в `C:/vrender/`
4. Запускаем QtCreator
5. В QtCreator открываем файл как проект: `C:/vrender/libvrender/CMakeLists.txt` (Файл - Открыть файл или проект...)
6. Откроется мастер, выбираем каталог сборки libvrender-build: `C:/vrender/libvrender-build` (папку нужно создать) и жмём Далее
7. Выбираем генератор: `Генератор для MinGW (Mingw as a GCC for Windows targets)`, жмём Запуск CMake и затем Завершить
8. Открывем файл как проект: `C:/vrender/vrender-gui/vrender-gui.pro`, в появившемся меню жмём Завершить
9. Заходим во вкладку Проекты, сверху выбираем vrender-gui, и потом Зависимости, здесь выбираем `vrender`
10. Жмём на зелёную стрелку Запустить (сначало произойдёт сборка, потом запуск)
11. Готово! Приятного пользования!
