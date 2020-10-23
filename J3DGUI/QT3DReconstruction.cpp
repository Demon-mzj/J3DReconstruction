#include "QT3DReconstruction.h"
using namespace std;
QT3DReconstruction::QT3DReconstruction(QWidget *parent)
	: QMainWindow(parent)
{
	ui.setupUi(this);

	QTimer *timer = new QTimer(this); //this Ϊparent��, ��ʾ��ǰ����

	connect(timer, SIGNAL(timeout()), this, SLOT(timerSlot()));

	timer->start(500); // 1000����, ���� 1 ��
	setWindowFlags(windowFlags()& ~Qt::WindowMaximizeButtonHint);
	setFixedSize(this->width(), this->height());
	PlyIO* ply = new PlyIO("");


}


void QT3DReconstruction::on_actionMatchFeature_triggered()
{
	dlgmf.exec();
}

void QT3DReconstruction::on_actionSFM_triggered()
{
	dlgsfm.exec();
}


LPCWSTR QT3DReconstruction::LPSTRtoLPCWSTR(char* szStr)
{
	WCHAR wszClassName[256];
	memset(wszClassName, 0, sizeof(wszClassName));
	MultiByteToWideChar(CP_ACP, 0, szStr, strlen(szStr) + 1, wszClassName,
		sizeof(wszClassName) / sizeof(wszClassName[0]));
	return wszClassName;
}


void QT3DReconstruction::on_action_viewMVS_triggered()
{

}

void QT3DReconstruction::on_action_addSensorWidth_triggered()
{
	dlgasw.exec();
}

//void QT3DReconstruction::on_action_viewPLY_triggered()
//{
//	QMessageBox::information(NULL, "�ݲ�֧��ply��ʽ�������j3dģ��", "����", QMessageBox::Yes, NULL);
//	return;
//	QString fileName = QFileDialog::getOpenFileName(NULL, "ViewPLY", ".", "*.ply");
//	if (fileName == "")
//		return;
//	//QMessageBox::information(NULL, "", filename, QMessageBox::Yes, NULL);
//
//	QByteArray buf = fileName.toLatin1(); // must
//
//	PlyIO* ply = new PlyIO(buf.data());
//	if (false == ply->open()) {
//		ui.textBrowser->insertPlainText("\nģ�ͼ���ʧ��");
//		ui.textBrowser->update();
//		return;
//	}
//	delete Global::ply;
//	Global::ply = ply;
//	//ui.J3DViewerWidget->setPly();
//
//	//QProcess myProcess(this);
//	//QString program = "J3DViewer.exe";
//	//QStringList arguments;
//	//arguments << fileName;
//	//myProcess.startDetached(program, arguments, NULL, NULL);
//
//}

void QT3DReconstruction::on_actionDenseCloud_triggered()
{
	dlgdense.exec();
}

void QT3DReconstruction::on_action_reconstrctMesh_triggered()
{
	dlgrm.exec();
}

