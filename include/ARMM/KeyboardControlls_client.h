#ifndef KEYBOARD_CONTROLSCLIENT_H
#define KEYBOARD_CONTROLSCLIENT_H

//boost API
#include <boost/shared_ptr.hpp>

namespace ARMM
{
	class osg_Client;

	class KeyboardController_client
	{
	public:
		KeyboardController_client(){};
		~KeyboardController_client(){};
		int		check_input();
		void	set_input(const int & key, boost::shared_ptr<osg_Client> osgrender);

	private:
		inline bool getKey(int key);

	};
}
#endif
