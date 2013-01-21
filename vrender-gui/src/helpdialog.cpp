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

#include "helpdialog.h"
#include "ui_helpdialog.h"

HelpDialog::HelpDialog(QWidget *parent) :
	QDialog(parent),
	ui(new Ui::HelpDialog)
{
	ui->setupUi(this);
}

HelpDialog::~HelpDialog()
{
	delete ui;
}

void HelpDialog::on_close_button_clicked()
{
	close();
}

void HelpDialog::set_plain_text(const QString text)
{
	ui->main_text_edit->setPlainText(text);
}

void HelpDialog::set_html_text(const QString text)
{
	ui->main_text_edit->setText(text);
}

