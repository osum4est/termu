#include <stdexcept>
#include <string>

class gui_exception : public std::runtime_error {
public:
	explicit gui_exception(const std::string &message) : runtime_error(message) {
	}
};
