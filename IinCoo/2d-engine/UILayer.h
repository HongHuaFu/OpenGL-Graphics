#pragma once
#include "Layer.h"

namespace Engine
{
	class UILayer : public Layer
	{
	public:
		UILayer():Layer("UILayer"){ }

		~UILayer() = default;

		virtual void OnAttach() override;
		virtual void OnDetach() override;

		void Begin();
		void End();

	private:
		float mTime = 0.0f;
	};
}
