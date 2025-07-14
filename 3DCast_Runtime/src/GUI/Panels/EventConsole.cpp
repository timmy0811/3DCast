#include "EventConsole.h"

#include <imgui.h>
#include <3DCast/Core/Log.h>

void Runtime::GUI::EventConsole::OnImGuiRender()
{
	static std::string concatenatedText;
	static size_t prevLogSize = 0;

	ImGui::Begin("Log Output");

	const auto imguiSink = Cast::Log::GetImGuiSink();
	const auto& logBuffer = imguiSink->GetLog();
	const size_t totalLines = logBuffer.size();

	bool logChanged = totalLines != prevLogSize;

	if (logChanged) {
		const ImVec2 windowSize = ImGui::GetWindowSize();
		static const float lineHeightRec = 1.f / ImGui::GetTextLineHeightWithSpacing();
		const int visibleLines = static_cast<int>((windowSize.y - ImGui::GetStyle().WindowPadding.y * 2) * lineHeightRec);

		concatenatedText.clear();
		concatenatedText.reserve(5000);

		const size_t startIndex = totalLines > visibleLines ? totalLines - visibleLines : 0;
		for (size_t i = startIndex; i < totalLines; ++i) {
			concatenatedText.append(logBuffer[i]);
		}

		prevLogSize = totalLines;
	}

	ImGui::BeginChild("ScrollingRegion", ImVec2(0, 0), false, ImGuiWindowFlags_HorizontalScrollbar);

	ImGui::TextUnformatted(concatenatedText.c_str());

	if (logChanged) {
		ImGui::SetScrollHereY(1.0f);
	}

	ImGui::EndChild();
	ImGui::End();
}
