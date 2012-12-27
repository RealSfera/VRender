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

#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QtGui>
#include "glwindow.h"
#include "helpdialog.h"

namespace Ui {
	class MainWindow;
}

class MainWindow : public QMainWindow
{
		Q_OBJECT
		
	public:
		explicit MainWindow(QWidget *parent = 0);
		~MainWindow();
		
	private slots:
		void on_build_start_clicked();
		void on_exit_action_triggered() {qApp->quit();}
		void on_about_program_action_triggered();
		void on_light_rot_auto_clicked();
		void on_light_rot_angle_valueChanged(int arg1);
		void on_light_rot_auto_toggled(bool checked);
		void on_isolevel_animate_box_toggled(bool checked);
		void on_isolevel_value_valueChanged(double arg1);
		
		void on_build_help_action_triggered();
		
		void on_antialiasing_box_clicked();
		
		void on_multithreading_box_toggled(bool checked);
		
	private:
		Ui::MainWindow *ui;
		GLWindow *main_gl_window;
		HelpDialog *help_dialog;
		bool antialiasing_checked;
		int timer_id;
		
		void timerEvent(QTimerEvent *event);
		void closeEvent(QCloseEvent *) {qApp->quit();}
};

#endif // MAINWINDOW_H
