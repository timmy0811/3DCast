#include "EventConsole.h"

#include <imgui.h>
#include <3DCast/Core/Log.h>

void Runtime::GUI::EventConsole::OnImGuiRender()
{
	static size_t prevLogSize = 0;

	ImGui::Begin("Log Output");

	// Retrieve our custom sink instance.
	auto imguiSink = Cast::Log::GetImGuiSink();
	const auto& logBuffer = imguiSink->GetLog();
	size_t totalLines = logBuffer.size();

	// Only show the last 50 lines.
	size_t startIndex = totalLines > 50 ? totalLines - 50 : 0;

	ImGui::BeginChild("ScrollingRegion", ImVec2(0, 0), false, ImGuiWindowFlags_HorizontalScrollbar);

	for (size_t i = startIndex; i < totalLines; ++i)
	{
		ImGui::TextUnformatted(logBuffer[i].c_str());
	}

	// If new lines have been added, scroll automatically to the bottom.
	if (totalLines != prevLogSize)
	{
		ImGui::SetScrollHereY(1.0f);
		prevLogSize = totalLines;
	}

	ImGui::EndChild();
	ImGui::End();
}