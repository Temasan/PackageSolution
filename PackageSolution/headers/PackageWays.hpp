#pragma once
#include "Packager.hpp"
namespace PackageWays {
	class FCNR;
	//using PackagerImpl = Packanger<PackageWays::FCNR>;

	class FCNR {
        class FCNRBox{

        };
	public:
        FCNR(std::pair<Rectangle, std::vector<Rectangle>> &&source)
            : m_height(std::move(source.first.height()))
            , m_width(std::move(source.first.width()))
            , m_rectangles(std::move(source.second)){ }

        Packanger::result_type execute() {
			std::vector<RectangleStoraged> result;
			//return std::make_pair(std::size_t{1}, result);
		}
    private:
        size_t m_height;
        size_t m_width;
        std::vector<Rectangle> m_rectangles;
	};
	struct Burke {

	};
	struct SF {

	};
};