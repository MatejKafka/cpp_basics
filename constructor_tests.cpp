#include <iostream>
#include <vector>

class X {
public:
	X() {
		std::cout << ">>> default constructor at 0x" << this << "\n";
	}

	X(const X&) {
		std::cout << ">>> copy constructor at 0x" << this << "\n";
	}

	// the noexcept is important, otherwise most built-in collections will refuse to use the move constructor
	X(X&&) noexcept {
		std::cout << ">>> move constructor at 0x" << this << "\n";
	}

	X& operator=(const X& other) {
		std::cout << ">>> copy assignment from 0x" << &other << " to 0x" << this << "\n";
		return *this;
	}

	X& operator=(X&& other) noexcept {
		std::cout << ">>> move assignment from 0x" << &other << " to 0x" << this << "\n";
		return *this;
	}
};

std::ostream& operator<<(std::ostream& os, const X& x) {
	return os << ">>> outputting X at 0x" << &x;
}


void main() {
	std::cout << "first init\n";
	X x{};

	std::vector<X> vec{};
	std::cout << "push back x\n";
	vec.push_back(x);
	std::cout << "push back new instance\n";
	vec.push_back(X{});
	std::cout << "emplace_back\n";
	vec.emplace_back();
	std::cout << "resize\n";
	vec.resize(8);

	std::cout << "assign\n";
	X x2{};
	x2 = x;
	std::cout << "assign move\n";
	X x3{};
	x3 = std::move(x);
}