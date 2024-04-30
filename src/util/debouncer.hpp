
#include <chrono>

namespace big
{
	template<typename T = std::chrono::milliseconds>
	class debouncer
	{
		std::chrono::system_clock::time_point last_time = std::chrono::system_clock::time_point::min();
		int delay;

	public:
		debouncer(int _delay = 0)
		{
			delay = _delay;
		}

		void reset(int _delay)
		{
			last_time = std::chrono::system_clock::now();
			delay     = _delay;
		}

		bool has_debounced()
		{
			auto currentTime = std::chrono::system_clock::now();
			auto diff        = std::chrono::duration_cast<T>(currentTime - last_time);

			if (diff.count() >= delay)
			{
				last_time = currentTime;
				return true;
			}

			return false;
		}
	};
}