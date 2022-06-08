#pragma once
#include <algorithm>
#include <map>
#include "Packager.hpp"
namespace PackageWays {
	namespace{
        std::vector<Rectangle> sortedByHeight(std::vector<Rectangle> const &source){
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
        class FCNRBox {
        public:
            enum putResult{
                ERROR_OK,
                ERROR_FILLED,
                ERROR_IMPOSSIBLE
            };

            FCNRBox(Rectangle const &box, size_t boxNumber)
                : m_height(box.height())
                , m_width(box.width())
                , m_boxNumber(boxNumber){}

            putResult tryPutFloor(size_t level, Rectangle const &rect){
                auto &floorFilled = m_filledByFloor[level];
                // first floor case
                if(floorFilled.empty()){
                    size_t upperLimit = m_height;
                    size_t bottom_y = 0;
                    // if levels already init on this level
                    if(m_levelHeight.size() > level)
                    {
                        bottom_y = level > 0 ? m_levelHeight[level - 1] : 0;
                        upperLimit = m_levelHeight[level];
                    }
                    // new level data
                    else if(m_levelHeight.size() + 1 > level && !m_levelHeight.empty()){
                        bottom_y = level > 0 ? m_levelHeight[level - 1] : 0;
                    }

                    size_t rectUpperY = bottom_y + rect.height();
                    if(rectUpperY > upperLimit || rect.width() > m_width){
                        if(upperLimit == m_height)
                            return ERROR_IMPOSSIBLE;
                        return ERROR_FILLED;
                    }
                    floorFilled.emplace_back(std::move(RectangleStoraged(std::move(std::make_pair(0, bottom_y)),
                                                               std::move(std::make_pair(rect.width(), rectUpperY)), m_boxNumber)));
                    // init new level
                    if(upperLimit == m_height)
                        m_levelHeight.emplace_back(rectUpperY);
                }
                else {
                    size_t left_x = floorFilled.rbegin()->getRightUpper().first;
                    if(left_x + rect.width() > m_width){
                        return ERROR_FILLED;
                    }
                    std::pair<size_t, size_t> leftBottom = std::make_pair(left_x, floorFilled.rbegin()->getLeftBottom().second);
                    std::pair<size_t, size_t> rightUpper = std::make_pair(left_x + rect.width(), floorFilled.rbegin()->getLeftBottom().second + rect.height());

                    for(auto const &ceil: m_filledByCeil[level]){
                        if(isFloorCeilIntersect(leftBottom, rightUpper, ceil.getLeftBottom(), ceil.getRightUpper())){
                            return ERROR_FILLED;
                        }
                    }
                    floorFilled.emplace_back(std::move(RectangleStoraged(std::move(leftBottom), std::move(rightUpper), m_boxNumber)));
                }
                return ERROR_OK;
            }

            putResult tryPutCeil(size_t level, Rectangle const &rect){
                auto &floorFilled = m_filledByFloor[level];
                auto &ceilFilled = m_filledByCeil[level];

                Rectangle rotRect{std::move(rect.rotate())};
                size_t right_x = ceilFilled.empty() ? m_width : ceilFilled.rbegin()->getLeftBottom().first;
                if(right_x < rotRect.width()){
                    return ERROR_FILLED;
                }
                size_t upperLimit = m_height;
                size_t bottom_y = 0;
                // if levels already init on this level
                if(m_levelHeight.size() > level)
                {
                    bottom_y = level > 0 ? m_levelHeight[level - 1] : 0;
                    upperLimit = m_levelHeight[level];
                }
                // new level data
                else if(m_levelHeight.size() + 1 > level && !m_levelHeight.empty()){
                    bottom_y = level > 0 ? m_levelHeight[level - 1] : 0;
                }
                if(bottom_y + rotRect.height() > upperLimit){
                    if(upperLimit == m_height)
                        return ERROR_IMPOSSIBLE;
                    return ERROR_FILLED;
                }
                std::pair<size_t, size_t> leftBottom = std::make_pair(right_x - rotRect.width(),
                                                                      bottom_y);
                std::pair<size_t, size_t> rightUpper = std::make_pair(right_x,
                                                                      bottom_y + rotRect.height());
                for(auto const & floorRect: floorFilled){
                    if(isFloorCeilIntersect(floorRect.getLeftBottom(), floorRect.getRightUpper(), leftBottom, rightUpper)){
                        return ERROR_FILLED;
                    }
                }
                ceilFilled.emplace_back(std::move(RectangleStoraged(std::move(leftBottom), std::move(rightUpper), m_boxNumber)));
                // init new level
                if(upperLimit == m_height){
                    m_levelHeight.emplace_back(upperLimit);
                }
                return ERROR_OK;
            }

            void fill(std::vector<RectangleStoraged> &storaged){
                for(auto & it: m_filledByFloor){
                    storaged.insert(storaged.end(), it.second.begin(), it.second.end());
                }
                for(auto & it: m_filledByCeil){
                    storaged.insert(storaged.end(), it.second.begin(), it.second.end());
                }
            }
            bool isClear(){
                return m_filledByFloor.empty() && m_filledByCeil.empty();
            }
        private:
            inline bool isFloorCeilIntersect(std::pair<size_t, size_t> const &leftBottomFloor, std::pair<size_t, size_t> const &rightUpperFloor,
                                    std::pair<size_t, size_t> const &leftBottomCeil, std::pair<size_t, size_t> const &rightUpperCeil)
            {
                if(std::min(rightUpperFloor.first, rightUpperCeil.first) - std::max(leftBottomFloor.first, leftBottomCeil.first) > 0
                    && std::min(rightUpperFloor.second, rightUpperCeil.second) - std::max(leftBottomFloor.second, leftBottomCeil.second) > 0){
                    return true;
                }
                return false;
            }

            std::map<size_t, std::vector<RectangleStoraged>> m_filledByFloor;
            std::map<size_t, std::vector<RectangleStoraged>> m_filledByCeil;
            std::vector<size_t> m_levelHeight;
            size_t m_height;
            size_t m_width;
            size_t m_boxNumber;
        };
	public:
        FCNR(std::pair<Rectangle, std::vector<Rectangle>> &&source)
            : m_box(std::move(source.first))
            , m_rectangles(std::move(source.second)){ }

        Packanger::result_type execute() {
			std::vector<Rectangle> sortered{std::move(sortedByHeight(m_rectangles))};
            std::vector<FCNRBox> filledBoxes;
            Rectangle box{m_box.height() > m_box.width() ? m_box : m_box.rotate()};
            size_t level = 0;
            size_t filledBoxSize = 0;
            for(auto rect = sortered.begin(); rect != sortered.end(); ){
                auto &curBox = filledBoxes.size() < filledBoxSize + 1 ? filledBoxes.emplace_back(FCNRBox(box, filledBoxSize))
                        : filledBoxes[filledBoxSize];
                if(rect->width() > box.width() || rect->height() > box.height()){
                    continue;
                }
                FCNRBox::putResult fillFloor = curBox.tryPutFloor(level, *rect);
                FCNRBox::putResult fillCeil;
                if(fillFloor != FCNRBox::ERROR_OK) {
                    fillCeil = curBox.tryPutCeil(level, *rect);
                }
                if(fillFloor == FCNRBox::ERROR_OK ||
                    fillCeil == FCNRBox::ERROR_OK){
                    ++rect;
                    filledBoxSize = 0;
                    level = 0;
                } else if(fillFloor == FCNRBox::ERROR_IMPOSSIBLE || fillCeil == FCNRBox::ERROR_IMPOSSIBLE){
                    if(curBox.isClear()){
                        ++rect;
                        filledBoxSize = 0;
                        level = 0;
                    }
                    else{
                        ++filledBoxSize;
                        level = 0;
                    }
                }
                else{
                    ++level;
                }
            }

            std::vector<RectangleStoraged> result;
            for(auto & it: filledBoxes){
                it.fill(result);
            }
            return std::make_tuple(filledBoxes.size(), box, std::move(result));
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