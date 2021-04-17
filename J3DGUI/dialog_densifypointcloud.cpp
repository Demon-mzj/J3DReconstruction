#include "dialog_densifypointcloud.h"
#include "ui_dialog_densifypointcloud.h"

Dialog_DensifyPointCloud::Dialog_DensifyPointCloud(QWidget *parent) :
	QDialog(parent),
	ui(new Ui::Dialog_DensifyPointCloud)
{
	ui->setupUi(this);
}

Dialog_DensifyPointCloud::~Dialog_DensifyPointCloud()
{
	delete ui;
}

void Dialog_DensifyPointCloud::on_pushButton_browseInputDir_clicked()
{
	Global::densifyInputDir = QFileDialog::getExistingDirectory(this, u8"����ؽ�����ļ��� ", "", NULL);
	ui->lineEdit_inputDir->setText(Global::densifyInputDir);
}

void Dialog_DensifyPointCloud::on_pushButton_browseOutputDir_clicked()
{
	Global::densifyOutputDir = QFileDialog::getExistingDirectory(this, u8"�����������ļ�Ŀ¼ ", "", NULL);
	ui->lineEdit_OutputDir->setText(Global::densifyOutputDir);
}

void Dialog_DensifyPointCloud::on_btn_OK_clicked()
{
	if (Global::GetProcessidFromName("J3DEngine.exe") == 0)
	{
		QMessageBox::critical(this, u8"���� ", u8"δ�ҵ�J3DEngine���� ", QMessageBox::Ok, QMessageBox::Ok);
		return;
	}
	else
		Global::connectEngine();


	if (ui->lineEdit_inputDir->text() == "")
	{
		QMessageBox::critical(this, u8"���� ", u8"δ�����ؽ����·�� ", QMessageBox::Ok, QMessageBox::Ok);
		return;
	}
	if (ui->lineEdit_OutputDir->text() == "")
	{
		QMessageBox::critical(this, u8"���� ", u8"δ�������·�� ", QMessageBox::Ok, QMessageBox::Ok);
		return;
	}

	Global::densifyInputDir = ui->lineEdit_inputDir->text() + "/SparseCloud.J3D";
	Global::densifyOutputDir = ui->lineEdit_OutputDir->text() + "/DenseCloud.J3D";
	Global::densifyWorkingDir = ui->lineEdit_OutputDir->text();

	_mkdir("C:\\ProgramData\\J3DEngine");

	QFile cmdcache("C:\\ProgramData\\J3DEngine\\cmdCache.tmp");

	if (cmdcache.open(QIODevice::WriteOnly | QIODevice::Text | QIODevice::Truncate))
	{
		QString head = "densifypointcloud\n";
		cmdcache.write(head.toUtf8());
		cmdcache.write(Global::densifyInputDir.toUtf8());
		cmdcache.write("\n");
		cmdcache.write(Global::densifyWorkingDir.toUtf8());
		cmdcache.write("\n");
		cmdcache.write(Global::densifyOutputDir.toUtf8());
		cmdcache.write("\n");
		cmdcache.close();
		QMessageBox::information(NULL, u8"���", u8"������� ", QMessageBox::Yes, NULL);
		PostThreadMessageA(Global::engineTid, CMD_EXPORTDENSECLOUD, 0, 0);
		Global::tasking = true;
		this->close();
	}
	else
	{
		QMessageBox::information(NULL, u8"����", u8"�޷����ʻ����ļ�������Ȩ�ޣ���ʹ�ù���Ա������� ", QMessageBox::Yes, NULL);
	}

}

void Dialog_DensifyPointCloud::on_btn_CANCEL_clicked()
{
	this->close();
}
