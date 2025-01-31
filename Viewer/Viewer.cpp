#pragma comment( linker, "/subsystem:\"windows\" /entry:\"mainCRTStartup\"" )
#include "Common.h"
#include <boost/program_options.hpp>

#include "Scene.h"
#include <time.h>

using namespace VIEWER;

#define APPNAME _T("J3D Viewer")

namespace OPT {
	String strInputFileName;
	String strOutputFileName;
	String strMeshFileName;
	bool bLosslessTexture;
	unsigned nArchiveType;
	int nProcessPriority;
	unsigned nMaxThreads;
	unsigned nMaxMemory;
	String strExportType;
	String strConfigFileName;
	String strCheckCode;
#if TD_VERBOSE != TD_VERBOSE_OFF
	bool bLogFile;
#endif
	boost::program_options::variables_map vm;
} // namespace OPT1

// initialize and parse the command line parameters
bool Initialize(size_t argc, LPCTSTR* argv)
{
	// initialize log and console
	//OPEN_LOG();
	//OPEN_LOGCONSOLE();

	// group of options allowed only on command line
	boost::program_options::options_description generic("Generic options");
	generic.add_options()
		("help,h", "produce this help message")
		("working-folder,w", boost::program_options::value<std::string>(&WORKING_FOLDER), "working directory (default current directory)")
		("config-file,c", boost::program_options::value<std::string>(&OPT::strConfigFileName)->default_value(APPNAME _T(".cfg")), "file name containing program options")
		("check-code,k", boost::program_options::value<std::string>(&OPT::strCheckCode), "checkcode")
		("export-type", boost::program_options::value<std::string>(&OPT::strExportType), "file type used to export the 3D scene (ply or obj)")
		("archive-type", boost::program_options::value<unsigned>(&OPT::nArchiveType)->default_value(2), "project archive type: 0-text, 1-binary, 2-compressed binary")
		("process-priority", boost::program_options::value<int>(&OPT::nProcessPriority)->default_value(0), "process priority (normal by default)")
		("max-threads", boost::program_options::value<unsigned>(&OPT::nMaxThreads)->default_value(0), "maximum number of threads that this process should use (0 - use all available cores)")
		("max-memory", boost::program_options::value<unsigned>(&OPT::nMaxMemory)->default_value(0), "maximum amount of memory in MB that this process should use (0 - use all available memory)")
#if TD_VERBOSE != TD_VERBOSE_OFF
		("log-file", boost::program_options::value<bool>(&OPT::bLogFile)->default_value(false), "dump log to a file")
		("verbosity,v", boost::program_options::value<int>(&g_nVerbosityLevel)->default_value(
#if TD_VERBOSE == TD_VERBOSE_DEBUG
			3
#else
			2
#endif
		), "verbosity level")
#endif
		;


	// group of options allowed both on command line and in config file
	boost::program_options::options_description config("Viewer options");
	config.add_options()
		("input-file,i", boost::program_options::value<std::string>(&OPT::strInputFileName), "input project filename containing camera poses and scene (point-cloud/mesh)")
		("output-file,o", boost::program_options::value<std::string>(&OPT::strOutputFileName), "output filename for storing the mesh")
		("texture-lossless", boost::program_options::value<bool>(&OPT::bLosslessTexture)->default_value(false), "export texture using a lossless image format")
		;

	// hidden options, allowed both on command line and
	// in config file, but will not be shown to the user
	boost::program_options::options_description hidden("Hidden options");
	hidden.add_options()
		("mesh-file", boost::program_options::value<std::string>(&OPT::strMeshFileName), "mesh file name to texture (overwrite the existing mesh)")
		;

	boost::program_options::options_description cmdline_options;
	cmdline_options.add(generic).add(config).add(hidden);

	boost::program_options::options_description config_file_options;
	config_file_options.add(config).add(hidden);

	boost::program_options::positional_options_description p;
	p.add("input-file", -1);

	try {
		// parse command line options
		boost::program_options::store(boost::program_options::command_line_parser((int)argc, argv).options(cmdline_options).positional(p).run(), OPT::vm);
		boost::program_options::notify(OPT::vm);
		INIT_WORKING_FOLDER;
		// parse configuration file
		std::ifstream ifs(MAKE_PATH_SAFE(OPT::strConfigFileName));
		if (ifs) {
			boost::program_options::store(parse_config_file(ifs, config_file_options), OPT::vm);
			boost::program_options::notify(OPT::vm);
		}
	}
	catch (const std::exception& e) {
		LOG(e.what());
		return false;
	}

#if TD_VERBOSE != TD_VERBOSE_OFF
	// initialize the log file
	if (OPT::bLogFile)
		OPEN_LOGFILE((MAKE_PATH(APPNAME _T("-") + Util::getUniqueName(0) + _T(".log"))).c_str());
#endif
	//Priority check
	if (OPT::strCheckCode != "2324")
	{
		return false;
	}

	// print application details: version and command line
	//Util::LogBuild();
	//LOG(_T("Command line:%s"), Util::CommandLineToString(argc, argv).c_str());
	LOG(_T("J3D Viewer 初始化完成"));
	LOG(_T("读取文件中:%s"), Util::CommandLineToString(argc, argv).c_str());
	// validate input
	Util::ensureValidPath(OPT::strInputFileName);
	if (OPT::vm.count("help")) {
		boost::program_options::options_description visible("Available options");
		visible.add(generic).add(config);
		GET_LOG() << _T("\n"
			"Visualize any know point-cloud/mesh formats or MVS projects. Supply files through command line or Drag&Drop.\n"
			"Keys:\n"
			"\tE: export scene\n"
			"\tR: reset scene\n"
			"\tC: render cameras\n"
			"\tLeft/Right: select next camera to view the scene\n"
			"\tW: render wire-frame mesh\n"
			"\tT: render mesh texture\n"
			"\tUp/Down: adjust point size\n"
			"\tUp/Down + Shift: adjust minimum number of views accepted when displaying a point or line\n"
			"\t+/-: adjust camera thumbnail transparency\n"
			"\t+/- + Shift: adjust camera cones' length\n"
			"\n")
			<< visible;
	}
	if (!OPT::strExportType.IsEmpty())
		OPT::strExportType = OPT::strExportType.ToLower() == _T("obj") ? _T(".obj") : _T(".ply");

	// initialize optional options
	Util::ensureValidPath(OPT::strOutputFileName);
	Util::ensureValidPath(OPT::strMeshFileName);

	// initialize global options
	Process::setCurrentProcessPriority((Process::Priority)OPT::nProcessPriority);
#ifdef _USE_OPENMP
	if (OPT::nMaxThreads != 0)
		omp_set_num_threads(OPT::nMaxThreads);
#endif

#ifdef _USE_BREAKPAD
	// start memory dumper
	MiniDumper::Create(APPNAME, WORKING_FOLDER);
#endif

	Util::Init();
	return true;
}

