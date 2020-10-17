#define _USE_MATH_DEFINES
#define NOMINMAX
#include <iostream>
#include <Windows.h>
#include "PresetConfigureImages.h"
#include "MatchImages.h"
#include "StructureFromMotion.h"
#include "StructureFromPoses.h"
#include "ExportSparseCloud.h"
#include <algorithm>
#include <direct.h>
#include <fstream>
#include "Global.h"
#include "MVSEngine.h"



HWND m_hWnd;
//"3285.55;0;1480.14;0;3292.21;2025.93;0;0;1");// EigenMatrixFormat"f;0;ppx;0;f;ppy;0;0;1"

std::string FileName_OutputDensifyCloud;

int STATE_RETURN;


bool saveTid()
{
	int tid = GetCurrentThreadId();
	ofstream file;
	locale loc = locale::global(locale(""));
	file.open("C:\\ProgramData\\J3DEngine\\ProgramCache.tmp", ios::out | ios::trunc);
	locale::global(loc);
	if (!file.is_open())
		return false;
	file << std::to_string(tid) << endl;
	file.close();
	return true;
}

VOID CALLBACK TimerProc(HWND hWnd, UINT uMsg, UINT_PTR idEvent, DWORD dwTime)
{
	m_hWnd = FindWindow("Qt5QWindowIcon", "J3DGUI");
	if (!saveTid() || !Global::saveProcess())
		std::cout << "д�뻺��ʧ�ܣ��������Ȩ�� " << endl;
}
void MsgProc(UINT msg, WPARAM wp, LPARAM lp)
{
	if (!m_hWnd) {
		return;
	}

	switch (msg) 
	{
	case CMD_MATCHFEATURES: {
		Global::process = PROCESSWORKING;
		Global::saveProcess();
		std::string imagesInputDir;
		std::string sensorWidthDataBaseDir = "./SenWidDB.txt";
		std::string matchesOutputDir;
		std::string EigenMatrix;// EigenMatrixFormat"f;0;ppx;0;f;ppy;0;0;1"
		std::string describerMethod;
		std::string featureQuality;
		bool upRight;
		bool forceCompute;
		std::string geometricModel;
		float distanceRatio;
		bool forceMatch;

		std::cout << "\n������У�MATCHFEATURES \n" << std::endl;
		ifstream cmdCache;
		cmdCache.open(("C:\\ProgramData\\J3DEngine\\cmdCache.tmp"), ios::in | ios::_Nocreate);
		if (!cmdCache)
		{
			std::cout << "����ʧ��,�޷���ȡ�������\n" << std::endl;
			Global::process = PROCESSERROR;
			break;
		}

		std::string temp;
		getline(cmdCache, temp);
		if (temp != "matchfeature")
		{
			std::cout << "����ʧ��,�޷���ȡ�������\n" << std::endl;
			Global::process = PROCESSERROR;
			break;
		}

		getline(cmdCache, temp);
		imagesInputDir = temp;

		getline(cmdCache, temp);
		matchesOutputDir = temp;

		getline(cmdCache, temp);
		EigenMatrix = temp;
		if (EigenMatrix == "NULL")
		{
			EigenMatrix = "";
		}

		getline(cmdCache, temp);
		describerMethod = temp;

		getline(cmdCache, temp);
		featureQuality = temp;

		getline(cmdCache, temp);
		if (temp == "0")
			upRight = false;
		else
			upRight = true;

		getline(cmdCache, temp);
		if (temp == "0")
			forceCompute = false;
		else
			forceCompute = true;

		getline(cmdCache, temp);
		geometricModel = temp;

		getline(cmdCache, temp);
		distanceRatio = atof(temp.c_str());

		getline(cmdCache, temp);
		if (temp == "0")
			forceMatch = false;
		else
			forceMatch = true;

		cmdCache.close();

		STATE_RETURN = LoadingImages(imagesInputDir, matchesOutputDir, sensorWidthDataBaseDir, EigenMatrix);
		if (STATE_RETURN == EXIT_FAILURE)
		{
			std::cout << "����ͼƬʧ��" << std::endl;
			Global::process = PROCESSERROR;
			break;
		}
		std::cout << "����ͼƬ�ɹ�" << std::endl;

		STATE_RETURN = GetFeatures(
			matchesOutputDir + "/sfm_data.json",
			matchesOutputDir,
			describerMethod,
			"",
			upRight,
			forceCompute);

		if (STATE_RETURN == EXIT_FAILURE)
		{
			std::cout << "��ȡ������Ϣʧ��" << std::endl;
			Global::process = PROCESSERROR;
			break;

		}
		std::cout << "��ȡ��������ɣ�׼����ʼƥ��������" << std::endl;

		STATE_RETURN = GetMatches(
			matchesOutputDir + "/sfm_data.json",
			matchesOutputDir,
			geometricModel,
			"",
			"AUTO",
			-1,
			2048,
			0U,
			distanceRatio, forceMatch);

		if (STATE_RETURN == EXIT_FAILURE)
		{
			std::cout << "ƥ��������Ϣʧ��" << std::endl;
			Global::process = PROCESSERROR;
			break;
		}
		std::cout << "ƥ�����" << std::endl;
		std::cout << "\n�������" << std::endl;
		Global::process = PROCESSCLOSE;
		
	}
	break;
	case CMD_SFMANDSFP: {
		Global::process = PROCESSWORKING;
		Global::saveProcess();
		std::string matchesDir, sfmOutputDir;
		int triangulationMethod, resectionMethod;
		std::cout << "\n������У�SFMANDSFP \n" << std::endl;
		ifstream cmdCache;
		cmdCache.open(("C:\\ProgramData\\J3DEngine\\cmdCache.tmp"), ios::in | ios::_Nocreate);
		if (!cmdCache)
		{
			std::cout << "����ʧ��,�޷���ȡ�������\n" << std::endl;
			Global::process = PROCESSERROR;
			break;
		}

		std::string temp;
		getline(cmdCache, temp);
		if (temp != "sfmandsfp")
		{
			std::cout << "����ʧ��,�޷���ȡ�������\n" << std::endl;
			Global::process = PROCESSERROR;
			break;
		}

		getline(cmdCache, temp);
		matchesDir = temp;

		getline(cmdCache, temp);
		sfmOutputDir = temp;

		getline(cmdCache, temp);
		triangulationMethod = atoi(temp.c_str());

		getline(cmdCache, temp);
		resectionMethod = atoi(temp.c_str());

		cmdCache.close();
		STATE_RETURN = StructureFromMotion(
			matchesDir + "\\sfm_data.json",
			matchesDir,
			"",
			sfmOutputDir,
			"",
			"",
			"ADJUST_ALL",
			3,
			false,
			false,
			triangulationMethod,
			resectionMethod);
		if (STATE_RETURN == EXIT_FAILURE)
		{
			std::cout << "SFM�ؽ�ʧ��" << std::endl;
			Global::process = PROCESSERROR;
			break;

		}
		std::cout << "���е�����ɫ" << std::endl;
		STATE_RETURN = PrintPointColors(sfmOutputDir + "/sfm_data.bin", sfmOutputDir + "/colored.ply");
		if (STATE_RETURN == EXIT_FAILURE)
		{
			std::cout << "SFM������ɫʧ��" << std::endl;
			Global::process = PROCESSERROR;
			break;
		}
		std::cout << "����SFP�ع�" << std::endl;
		STATE_RETURN = StructureFromPoses(
			sfmOutputDir + "/sfm_data.bin",
			matchesDir,
			sfmOutputDir + "/robust.bin",
			matchesDir + "/matches.f.bin");
		if (STATE_RETURN == EXIT_FAILURE)
		{
			std::cout << "SFP�ع�ʧ��" << std::endl;
			Global::process = PROCESSERROR;
			break;
		}
		std::cout << "SFP�ع��ɹ�" << std::endl;
		std::cout << "����ϡ��������" << std::endl;
		STATE_RETURN = ExportSparseCloud(
			sfmOutputDir + "/robust.bin",
			sfmOutputDir + "/SparseCloud.j3d",
			sfmOutputDir + "/undistorted_images");
		if (STATE_RETURN == EXIT_FAILURE)
		{
			std::cout << "���ϡ�����ʧ��" << std::endl;
			Global::process = PROCESSERROR;
			break;
		}
		std::cout << "\n�������" << std::endl;
		Global::process = PROCESSCLOSE;

	}
	break;
	case CMD_EXPORTDENSECLOUD: {
		Global::process = PROCESSWORKING;
		Global::saveProcess();
		std::string densifyInputDir, densifyOutputDir, densifyWorkingDir;


		ifstream cmdCache;
		cmdCache.open(("C:\\ProgramData\\J3DEngine\\cmdCache.tmp"), ios::in | ios::_Nocreate);
		if (!cmdCache)
		{

			MessageBoxA(NULL, "����ʧ��,�޷���ȡ�������", "����", MB_OK);
			Global::process = PROCESSERROR;
			break;
		}

		std::string temp;
		getline(cmdCache, temp);
		if (temp != "densifypointcloud")
		{
			MessageBoxA(NULL, "����ʧ��,�޷���ȡ�������", "����", MB_OK);

			Global::process = PROCESSERROR;
			break;
		}

		getline(cmdCache, temp);
		densifyInputDir = temp;

		getline(cmdCache, temp);
		densifyWorkingDir = temp;

		getline(cmdCache, temp);
		densifyOutputDir = temp;


		cmdCache.close();
		char* cmd[7];
		char t[200];
		GetModuleFileNameA(NULL, t, 200);
		cmd[0] = t;
		cmd[1] = "-i";
		cmd[2] = (char*)densifyInputDir.data();
		cmd[3] = "-w";
		cmd[4] = (char*)densifyWorkingDir.data();
		cmd[5] = "-o";
		cmd[6] = (char*)densifyOutputDir.data();
		STATE_RETURN = !MVSEngine::DensifyPointCloud(7, cmd);
		Global::process = STATE_RETURN;
		return;
	}
	break;
	case CMD_RECONSTRUCTMESH: {
		Global::process = PROCESSWORKING;
		Global::saveProcess();
		std::string reconstructMeshInputDir, reconstructMeshOutputDir, reconstructMeshWorkingDir;
		ifstream cmdCache;
		cmdCache.open(("C:\\ProgramData\\J3DEngine\\cmdCache.tmp"), ios::in | ios::_Nocreate);
		if (!cmdCache)
		{
			MessageBoxA(NULL, "����ʧ��,�޷���ȡ�������", "����", MB_OK);
			Global::process = PROCESSERROR;
			break;
		}

		std::string temp;
		getline(cmdCache, temp);
		if (temp != "reconstructmesh")
		{
			MessageBoxA(NULL, "����ʧ��,�޷���ȡ�������", "����", MB_OK);
			Global::process = PROCESSERROR;
			break;
		}

		getline(cmdCache, temp);
		reconstructMeshInputDir = temp;

		getline(cmdCache, temp);
		reconstructMeshWorkingDir = temp;

		getline(cmdCache, temp);
		reconstructMeshOutputDir = temp;


		cmdCache.close();
		char* cmd[9];
		char t[200];
		GetModuleFileNameA(NULL, t, 200);
		cmd[0] = t;
		cmd[1] = "-i";
		cmd[2] = (char*)reconstructMeshInputDir.data();
		cmd[3] = "-d";
		cmd[4] = "4";
		cmd[5] = "-o";
		cmd[6] = (char*)reconstructMeshOutputDir.data();
		cmd[7] = "-w";
		cmd[8] = (char*)reconstructMeshWorkingDir.data();
		int status = MVSEngine::ReconstructMesh(9, cmd);
		cmd[0] = t;
		cmd[1] = "-i";
		cmd[2] = (char*)reconstructMeshOutputDir.data();
		cmd[3] = "--resolution-level";
		cmd[4] = "2";
		cmd[5] = "-o";
		cmd[6] = (char*)reconstructMeshOutputDir.data();
		cmd[7] = "-w";
		cmd[8] = (char*)reconstructMeshWorkingDir.data();
		status = MVSEngine::RefineMesh(9, cmd);
		if (status == EXIT_SUCCESS) {
			Global::process = PROCESSCLOSE;
		}
		else {
			Global::process = PROCESSWORKING;
		}
	}
	break;
	case CMD_TEXTUREMESH: {
		Global::process = PROCESSWORKING;
		Global::saveProcess();
		std::string textureMeshInputDir, textureMeshOutputDir, textureMeshWorkingDir, exportFormat;
		ifstream cmdCache;
		cmdCache.open(("C:\\ProgramData\\J3DEngine\\cmdCache.tmp"), ios::in | ios::_Nocreate);
		if (!cmdCache)
		{
			MessageBoxA(NULL, "����ʧ��,�޷���ȡ�������", "����", MB_OK);
			Global::process = PROCESSERROR;
			break;
		}

		std::string temp;
		getline(cmdCache, temp);
		if (temp != "texturemesh")
		{
			MessageBoxA(NULL, "����ʧ��,�޷���ȡ�������", "����", MB_OK);
			Global::process = PROCESSERROR;
			break;
		}

		getline(cmdCache, temp);
		textureMeshInputDir = temp;

		getline(cmdCache, temp);
		textureMeshWorkingDir = temp;

		getline(cmdCache, temp);
		textureMeshOutputDir = temp;

		getline(cmdCache, temp);
		exportFormat = temp;


		cmdCache.close();
		char* cmd[9];
		char t[200];
		GetModuleFileNameA(NULL, t, 200);
		cmd[0] = t;
		cmd[1] = "-i";
		cmd[2] = (char*)textureMeshInputDir.data();
		cmd[3] = "-o";
		cmd[4] = (char*)textureMeshOutputDir.data();
		cmd[5] = "-w";
		cmd[6] = (char*)textureMeshWorkingDir.data();
		cmd[7] = "--export-type";
		cmd[8] = (char*)exportFormat.data();
		int status = MVSEngine::TextureMesh(9, cmd);
		//Global::resizeTextureFile(textureMeshOutputDir);
		if (status == EXIT_SUCCESS) {
			Global::process = PROCESSCLOSE;
		}
		else {
			Global::process = PROCESSWORKING;
		}
		
		
	}
	break;
	case CMD_FULLAUTO: 
	{
		Global::process = PROCESSWORKING;
		Global::saveProcess();


		std::string imagesInputDir;
		std::string sensorWidthDataBaseDir = "./SenWidDB.txt";
		std::string matchesOutputDir;
		std::string EigenMatrix;// EigenMatrixFormat"f;0;ppx;0;f;ppy;0;0;1"
		std::string describerMethod;
		std::string featureQuality;
		bool upRight;
		bool forceCompute;
		std::string geometricModel;
		float distanceRatio;
		bool forceMatch;

		std::string matchesDir, sfmOutputDir;
		int triangulationMethod, resectionMethod;

		std::string densifyInputDir, densifyOutputDir, densifyWorkingDir;

		std::string reconstructMeshInputDir, reconstructMeshOutputDir, reconstructMeshWorkingDir;

		std::string textureMeshInputDir, textureMeshOutputDir, textureMeshWorkingDir, exportFormat;

		std::cout << "\n������У�FULLAUTO \n" << std::endl;
		ifstream cmdCache;
		cmdCache.open(("C:\\ProgramData\\J3DEngine\\fullautoCmdCache.tmp"), ios::in | ios::_Nocreate);
		if (!cmdCache)
		{
			std::cout << "����ʧ��,�޷���ȡ�������\n" << std::endl;
			Global::process = PROCESSERROR;
			break;
		}

		std::string temp;
		getline(cmdCache, temp);
		if (temp != "matchfeature")
		{
			std::cout << "����ʧ��,�޷���ȡ�������\n" << std::endl;
			Global::process = PROCESSERROR;
			break;
		}

		getline(cmdCache, temp);
		imagesInputDir = temp;
		getline(cmdCache, temp);
		matchesOutputDir = temp;
		getline(cmdCache, temp);
		EigenMatrix = temp;
		if (EigenMatrix == "NULL")
		{
			EigenMatrix = "";
		}
		getline(cmdCache, temp);
		describerMethod = temp;
		getline(cmdCache, temp);
		featureQuality = temp;
		getline(cmdCache, temp);
		if (temp == "0")
			upRight = false;
		else
			upRight = true;
		getline(cmdCache, temp);
		if (temp == "0")
			forceCompute = false;
		else
			forceCompute = true;
		getline(cmdCache, temp);
		geometricModel = temp;
		getline(cmdCache, temp);
		distanceRatio = atof(temp.c_str());
		getline(cmdCache, temp);
		if (temp == "0")
			forceMatch = false;
		else
			forceMatch = true;
		getline(cmdCache, temp);
		if (temp != "sfmandsfp")
		{
			std::cout << "����ʧ��,�޷���ȡ�������\n" << std::endl;
			Global::process = PROCESSERROR;
			break;
		}

		getline(cmdCache, temp);
		matchesDir = temp;

		getline(cmdCache, temp);
		sfmOutputDir = temp;

		getline(cmdCache, temp);
		triangulationMethod = atoi(temp.c_str());

		getline(cmdCache, temp);
		resectionMethod = atoi(temp.c_str());

		getline(cmdCache, temp);
		if (temp != "densifypointcloud")
		{
			MessageBoxA(NULL, "����ʧ��,�޷���ȡ�������", "����", MB_OK);

			Global::process = PROCESSERROR;
			break;
		}

		getline(cmdCache, temp);
		densifyInputDir = temp;

		getline(cmdCache, temp);
		densifyWorkingDir = temp;

		getline(cmdCache, temp);
		densifyOutputDir = temp;

		getline(cmdCache, temp);
		if (temp != "reconstructmesh")
		{
			MessageBoxA(NULL, "����ʧ��,�޷���ȡ�������", "����", MB_OK);
			Global::process = PROCESSERROR;
			break;
		}

		getline(cmdCache, temp);
		reconstructMeshInputDir = temp;

		getline(cmdCache, temp);
		reconstructMeshWorkingDir = temp;

		getline(cmdCache, temp);
		reconstructMeshOutputDir = temp;

		getline(cmdCache, temp);
		if (temp != "texturemesh")
		{
			MessageBoxA(NULL, "����ʧ��,�޷���ȡ�������", "����", MB_OK);
			Global::process = PROCESSERROR;
			break;
		}

		getline(cmdCache, temp);
		textureMeshInputDir = temp;

		getline(cmdCache, temp);
		textureMeshWorkingDir = temp;

		getline(cmdCache, temp);
		textureMeshOutputDir = temp;

		getline(cmdCache, temp);
		exportFormat = temp;

		cmdCache.close();



		STATE_RETURN = LoadingImages(imagesInputDir, matchesOutputDir, sensorWidthDataBaseDir, EigenMatrix);
		if (STATE_RETURN == EXIT_FAILURE)
		{
			std::cout << "����ͼƬʧ��" << std::endl;
			Global::process = PROCESSERROR;
			break;
		}
		std::cout << "����ͼƬ�ɹ�" << std::endl;
		STATE_RETURN = GetFeatures(
			matchesOutputDir + "/sfm_data.json",
			matchesOutputDir,
			describerMethod,
			"",
			upRight,
			forceCompute);
		Sleep(3000);

		if (STATE_RETURN == EXIT_FAILURE)
		{
			std::cout << "��ȡ������Ϣʧ��" << std::endl;
			Global::process = PROCESSERROR;
			break;

		}
		std::cout << "��ȡ��������ɣ�׼����ʼƥ��������" << std::endl;
		STATE_RETURN = GetMatches(
			matchesOutputDir + "/sfm_data.json",
			matchesOutputDir,
			geometricModel,
			"",
			"AUTO",
			-1,
			2048,
			0U,
			distanceRatio, forceMatch);
		Sleep(3000);

		if (STATE_RETURN == EXIT_FAILURE)
		{
			std::cout << "ƥ��������Ϣʧ��" << std::endl;
			Global::process = PROCESSERROR;
			break;
		}
		std::cout << "ƥ�����" << std::endl;
		STATE_RETURN = StructureFromMotion(
			matchesDir + "\\sfm_data.json",
			matchesDir,
			"",
			sfmOutputDir,
			"",
			"",
			"ADJUST_ALL",
			3,
			false,
			false,
			triangulationMethod,
			resectionMethod);
		Sleep(3000);
		if (STATE_RETURN == EXIT_FAILURE)
		{
			std::cout << "SFM�ؽ�ʧ��" << std::endl;
			Global::process = PROCESSERROR;
			break;

		}
		std::cout << "���е�����ɫ" << std::endl;
		STATE_RETURN = PrintPointColors(sfmOutputDir + "/sfm_data.bin", sfmOutputDir + "/colored.ply");
		Sleep(3000);
		if (STATE_RETURN == EXIT_FAILURE)
		{
			std::cout << "SFM������ɫʧ��" << std::endl;
			Global::process = PROCESSERROR;
			break;
		}
		std::cout << "����SFP�ع�" << std::endl;
		STATE_RETURN = StructureFromPoses(
			sfmOutputDir + "/sfm_data.bin",
			matchesDir,
			sfmOutputDir + "/robust.bin",
			matchesDir + "/matches.f.bin");
		Sleep(3000);
		if (STATE_RETURN == EXIT_FAILURE)
		{
			std::cout << "SFP�ع�ʧ��" << std::endl;
			Global::process = PROCESSERROR;
			break;
		}
		std::cout << "SFP�ع��ɹ�" << std::endl;
		std::cout << "����ϡ��������" << std::endl;
		STATE_RETURN = ExportSparseCloud(
			sfmOutputDir + "/robust.bin",
			sfmOutputDir + "/SparseCloud.j3d",
			sfmOutputDir + "/undistorted_images");
		Sleep(3000);
		if (STATE_RETURN == EXIT_FAILURE)
		{
			std::cout << "���ϡ�����ʧ��" << std::endl;
			Global::process = PROCESSERROR;
			break;
		}
		char* cmd[7];
		char t[200];
		GetModuleFileNameA(NULL, t, 200);
		cmd[0] = t;
		cmd[1] = "-i";
		cmd[2] = (char*)densifyInputDir.data();
		cmd[3] = "-w";
		cmd[4] = (char*)densifyWorkingDir.data();
		cmd[5] = "-o";
		cmd[6] = (char*)densifyOutputDir.data();
		STATE_RETURN = MVSEngine::DensifyPointCloud(7, cmd);

		Sleep(5000);
		char* cmd1[9];
		char t1[200];
		GetModuleFileNameA(NULL, t1, 200);
		cmd1[0] = t1;
		cmd1[1] = "-i";
		cmd1[2] = (char*)reconstructMeshInputDir.data();
		cmd1[3] = "-d";
		cmd1[4] = "4";
		cmd1[5] = "-o";
		cmd1[6] = (char*)reconstructMeshOutputDir.data();
		cmd1[7] = "-w";
		cmd1[8] = (char*)reconstructMeshWorkingDir.data();
		STATE_RETURN = MVSEngine::ReconstructMesh(9, cmd1);
		Sleep(5000);
		cmd1[0] = t1;
		cmd1[1] = "-i";
		cmd1[2] = (char*)reconstructMeshOutputDir.data();
		cmd1[3] = "--resolution-level";
		cmd1[4] = "2";
		cmd1[5] = "-o";
		cmd1[6] = (char*)reconstructMeshOutputDir.data();
		cmd1[7] = "-w";
		cmd1[8] = (char*)reconstructMeshWorkingDir.data();
		STATE_RETURN = MVSEngine::RefineMesh(9, cmd1);
		Sleep(5000);
		char* cmd2[9];
		char t2[200];
		GetModuleFileNameA(NULL, t2, 200);
		cmd2[0] = t2;
		cmd2[1] = "-i";
		cmd2[2] = (char*)textureMeshInputDir.data();
		cmd2[3] = "-o";
		cmd2[4] = (char*)textureMeshOutputDir.data();
		cmd2[5] = "-w";
		cmd2[6] = (char*)textureMeshWorkingDir.data();
		cmd2[7] = "--export-type";
		cmd2[8] = (char*)exportFormat.data();
		STATE_RETURN = MVSEngine::TextureMesh(9, cmd2);

		Global::process = PROCESSCLOSE;
	}
	}


	return;
}

