
#include <chrono>

namespace big
{
	template<typename T = std::chrono::milliseconds, int Delay = 0>
	class throttle
	{
		std::chrono::system_clock::time_point last_time = std::chrono::system_clock::time_point::min();
		T delay;

	public:
		throttle() :
		    delay(Delay)
		{
		}

		void reset()
		{
			last_time = std::chrono::system_clock::now();
		}

		void reset(int new_delay)
		{
			delay     = T(new_delay);
			last_time = std::chrono::system_clock::now();
		}


		bool has_time_passed()
		{
			auto now  = std::chrono::system_clock::now();

			if (now - last_time >= delay)
			{
				last_time = now;
				return true;
			}

			return false;
		}
	};
}