/*
 *  Copyright (C) 2012 Evgeny Panov
 *  This file is part of vrender-gui.
 *
 *  vrender-gui is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  vrender-gui is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with vrender-gui.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "glwindow.h"

#define VERSION STRINGIFY(0.8.5)

static const QString build_help_text = QString::fromUtf8("<h2><center>Основы языка построения</center></h2>" \
								 "<p>	Данный язык был специально разработан для упрощения его изучения и простоты работы с ним. За основу были взяты выражения из языка программирования C, а также одна конструкция из Фортрана и несколько специализированных (которые не встречаются в других языках и выражениях).</p>" \
												   "<p>	Он специально был построен, чтобы максимально быть похожим на обычные математические выражения.</p>" \
												   "<p>	В языке присутсвуют: выражения, переменные, функции, условия, циклы (сумма)</p>" \
												   "<p>	А теперь пройдемся по его основным элементам.</p>" \
												   "<p><h3>Алфавит языка:</h3></p>" \
												   "<p>	Можно использовать буквы латинского алфавита (a-z и A-Z), цифры (0-9), а также следующие спец-символы: '+',  '-',  '*',  '\\',  ',',  '?',  ':',  '.',  ';',  '(',  ')',  '!',  '=',  '>',  '&lt;' </p>" \
												   "<p><h3>Переменные:</h3></p>" \
												   "<p>	Как и в математике, в этом языке можно создавать переменные, присваивать им значения и затем использовать их в выражениях. И так, перед тем, как использовать переменную, нужно сначало её создать. Делается это так: \"a = 0;\" Таким образом Вы создали переменную 'a' и присвоили ей значение 0. Наверно Вы обратили внимание, что в конце стоит точка-с-запятой, это обязательный элемент, говорящий о конце выражения. Самое интересное, что можно написать дальше после точки-с-запятой следующее выражение, таким образом можно в одной строчке написать всю функцию построения, просто разделяя выражения точкой-с-запятой. </p>"\
												   "<p>	А теперь вернёмся к переменным. Имя переменной должно обязательно начинаться с буквы, а дальше можно использовать только буквы латинского алфавита и/или цифры. Длина имени ограничена 63 символами. При создании переменной, после знака присваивания (=), можно использовать любые выражения. Интересный момент, само присваивание (и создание) также можно производить внутри выражения, но сначало рассмотрим сами выражения.</p>" \
												   "<p><h3>Выражения:</h3></p>" \
												   "<p>	Выражение - это последовательность переменных, констант (чисел) и функций, соединённых между собой арифметическими, логическими и другими операциями.</p>" \
												   "<p>	Выражения чаще всего начинаются с присваивания, но можно писать выражения, начинающиеся с чего угодно, главное чтобы оно было синтаксически правильным и заканчивалось точкой-с-запятой. Также как и в математике можно писать такие вещи: \"a = 1 + 2;\" Как видите, всё просто. </p>"
												   "<p> В выражениях можно использовать следующие операции: ** (возведение в степень), + (сложение), - (вычитание), * (умножение), \\ (деление), > (больше), &lt; (меньше), >= (больше или равно), &lt;= (меньше или равно), == (равно), != (неравно). Логические операции(> &lt; >= &lt;= == !=) работают немного иначе, чем обычно. Например так: \" a = 1 > 2; \". Здесь a будет равно 0, так как 1 меньше 2, результат ложь (0), или такой пример: \" b = 3; a = b > 2; \", здесь a = 1, т.к. 3 больше 2, результат истина (1), таким образом результатом логического выражения является 1 или 0. Вот ещё пример: \" a = 3; b = 4; c = (a > 3) + 2 * (b > 2); \", где c будет равно 2.</p>" \
												   "<p>Обратите внимание: в выражениях могут присутствовать как целые числа, так и вещественные числа (с точкой). Так например 1 - целое число, 1.3 - вещественное, но не каких проверок типов в языке нет, таким образом в языке есть только вещественный тип и все целые числа преобразуются в вещественные. Также нужно учитывать, что в некоторых конструкциях (циклы, факториал...) используется только целая часть числа.</p>" \
												   "<p>	Более сложными элементами являются составные операции: += (сложить и присвоить), -= (вычесть и присвоить), *= (умножить и присвоить), /= (делить и присвоить). Работают они так: сначало вычисляется выражение справа от них, затем с результатом и значением переменной слева производится соотвествующая операция (+ - * /) и после чего полученное значение присваивается переменной слева. Пример: \" b = 2; b += 1; \", где b будет равно 3. </p>" \
												   "<p>	Также присутствуют унарные операции (работают над одним операндом): ! (факториал), - (унарный минус), + (унарный плюс). Пример: \" a = -2; b = +3; c = 4! + (a + b)!; \"</p>" \
												   "<p>	Для ведения порядка вычисления (задания приоритета вычисления) используются круглые скобки (). Например: \" a = 1+2*3; b = (1+2)*3; \". Здесь a будет равно 7, а b = 9, т.к. сначало вычислится (1+2), а потом результат умножится на 3. </p>" \
												   "<p><h3>Функции:</h3></p>" \
												   "<p>	Для вызова (и получения значения) функции требуется написать её имя, затем идёт открывающая скобка, за ней следуют аргументы, разделённые запятыми и затем ставится закрывающая скобка. Вот пример: \" x = 0; a = sin(x); \". Здесь a будет присвоенно значение sin(0), т.е. 0. В языке предусмотрен большой набор функций. Вот их список:</p>" \
												   "<p>sin(x) - синус</p>" \
												   "<p>cos(x) - косинус</p>" \
												   "<p>tan(x) - тангенс</p>" \
												   "<p>sqrt(x) - квадратный корень из x</p>" \
												   "<p>min(a, b) - минимальное из двух чисел</p>" \
												   "<p>max(a, b) - максимальное из двух чисел</p>" \
												   "<p>cerp(a, b, t) - косинусная интерполяция</p>" \
												   "<p>lerp(a, b, t) - линейная интерполяция</p>" \
												   "<p>radian(x) - перевести x в радианы</p>" \
												   "<p>degree(x) - перевести x в градусы</p>" \
												   "<p>clamp(x, a, b) - возвращает x приведённое к интервалу [a, b]</p>" \
												   "<p>abs(a) - абсолютное значение</p>" \
												   "<p>asin(x) - арксинус</p>" \
												   "<p>acos(x) - арккосинус</p>" \
												   "<p>ceil(x) - округление до ближайшего большего целого числа</p>" \
												   "<p>cosh(x) - гиперболический косинус</p>" \
												   "<p>sinh(x) - гиперболический синус</p>" \
												   "<p>tanh(x) - гиперболический тангенс</p>" \
												   "<p>exp(x) - экспонента (x степень)</p>" \
												   "<p>floor(x) - округление до ближайшего меньшего целого числа</p>" \
												   "<p>fmod(a, b) - вычисление остатка от деления нацело для чисел с плавающей точкой</p>" \
												   "<p>ln(x) - натуральный логарифм</p>" \
												   "<p>log10(x) - логарифм по основанию 10</p>" \
												   "<p>atan(x) - арктангенс</p>" \
												   "<p>cbrt(x) - кубический корень из x</p>" \
												   "<p>dot2(x1, y1, x2, y2) - скалярное произведение между векторами (x1, y1) и (x2, y2)</p>" \
												   "<p>distance3(x1, y1, z1, x2, y2, z2) - расстояние между точками (x1, y1, z1) и (x2, y2, z2)</p>" \
												   "<p>dot3(x1, y1, z1, x2, y2, z2) - скалярное произведение между векторами (x1, y1, z1) и (x2, y2, z2)</p>" \
												   "<p>length3(x, y, z) - длина вектора (x, y, z)</p>" \
												   "<p>distance2(x1, y1, x2, y2) - расстояние между точками (x1, y1) и (x2, y2)</p>" \
												   "<p>length2(x, y) - длина вектора (x, y)</p>" \
												   "<p>vnoise1(x) - численный шум в точке (x)</p>" \
												   "<p>vnoise2(x, y) - численный шум в точке (x, y)</p>" \
												   "<p>vnoise3(x, y, z) - численный шум в точке (x, y, z)\n</p>" \
												   "<p></p>" \
												   "<p><h3>Условия:</h3></p>" \
												   "<p>	В языке предусмотрена возможность создания разветвления, т. е. возможность изменения направления хода вычисления. Основным оператором для разветвлений является - тернарный оператор (условие) ? (выражение1) : (выражение2). Работает он следующим образом: сначало вычисляется условие, если оно истинно (не равно 0), то вычисляется выражение1, иначе вычисляется выражение2, результат вычисления выражения становится результатом всего разветвления. Пример: \" b = 3; c = cos(0); a = c > b ? c - b : b - c; \". Здесь a будет равно 2, т. к. c > b ложно (1 > 3), то вычисляется b - c (3 - 1).</p>" \
												   "<p></p>" \
												   "<p><h3>Циклы:</h3></p>" \
												   "<p>	Иногда возникает необходимость вычислить некоторое выражение несколько раз или просто вычислить сумму. Для это в языке предусмотрен оператор цикла a..b: выражение. Работает он так: сначало вычисляется a (это может быть выражением в скобках), затем b (тоже может быть выражением), после чего запускается цикличное вычисление выражения (abs(a - b) + 1)-раз, т.е. кол-во итераций равно разности между a и b плюс 1, не учитывая знак. Результаты вычислений суммируются и в конце эта сумма становится результатом всего цикла. Пример: \" b = 2; a = 0..3: b + 1; \". Здесь a будет равно 12, так как b + 1 будет просуммировано 4 раза. Есть очень интересный элемент: внутри цикла начинает действовать специальная переменная i, которая равна номеру итерации (прохода) цикла. Так например: \" b = 2; a = 0..3: (b+1)*i; \". Здесь a будет равно 18, т. к. в выражении (b+1)*i, i будет менятся от 0 до 3 и таким образом вычислится сумма произведений 3*i (3*0+3*1+3*2+3*3).</p>" \
												   "<p></p>" \
												   "<p></p>" \
												   "<p><h3>Встроенные переменные:</h3></p>" \
												   "<p>Так как данный язык создавался в первую очередь для построения скалярных полей, то в нём предусмотрены специальные переменные, которым в начале вычисления присваиваются значения. Переменные x, y, z являются встроенными и их значение равно координатам вычисляемой точки внутри скалярного поля. Их значения изменяются от 0 до (размер скалярного поля - 1). Эти переменные можно изменять, но это ни на что, не повлияет.</p>" \
												   "<p>Переменная d является самой основной, так как присвоенное ей значение становится значением в соотвествующей точке скалярного поля, т. е. она является выходной переменной. По-умолчанию она равна 0.</p>" \
												   "<p></p>" \
												   "<p></p>");

static const QString program_help_text = QString::fromUtf8("<h2><center>Справка по программе VRender "VERSION"</center></h2>");

MainWindow::MainWindow(QWidget *parent) :
	QMainWindow(parent),
	ui(new Ui::MainWindow)
{
	ui->setupUi(this);
	
	build_help_dialog = new HelpDialog(this);
	build_help_dialog->setWindowTitle(QString::fromUtf8("Справка по построению"));
	build_help_dialog->set_html_text(build_help_text);
	
	program_help_dialog = new HelpDialog(this);
	program_help_dialog->setWindowTitle(QString::fromUtf8("Справка по программе"));
	program_help_dialog->set_html_text(program_help_text);
	
	main_gl_window = new GLWindow(this);
	
	antialiasing_checked = true;

	if(QThread::idealThreadCount() >= 2) {
		ui->multithreading_box->setChecked(true);
		ui->threads_num_box->setValue(QThread::idealThreadCount());
	}

	ui->grid_size_value_x->setMaximum(ui->volume_size_value_x->value());
	ui->grid_size_value_y->setMaximum(ui->volume_size_value_y->value());
	ui->grid_size_value_z->setMaximum(ui->volume_size_value_z->value());

	ui->render_farme_layout->addWidget(main_gl_window);

	timer_id = startTimer(1);
}

MainWindow::~MainWindow()
{
	delete ui;
}

void MainWindow::timerEvent(QTimerEvent *)
{
	char text[32];
	
	// обновляем fps
	sprintf(text, "FPS: %i", main_gl_window->get_fps());
	ui->fps_label->setText(QString(text));

	// устанавливаем параметры освещения и материала
	main_gl_window->set_gamma_factor(ui->coef_gamma->value()/10.0f);
	main_gl_window->set_ambient_factor(ui->coef_ambient->value());
	main_gl_window->set_diffuse_factor(ui->coef_diffuse->value());
	main_gl_window->set_specular_factor(ui->coef_specular->value());
	main_gl_window->set_material_shininess(ui->spec_shininess->value());
	main_gl_window->set_material_color(vec3f( ui->mat_color_red->value()/255.0f, ui->mat_color_green->value()/255.0f, ui->mat_color_blue->value()/255.0f ),
									   vec3f( ui->mat_back_color_red->value()/255.0f, ui->mat_back_color_green->value()/255.0f, ui->mat_back_color_blue->value()/255.0f ));
	main_gl_window->set_light_color(vec3f( ui->light_color_red->value()/255.0f, ui->light_color_green->value()/255.0f, ui->light_color_blue->value()/255.0f ));
	main_gl_window->set_light_spec_color(vec3f( ui->spec_color_red->value()/255.0f, ui->spec_color_green->value()/255.0f, ui->spec_color_blue->value()/255.0f ));
	main_gl_window->set_light_rot_step(ui->light_rot_angle_step->value());
	
	// устанавливаем параметры камеры
	//main_gl_window->set_camera_step(ui->camera_step_spin->value());
	//main_gl_window->set_camera_move_speed(ui->camera_move_speed_spin->value());
	
	// устанавливаем изо-уровень
	main_gl_window->set_isolevel_begin(ui->isolevel_value_begin->value());
	main_gl_window->set_isolevel_end(ui->isolevel_value_end->value());
	main_gl_window->set_isolevel_step(ui->isolevel_value_step->value());
}

void MainWindow::on_build_start_clicked()
{
	// устанавливаем текст функции, параметры volume и запускаем построитель
	if(strcmp(ui->build_function_text->toPlainText().toAscii().data(), "")) {
		
		QMessageBox *builder_msg = new QMessageBox(this);
		builder_msg->setAttribute(Qt::WA_DeleteOnClose);
		builder_msg->setWindowModality(Qt::NonModal);
		builder_msg->setWindowTitle(QString::fromUtf8("Построение..."));
		builder_msg->setText(QString::fromUtf8("Идёт построение скалярного поля, подождите..."));
		
		builder_msg->show();
		qApp->processEvents();
		
		main_gl_window->set_grid_size(vec3ui(ui->grid_size_value_x->value(), ui->grid_size_value_y->value(), ui->grid_size_value_z->value()));
		main_gl_window->set_volume_size(vec3ui(ui->volume_size_value_x->value(), ui->volume_size_value_y->value(), ui->volume_size_value_z->value()));

		ui->grid_size_value_x->setMaximum(ui->volume_size_value_x->value());
		ui->grid_size_value_y->setMaximum(ui->volume_size_value_y->value());
		ui->grid_size_value_z->setMaximum(ui->volume_size_value_z->value());

		if( main_gl_window->set_function_text(ui->build_function_text->toPlainText().toAscii().data()) )
			main_gl_window->begin_generation();
		
		builder_msg->close();
		
	} else {
		QMessageBox::critical(this, QString::fromUtf8("Ошибка"), QString::fromUtf8("Поле с функцией пустое"));
	}
}

void MainWindow::on_about_program_action_triggered()
{
    
	QMessageBox::information(this, QString::fromUtf8("О программе..."), 
								   QString::fromUtf8("VRender - это программа для построения и визуализации скалярных полей\n\n" \
													 "Версия: "VERSION"\n" \
													 "Автор: Панов 'Sfera' Евгений\n\n" \
													 "Лицензия: GPL v3 (см. файл LICENSE)\n"));
	
}

void MainWindow::on_light_rot_angle_valueChanged(int)
{
    main_gl_window->set_light_angle(ui->light_rot_angle->value());
}

void MainWindow::on_light_rot_auto_toggled(bool checked)
{
	main_gl_window->set_light_animation(checked == true ? 1 : 0);
	if(checked) {
		ui->light_rot_angle->setEnabled(false);
		ui->light_rot_angle_step->setEnabled(true);
	} else {
		ui->light_rot_angle->setEnabled(true);
		ui->light_rot_angle_step->setEnabled(false);
	}
}

void MainWindow::on_light_rot_auto_clicked()
{
	
}

void MainWindow::on_isolevel_animate_box_toggled(bool checked)
{
    main_gl_window->set_isolevel_animation(checked == true ? 1 : 0);
	
	if(checked) {
		ui->isolevel_value_begin->setEnabled(true);
		ui->isolevel_value_end->setEnabled(true);
		ui->isolevel_value_step->setEnabled(true);
		ui->isolevel_value->setEnabled(false);
	} else {
		ui->isolevel_value_begin->setEnabled(false);
		ui->isolevel_value_end->setEnabled(false);
		ui->isolevel_value_step->setEnabled(false);
		ui->isolevel_value->setEnabled(true);
	}
}

void MainWindow::on_isolevel_value_valueChanged(double)
{
    main_gl_window->set_isolevel(ui->isolevel_value->value());
}

void MainWindow::on_build_help_action_triggered()
{
    build_help_dialog->show();
}

void MainWindow::on_antialiasing_box_clicked()
{
	QMessageBox message_box(this);
	
	message_box.setWindowTitle(QString::fromUtf8("Сглаживание"));
	message_box.setText(QString::fromUtf8("Внимание! Изменение сглаживания сбрасывает все настройки, в том числе и функцию! Продолжить?"));
	message_box.setStandardButtons(QMessageBox::No | QMessageBox::Yes);
	message_box.setDefaultButton(QMessageBox::No);
	
	int result = message_box.exec();
	
	if(result == QMessageBox::Yes) {
		antialiasing_checked = !antialiasing_checked;
		main_gl_window->set_antialiasing(antialiasing_checked);
	} else {
		ui->antialiasing_box->setChecked(antialiasing_checked);
	}
}

void MainWindow::on_multithreading_box_toggled(bool checked)
{
	ui->threads_num_box->setEnabled(checked);
	main_gl_window->set_number_of_threads( checked ? ui->threads_num_box->value() : 1 );
}

// Вызов диалога выбора цвета и установка выбранного цвета

void MainWindow::on_material_front_color_button_clicked()
{
	QColorDialog color_dialog(
				QColor::fromRgb(ui->mat_color_red->value(),
								ui->mat_color_green->value(),
								ui->mat_color_blue->value()), this);
	
	if(color_dialog.exec() == QDialog::Accepted) {
		
		QColor color = color_dialog.selectedColor();
		
		ui->mat_color_red->setValue(color.red());
		ui->mat_color_green->setValue(color.green());
		ui->mat_color_blue->setValue(color.blue());
		
		main_gl_window->set_material_color(vec3f( color.red()/255.0f, color.green()/255.0f, color.blue()/255.0f ),
										   vec3f( ui->mat_back_color_red->value()/255.0f, ui->mat_back_color_green->value()/255.0f, ui->mat_back_color_blue->value()/255.0f ));
	}
}

void MainWindow::on_light_color_button_clicked()
{
	QColorDialog color_dialog(
				QColor::fromRgb(ui->light_color_red->value(),
								ui->light_color_green->value(),
								ui->light_color_blue->value()), this);
	
	if(color_dialog.exec() == QDialog::Accepted) {
		
		QColor color = color_dialog.selectedColor();
		
		ui->light_color_red->setValue(color.red());
		ui->light_color_green->setValue(color.green());
		ui->light_color_blue->setValue(color.blue());
		
		main_gl_window->set_light_color(vec3f( color.red()/255.0f, color.green()/255.0f, color.blue()/255.0f ));  
	}
}

void MainWindow::on_specular_color_button_clicked()
{
	QColorDialog color_dialog(
				QColor::fromRgb(ui->spec_color_red->value(),
								ui->spec_color_green->value(),
								ui->spec_color_blue->value()), this);
	
	if(color_dialog.exec() == QDialog::Accepted) {
		
		QColor color = color_dialog.selectedColor();
		
		ui->spec_color_red->setValue(color.red());
		ui->spec_color_green->setValue(color.green());
		ui->spec_color_blue->setValue(color.blue());
		
		main_gl_window->set_light_spec_color(vec3f( color.red()/255.0f, color.green()/255.0f, color.blue()/255.0f ));  
	}
}

void MainWindow::on_material_back_color_button_clicked()
{
	QColorDialog color_dialog(
				QColor::fromRgb(ui->mat_back_color_red->value(),
								ui->mat_back_color_green->value(),
								ui->mat_back_color_blue->value()), this);
	
	if(color_dialog.exec() == QDialog::Accepted) {
		
		QColor color = color_dialog.selectedColor();
		
		ui->mat_back_color_red->setValue(color.red());
		ui->mat_back_color_green->setValue(color.green());
		ui->mat_back_color_blue->setValue(color.blue());
		
		main_gl_window->set_material_color(vec3f( ui->mat_color_red->value()/255.0f, ui->mat_color_green->value()/255.0f, ui->mat_color_blue->value()/255.0f ), 
										   vec3f( color.red()/255.0f, color.green()/255.0f, color.blue()/255.0f ));  
	}
}

void MainWindow::on_threads_num_box_valueChanged(int)
{
	main_gl_window->set_number_of_threads( ui->multithreading_box->isChecked() ? ui->threads_num_box->value() : 1 );
}


void MainWindow::on_grid_size_value_x_valueChanged(int)
{
	main_gl_window->set_grid_size(vec3ui(ui->grid_size_value_x->value(), ui->grid_size_value_y->value(), ui->grid_size_value_z->value()));
}
void MainWindow::on_grid_size_value_y_valueChanged(int)
{
	main_gl_window->set_grid_size(vec3ui(ui->grid_size_value_x->value(), ui->grid_size_value_y->value(), ui->grid_size_value_z->value()));
}
void MainWindow::on_grid_size_value_z_valueChanged(int)
{
	main_gl_window->set_grid_size(vec3ui(ui->grid_size_value_x->value(), ui->grid_size_value_y->value(), ui->grid_size_value_z->value()));
}

void MainWindow::on_program_help_action_triggered()
{
    program_help_dialog->show();
}

void MainWindow::on_obj_export_action_triggered()
{
	QString filename = QFileDialog::getSaveFileName(this, 
													QString::fromUtf8("Экспорт в Wavefront (.obj)"), 
													"", 
													QString::fromUtf8("Wavefront (*.obj)"));
	
	if(filename != "") {
		char *buffer = NULL;
		
		if(!render_export_obj(&buffer)) {
			QMessageBox::critical(this, 
								  QString::fromUtf8("Ошибка экспорта"), 
								  QString::fromUtf8("Ошибка экспортировании данных текущего объекта!"));
			if(buffer)
				free(buffer);
			return;
		}
		
		QFile file(filename);
		
		if(!file.open(QIODevice::WriteOnly)) {
			QMessageBox::critical(this, 
								  QString::fromUtf8("Ошибка экспорта"), 
								  QString::fromUtf8("Ошибка при открытии файла для сохранения!"));
			free(buffer);
			return;
		}
				
		QTextStream text_stream(&file);
		text_stream << buffer;
		text_stream.flush();
		file.close();
		
		free(buffer);
	}
}