int main()
{
	MSG msg;
	SetTimer(NULL, 0, 1000, NULL);
	_mkdir("C:\\ProgramData\\J3DEngine");
	HANDLE hStdin = GetStdHandle(STD_INPUT_HANDLE);
	DWORD mode;
	GetConsoleMode(hStdin, &mode);
	mode &= ~ENABLE_QUICK_EDIT_MODE; //�Ƴ����ٱ༭ģʽ
	mode &= ~ENABLE_INSERT_MODE; //�Ƴ�����ģʽ
	mode &= ~ENABLE_MOUSE_INPUT;
	SetConsoleMode(hStdin, mode);

	SetConsoleTitle("J3DEngine");
	int tid = GetCurrentThreadId();
	if (!saveTid())
	{
		std::cout << "д�뻺��ʧ�ܣ��������Ȩ�� " << endl;
		return -1;
	}
	m_hWnd = FindWindow("Qt5QWindowIcon", "J3DGUI");
	if (m_hWnd) {
		printf("�ҵ�J3DGUI����");
		SendMessage(m_hWnd, WM_USER, tid, 0);
	}

	std::cout << "\n-----------------------------------" << std::endl;
	std::cout << "        ��ӭʹ��J3DEngine V1.6        " << std::endl;
	std::cout << "            �����ʼ���ɹ�             " << std::endl;
	std::cout << "        ��ʹ��J3DGUI������ָ��       " << std::endl;
	std::cout << "     @Basic All rights reserved    " << std::endl;
	std::cout << "-----------------------------------" << std::endl;
	Global::processProject = 0;
	Global::processState = 0;
	Global::process = PROCESSWORKING;


	while (int a = GetMessage(&msg, NULL, 0, 0))
	{
		if (a != -1)
		{
			if (msg.message == WM_TIMER)
				TimerProc(NULL, 0, 0, 0);
			MsgProc(msg.message, msg.wParam, msg.lParam);
		}

	}

	return 0;
}
