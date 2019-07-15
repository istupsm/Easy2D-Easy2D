#include <e2dbase.h>
#include <e2dmanager.h>
#include <e2dtool.h>


// ������Ϸ��ֹ
static bool s_bEndGame = true;
// ������Ϸ��ͣ
static bool s_bPaused = false;
// �Ƿ���й���ʼ��
static bool s_bInitialized = false;
// ��Ϸ����
static easy2d::String s_sGameName;


bool easy2d::Game::init(const String& name, const String& mutexName)
{
	if (s_bInitialized)
	{
		E2D_WARNING(L"The game has been initialized!");
		return false;
	}

	if (!mutexName.empty())
	{
		// �������̻�����
		String fullMutexName = L"Easy2DApp-" + mutexName;
		HANDLE hMutex = ::CreateMutex(nullptr, TRUE, fullMutexName.c_str());

		if (hMutex == nullptr)
		{
			E2D_WARNING(L"CreateMutex Failed!");
		}
		else if (::GetLastError() == ERROR_ALREADY_EXISTS)
		{
			// ��������Ѿ����ڲ����������У�������ʾ
			Window::error(L"��Ϸ�������������д򿪣�", L"��ʾ");
			// �رս��̻�����
			::CloseHandle(hMutex);
			return false;
		}
	}

	// ��ʼ�� COM ���
	if (FAILED(CoInitialize(nullptr)))
	{
		E2D_ERROR(L"��ʼ�� COM ���ʧ��");
		return false;
	}

	// �����豸�޹���Դ
	if (!Renderer::__createDeviceIndependentResources())
	{
		E2D_ERROR(L"��Ⱦ���豸�޹���Դ����ʧ��");
		return false;
	}

	// ��ʼ������
	if (!Window::__init())
	{
		E2D_ERROR(L"��ʼ������ʧ��");
		return false;
	}

	// �����豸�����Դ
	if (!Renderer::__createDeviceResources())
	{
		E2D_ERROR(L"��Ⱦ���豸�����Դ����ʧ��");
		return false;
	}

	// ��ʼ�� DirectInput
	if (!Input::__init())
	{
		E2D_ERROR(L"��ʼ�� DirectInput ʧ��");
		return false;
	}

	// ��ʼ��������
	if (!Music::__init())
	{
		E2D_ERROR(L"��ʼ�� XAudio2 ʧ��");
		return false;
	}

	// ��ʼ��·��
	if (!Path::__init(name))
	{
		E2D_WARNING(L"Path::__init failed!");
	}

	// ������Ϸ����
	s_sGameName = name;

	// ��ʼ���ɹ�
	s_bInitialized = true;

	return s_bInitialized;
}

void easy2d::Game::start()
{
	if (!s_bInitialized)
	{
		E2D_WARNING(L"��ʼ��Ϸǰδ���г�ʼ��");
		return;
	}

	// ��ʼ������������
	SceneManager::__init();
	// ��ʾ����
	::ShowWindow(Window::getHWnd(), SW_SHOWNORMAL);
	// ˢ�´�������
	::UpdateWindow(Window::getHWnd());
	// ����������Ϣ
	Window::__poll();
	// ��ʼ����ʱ
	Time::__init();

	s_bEndGame = false;

	while (!s_bEndGame)
	{
		// ����������Ϣ
		Window::__poll();
		// ˢ��ʱ��
		Time::__updateNow();

		// �ж��Ƿ�ﵽ��ˢ��״̬
		if (Time::__isReady())
		{
			Input::__update();			// ��ȡ�û�����
			Timer::__update();			// ���¶�ʱ��
			ActionManager::__update();	// ���¶���������
			SceneManager::__update();	// ���³�������
			Renderer::__render();		// ��Ⱦ��Ϸ����
			GC::clear();				// �����ڴ�

			Time::__updateLast();		// ˢ��ʱ����Ϣ
		}
		else
		{
			Time::__sleep();			// �����߳�
		}
	}

	s_bEndGame = true;
}

void easy2d::Game::pause()
{
	s_bPaused = true;
}

void easy2d::Game::resume()
{
	if (s_bInitialized && s_bPaused)
	{
		Game::reset();
	}
	s_bPaused = false;
}

void easy2d::Game::reset()
{
	if (s_bInitialized && !s_bEndGame)
	{
		Time::__reset();
		ActionManager::__resetAll();
		Timer::__resetAll();
	}
}

bool easy2d::Game::isPaused()
{
	return s_bPaused;
}

void easy2d::Game::quit()
{
	s_bEndGame = true;	// ���������������Ϸ�Ƿ����
}

void easy2d::Game::destroy()
{
	if (!s_bInitialized)
		return;

	// ɾ�����г���
	SceneManager::__uninit();
	// ɾ�����������
	Input::__clearListeners();
	// ɾ������
	ActionManager::__uninit();
	// �������ֲ�������Դ
	Player::__uninit();
	// ��ն�ʱ��
	Timer::__uninit();
	// ɾ�����ж���
	GC::clear();
	// ���ͼƬ����
	Image::clearCache();
	// �������������Դ
	Music::__uninit();
	// �ر�����
	Input::__uninit();
	// ������Ⱦ�����Դ
	Renderer::__discardResources();
	// ���ٴ���
	Window::__uninit();

	CoUninitialize();

	s_bInitialized = false;
}

easy2d::String easy2d::Game::getName()
{
	return s_sGameName;
}