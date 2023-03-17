#include <iostream>

class X {
public:
	X() {
		std::cout << "default constructor at 0x" << this << "\n";
	}

	X(const X&) {
		std::cout << "copy constructor at 0x" << this << "\n";
	}

	X(X&&) {
		std::cout << "move constructor 0x" << this << "\n";
	}

	X& operator=(const X&) {
		std::cout << "copy assignment\n";
		return *this;
	}

	X& operator=(X&&) {
		std::cout << "move assignment\n";
		return *this;
	}
};

std::ostream& operator<<(std::ostream& os, const X& x) {
	return os << "outputting X at 0x" << &x;
}