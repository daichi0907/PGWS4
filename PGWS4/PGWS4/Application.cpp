#include "Application.h"
#include <tchar.h>
#include "Dx12Wrapper.h"
#include "PMDRenderer.h"
#include "PMDActor.h"

using namespace DirectX;
using namespace Microsoft::WRL;

using namespace DirectX;
using namespace Microsoft::WRL;

///@brief �R���\�[����ʂɃt�H�[�}�b�g�t���������\��
///@param format �t�H�[�}�b�g(%d�Ƃ�%f�Ƃ���)
///@param �ϒ�����
///@remarks���̊֐��̓f�o�b�O�p�ł��B�f�o�b�O���ɂ������삵�܂���
void DebugOutputFormatString(const char* format, ...)
{
#ifdef _DEBUG
	va_list valist;
	va_start(valist, format);
	vprintf(format, valist);
	va_end(valist);
#endif
}

static inline void ThrowIfFailed(HRESULT hr)
{
	if (FAILED(hr))
	{
		throw std::exception();
	}
}

//�ʓ|�ł����A�E�B���h�E�v���V�[�W���͕K�{�Ȃ̂ŏ����Ă����܂�
LRESULT WindowProcedure(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam)
{
	//�E�B���h�E���j�����ꂽ��Ă΂�܂�
	if (msg == WM_DESTROY)
	{
		PostQuitMessage(0);//OS�ɑ΂��āu�������̃A�v���͏I�����v�Ɠ`����
		return 0;
	}
	return DefWindowProc(hwnd, msg, wparam, lparam);//�K��̏������s��
}

static HWND CreateGameWindow(WNDCLASSEX& windowClass, UINT width, UINT height)
{
	windowClass.cbSize = sizeof(WNDCLASSEX);
	windowClass.lpfnWndProc = (WNDPROC)WindowProcedure;//�R�[���o�b�N�֐��̎w��
	windowClass.lpszClassName = _T("DirectXTest");//�A�v���P�[�V�����N���X��(�K���ł����ł�)
	windowClass.hInstance = GetModuleHandle(0);//�n���h���̎擾
	RegisterClassEx(&windowClass);//�A�v���P�[�V�����N���X(���������̍�邩���낵������OS�ɗ\������)

	RECT wrc = { 0, 0, (LONG)width, (LONG)height };//�E�B���h�E�T�C�Y�����߂�
	AdjustWindowRect(&wrc, WS_OVERLAPPEDWINDOW, false);//�E�B���h�E�̃T�C�Y�͂�����Ɩʓ|�Ȃ̂Ŋ֐����g���ĕ␳����
	//�E�B���h�E�I�u�W�F�N�g�̐���
	return CreateWindow(windowClass.lpszClassName,//�N���X���w��
		_T("DX12�e�X�g"),//�^�C�g���o�[�̕���
		WS_OVERLAPPEDWINDOW,//�^�C�g���o�[�Ƌ��E��������E�B���h�E�ł�
		CW_USEDEFAULT,//�\��X���W��OS�ɂ��C�����܂�
		CW_USEDEFAULT,//�\��Y���W��OS�ɂ��C�����܂�
		wrc.right - wrc.left,//�E�B���h�E��
		wrc.bottom - wrc.top,//�E�B���h�E��
		nullptr,//�e�E�B���h�E�n���h��
		nullptr,//���j���[�n���h��
		windowClass.hInstance,//�Ăяo���A�v���P�[�V�����n���h��
		nullptr);//�ǉ��p�����[�^
}

bool Application::Init()
{
	ThrowIfFailed(CoInitializeEx(0, COINIT_MULTITHREADED)); // �Ă΂Ȃ��Ɠ��삵�Ȃ����\�b�h������

	_hwnd = CreateGameWindow(_windowClass, _window_width, _window_height); // �E�B���h�E����

	_dx12.reset(new Dx12Wrapper(_hwnd, _window_width, _window_height));
	_pmdRenderer.reset(new PMDRenderer(*_dx12));

	//_pmdActor.reset(new PMDActor("Model/�������J.pmd", *_pmdRenderer));
	//_pmdActor.reset(new PMDActor("Model/�����~�Nmetal.pmd", *_pmdRenderer));
	_pmdActor.reset(new PMDActor("Model/�����~�N.pmd", *_pmdRenderer));
	//_pmdActor->LoadVMDFile("motion/swing.vmd", "pose");
	//_pmdActor->LoadVMDFile("motion/swing2.vmd", "pose");
	//_pmdActor->LoadVMDFile("motion/motion.vmd", "pose");
	_pmdActor->LoadVMDFile("motion/�r�o�n�s.vmd", "pose");
	_pmdActor->PlayAnimation();

	return true;
}

void Application::Run()
{
	ShowWindow(_hwnd, SW_SHOW); // �E�B���h�E�\��

	while (true)
	{
		MSG msg;
		if (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE))
		{
			// �A�v���P�[�V�������I���Ƃ��� message �� WM_QUIT�ɂȂ�
			if (msg.message == WM_QUIT)
			{
				break;
			}

			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}

		_dx12->BeginDraw();

		auto _cmdList = _dx12->CommandList();
		
		_cmdList->SetPipelineState(_pmdRenderer->GetPipelineState());

		_cmdList->SetGraphicsRootSignature(_pmdRenderer->GetRootSignature());

		_cmdList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
		
		_dx12->ApplySceneDescHeap();

		_pmdActor->Update();
		_pmdActor->Draw();

		_dx12->EndDraw();		
	}
}

void Application::Terminate()
{
	// �����N���X�g��񂩂�o�^����
	UnregisterClass(_windowClass.lpszClassName, _windowClass.hInstance);
}

Application::Application()
{
}

Application::~Application()
{
}

Application& Application::Instance()
{
	static Application instance;
	return instance;
}