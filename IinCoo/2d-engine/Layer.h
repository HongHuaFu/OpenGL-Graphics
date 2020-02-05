#pragma once
#include <string>
#include <vector>
#include "Event.h"
#include "Timer.h"


namespace Engine
{
	class Layer
	{
	public:
		Layer(const std::string& name = "Layer") : mLayerName(name) { }
		virtual ~Layer() = default;

		//在push到layerstack时调用
		virtual void OnAttach() {}

		//在销毁时调用
		virtual void OnDetach() {}

		//每帧更新时调用
		virtual void OnUpdate(Timer) {}

		//UI渲染时调用
		virtual void OnUIRender() {}

		//层的事件函数
		virtual void OnEvent(Event&) {}

		inline const std::string& GetLayerName() const { return mLayerName; }
	protected:
		std::string mLayerName;
	};

	class LayerStack
	{
	public:
		LayerStack() = default;
		~LayerStack();

		void PushLayer(Layer* layer);
		void PushOverlay(Layer* overlay);
		void PopLayer(Layer* layer);
		void PopOverlay(Layer* overlay);

	public:
		inline std::vector<Layer*>::iterator Begin() { return mLayers.begin(); }
		inline std::vector<Layer*>::iterator End() { return mLayers.end(); }
		inline std::vector<Layer*>::reverse_iterator ReverseBegin() { return mLayers.rbegin(); }
		inline std::vector<Layer*>::reverse_iterator ReverseEnd() { return mLayers.rend(); }
		inline std::vector<Layer*>::const_iterator Begin() const { return mLayers.begin(); }
		inline std::vector<Layer*>::const_iterator End()	const { return mLayers.end(); }
		inline std::vector<Layer*>::const_reverse_iterator ReverseBegin() const { return mLayers.rbegin(); }
		inline std::vector<Layer*>::const_reverse_iterator ReverseEnd() const { return mLayers.rend(); }

	private:
		std::vector<Layer*> mLayers;

		//层的种类次序
		unsigned int mLayerIndex = 0;
	};
}

