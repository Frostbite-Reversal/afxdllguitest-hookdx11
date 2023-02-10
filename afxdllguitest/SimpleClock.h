#pragma once
#include<chrono>
#include<thread>
#include<timeapi.h>
#pragma comment(lib, "winmm.lib")
#include<filesystem>
#include<limits>
#include<unordered_map>
#define STATUS_SUCCESS 0
#define STATUS_TIMER_RESOLUTION_NOT_SET 0xC0000245
static NTSTATUS(NTAPI* NtSetTimerResolution)(ULONG DesiredResolution, BOOLEAN SetResolution, PULONG CurrentResolution) = (NTSTATUS(NTAPI*)(ULONG, BOOLEAN, PULONG)) GetProcAddress(GetModuleHandleA("ntdll.dll"), "NtSetTimerResolution");
#define BUFFER_SIZE 64
namespace Math {
	class Predictor {
	public:
		std::vector<DWORD64> m_vecAddr;
		double A = 0, B = 0;
		Predictor() { m_vecAddr.reserve(BUFFER_SIZE); }
		~Predictor() = default;
		inline void AddAddr(DWORD64 addr) {
			if (m_vecAddr.size() >= BUFFER_SIZE) m_vecAddr.erase(m_vecAddr.begin());
			m_vecAddr.push_back(addr);
		}
		inline std::pair<double, double> GetRegressionEquation() {
			if (m_vecAddr.size() < 2)return { 0,m_vecAddr.back() };
			double sumX = 0, sumY = 0, sumXY = 0, sumX2 = 0;
			for (size_t i = 0; i < m_vecAddr.size(); ++i) {
				sumX += i;
				sumY += m_vecAddr[i];
				sumXY += i * m_vecAddr[i];
				sumX2 += i * i;
			}
			double n = (double)m_vecAddr.size();
			double a = (n * sumXY - sumX * sumY) / (n * sumX2 - sumX * sumX);
			double b = (sumY - a * sumX) / n;
			return { a,b };
		}
		inline double GetAjustR2() {//调整后的趋势线拟合程度
			if (m_vecAddr.size() < 2) return 0;
			double x = 0, y = 0, xy = 0, x2 = 0, y2 = 0;
			for (int i = 0; i < m_vecAddr.size(); i++) {
				x += i;
				y += m_vecAddr[i];
				xy += i * m_vecAddr[i];
				x2 += i * i;
				y2 += m_vecAddr[i] * m_vecAddr[i];
			}
			double n = (double)m_vecAddr.size();
			double r = (n * xy - x * y) / sqrt((n * x2 - x * x) * (n * y2 - y * y));
			return (r * r - 1.0 / n) / (1 - 1.0 / n);
		}
		inline DWORD64 GetPredictAddr(DWORD64 addr) {
			AddAddr(addr);
			if (m_vecAddr.size() >= 2) {
				double R2 = GetAjustR2();
				if (R2 > 0.7) {
					auto [a, b] = GetRegressionEquation();
					return (DWORD64)(a * m_vecAddr.size() + b);
				}
			}
			return addr;
		}
		inline DWORD64 operator()(DWORD64 addr) { return GetPredictAddr(addr); }
	};
}
class SimpleClock {
private:
	using _internal_clock = std::chrono::high_resolution_clock;
	using timepoint = _internal_clock::time_point;
	using millisecond = std::chrono::milliseconds;
	using microsecond = std::chrono::microseconds;
	using nanoseconds = std::chrono::nanoseconds;
	Math::Predictor m_predictor;
	LARGE_INTEGER perfCnt;
	SimpleClock() {
		m_begin = now();
		srand((unsigned)time(0));
		SetSystemTimerReslutionBegin(0.5f);
		QueryPerformanceFrequency(&perfCnt);
		SetPriorityClass(GetCurrentProcess(), REALTIME_PRIORITY_CLASS);
	}
	SimpleClock(const SimpleClock& _clock) = delete;
	SimpleClock(SimpleClock&&) = delete;
	void operator= (const SimpleClock&) = delete;
	std::unordered_map<std::string, HANDLE> m_sleepEvents;
	bool m_bNatieApiOK = true;
	ULONG m_CurrentResolution = 0;
	inline void SetSystemTimerReslutionBegin(float _fResolution) {
		ULONG m_DesiredReslution = (ULONG)(10000 * _fResolution);
		if (NtSetTimerResolution(m_DesiredReslution, TRUE, &m_CurrentResolution) != STATUS_SUCCESS) {
			m_bNatieApiOK = false;
			timeBeginPeriod((int)_fResolution);
		}
	}
	inline void SetSystemTimerReslutionEnd(float _fResolution) {
		if (m_bNatieApiOK) {
			ULONG m_DesiredReslution = (ULONG)(10000 * _fResolution);
			NtSetTimerResolution(m_DesiredReslution, FALSE, &m_CurrentResolution);
		}
		else {
			timeEndPeriod((int)_fResolution);
		}
	}
public:
	~SimpleClock()noexcept {
		for (auto& sleepEvent : m_sleepEvents) CloseHandle(sleepEvent.second);
		SetSystemTimerReslutionEnd(0.5f);
		m_sleepEvents.clear();
	};
	timepoint m_begin;
	static SimpleClock& GetInstance() { static SimpleClock m_instance; return m_instance; }
	inline timepoint now() { return _internal_clock::now(); }
	inline float duration(timepoint _begin, timepoint _end) { return TimeCast(_end - _begin); }
	inline float operator()() { return duration(m_begin, now()) / 1000.0f; }
	inline void Sleep(int _millisecond) { std::this_thread::sleep_for(millisecond(_millisecond)); }
	inline void SleepEvent(std::string _name, int _millisecond) { WaitForSingleObject(GetSleepEvent(_name), _millisecond); }
	inline HANDLE GetSleepEvent(const std::string& _eventName) {
		if (auto eventiter = m_sleepEvents.find(_eventName); eventiter != m_sleepEvents.end()) {
			return eventiter->second;
		}
		else {
			HANDLE sleepevt = OpenEventA(EVENT_ALL_ACCESS, FALSE, _eventName.c_str());
			if (sleepevt == NULL)sleepevt = CreateEventA(NULL, TRUE, FALSE, _eventName.c_str());
			m_sleepEvents.insert(std::make_pair(_eventName, sleepevt));
			return sleepevt;
		}
	}
	inline void CloseSleepEvent(std::string _eventName) {
		CloseHandle(m_sleepEvents[_eventName]);
		m_sleepEvents.erase(_eventName);
	}
	inline timepoint after(timepoint now, float millisecond) {
		return now + microsecond((LONGLONG)millisecond * 1000);
	}
	inline void Sleep_Until(timepoint _timepoint) {
		if (_timepoint > now()) { std::this_thread::sleep_until(_timepoint); }
	}
	template<class T = nanoseconds>inline decltype(auto) TimeCast(const nanoseconds& time1) {
		return std::chrono::duration_cast<T>(time1).count() / 1000.0f;
	};
	inline void HighResolutionSleep(float  _millisecond, std::string name = "") {//睡眠毫秒
		auto starttime = _internal_clock::now();
		if ((int)_millisecond > 0) WaitForSingleObject(GetSleepEvent(name), (int)_millisecond);//向下取整
		auto resttime = _millisecond * 1000.0f - TimeCast(_internal_clock::now() - starttime);
		if (resttime > 0) {
			resttime = std::clamp<float>(resttime, 0, 5000);//0-5s内 
			LARGE_INTEGER start, now;
			QueryPerformanceCounter(&start);
			do {
				QueryPerformanceCounter(&now);
			} while ((m_predictor(now.QuadPart) - start.QuadPart) * 1e+6 / float(perfCnt.QuadPart) < resttime);
		}
	}

};
inline SimpleClock& GlobalClock = SimpleClock::GetInstance();


