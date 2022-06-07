#pragma once
#include <thread>
#include <mutex>
#include <vector>
#include <list>
#include "Rectangle.hpp"


class Packanger {
public:
	using result_type = std::tuple<std::size_t, Rectangle, std::vector<RectangleStoraged>>;
	using source_type = std::pair<Rectangle, std::vector<Rectangle>>;
	Packanger() = default;

    template<typename ...Args>
	result_type getBetterResult(Args &&... packagers) {
		m_results.clear();
		m_results.reserve(sizeof...(packagers));

		std::vector<std::thread> threads;
		initThread(threads, std::forward<Args>(packagers)...);
		for (auto& thr : threads) {
			thr.join();
		}
		return m_results.front();
	}
private:

	template<typename TArg, typename ...TEndArgs>
	void initThread(std::vector<std::thread> &threads,
		TArg &&currendPackager, TEndArgs &&... otherPackagers) {
		addInThread(threads, std::forward<TArg>(currendPackager));
		initThread(threads, std::forward<TEndArgs>(otherPackagers)...);
	}

	template<typename TArg>
	void initThread(std::vector<std::thread>& threads,
		TArg&& currendPackager) {
		addInThread(threads, std::forward<TArg>(currendPackager));
	}

	template<typename TArg>
	void addInThread(std::vector<std::thread>& threads, TArg &&package) {
		threads.emplace_back(std::thread([&]() {
			this->fill_results(std::move(package.execute()));
		}));
	}

	void fill_results(result_type&& result) {
		std::lock_guard<std::mutex> lk(m_fillMutex);
		for (auto it = m_results.begin(); it != m_results.end(); ++it) {
			if (std::get<0>(*it) >=std::get<0>(result)) {
				m_results.insert(it, std::move(result));
				return;
			}
		}
		m_results.push_back(std::move(result));
	}
	std::mutex m_fillMutex;
	std::vector<result_type> m_results;
};