void QT3DReconstruction::timerSlot()
{

	if (Global::GetProcessidFromName("J3DEngine.exe") != 0)
	{
		Global::connectEngine();
		if (ui.label_engine->text() != u8"�ɹ����ӵ�J3DEngine ")
		{
			ui.textBrowser->insertPlainText(u8"��J3DEngine�ɹ���������\n");
			QPalette pa;
			pa.setColor(QPalette::WindowText, Qt::green);
			ui.label_engine->setPalette(pa);
			ui.label_engine->setText(u8"�ɹ����ӵ�J3DEngine ");
		}
	}
	else
	{
		if (ui.label_engine->text() != u8"δ���ӵ�J3DEngine ")
		{
			ui.textBrowser->insertPlainText(u8"��J3DEngineʧȥ���ӣ�֮�����³�������\n");
			QPalette pa;
			pa.setColor(QPalette::WindowText, Qt::red);
			ui.label_engine->setPalette(pa);
			ui.label_engine->setText(u8"δ���ӵ�J3DEngine ");
		}
	}


	if (Global::tasking)
	{
		Global::getProcessMsg();
		qDebug("%d %d %d", Global::process, Global::processProject, Global::processState);
		if (Global::process == PROCESSCLOSE)
		{
			ifstream cmdCache;
			cmdCache.open(("C:\\ProgramData\\J3DEngine\\cmdCache.tmp"), ios::in | ios::_Nocreate);
			std::string temp;
			getline(cmdCache, temp);
			if (temp == "sfmandsfp")
			{
				QString fileName = Global::sfmOutputDir + "/SparseCloud.j3d";
				if (fileName == "")
				{
					QMessageBox::information(NULL, u8"ʧ��", "��j3d�ļ�ʧ�ܣ�����·���Ƿ���ȷ ", QMessageBox::Ok, QMessageBox::Ok);
					return;

				}
				openJ3DView(fileName);
			}
			else if (temp == "densifypointcloud") {
				QString fileName = Global::densifyWorkingDir + "/DenseCloud.j3d";
				if (fileName == "")
				{
					QMessageBox::information(NULL, u8"ʧ��", "��j3d�ļ�ʧ�ܣ�����·���Ƿ���ȷ ", QMessageBox::Ok, QMessageBox::Ok);
					return;

				}
				openJ3DView(fileName);
			}
			else if (temp == "reconstructmesh") {
				QString fileName = Global::reconstructMeshWorkingDir + "/TIN_Mesh.j3d";
				if (fileName == "")
				{
					QMessageBox::information(NULL, u8"ʧ��", "��j3d�ļ�ʧ�ܣ�����·���Ƿ���ȷ ", QMessageBox::Ok, QMessageBox::Ok);
					return;

				}
				openJ3DView(fileName);
			}
			else if (temp == "texturemesh") {
				QString fileName = Global::reconstructMeshWorkingDir + "/TEXTURE_Mesh.j3d";
				if (fileName == "")
				{
					QMessageBox::information(NULL, u8"ʧ��", "��j3d�ļ�ʧ�ܣ�����·���Ƿ���ȷ ", QMessageBox::Ok, QMessageBox::Ok);
					return;

				}
				openJ3DView(fileName);
			}
			QMessageBox::information(NULL, u8"���", u8"������ɣ� ", QMessageBox::Ok, QMessageBox::Ok);
			Global::tasking = false;
			ui.label_process->setText(u8"�ȴ����� ");
			ui.progressBar->setValue(0);
			return;
		}
		else if (Global::process == PROCESSERROR)
		{
			QMessageBox::information(NULL, u8"ʧ��", "����ʧ�ܣ�����������־��J3DEngine��ѯ ", QMessageBox::Ok, QMessageBox::Ok);
			Global::tasking = false;
			ui.label_process->setText(u8"�ȴ����� ");
			ui.progressBar->setValue(0);
			return;
		}
		else if (Global::processProject < 1)
		{
			ui.label_process->setText(u8"�ȴ����� ");
			ui.progressBar->setValue(0);
			return;
		}
		else
		{

			switch (Global::processProject)
			{
			case LISTIMAGES:
			{
				ui.label_process->setText(u8"��������У�����ͼƬ ");
				ui.progressBar->setValue(Global::processState);
				break;
			}
			case COMPUTEFEATURES:
			{
				ui.label_process->setText(u8"��������У����������� ");
				ui.progressBar->setValue(Global::processState);
				break;
			}
			case MATCHFEATURES:
			{
				ui.label_process->setText(u8"��������У�ƥ�������� ");
				ui.progressBar->setValue(Global::processState);
				break;
			}
			case SFM:
			{
				ui.label_process->setText(u8"��������У���ά�ؽ�_SFM ");
				ui.progressBar->setValue(Global::processState);
				break;
			}
			case SFP:
			{
				ui.label_process->setText(u8"��������У���ά�ؽ�_SFP ");
				ui.progressBar->setValue(Global::processState);
				break;
			}
			case COLORED:
			{
				ui.label_process->setText(u8"��������У�������ɫ ");
				ui.progressBar->setValue(Global::processState);
				break;
			}
			case SPARSECLOUD:
			{
				ui.label_process->setText(u8"��������У�����ϡ����� ");
				ui.progressBar->setValue(Global::processState);
				break;
			}
			case DENSE:
			{
				ui.label_process->setText(u8"��������У������ܼ����� ");
				ui.progressBar->setValue(Global::processState);
				break;
			}
			case REMESH:
			{
				ui.label_process->setText(u8"��������У�����������ģ�� ");
				ui.progressBar->setValue(Global::processState);
				break;
			}
			case REFINE:
			{
				ui.label_process->setText(u8"��������У��Ż�������ģ�� ");
				ui.progressBar->setValue(Global::processState);
				break;
			}
			case TEXTURE:
			{
				ui.label_process->setText(u8"��������У���������ӳ�� ");
				ui.progressBar->setValue(Global::processState);
				break;
			}
			default:
			{
				ui.label_process->setText(u8"�ȴ����� ");
				ui.progressBar->setValue(0);
				break;
			}

			}


		}
	}
}


