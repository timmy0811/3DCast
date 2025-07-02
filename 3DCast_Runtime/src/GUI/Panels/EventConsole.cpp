#include "EventConsole.h"

#include <imgui.h>
#include <3DCast/Core/Log.h>

void Runtime::GUI::EventConsole::OnImGuiRender()
{
	static size_t prevLogSize = 0;

	ImGui::Begin("Log Output");

	const auto imguiSink = Cast::Log::GetImGuiSink();
	const auto& logBuffer = imguiSink->GetLog();
	const size_t totalLines = logBuffer.size();

	const size_t startIndex = totalLines > 50 ? totalLines - 50 : 0;

	ImGui::BeginChild("ScrollingRegion", ImVec2(0, 0), false, ImGuiWindowFlags_HorizontalScrollbar);

	for (size_t i = startIndex; i < totalLines; ++i)
	{
		ImGui::TextUnformatted(logBuffer[i].c_str());
	}

	if (totalLines != prevLogSize)
	{
		ImGui::SetScrollHereY(1.0f);
		prevLogSize = totalLines;
	}

	ImGui::EndChild();
	ImGui::End();
}
