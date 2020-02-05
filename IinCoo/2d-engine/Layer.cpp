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


	//插入一个新种类的层
	void LayerStack::PushLayer(Layer* layer)
	{
		//使用emplace代替insert避免二次构造以提高性能
		mLayers.emplace(mLayers.begin() + mLayerIndex, layer);
		mLayerIndex += 1;
		layer->OnAttach();
	}


	//同种类层插入
	void LayerStack::PushOverlay(Layer* overlay)
	{
		mLayers.emplace_back(overlay);
		overlay->OnAttach();
	}


	//弹出一个新种类的层
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

	//同种类弹出
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