void QT3DReconstruction::on_action_triggered() //textureMesh
{
	dlgtm.exec();
}

void QT3DReconstruction::on_actionopen_mvs_file_triggered()
{
	QString fileName = QFileDialog::getOpenFileName(NULL, "ViewJ3D", ".", "J3D Model Format(*.j3d);;Stanford Polygon File Format(*.ply);;All Files(*.*)");
	if (fileName == "")
	{
		QMessageBox::information(NULL, u8"ʧ��", u8"��j3d�ļ�ʧ�ܣ�����·���Ƿ���ȷ ", QMessageBox::Ok, QMessageBox::Ok);
		return;

	}
	openJ3DView(fileName);

}

bool QT3DReconstruction::openJ3DView(QString fileName)
{
	delete this->ui.widget;
	QString cmd = "J3DView.dll -k 2324 -i " + fileName;
	STARTUPINFO si = { sizeof(si) };
	PROCESS_INFORMATION pi;
	si.dwFlags = STARTF_USESHOWWINDOW;
	si.wShowWindow = true;
	QFile Processcache("C:\\ProgramData\\J3DEngine\\ViewerCache.tmp");
	if (!Processcache.open(QIODevice::WriteOnly | QIODevice::Text | QIODevice::Truncate))
	{
		QMessageBox::information(NULL, u8"ʧ��", u8"�򿪻����ļ�ʧ�ܣ�����Ȩ��    ", QMessageBox::Ok, QMessageBox::Ok);
		return false;
	}
	Processcache.write("1");
	Processcache.close();
	if (!CreateProcess(
		NULL,
		(LPSTR)cmd.toStdWString().c_str(),
		NULL,
		NULL,
		FALSE,
		CREATE_NEW_CONSOLE,
		NULL,
		NULL, &si, &pi))
	{
		QMessageBox::information(NULL, u8"ʧ��", u8"��j3d�ļ�ʧ�ܣ�Viewer�����ļ������� ", QMessageBox::Ok, QMessageBox::Ok);
		return false;
	}
	time_t tm = time(NULL);
	QPalette pa;

	while (!Global::CheckViewerMsg()) {

		pa.setColor(QPalette::WindowText, Qt::yellow);
		ui.label_engine->setPalette(pa);
		ui.label_engine->setText(u8"���ڴ�J3Dģ���ļ� ");
		if (time(NULL) - tm > 60) {
			QMessageBox::information(NULL, u8"ʧ��", u8"��j3d�ļ�ʧ�ܣ��볢���ù���Ա���������� ", QMessageBox::Ok, QMessageBox::Ok);
			WinExec("taskkill /f /im J3DView.dll", SW_HIDE);
			return false;
		}
	}
	this->ui.widget = new mvsviewer(1, this->ui.centralWidget);
	this->ui.widget->setObjectName(QString::fromUtf8("widget"));
	this->ui.widget->setGeometry(QRect(10, 70, 1361, 661));
	this->ui.widget->show();
	return true;
}

void QT3DReconstruction::on_action_fullauto_triggered()
{
	
	dlgfa.exec();
}
