#include "EventLog.hpp"
#include "data_path.hpp"

static constexpr int kMaxEntryNum{20};
static constexpr int kFontSize{800};
static constexpr float kFontLineSpace{0.05f};
static constexpr float kFontStartX{0.61f};
static constexpr float kFontStartY{0.55f};

EventLog::EventLog() : event_dialog_(data_path("ariblk.ttf").c_str(), kFontColors.at("black"), glm::vec4(0.6f, 0.6f, 0.9f, -0.5f), kFontSize, kFontColors.at("white"))
{
	event_dialog_.SetVisibility(true);
}

void EventLog::LogEvent(const std::string &log)
{
	if (entry_num_ < kMaxEntryNum)
	{
		// Not full
		event_dialog_.AddText(log.c_str(), glm::vec2(kFontStartX, kFontStartY - entry_num_ * kFontLineSpace));
		++entry_num_;
	}
	else
	{
		// Full
		event_dialog_.GetText(replace_pointer_)->SetText(log.c_str(), kFontSize);
		replace_pointer_ = (replace_pointer_ + 1) % kMaxEntryNum;

		// Rotate
		for (int i = 0; i < kMaxEntryNum; ++i)
		{
			event_dialog_.GetText((replace_pointer_ + i) % kMaxEntryNum)->SetPos(glm::vec2(kFontStartX, kFontStartY - i * kFontLineSpace));
		}
	}
}

void EventLog::Draw(const glm::uvec2 &drawable_size)
{
	event_dialog_.Draw(drawable_size);
}
