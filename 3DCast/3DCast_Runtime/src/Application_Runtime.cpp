#include <3DCast.h>

class Application_Runtime : public Cast::Application {
public:
	Application_Runtime() {}
	~Application_Runtime() {}
};

Cast::Application* Cast::CreatApplication() {
	return new Application_Runtime();
}