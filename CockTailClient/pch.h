#pragma once

#include <vector>
#include <memory>
#include <random>
#include <chrono>

#include <locale>

#include <coroutine>

// �Ŵ���
#include "WindowManager.h"
#include "SceneManager.h"
#include "NavigationManager.h"
#include "SoundManager.h"
#include "SceneBase.h"
#include "InputManager.h"
#include "Timer.h"
#include "GraphicsManager.h"
#include "GameManager.h"
#include "Util/ObjectPool.h"

// ���� ������Ʈ�� ������Ʈ
#include "Object/GameObject.h"
#include "Component/Camera.h"
#include "Component/BasicCameraController.h"
#include "Component/Transform.h"
#include "Component/MeshRenderer.h"
#include "Component/NavMeshAgent.h"
#include "Component/Monobehavior.h"


// ���� ������ ���ϰ� ���� ����..
using namespace GameEngineSpace;

// ���̺귯�� �߰�.
// 64�� ��
#ifdef x64
#ifdef _DEBUG
#pragma comment(lib, "FootGameEngine_Debug_x64.lib")
#else
#pragma comment(lib, "FootGameEngine_Release_x64.lib")
#endif
// 86�� ��.
#else
#ifdef _DEBUG
#pragma comment(lib, "FootGameEngine_Debug_x86.lib")
#else
#pragma comment(lib, "FootGameEngine_Release_x86.lib")
#endif
#endif