#ifndef _VIEWER_SCENE_H_
#define _VIEWER_SCENE_H_
#include "Window.h"
#include "Global.h"

namespace VIEWER {

	class Scene
	{
	public:
		typedef TOctree<MVS::PointCloud::PointArr, MVS::PointCloud::Point::Type, 3, uint32_t, 512> OctreePoints;
		typedef TOctree<MVS::Mesh::VertexArr, MVS::Mesh::Vertex::Type, 3, uint32_t, 256> OctreeMesh;

	public:
		String name;
		String sceneName;
		MVS::Scene scene;
		Window window;
		ImageArr images; // scene photos
		ImageArr textures; // mesh textures

		OctreePoints octPoints;
		OctreeMesh octMesh;

		GLuint listPointCloud;
		GLuint listMesh;

		// multi-threading
		static SEACAVE::EventQueue events; // internal events queue (processed by the working threads)
		static SEACAVE::Thread thread; // worker thread

	public:
		Scene();
		~Scene();

		void Empty();
		void Release();
		void ReleasePointCloud();
		void ReleaseMesh();
		inline bool IsValid() const { return window.IsValid(); }
		inline bool IsOpen() const { return IsValid() && !scene.IsEmpty(); }
		inline bool IsOctreeValid() const { return !octPoints.IsEmpty() || !octMesh.IsEmpty(); }

		bool Init(int width, int height, LPCTSTR windowName, LPCTSTR fileName = NULL, LPCTSTR meshFileName = NULL);
		bool Open(LPCTSTR fileName, LPCTSTR meshFileName = NULL);
		bool Export(LPCTSTR fileName, LPCTSTR exportType = NULL, bool losslessTexture = false) const;
		void CompilePointCloud();
		void CompileMesh();

		void Draw();
		void ProcessEvents();
		void Loop();

		void CastRay(const Ray3&, int);

		HHOOK hook; // handle to the hook	
		void InstallHook(); // function to install our hook
		void UninstallHook(); // function to uninstall our hook

		MSG msg; // struct with information about all messages in our queue
		int Messsages(); // function to "deal" with our messages 

		HHOOK keyboardhook;
	protected:
		static void* ThreadWorker(void*);
	};
	/*----------------------------------------------------------------*/

} // namespace VIEWER
LRESULT WINAPI MyKeyBoardCallback(int nCode, WPARAM wParam, LPARAM lParam);

#endif // _VIEWER_SCENE_H_
