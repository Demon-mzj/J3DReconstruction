#include "Dialog_ReconstructMesh.h"
#include "ui_dialog_reconstructmesh.h"

Dialog_ReconstructMesh::Dialog_ReconstructMesh(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::Dialog_ReconstructMesh)
{
    ui->setupUi(this);
}

Dialog_ReconstructMesh::~Dialog_ReconstructMesh()
{
    delete ui;
}

void Dialog_ReconstructMesh::on_btn_OK_clicked()
{
	if (Global::GetProcessidFromName("J3DEngine.exe") == 0)
	{
		QMessageBox::critical(this, "错误 ", "未找到J3DEngine进程 ", QMessageBox::Ok, QMessageBox::Ok);
		return;
	}
	else
		Global::connectEngine();


	if (ui->lineEdit_inputDir->text() == "")
	{
		QMessageBox::critical(this, "错误 ", "未输入密集点云结果路径 ", QMessageBox::Ok, QMessageBox::Ok);
		return;
	}
	if (ui->lineEdit_OutputDir->text() == "")
	{
		QMessageBox::critical(this, "错误 ", "未输入输出路径 ", QMessageBox::Ok, QMessageBox::Ok);
		return;
	}

	Global::reconstructMeshInputDir = ui->lineEdit_inputDir->text() + "/DenseCloud.j3d";
	Global::reconstructMeshOutputDir = ui->lineEdit_OutputDir->text() + "/TIN_Mesh.j3d";
	Global::reconstructMeshWorkingDir = ui->lineEdit_OutputDir->text();

	_mkdir("C:\\ProgramData\\J3DEngine");

	QFile cmdcache("C:\\ProgramData\\J3DEngine\\cmdCache.tmp");

	if (cmdcache.open(QIODevice::WriteOnly | QIODevice::Text | QIODevice::Truncate))
	{
		QString head = "reconstructmesh\n";
		cmdcache.write(head.toUtf8());
		cmdcache.write(Global::reconstructMeshInputDir.toUtf8());
		cmdcache.write("\n");
		cmdcache.write(Global::reconstructMeshWorkingDir.toUtf8());
		cmdcache.write("\n");
		cmdcache.write(Global::reconstructMeshOutputDir.toUtf8());
		cmdcache.write("\n");
		cmdcache.close();
		QMessageBox::information(NULL, "完成", "配置完成 ", QMessageBox::Yes, NULL);
		PostThreadMessageA(Global::engineTid, CMD_RECONSTRUCTMESH, 0, 0);
		Global::tasking = true;
		this->close();
	}
	else
	{
		QMessageBox::information(NULL, "错误", "无法访问缓存文件，请检查权限 ", QMessageBox::Yes, NULL);
	}

}

void Dialog_ReconstructMesh::on_btn_CANCEL_clicked()
{
	this->close();
}

void Dialog_ReconstructMesh::on_pushButton_browseInputDir_clicked()
{
	Global::reconstructMeshInputDir = QFileDialog::getExistingDirectory(this, "浏览密集点云文件目录 ", "", NULL);
	ui->lineEdit_inputDir->setText(Global::reconstructMeshInputDir);
}

void Dialog_ReconstructMesh::on_pushButton_browseOutputDir_clicked()
{
	Global::reconstructMeshOutputDir = QFileDialog::getExistingDirectory(this, "浏览输出三角网模型文件目录 ", "", NULL);
	ui->lineEdit_OutputDir->setText(Global::reconstructMeshOutputDir);
}
