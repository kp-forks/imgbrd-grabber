#include <QSettings>
#include <QDir>
#include <QDirIterator>
#include <QCryptographicHash>
#include <QMessageBox>
#include "md5-fix.h"
#include "functions.h"
#include "ui_md5-fix.h"

md5Fix::md5Fix(QWidget *parent) : QDialog(parent), ui(new Ui::md5Fix)
{
	ui->setupUi(this);

	QSettings settings(savePath("settings.ini"), QSettings::IniFormat);
	ui->lineFolder->setText(settings.value("Save/path").toString());
	ui->lineFilename->setText(settings.value("Save/filename").toString());
	ui->progressBar->hide();

	resize(size().width(), 0);
}

md5Fix::~md5Fix()
{
	delete ui;
}

void md5Fix::on_buttonCancel_clicked()
{
	emit rejected();
	close();
}

void md5Fix::on_buttonStart_clicked()
{
	ui->buttonStart->setEnabled(false);

	// Check that directory exists
	QDir dir(ui->lineFolder->text());
	if (!dir.exists())
	{
		error(this, tr("Ce dossier n'existe pas."));
		ui->buttonStart->setEnabled(true);
		return;
	}

	// Make sure the input is valid
	if (!ui->radioForce->isChecked() && !ui->lineFilename->text().contains("%md5%"))
	{
		error(this, tr("Si vous voulez récupérer le MD5 depuis le nom de fichier, vous devez include le token %md5% dans celui-ci."));
		ui->buttonStart->setEnabled(true);
		return;
	}

	// Get all files from the destination directory
	typedef QPair<QString,QString> QStringPair;
	QVector<QStringPair> files = QVector<QStringPair>();
	QDirIterator it(dir, QDirIterator::Subdirectories);
	while (it.hasNext())
	{
		it.next();
		if (!it.fileInfo().isDir())
		{
			int len = it.filePath().length() - dir.absolutePath().length() - 1;
			files.append(QPair<QString,QString>(it.filePath().right(len), it.filePath()));
		}
	}

	if (files.count() > 0)
	{
		// Show progresss bar
		ui->progressBar->setValue(0);
		ui->progressBar->setMaximum(files.size());
		ui->progressBar->show();

		// Open MD5 file
		QFile f(savePath("md5s.txt"));
		if (!f.open(QFile::WriteOnly | QFile::Truncate))
		{
			error(this, tr("Impossible d'ouvrir le fichier de MD5."));
			ui->progressBar->hide();
			ui->buttonStart->setEnabled(true);
			return;
		}

		// Parse all files
		for (QStringPair file : files)
		{
			QString md5 = "";
			if (ui->radioForce->isChecked())
			{
				QFile fle(file.second);
				fle.open(QFile::ReadOnly);
				md5 = QCryptographicHash::hash(fle.readAll(), QCryptographicHash::Md5).toHex();
			}
			else
			{
				QRegExp regx("%([^%]*)%");
				QString reg = QRegExp::escape(ui->lineFilename->text());
				int pos = 0, cur = 0, id = -1;
				while ((pos = regx.indexIn(ui->lineFilename->text(), pos)) != -1)
				{
					pos += regx.matchedLength();
					reg.replace(regx.cap(0), "(.+)");
					if (regx.cap(1) == "md5")
					{ id = cur; }
					cur++;
				}
				QRegExp rx(reg);
				rx.setMinimal(true);
				pos = 0;
				while ((pos = rx.indexIn(file.first, pos)) != -1)
				{
					pos += rx.matchedLength();
					md5 = rx.cap(id + 1);
				}
			}
			if (!md5.isEmpty())
			{ f.write(QString(md5 + file.second + "\n").toUtf8()); }
			ui->progressBar->setValue(ui->progressBar->value() + 1);
		}
		f.close();
	}

	// Hide progresss bar
	ui->progressBar->hide();
	ui->progressBar->setValue(0);
	ui->progressBar->setMaximum(0);

	ui->buttonStart->setEnabled(true);

	QMessageBox::information(this, tr("Terminé"), tr("%n MD5 chargé(s)", "", files.count()));
}