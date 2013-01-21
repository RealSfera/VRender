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

#ifndef HELPDIALOG_H
#define HELPDIALOG_H

#include <QDialog>

namespace Ui {
	class HelpDialog;
}

class HelpDialog : public QDialog
{
		Q_OBJECT
		
	public:
		explicit HelpDialog(QWidget *parent = 0);
		~HelpDialog();
		
		void set_plain_text(const QString text);
		void set_html_text(const QString text);
		
	private slots:
		void on_close_button_clicked();
		
	private:
		Ui::HelpDialog *ui;
};

#endif // HELPDIALOG_H
