#pragma once
#include <algorithm>
#include <map>
#include "Packager.hpp"
namespace PackageWays {
	namespace{
        std::vector<Rectangle> &&sortedByHeight(std::vector<Rectangle> const &source){
            std::vector<Rectangle> result;
            for(auto const &elem: source){
                result.push_back(elem.width() > elem.height() ? elem.rotate() : elem);
            }
            std::sort(result.begin(), result.end(), [](Rectangle const &first, Rectangle const &second){
               return first.height() > second.height();
            });
            return std::move(result);
        }
    }

	class FCNR {
        class FCNRBox{
        public:
            enum putResult{
                ERROR_OK,
                ERROR_FLOOR_FILL,
                ERROR_CEIL_FILL,
                ERROR_IMPOSSIBLE
            };

            FCNRBox(Rectangle const &box)
                : m_height(box.height())
                , m_width(box.width()){}

            putResult tryPutFloor(size_t level, Rectangle const &rect){
                auto floorFilled = m_filledByFloor[level];
                if(floorFilled.empty()){
                    size_t filledHeight = level == 0 ? 0 : m_filledByFloor[level - 1].begin()->getRightUpper().second ;
                    if(filledHeight + rect.height() > m_height || rect.width() > m_width){
                        return ERROR_IMPOSSIBLE;
                    }
                    floorFilled.emplace_back(std::move(RectangleStoraged(std::move(std::make_pair(0, filledHeight)),
                                                               std::move(std::make_pair(rect.width(), filledHeight + rect.height())), level)));
                }
                else {
                    size_t filledWidth = floorFilled.rbegin()->getRightUpper().first;
                    if(filledWidth + rect.width() > m_width){
                        return ERROR_FLOOR_FILL;
                    }
                    std::pair<size_t, size_t> leftBottom = std::make_pair(filledWidth, floorFilled.rbegin()->getLeftBottom().second);
                    std::pair<size_t, size_t> rightUpper = std::make_pair(filledWidth + rect.width(), floorFilled.rbegin()->getLeftBottom().second + rect.height());

                    for(auto const &ceil: m_filledByCeil[level]){
                        if(isFloorCeilIntersect(leftBottom, rightUpper, ceil.getLeftBottom(), ceil.getRightUpper())){
                            return ERROR_FLOOR_FILL;
                        }
                    }
                    floorFilled.emplace_back(std::move(RectangleStoraged(std::move(leftBottom), std::move(rightUpper), level)));
                }
                return ERROR_OK;

            }
            putResult tryPutCeil(size_t level, Rectangle const &rect){
                auto floorFilled = m_filledByFloor[level];
                if(floorFilled.empty()){
                    return ERROR_IMPOSSIBLE;
                }
                auto ceilFilled = m_filledByCeil[level];
                Rectangle rotRect{std::move(rect.rotate())};
                size_t filledWidth = ceilFilled.empty() ? 0 : ceilFilled.rbegin()->getLeftBottom().first;
                if(filledWidth + rotRect.width() > m_width){
                    return ERROR_CEIL_FILL;
                }
                std::pair<size_t, size_t> rightUpper = std::make_pair(filledWidth,
                                                                      floorFilled.begin()->getRightUpper().second);
                std::pair<size_t, size_t> leftBottom = std::make_pair(filledWidth + rotRect.width(),
                                                                      floorFilled.begin()->getRightUpper().second - rotRect.height());
                for(auto const & floorRect: floorFilled){
                    if(isFloorCeilIntersect(floorRect.getLeftBottom(), floorRect.getRightUpper(), leftBottom, rightUpper)){
                        return ERROR_CEIL_FILL;
                    }
                }
                ceilFilled.emplace_back(std::move(RectangleStoraged(std::move(leftBottom), std::move(rightUpper), level)));
            }

            void fill(std::vector<RectangleStoraged> &storaged){
                for(auto & it: m_filledByFloor){
                    storaged.insert(storaged.end(), it.second.begin(), it.second.end());
                }
                for(auto & it: m_filledByCeil){
                    storaged.insert(storaged.end(), it.second.begin(), it.second.end());
                }
            }
        private:
            inline bool isFloorCeilIntersect(std::pair<size_t, size_t> const &leftBottomFloor, std::pair<size_t, size_t> const &rightUpperFloor,
                                    std::pair<size_t, size_t> const &leftBottomCeil, std::pair<size_t, size_t> const &rightUpperCeil)
            {
                if(rightUpperFloor.second > leftBottomCeil.second){
                    return (leftBottomFloor.first < leftBottomCeil.first && rightUpperFloor.first > rightUpperCeil.first) ||
                        (leftBottomFloor.first > leftBottomCeil.first && leftBottomFloor.first < rightUpperCeil.first) ||
                        (rightUpperFloor.first > leftBottomCeil.first && leftBottomFloor.first < rightUpperCeil.first);
                }
                return false;
            }

            std::map<size_t, std::vector<RectangleStoraged>> m_filledByFloor;
            std::map<size_t, std::vector<RectangleStoraged>> m_filledByCeil;
            size_t m_height;
            size_t m_width;
        };
	public:
        FCNR(std::pair<Rectangle, std::vector<Rectangle>> &&source)
            : m_box(std::move(source.first))
            , m_rectangles(std::move(source.second)){ }

        Packanger::result_type execute() {
			std::vector<Rectangle> sortered{std::move(sortedByHeight(m_rectangles))};
            std::vector<FCNRBox> filledBoxes;
            size_t level = 0;
            bool repeated = false;
            for(auto rect = sortered.begin(); rect != sortered.end(); ){
                auto &curBox = filledBoxes.size() < level + 1 ? filledBoxes.emplace_back(FCNRBox(m_box))
                        : filledBoxes[level];
                auto fillRes = curBox.tryPutFloor(level, *rect);
                if(fillRes == FCNRBox::ERROR_FLOOR_FILL) {
                    fillRes = curBox.tryPutCeil(level, *rect);
                    if (fillRes == FCNRBox::ERROR_CEIL_FILL) {
                        ++level;
                        continue;
                    }
                }
                if(fillRes == FCNRBox::ERROR_IMPOSSIBLE){
                    if(repeated){
                        ++rect;
                    }
                    else {
                        repeated = true;
                    }
                }
                else if(fillRes == FCNRBox::ERROR_OK){
                    ++rect;
                }
            }

            std::vector<RectangleStoraged> result;
            for(auto & it: filledBoxes){
                it.fill(result);
            }
            return std::make_tuple(level + 1, m_box, std::move(result));
		}
    private:
        Rectangle m_box;
        std::vector<Rectangle> m_rectangles;
	};
	struct Burke {

	};
	struct SF {

	};
};