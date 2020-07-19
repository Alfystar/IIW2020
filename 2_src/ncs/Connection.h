//
// Created by alfystar on 19/07/20.
//

#ifndef HTTP_IMAGESERVER_CONNECTION_H
#define HTTP_IMAGESERVER_CONNECTION_H


#include <poll.h>

namespace NCS{
	class Connection{
	public:
		int fd;
		static long count;

		Connection();

		~Connection();

		void compilePollFD(struct pollfd *poolFd);

	};
}

#endif //HTTP_IMAGESERVER_CONNECTION_H
