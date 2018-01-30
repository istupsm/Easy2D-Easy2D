#include "..\ebase.h"
#include "..\emanagers.h"

static ID2D1Factory * s_pDirect2dFactory = nullptr;
static ID2D1HwndRenderTarget * s_pRenderTarget = nullptr;
static ID2D1SolidColorBrush * s_pSolidBrush = nullptr;
static IWICImagingFactory * s_pIWICFactory = nullptr;
static IDWriteFactory * s_pDWriteFactory = nullptr;
static D2D1_COLOR_F s_nClearColor = D2D1::ColorF(D2D1::ColorF::Black);


bool e2d::ERenderer::__createDeviceIndependentResources()
{
	// �����豸�޹���Դ�����ǵ��������ںͳ����ʱ����ͬ
	HRESULT hr = D2D1CreateFactory(
		D2D1_FACTORY_TYPE_SINGLE_THREADED, 
		&s_pDirect2dFactory
	);

	ASSERT(SUCCEEDED(hr), "Create Device Independent Resources Failed!");

	if (SUCCEEDED(hr))
	{
		// ���� WIC ��ͼ����������ͳһ�������ָ�ʽ��ͼƬ
		hr = CoCreateInstance(
			CLSID_WICImagingFactory,
			NULL,
			CLSCTX_INPROC_SERVER,
			IID_IWICImagingFactory,
			reinterpret_cast<void**>(&s_pIWICFactory)
		);
		ASSERT(SUCCEEDED(hr), "Create WICImagingFactory Failed!");
	}

	if (SUCCEEDED(hr))
	{
		// ���� DirectWrite ����
		hr = DWriteCreateFactory(
			DWRITE_FACTORY_TYPE_SHARED,
			__uuidof(IDWriteFactory),
			reinterpret_cast<IUnknown**>(&s_pDWriteFactory)
		);
		ASSERT(SUCCEEDED(hr), "Create DirectWrite Factory Failed!");
	}

	return SUCCEEDED(hr);
}

bool e2d::ERenderer::__createDeviceResources()
{
	HRESULT hr = S_OK;
	
	if (!s_pRenderTarget)
	{
		HWND hWnd = EWindow::getHWnd();

		// �����豸�����Դ����Щ��ԴӦ�� Direct3D �豸��ʧʱ�ؽ���
		// ���統 isVisiable ���޸ģ��ȵ�
		RECT rc;
		GetClientRect(hWnd, &rc);

		D2D1_SIZE_U size = D2D1::SizeU(
			rc.right - rc.left,
			rc.bottom - rc.top
		);

		// ����һ�� Direct2D ��ȾĿ��
		hr = s_pDirect2dFactory->CreateHwndRenderTarget(
			D2D1::RenderTargetProperties(),
			D2D1::HwndRenderTargetProperties(
				hWnd,
				size),
			&s_pRenderTarget
		);

		ASSERT(SUCCEEDED(hr), "Create Render Target Failed!");
	}

	if (SUCCEEDED(hr))
	{
		// ������ˢ
		hr = s_pRenderTarget->CreateSolidColorBrush(
			D2D1::ColorF(D2D1::ColorF::White),
			&s_pSolidBrush
		);
		ASSERT(SUCCEEDED(hr), "Create Solid Color Brush Failed!");
	}
	return SUCCEEDED(hr);
}

void e2d::ERenderer::__discardDeviceResources()
{
	SafeReleaseInterface(&s_pRenderTarget);
	SafeReleaseInterface(&s_pSolidBrush);
}

void e2d::ERenderer::__discardResources()
{
	SafeReleaseInterface(&s_pDirect2dFactory);
	SafeReleaseInterface(&s_pRenderTarget);
	SafeReleaseInterface(&s_pSolidBrush);
	SafeReleaseInterface(&s_pIWICFactory);
	SafeReleaseInterface(&s_pDWriteFactory);
}

void e2d::ERenderer::__render()
{
	HRESULT hr = S_OK;

	// �����豸�����Դ
	ERenderer::__createDeviceResources();

	// ��ʼ��Ⱦ
	s_pRenderTarget->BeginDraw();
	// ʹ�ñ���ɫ�����Ļ
	s_pRenderTarget->Clear(s_nClearColor);

	// ��Ⱦ����
	ESceneManager::__render();

	// ��ֹ��Ⱦ
	hr = s_pRenderTarget->EndDraw();

	if (hr == D2DERR_RECREATE_TARGET)
	{
		// ��� Direct3D �豸��ִ�й�������ʧ����������ǰ���豸�����Դ
		// ������һ�ε���ʱ�ؽ���Դ
		hr = S_OK;
		ERenderer::__discardDeviceResources();
	}

	if (FAILED(hr))
	{
		// ��Ⱦʱ������δ֪�Ĵ����˳���Ϸ
		ASSERT(false, L"Renderer error!");
		EGame::quit();
	}
}


void e2d::ERenderer::setBackgroundColor(UINT32 color)
{
	s_nClearColor = D2D1::ColorF(color);
}

ID2D1Factory * e2d::ERenderer::getID2D1Factory()
{
	return s_pDirect2dFactory;
}

ID2D1HwndRenderTarget * e2d::ERenderer::getRenderTarget()
{
	return s_pRenderTarget;
}

ID2D1SolidColorBrush * e2d::ERenderer::getSolidColorBrush()
{
	return s_pSolidBrush;
}

IWICImagingFactory * e2d::ERenderer::getIWICImagingFactory()
{
	return s_pIWICFactory;
}

IDWriteFactory * e2d::ERenderer::getIDWriteFactory()
{
	return s_pDWriteFactory;
}