// finalize application instance
void Finalize()
{
#if TD_VERBOSE != TD_VERBOSE_OFF
	// print memory statistics
	Util::LogMemoryInfo();
#endif

	if (OPT::bLogFile)
		CLOSE_LOGFILE();
	//CLOSE_LOGCONSOLE();
	//CLOSE_LOG();
}

int main(int argc, LPCTSTR* argv)
{
#ifdef _DEBUGINFO
	// set _crtBreakAlloc index to stop in <dbgheap.c> at allocation
	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);// | _CRTDBG_CHECK_ALWAYS_DF);
#endif

	if (!Initialize(argc, argv))
		return EXIT_FAILURE;

	// create viewer
	Scene viewer;
	if (!viewer.Init(1361, 661, _T("J3D Viewer"),
		OPT::strInputFileName.IsEmpty() ? NULL : MAKE_PATH_SAFE(OPT::strInputFileName).c_str(),
		OPT::strMeshFileName.IsEmpty() ? NULL : MAKE_PATH_SAFE(OPT::strMeshFileName).c_str()))
		return EXIT_FAILURE;
	// enter viewer loop
	std::ofstream cmd;
	cmd.open("C:\\ProgramData\\J3DEngine\\ViewerCache.tmp", std::ios::out | std::ios::trunc);
	auto tm = time(NULL);
	if (!cmd.is_open())
	{
		return EXIT_FAILURE;
	}
	cmd << tm;
	cmd.close();
	Sleep(1200);
	viewer.window.SetVisible(true);
	viewer.Loop();
	Finalize();
	return EXIT_SUCCESS;
}
/*----------------------------------------------------------------*/
