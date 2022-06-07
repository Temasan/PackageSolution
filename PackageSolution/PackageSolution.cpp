// PackageSolution.cpp: определяет точку входа для приложения.
//

#include "PackageSolution.h"
#include "headers/Packager.hpp"
#include "headers/PackageWays.hpp"
#include "headers/Rectangle.hpp"
#include <iostream>
#include <string>

int main(){
	std::string line;
	//using PackagerImpl = PackageWays::PackagerImpl;
    Packanger::source_type source;
	bool firstLine = true;
	while (std::cin >> line) {
		if (std::cin.fail()) {
			break;
		}
		auto splitPos = line.find(",");
		if (splitPos == line.npos) {
			break;
		}
		if (firstLine) {
            Rectangle box(std::stoi(line.substr(0, splitPos)),
				std::stoi(line.substr(splitPos + 1)));
			if (!box.height() || !box.width()) {
				break;
			}
			source.first = std::move(box);
			firstLine = false;
		}
		else {
			Rectangle rectangle(std::stoi(line.substr(0, splitPos)),
				std::stoi(line.substr(splitPos + 1)));
			if (!rectangle.height() || !rectangle.width()) {
				break;
			}
			source.second.emplace_back(std::move(rectangle));
		}
	}
	if (source.second.empty() || !source.first.height() || !source.first.width()) {
		return -1;
	}
	auto res = Packanger().getBetterResult(std::move(PackageWays::FCNR(std::move(source))));
	std::cout << std::get<0>(res) << "," << std::get<1>(res).to_string() << "\n";
	for (auto const& it : std::get<2>(res)) {
		std::cout << it.to_string() << std::endl;
	}
	return 0;
}
