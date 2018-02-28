#pragma once
#include <d3d9.h>
#include "Singleton.h"

class D3D : public Singleton<D3D>
{public:
	void EndScene(IDirect3DDevice9* device);
};
