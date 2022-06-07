#pragma once
#include <string>
class Rectangle {
protected:
public:
	Rectangle(size_t width, size_t height)
		: m_height(height)
		, m_width(width) {}

    Rectangle()
        : m_height(0)
        , m_width(0) {}

	inline constexpr size_t height() const {
		return m_height;
	}

	inline constexpr size_t width() const {
		return m_width;
	}
    inline Rectangle rotate() const{
        return std::move(Rectangle(m_height, m_width));
    }
	std::string to_string() const {
		return std::string(std::to_string(m_width))
			.append(",")
			.append(std::to_string(m_height));
	}
private:
	size_t m_height;
	size_t m_width;
};

class RectangleStoraged {
public:
	RectangleStoraged(std::pair<size_t, size_t> left_bottom,
		std::pair<size_t, size_t>  right_upper,
		size_t container)
		: m_left_bottom(left_bottom)
		, m_right_upper(right_upper)
		, m_container(container) {}

	std::pair<size_t, size_t> const& getLeftBottom() const {
		return m_left_bottom;
	}

	std::pair<size_t, size_t> const& getRightUpper() const {
		return m_right_upper;
	}

    inline constexpr size_t height() const {
        return m_right_upper.second - m_left_bottom.second;
    }

    inline constexpr size_t width() const {
        return m_right_upper.first - m_left_bottom.first;
    }
	std::string to_string() const {
		return std::string(std::to_string(m_container))
			.append(",")
			.append(std::to_string(m_left_bottom.first))
			.append(",")
			.append(std::to_string(m_left_bottom.second))
			.append(",")

			.append(std::to_string(m_right_upper.first))
			.append(",")
			.append(std::to_string(m_left_bottom.second))
			.append(",")

			.append(std::to_string(m_right_upper.first))
			.append(",")
			.append(std::to_string(m_right_upper.second))
			.append(",")

			.append(std::to_string(m_left_bottom.first))
			.append(",")
			.append(std::to_string(m_right_upper.second));
	}
private:
	std::pair<size_t, size_t> m_left_bottom;
	std::pair<size_t, size_t> m_right_upper;
	size_t m_container;
};