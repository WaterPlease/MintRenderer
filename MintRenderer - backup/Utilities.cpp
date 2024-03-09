#include "pch.h"
#include "Utilities.h"

cFrameLimiter::cFrameLimiter() {
	StartTime = GetTime();
	MinFrameTime = 0;
}

void cFrameLimiter::SetMaxFPS(size_t FPS) {
	MaxFPS = FPS;

	if (MaxFPS > 0)
		MinFrameTime = static_cast<size_t>(1000000.f / static_cast<float>(MaxFPS));
	else
		MinFrameTime = 0;
}

void cFrameLimiter::Wait(bool bUpdateStartTime) {
	FinishTime = GetTime();
	while (GetInterval(StartTime, FinishTime) < MinFrameTime) {
		FinishTime = GetTime();
	}
	if (bUpdateStartTime)
		StartTime = FinishTime;
}

cFrameLimiter::cTimePoint cFrameLimiter::GetTime() {
	return std::chrono::high_resolution_clock::now();
}

size_t cFrameLimiter::GetInterval(const cTimePoint& StartTime, const cTimePoint& FinishTime) {
	return std::chrono::duration_cast<std::chrono::microseconds>(FinishTime - StartTime).count();
}
