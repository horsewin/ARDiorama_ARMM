#ifndef KEYBOARD_CONTROLSCLIENT_H
#define KEYBOARD_CONTROLSCLIENT_H

//Standard API
#include <string>
#include <vector>

//boost API
#include <boost/shared_ptr.hpp>

namespace ARMM
{
	class osg_Client;

	class KeyboardController_client
	{

	public:
		KeyboardController_client();
		~KeyboardController_client();
		int		check_input();
		void	set_input(const int & key, boost::shared_ptr<osg_Client> osgrender);

	private:
		inline bool getKey(int key);
		bool RegisteringObject(const char * filename, boost::shared_ptr<osg_Client> osgrender);
		void SwapingObject( boost::shared_ptr<osg_Client> osgrender );
		bool RegisteringSoftObject(const char * filename, boost::shared_ptr<osg_Client> osgrender);
		void AssignmentKeyinput(const char * settingFilename);

	private:
		std::vector<std::pair<unsigned int, std::string> > mKeyAssignment;
	};
}
#endif
