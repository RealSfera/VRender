/*
 *  Copyright (C) 2012-2013 Evgeny Panov
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

#define VERSION STRINGIFY(1.0.3)
#define MAGIC_NUMBER 0xE0E0A1B9

MainWindow::MainWindow(QWidget *parent) :
	QMainWindow(parent),
	ui(new Ui::MainWindow)
{
	ui->setupUi(this);
	
	build_help_dialog = new HelpDialog(this);
	build_help_dialog->setWindowTitle(QString::fromUtf8("Справка по построению"));
	QFile *build_lang_xhtml = new QFile(":/build_help");
	if(!build_lang_xhtml->exists()) {
		ERROR_MSG(":/build_help not exist\n");
		QMessageBox::critical(this, QString::fromUtf8("Ошибка"),
							   QString::fromUtf8("Ошибка при инициализации справки по построению!"));
		exit(-99);
	}
	build_lang_xhtml->open(QFile::ReadOnly);
	QString build_help_text = QString::fromUtf8(build_lang_xhtml->readAll().data());
	build_lang_xhtml->close();
	build_help_dialog->set_html_text(build_help_text);
	
	program_help_dialog = new HelpDialog(this);
	program_help_dialog->setWindowTitle(QString::fromUtf8("Справка по программе"));
	
	QFile *prog_help_xhtml = new QFile(":/prog_help");
	if(!prog_help_xhtml->exists()) {
		ERROR_MSG(":/prog_help not exist\n");
		QMessageBox::critical(this, QString::fromUtf8("Ошибка"),
							  QString::fromUtf8("Ошибка инициализации при справки по программе!"));
		exit(-99);
	}
	prog_help_xhtml->open(QFile::ReadOnly);
	QString prog_help_text = QString::fromUtf8(prog_help_xhtml->readAll().data());
	prog_help_xhtml->close();
	program_help_dialog->set_html_text(prog_help_text);
	
	main_gl_window = new GLWindow(this);

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
	
	// устанавливаем изо-уровень
	main_gl_window->set_isolevel_begin(ui->isolevel_value_begin->value());
	main_gl_window->set_isolevel_end(ui->isolevel_value_end->value());
	main_gl_window->set_isolevel_step(ui->isolevel_value_step->value());

	// получаем текущее значение изо-уровня и устанавливаем его в UI
	if(ui->isolevel_animate_box->isChecked())
		ui->isolevel_value->setValue(main_gl_window->get_isolevel());

	// получаем текущий угол источника света и устанавливаем его в UI
	if(ui->light_rot_auto->isChecked())
		ui->light_rot_angle->setValue(main_gl_window->get_light_angle());
}

void MainWindow::on_build_start_clicked()
{
	// устанавливаем текст функции, параметры volume и запускаем построитель
	if(!ui->build_function_text->toPlainText().isEmpty()) {
		
		QMessageBox *builder_msg = new QMessageBox(this);
		QPushButton *button = builder_msg->addButton("Stop", QMessageBox::ActionRole);
		builder_msg->setWindowTitle(QString::fromUtf8("Построение..."));
		builder_msg->setText(QString::fromUtf8("Идёт построение скалярного поля, подождите..."));
		
		builder_msg->show();
		qApp->processEvents();
		
		main_gl_window->set_grid_size(vec3ui(ui->grid_size_value_x->value(), ui->grid_size_value_y->value(), ui->grid_size_value_z->value()));
		main_gl_window->set_volume_size(vec3ui(ui->volume_size_value_x->value(), ui->volume_size_value_y->value(), ui->volume_size_value_z->value()));

		ui->grid_size_value_x->setMaximum(ui->volume_size_value_x->value());
		ui->grid_size_value_y->setMaximum(ui->volume_size_value_y->value());
		ui->grid_size_value_z->setMaximum(ui->volume_size_value_z->value());

		bool flag_is_cancle = false;

		if( main_gl_window->set_function_text(ui->build_function_text->toPlainText().toAscii().data()) ) {

			// создаём воркера, который будет работать в отдельном потоке
			// воркер запускает процедуру построения скалярного поля
			// это позволяет выполнять остановку построения, а также исправление надоедливого зависания программы


			BuildWorker worker(main_gl_window->get_volume_size(), main_gl_window->get_grid_size());
			QThread thread;

			worker.moveToThread(&thread);

			QObject::connect(&thread, SIGNAL(started()),	&worker, SLOT(process()));
			QObject::connect(&worker, SIGNAL(finished()),	&thread, SLOT(quit()));

			thread.start();

			// пока поток работает, обрабатываем события, ждём нажатие клавиши для остановки
			while(thread.isRunning()) {

				qApp->processEvents();

				if(!flag_is_cancle && (builder_msg->clickedButton() == button)) {
					flag_is_cancle = true;
					main_gl_window->stop_building();
				}

			}

			main_gl_window->update_render();

		}
		
		qApp->processEvents();
		if(!flag_is_cancle)
			builder_msg->close();
		
	} else {
		QMessageBox::critical(this, QString::fromUtf8("Ошибка"), QString::fromUtf8("Поле с функцией пустое"));
	}
}

void MainWindow::on_about_program_action_triggered()
{
	QMessageBox::about(this, QString::fromUtf8("О программе..."),
							 QString::fromUtf8("VRender - это программа для построения и визуализации скалярных полей\n\n" \
							 "Версия: "VERSION"\n" \
							 "Автор: Евгений 'Sfera' Панов\n" \
							 "Сайт проекта: https://github.com/Sfera2/VRender\n\n" \
							 "Лицензия: GPL v3 (см. файл LICENSE)\n"));
	
}

void MainWindow::on_light_rot_angle_valueChanged(int)
{
	if(!ui->light_rot_auto->isChecked())
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
	if(!ui->isolevel_animate_box->isChecked())
		main_gl_window->set_isolevel(ui->isolevel_value->value());
}

void MainWindow::on_build_help_action_triggered()
{
	build_help_dialog->show();
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
								  QString::fromUtf8("Ошибка при экспортировании данных текущего объекта!"));
			if(buffer)
				free(buffer);
			return;
		}
		
		if( !filename.endsWith(".obj", Qt::CaseInsensitive) ) {
			filename += ".obj";
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

		QMessageBox::information(this,
								 QString::fromUtf8("Экспорт в Wavefront"),
								 QString::fromUtf8("Объект успешно экспортирован в файл."));
	}
}

void MainWindow::on_volume_export_action_triggered()
{
	QString filename = QFileDialog::getSaveFileName(this,
													QString::fromUtf8("Экспорт скалярного поля (.vvol)"),
													"",
													QString::fromUtf8("VRender Volume (*.vvol)"));

	if(filename != "") {
		float *volume = NULL;

		vector3ui size;
		render_get_current_volume(&volume, &size);

		if(!volume) {
			QMessageBox::critical(this,
								  QString::fromUtf8("Ошибка экспорта"),
								  QString::fromUtf8("Ошибка при экспортировании данных скалярного поля!"));
			return;
		}

		if(!filename.endsWith(".vvol", Qt::CaseInsensitive)) {
			filename += ".vvol";
		}

		QFile file(filename);

		if(!file.open(QIODevice::WriteOnly)) {
			QMessageBox::critical(this,
								  QString::fromUtf8("Ошибка экспорта"),
								  QString::fromUtf8("Ошибка при открытии файла для экспорта!"));
			return;
		}

		// структура:
		// MAGIC_NUMBER
		// VRender
		// Версия текстом
		// Размер (будет на единицу больше): size.x size.y size.z
		// Данные

		QDataStream data_stream(&file);
		data_stream << (quint32) MAGIC_NUMBER;
		data_stream << QString("VRender");
		data_stream << QString(VERSION);
		data_stream << size.x; data_stream << size.y; data_stream << size.z;
		data_stream.writeRawData((const char*) volume, sizeof(float) * size.x*size.y*size.z);
		file.close();

		QMessageBox::information(this,
								 QString::fromUtf8("Экспорт скалярного поля"),
								 QString::fromUtf8("Скалярноное поле успешно экспортировано в файл."));
	}
}

void MainWindow::on_volume_import_action_triggered()
{
	QString filename = QFileDialog::getOpenFileName(this,
													QString::fromUtf8("Импорт скалярного поля (.vvol)"),
													"",
													QString::fromUtf8("VRender Volume (*.vvol)"));

	if(filename != "") {
		if(!filename.endsWith(".vvol", Qt::CaseInsensitive)) {
			filename += ".vvol";
		}


		QFile file(filename);

		if(!file.open(QIODevice::ReadOnly)) {
			QMessageBox::critical(this,
								  QString::fromUtf8("Ошибка импорта"),
								  QString::fromUtf8("Ошибка при открытии файла для импорта!"));
			return;
		}

		// структура:
		// MAGIC_NUMBER
		// VRender
		// Версия текстом
		// Размер: size.x size.y size.z
		// Данные

		QDataStream data_stream(&file);

		quint32 magic = 0;
		data_stream >> magic;

		if(magic != (quint32) MAGIC_NUMBER) {
			QMessageBox::critical(this,
								  QString::fromUtf8("Ошибка импорта"),
								  QString::fromUtf8("Неверный формат файла. [1]"));
			file.close();
			return;
		}

		QString vstring;
		data_stream >> vstring;
		if(vstring != QString("VRender")) {
			QMessageBox::critical(this,
								  QString::fromUtf8("Ошибка импорта"),
								  QString::fromUtf8("Неверный формат файла. [2]"));
			file.close();
			return;
		}

		QString version;
		data_stream >> version;

		if(version != QString(VERSION)) {

			bool is_found = false;

			// проверяем на совместимость с прошлыми версиями

			if(version == QString("1.0.1")) is_found = true;
			else if(version == QString("1.0.2")) is_found = true;
			//else if(version == QString("1.0.3")) is_found = true;

			if(!is_found) {
				QMessageBox::critical(this,
									  QString::fromUtf8("Ошибка импорта"),
									  QString::fromUtf8("Неверная версия импортируемого файла."));
				file.close();
				return;
			}
		}

		vector3ui size;
		data_stream >> size.x; data_stream >> size.y; data_stream >> size.z;

		float *volume = NULL;
		volume = new float[size.x*size.y*size.z];

		data_stream.readRawData((char*) volume, sizeof(float) * size.x*size.y*size.z);
		file.close();

		size = vec3ui_sub_c(size, 1); // т.к. размер на единицу больше
		render_set_external_volume(volume, size);
		delete [] volume;

		ui->volume_size_value_x->setValue(size.x);
		ui->volume_size_value_y->setValue(size.y);
		ui->volume_size_value_z->setValue(size.z);
		main_gl_window->set_volume_size(size);

		ui->build_function_text->setPlainText("");

		main_gl_window->update_render();

		QMessageBox::information(this,
							   QString::fromUtf8("Импорт скалярного поля"),
								QString::fromUtf8("Скалярное поле успешно импортировано из файла."));
	}
}
