#include "Layer.h"
#include <algorithm>

namespace Engine
{
	LayerStack::~LayerStack()
	{
		for(auto layer : mLayers)
		{
			layer->OnDetach();
			delete layer;
		}
	}


	//����һ��������Ĳ�
	void LayerStack::PushLayer(Layer* layer)
	{
		//ʹ��emplace����insert������ι������������
		mLayers.emplace(mLayers.begin() + mLayerIndex, layer);
		mLayerIndex += 1;
		layer->OnAttach();
	}


	//ͬ��������
	void LayerStack::PushOverlay(Layer* overlay)
	{
		mLayers.emplace_back(overlay);
		overlay->OnAttach();
	}


	//����һ��������Ĳ�
	void LayerStack::PopLayer(Layer* layer)
	{
		auto it = std::find(mLayers.begin(), mLayers.begin() + mLayerIndex, layer);
		if (it != mLayers.begin() + mLayerIndex)
		{
			layer->OnDetach();
			mLayers.erase(it);
			mLayerIndex -= 1;
		}
	}

	//ͬ���൯��
	void LayerStack::PopOverlay(Layer* overlay)
	{
		auto it = std::find(mLayers.begin() + mLayerIndex, mLayers.end(), overlay);
		if (it != mLayers.end())
		{
			overlay->OnDetach();
			mLayers.erase(it);
		}
	}
}


